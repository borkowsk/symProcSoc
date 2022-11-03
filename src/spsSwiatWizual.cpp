// //////////////////////////////////////////////////////////////////////////////
// Symulator Proces�w Sieciowych/Spolecznych (c) Instytut Studi�w Spo�ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
// //////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
// //////////////////////////////////////////////////////////////////////////////
//
// Funkcje wizualizacji �wiata  - wyodr�bnione
// �eby mo�na by�o �atwo zmieni� na bardziej zaawansowane �rodowisko
// //////////////////////////////////////////////////////////////////////////////

#include "spsModel.h"
#include "spsGenNode.h"
#include "spsGenProc.h"
#include "spsGenLink.h"
#include "spsParaLink.h"
#include "spsGenInfo.h"
#include "spsMatrixNode.h"

#include "symshell.h"
#include "sshutils.hpp"
#include "wieloboki.hpp"
#include "wb_ptr.hpp"
#include "lingo.hpp"

#include <sstream>
using namespace std;
using namespace wbrtm;

//CO wy�wietla� w ka�dym kroku
bool Swiat::WyswietlajWezly=true;
bool Swiat::WyswietlajProcesy=true;
bool Swiat::WyswietlajLinki=true;
bool Swiat::WyswietlajInformacje=true;
unsigned Swiat::OpisWezlow=1;
unsigned Swiat::OpisLinkow=-1;

//Skale - przez ile mno�y� X i Y zeby wype�ni� ca�y ekran?
static double SkalaX=1.0;
static double SkalaY=1.0;

//Wizualizacyjne
void _wizualizuj_komunikat_kropka(Komunikat* Inf);  //Pomocnicza - uproszczona wizualizacja komunikatu

void Swiat::Narysuj()
// Odrysowanie ca�o�ci
{
	Wizualizacja_po_kroku();
}

//Funkcje potrzebne do sortowania (quicksort) wg. Z - �eby panowac nad zas�anianiem
int sort_function_wezly(const void *a, const void *b)
{
   WezelSieci* Aw=*(WezelSieci**)a;
   WezelSieci* Bw=*(WezelSieci**)b;
   double Za=Aw->ZOrder();
   double Zb=Bw->ZOrder();
   if(Za>Zb) return 1;
   else if(Za==Zb) return 0;
		else return -1;
}

int sort_function_linki(const void *a, const void *b)
{
   Powiazanie* Aw=*(Powiazanie**)a;
   Powiazanie* Bw=*(Powiazanie**)b;
   double Za=Aw->ZOrder();
   double Zb=Bw->ZOrder();
   if(Za>Zb) return 1;
   else if(Za==Zb) return 0;
		else return -1;
}

//Tablice do wizualizacji tylko tego co jest naprawde i w odpowiednim ZOrder
static wb_dynarray<Powiazanie*> ListaPow;
static unsigned IlePowFakt=0;
static wb_dynarray<WezelSieci*> ListaWez;
static unsigned IleWezFakt=0;
static bool StrukturaSwiataNieZmieniona=false; //Flaga niezmienionej struktury

void Swiat::UwagaZmienionoStrukture()//     i    jej obsluga
{
   StrukturaSwiataNieZmieniona=false;
}

ElementModelu* Swiat::Inspekcja(int x,int y,bool Wizual)
//Odnajduje element wg. pozycji pikselowej
//i opcjonalnie od razu wy�wietla w oknie wizualizacji pe�ne informacje o nim
{
	double dX=double(x)/SkalaX;
	double dY=double(y)/SkalaY;
	WezelSieci* Szukany=NULL;

	for(unsigned i=0;i<Swiat::IleWezlow();i++)
	{
	   WezelSieci* Pom=Swiat::Wezel(i);
	   if(Pom==NULL) continue; //Mo�e by� pusty slot, kt�ry pomijamy
	   if(Pom->Trafiony(dX,dY))
	   {
		 Szukany=Pom; break;
	   }
	}

	if(Szukany==NULL) return NULL;

	cout<<_LPL("Inspekcja","Inspection of")<<" \""<<Szukany->Nazwa()<<"\" ["<<Szukany->MojID()<<"]"<<endl;

	if(Wizual)  //TERAZ W�A�CIWA PRACA ...
	{
		Szukany->AktualizujListeDanych();//Musimy mie� pewno�� �e na liscie s� w�a�ciwe warto�ci
		ssh_color back=255+127+RANDOM(127);
	   //	circle(SkalaX*Szukany->X(),SkalaY*Szukany->Y(),/*min(SkalaX,SkalaY)**/Szukany->R(0),254);
	   print_width(SkalaX*Szukany->X()-100,SkalaY*Szukany->Y()-char_height('X'),200,0,back,
					"[ %u ] ",Szukany->MojID());
		for(unsigned j=0;j<Szukany->IlePolDanych();j++)
		{
			const char* NazwaPola=Szukany->NazwaPola(j);
			const char* WartoPola=Szukany->WartoscPola(j);
			if(NazwaPola)
				cout<<NazwaPola;
			cout<<" :\t";
			if(WartoPola)
				cout<<WartoPola;
			cout<<endl;
			//Troch� toporne wy�wietlanie "ramki" z danymi
			print_width(SkalaX*Szukany->X()-100,SkalaY*Szukany->Y()+char_height('X')*j,
						200,0,back,
						" %s : %s%s",NazwaPola,WartoPola,
						"                                                                                      ");
		}
		cout<<endl;
	}

	return Szukany;
}

bool Swiat::Wizualizacja_po_kroku()
// Wykonanie pe�nej wizualizacji modelu w danej chwili
// Musz� byc posortowane wg. ZOrder() ale przecie� nie koniecznie co krok!!!
{
	//USTALENIE SKALI
	SkalaX=double(screen_width());
	SkalaY=double(screen_height());
	SkalaX/=hor;
	SkalaY/=ver;
	if(SkalaX<=0.000001 ||  SkalaY<=0.000001)
					return true; //Nic nie wyjdzie z tej wizualizacji ale musi zwr�ci� true bo si� przerwie!

	//REJESTRACJA i RYSOWANIE LINK�W
	if(!StrukturaSwiataNieZmieniona)//Przygotowanie posortowanej listy jak si� co� zmieni�o
	{
		if(Swiat::IlePowiazan()>0)
			ListaPow.alloc(Swiat::IlePowiazan());
		IlePowFakt=0;
		//Przepisywanie na list�
		for(unsigned i=0;i<Swiat::IlePowiazan();i++)
		{
			Powiazanie* Pom=Swiat::Lacze(i);
			if(Pom!=NULL) //Mo�e by� pusty slot
				ListaPow[IlePowFakt++]=Pom;
		}
		//Posortowanie wskaznikow wg. funkcji ZOrder() !!!
		if(IlePowFakt>0)
			qsort(ListaPow.get_ptr_val(),IlePowFakt,sizeof(Powiazanie*),sort_function_linki);
	}

	//Rysowanie powiazan z listy
	if(WyswietlajLinki)
	  for(unsigned i=0;i<IlePowFakt;i++)
	  {
		  if(ListaPow[i]->VWidocznosc()>1) //Linki na razie widoczne/niewidoczne
				ListaPow[i]->Narysuj();
	  }

	//REJESTRACJA W�Z��W
	if(!StrukturaSwiataNieZmieniona)//Przygotowanie posortowanej listy jak si� co� zmieni�o
	{
		if(Swiat::IleWezlow()>0)
			ListaWez.alloc(Swiat::IleWezlow());
		IleWezFakt=0;
		//Przepisywanie na tymczasow� list�
		for(unsigned i=0;i<Swiat::IleWezlow();i++)
		{
			WezelSieci* Pom=Swiat::Wezel(i);
			if(Pom!=NULL) //Mo�e by� pusty slot
				ListaWez[IleWezFakt++]=Pom;
		}
		//Posortowanie wskaznikow wg. funkcji ZOrder() !!!
		if(IleWezFakt>0)
			qsort(ListaWez.get_ptr_val(),IleWezFakt,sizeof(WezelSieci*),sort_function_wezly);
	}
	//Teraz ju� na pewno struktury danych pomocniczych s� aktualne
	StrukturaSwiataNieZmieniona=true;

	//Rysowanie wezlow z listy i ich proces�w
	for(unsigned i=0;i<IleWezFakt;i++)
	{
		if(WyswietlajWezly)
		{
		  if(ListaWez[i]->VWidocznosc()>1) //Wezly na razie tylko widoczne/niewidoczne
			 ListaWez[i]->Narysuj();
		}

		if(WyswietlajProcesy)
		{
			unsigned prawindeks=ListaWez[i]->MojID();
			unsigned ileproc=Swiat::IleProcesow(prawindeks);

			for(unsigned p=0;p<ileproc;p++)
			{
				Proces* Pr=Swiat::Proc(p,prawindeks);
				if(Pr)
				{
					if(Pr->VWidocznosc()>1) //Procesy te� na razie tylko widoczne/niewidoczne
							Pr->Narysuj();
				}
			}
		}
	}

	//Rysowanie komunikat�w - bez listy. Za du�o ich i za cz�sto si� zmieniaj�
	if(WyswietlajInformacje)
	 for(unsigned i=0;i<Swiat::IleInformacji();i++)
	 {
		Komunikat* Inf=Swiat::Info(i);
		if(Inf!=NULL)
		{
			switch(Inf->VWidocznosc())
			{
			 case 3: Inf->Narysuj(); break; //Wirtualna, z�o�ona wizualizacja
			 case 2: //Prosta wizualizacja - kropka!!!
			 case 1: _wizualizuj_komunikat_kropka(Inf);
					break;
			 case 0:
			 default: ;
					break;
			}
		}
	 }

	//OPCJONALNE WYPISYWANIE NAZW W�Z��W
	int	 transp=print_transparently(1);	// Wlacza drukowanie tekstu bez zamazywania t�a. Zwraca stan poprzedni
	for(unsigned i=0;i<IleWezFakt;i++)
	{
	 //ElementModelu::WirtualnyKonstruktor* Kons=ListaWez[i]->VKonstruktor();
	 //if(Kons!=NULL && OpisWezlow < Kons->IleNazw())
	 if(OpisWezlow < ListaWez[i]->IlePolDanych() )
	 {
		const char* Nazwa=ListaWez[i]->WartoscPola(OpisWezlow);
		int lenght=string_width(Nazwa);
		int height=char_height('Z');
		unsigned color=RANDOM(256);
		printc(SkalaX*ListaWez[i]->X()-lenght/2,
			   SkalaY*ListaWez[i]->Y()-height-1,// /2 ?
			   color,0,"%s",Nazwa);
	 }
	}
	print_transparently(transp); //Przywr�cenie stanu podstawowego

	return true;//Udalo si� jak zwykle :-)
}



void GenerycznyWezelSieci::Narysuj()
{
	 if(this->Ksztalt!=NULL)
		Ksztalt->Rysuj(SkalaX*_X,SkalaY*_Y,Col.R,Col.G,Col.B); //Kszta�t w kolorach
				//Waga w�z�a zaszyta jest w jego wielko�ci...
}

void WezelMacierzowy::Narysuj()
{
	 double StartX,StartY,Step;
	 double R=this->R(0);
	 if(Szerokosc>=Wysokosc)
	 {
		Step=(2*R)/Szerokosc;
	 }
	 else
	 {
		 Step=(2*R)/Wysokosc;
	 }
	 StartX=X()-0.5*Step*(Szerokosc-1);
	 StartY=Y()-0.5*Step*(Wysokosc-1);
	 for(unsigned i=0;i<Szerokosc;i++)
	 {
		 for(unsigned j=0;j<Wysokosc;j++)
		 {
			 if(Ksztalt!=NULL && Tablica!=NULL)
			 {
				unsigned index=j*Szerokosc+i;
				Ksztalt->Rysuj((StartX+i*Step)*SkalaX,
							   (StartY+j*Step)*SkalaY,
							   Tablica[index].R,
							   Tablica[index].G,
							   Tablica[index].B);
			 }
			 else
			 plot_rgb((StartX+i*Step)*SkalaX,
					  (StartY+j*Step)*SkalaY,
					  Col.R,Col.G,Col.B);  //Awaryjnie

		 }
     }
}

void GenerycznePowiazanie::Narysuj()
{
	if(_S==Swiat::INV_INDEX || _T == Swiat::INV_INDEX)
			cerr<<endl<<"Niezainicjowany link"<<endl;
	WezelSieci* Start=Swiat::Wezel(_S);
	if(Start==NULL)//Ups
			cerr<<endl<<"Link startuje z pustki"<<endl;
	WezelSieci* Koniec=Swiat::Wezel(_T);
	if(Koniec==NULL)//Ups
			cerr<<endl<<"Link celuje w pustke"<<endl;

	//Grubo�c zale�y od wagi i domyslnego R. Ale tu branego "przez p�"
	//oraz od parametr�w wizualizacji linku
	//float GenerycznePowiazanie::MINIMALNA_GRUBOSC_LINKU=0.01;
	//float GenerycznePowiazanie::MAKSYMALNA_GRUBOSC_LINKU=1;
	int grubosc=JakieRwProcWidth/100.0*MINIMALNA_GRUBOSC_LINKU
				+((JakieRwProcWidth/100.0*MAKSYMALNA_GRUBOSC_LINKU)*ver*Waga);

	unsigned R,G,B;
	if(Col.ARGB!=0) //Waga i kolor maj� znaczenie
	{                         assert(NumerKoloruTla>255); //Tylko szaro�ci!
		double tlo=(NumerKoloruTla-255)*(1-Waga);
		R=Col.R*Waga+tlo; //Srednie wa�one z tlem daj� kolory
		G=Col.G*Waga+tlo; //coraz bardziej zbli�one do szarego t�a
		B=Col.B*Waga+tlo; //im mniejsza waga
		set_pen_rgb(R,G,B,grubosc,SSH_LINE_SOLID);  //Free_style_pen
	}
	else
	{
		unsigned intens=255-unsigned(Waga*255)%256; //Odcien szaro�ci zale�y od wagi
		set_pen_rgb(intens,intens,intens,grubosc,SSH_LINE_SOLID);  //Free_style_pen
		//if(this->Kierunkowy())    Nie wiem czy to nie jest kosztowniejsze ni� niepotrzebne przypisanie
			R=G=B=intens;
	}

	//Samo rysowanie   SkalaX*_X,SkalaY*_Y,
	line_d(SkalaX*Start->X(),SkalaY*Start->Y(),SkalaX*Koniec->X(),SkalaY*Koniec->Y());

	if(this->Kierunkowy()) //Jak kierunkowy to trzeba doda� strza�k�
	{
	   double dX=SkalaX*Koniec->X()-SkalaX*Start->X();
	   double dY=SkalaY*Koniec->Y()-SkalaY*Start->Y();
				if(dY==0 && dX==0) //Zlosliwa sytuacja
				{
					clog<<"?"; goto ERROR;
				}
	   double Rad=atan2(dY,dX)+M_PI/2.0;  //Te� nie dzia�a bez korekty
	   Wielobok Arr=Wielobok::Namiot();//	(3,grubosc*1.5);
	   Arr.Centruj();
	   double MaX,MiX,MaY,MiY,cR,Skala;
	   Arr.Zakresy(MiX,MiY,MaX,MaY,cR);
	   Skala=(grubosc*1.66)/cR;
	   Arr.Skaluj(Skala,Skala);
        Arr.ObracajORad(Rad);
	   Arr.Rysuj((SkalaX*Koniec->X()+SkalaX*Start->X())/2.0,
				 (SkalaY*Koniec->Y()+SkalaY*Start->Y())/2.0,
				 R*0.75,G*0.75,B*0.75);
	}

	return;
	ERROR:
	{
		WezelSieci* W1=Swiat::Wezel(this->Poczatek());
		WezelSieci* W2=Swiat::Wezel(this->Koniec());
		clog<<"\nWadliwe powiazanie proste miedzy  ";
		clog<<W1->Nazwa()<<" a "<<W2->Nazwa()<<endl;
	}
}

void PowiazaniePaboliczne::Narysuj()
{
	if(_S==Swiat::INV_INDEX || _T == Swiat::INV_INDEX)
			{ cerr<<endl<<"Niezainicjowany link"<<endl;  return;}
	WezelSieci* Start=Swiat::Wezel(_S);
	if(Start==NULL)//Ups
			{ cerr<<endl<<"Link startuje z pustki"<<endl; exit(-5);}
	WezelSieci* Koniec=Swiat::Wezel(_T);
	if(Koniec==NULL)//Ups
			{ cerr<<endl<<"Link celuje w pustke"<<endl; exit(-6);}

	//set_pen(256+RANDOM(255),1,SSH_LINE_DOTTED);
	//line_d(SkalaX*Xa,SkalaY*Ya,SkalaX*Xb,SkalaY*Yb);//Debug
	//Grubo�c zale�y od wagi i domyslnego R. Ale tu branego "przez p�"
	//int grubosc=1+((JakieRwProcWidth/200)*ver*Waga);

	//Grubo�c zale�y od wagi i domyslnego R. Ale tu branego "przez p�"
	//oraz od parametr�w wizualizacji linku
	//float GenerycznePowiazanie::MINIMALNA_GRUBOSC_LINKU=0.01;
	//float GenerycznePowiazanie::MAKSYMALNA_GRUBOSC_LINKU=1;
	int grubosc=JakieRwProcWidth/100.0*MINIMALNA_GRUBOSC_LINKU
				+((JakieRwProcWidth/100.0*MAKSYMALNA_GRUBOSC_LINKU)*ver*Waga);

	unsigned R,G,B;
	if(Col.ARGB!=0) //Waga i kolor maj� znaczenie
	{                         assert(NumerKoloruTla>255); //Tylko szaro�ci!
		double tlo=(NumerKoloruTla-255)*(1-Waga);
		R=Col.R*Waga+tlo; //Srednie wa�one z tlem daj� kolory
		G=Col.G*Waga+tlo; //coraz bardziej zbli�one do szarego t�a
		B=Col.B*Waga+tlo; //im mniejsza waga
		set_pen_rgb(R,G,B,grubosc,SSH_LINE_SOLID);  //Free_style_pen
	}
	else
	{
		unsigned intens=255-unsigned(Waga*255)%256; //Odcien szaro�ci zale�y od wagi
		set_pen_rgb(intens,intens,intens,grubosc,SSH_LINE_SOLID);  //Free_style_pen
	}

	double Xp,Xk,Yp,Yk;
	PodajPozycje(0,true,Xp,Yp,NULL); //W razie potrzeby liczy pomocnicze zmienne
	if(krokow==0)
		{ GenerycznePowiazanie::Narysuj(); return;} //Awaryjnie

	double Krok=1.0/krokow;
	for(unsigned i=1;i<=krokow;i++)
	{
	   PodajPozycje(i*Krok,true,Xk,Yk,NULL);
	   line_d(SkalaX*Xp,SkalaY*Yp,SkalaX*Xk,SkalaY*Yk);
	   Xp=Xk;Yp=Yk;
	}

	if(this->Kierunkowy()) //Jak kierunkowy to trzeba doda� strza�k�
	{
	   PodajPozycje(0.5,true,Xp,Yp,NULL); //Polozenie srodka strzalki
	   PodajPozycje(0.66,true,Xk,Yk,NULL);//Do obliczenia kierunku
	   double dX=SkalaX*Xk-SkalaX*Xp;//SkalaX*Koniec->X()-SkalaX*Start->X();
	   double dY=SkalaY*Yk-SkalaY*Yp;//SkalaY*Koniec->Y()-SkalaY*Start->Y();
				if(dY==0 && dX==0) //Zlosliwa sytuacja
				{
					clog<<"?"; goto ERROR;
				}
	   double Rad=atan2(dY,dX)+M_PI/2.0;  //Nie dzia�a bez korekty
	   Wielobok Arr=Wielobok::Namiot();//	(3,grubosc*1.5);
	   Arr.Centruj();
	   double MaX,MiX,MaY,MiY,cR,Skala;
	   Arr.Zakresy(MiX,MiY,MaX,MaY,cR);
	   Skala=(grubosc*2)/cR;  //Takie s� zwykle cie�sze?
	   Arr.Skaluj(Skala,Skala);
        Arr.ObracajORad(Rad);
	   Arr.Rysuj(SkalaX*Xp,
				 SkalaY*Yp,
				 R*0.85,G*0.85,B*0.85);
	}

	return;
	ERROR:
	{
		WezelSieci* W1=Swiat::Wezel(this->Poczatek());
		WezelSieci* W2=Swiat::Wezel(this->Koniec());
		clog<<"\nWadliwe powiazanie paraboliczne miedzy ";
		clog<<W1->Nazwa()<<" a "<<W2->Nazwa()<<endl;
	}
}

void GeneryczneInfo::Narysuj()
{
	Powiazanie* Lacze;
	if(Link==Swiat::INV_INDEX)
		return; //Nierysowalny
	if((Lacze=Swiat::Lacze(Link))==NULL) return; //Te� nierysowalny
	if((Swiat::Wezel(Lacze->Poczatek()))==NULL) return; //Ten te� nierysowalny
	if((Swiat::Wezel(Lacze->Koniec()))==NULL) return; //i ten nierysowalny

	//Ustalanie kolorowania prostego lub alternatywnie:
	unsigned R=Col.R; //Srednie wa�one z tlem daj� kolory
	unsigned G=Col.G; //coraz bardziej zbli�one do szarego t�a
	unsigned B=Col.B; //im mniejsza waga

	//Obliczanie skalowania:  SkalaX*_X,SkalaY*_Y,
	double X,Y;
	Lacze->PodajPozycje(JakDostawa(),Kierunek,X,Y);
	//Grubo�c zale�y od wagi i domyslnego R. Ale tu branego "przez p�"
	double Rad=((JakieRwProcWidth/800)*ver)*min(SkalaX,SkalaY);
	if(Rad<1) Rad=1;
	RysujKsztalt(SkalaX*X,SkalaY*Y,Rad,R,G,B);
}

void GeneryczneInfo::RysujKsztalt(float X,float Y,float Rad,unsigned R,unsigned G,unsigned B)
{
	set_pen_rgb(R/2,G/2,B/2,0,SSH_LINE_SOLID);  //Free_style_pen
	set_brush_rgb(R,G,B);
	fill_circle_d(X,Y,Rad);
}

void Swiat::Wymazuj()
// Wyczyszczenie calo�ci - mo�e by� "na skr�ty"
{
   fill_rect(0,0,screen_width(),screen_height(),NumerKoloruTla);
}

void GenerycznyWezelSieci::Wymazuj()
{                             assert(NumerKoloruTla>255); //Tylko szaro�ci!
	 if(this->Ksztalt!=NULL)
		Ksztalt->Rysuj(SkalaX*_X,SkalaY*_Y,NumerKoloruTla-255,NumerKoloruTla-255,NumerKoloruTla-255); //Kszta�t jak t�o
				//T�o jest zawsze odcieniem szaro�ci a kolory szare maj� w symshellu indeksy w zakresie [256..511]
}

void WezelMacierzowy::Wymazuj()
{
	//NumerKoloruTla
}

void GenerycznyProces::Wymazuj()
{
	//NumerKoloruTla
	WezelSieci* Mw=Swiat::Wezel(Procesor());
	if(Mw==NULL)
		cerr<<endl<<"Proces na nieistniej�cym wezle"<<endl;
	unsigned ile=Swiat::IleProcesow(Procesor());
	double R=Mw->R(1.5*M_PI);
	double Y=Mw->Y()-0.1*R+this->MojID();//Indeks procesu na w�le
	double X1=Mw->X()-R;
	double X2=X1+2*R;
	set_pen(254/*NumerKoloruTla*/,SkalaY,SSH_LINE_SOLID);  //Kolor t�a
	line_d(SkalaX*X1,SkalaY*Y,SkalaX*X2,SkalaY*Y);
}

void GenerycznePowiazanie::Wymazuj()
{
   //NumerKoloruTla
}

void GeneryczneInfo::Wymazuj()
{
	//NumerKoloruTla
	Powiazanie* Lacze;
	if(Link==Swiat::INV_INDEX)
		return; //Nierysowalny
	if((Lacze=Swiat::Lacze(Link))==NULL) return; //Te� nierysowalny
	if((Swiat::Wezel(Lacze->Poczatek()))==NULL) return; //Ten te� nierysowalny
	if((Swiat::Wezel(Lacze->Koniec()))==NULL) return; //i ten nierysowalny

	//Kolorowanie
	set_pen_rgb(NumerKoloruTla-255,NumerKoloruTla-255,NumerKoloruTla-255,0,SSH_LINE_SOLID);  //Free_style_pen
	set_brush(NumerKoloruTla);

	//Rysowanie  SkalaX*_X,SkalaY*_Y,
	double X,Y;
	Lacze->PodajPozycje(JakDostawa(),Kierunek,X,Y);
	//Grubo�c zale�y od wagi i domyslnego R. Ale tu branego "przez p�"
	double Rad=((JakieRwProcWidth/800)*ver)*min(SkalaX,SkalaY);
	if(Rad<1) Rad=1;
	fill_circle_d(SkalaX*X,SkalaY*Y,Rad);
}


void PowiazaniePaboliczne::Wymazuj()
{
   //NumerKoloruTla
}

/// Na zakonczenie kroku trzeba odnowi� graficzny wiersz statusu
bool Swiat::Status_po_kroku()
{
	std::ostringstream Bufor;
	long IleMozna=IleMoznaInformacji();
	long IleJest=IleInformacji();

	Bufor<<Swiat::NumerKroku()<<_LPL(". krok","-th step")
         <<" "<<IleMozna<<_LPL(" Inform."," Messg.")<<"         "<<'\0';
	printc(0,screen_height()-char_height('X'),128,255+128,"%s", Bufor.str().c_str() );
	return true;
}

void _wizualizuj_komunikat_kropka(Komunikat* Inf)
//Pomocnicza - uproszczona wizualizacja komunikatu
{
	unsigned Kanal=Inf->Kanal();
	Powiazanie* Link=Swiat::Lacze(Kanal);
	if(Link==NULL) return; //co� nie wysz�o
	double X,Y;
	Link->PodajPozycje(Inf->JakDostawa(),Inf->KierunekZgodny(),X,Y,Inf);
	DziedzinaWKolorze Dz=Inf->PodajDziedzine();
	plot_rgb(X*SkalaX,Y*SkalaY,Dz.R,Dz.G,Dz.B);
}

bool testy_graficzne()
{
//Proste testy
 Wielobok Ostrze(Wielobok::Namiot());
 Wielobok Test(8,50);

 Test.Rysuj(hor/2,ver/2,128);
 Ostrze.Centruj();
 //Ostrze.Skaluj(0.2,0.4);
    Ostrze.ObracajORad(M_PI / 2);
 Ostrze.Rysuj(hor/4,ver/2,32,128,32);

 return true;
}


void GenerycznyProces::Narysuj()
{
	WezelSieci* Mw=Swiat::Wezel(Procesor());
	if(Mw==NULL)
		cerr<<endl<<"Proces na nieistniej�cym wezle"<<endl;
	unsigned ile=Swiat::IleProcesow(Procesor());

	double R=Mw->R(1.5*M_PI);
	double Y=Mw->Y()-0.1*R+this->MojID();//Indeks procesu na w�le
	double X1=Mw->X()-R;
	double X2=X1+2*R;

	double Zaaw=this->JakZaawansowany();

	//Pasek podkre�lenia projektu bliskiego zakonczenia
	if(Zaaw>0.99)
	{
		set_pen_rgb(255,200,0,SkalaY,SSH_LINE_SOLID);  //Kolor przerobionego
		line_d(SkalaX*X1,SkalaY*Y+1,SkalaX*X2,SkalaY*Y+1);//Dodatkowe podkre�lenie
	}

	//DziedzKol Pom=Mw->PodajDziedzine();//Pom.R<<=1;Pom.G<<=1;Pom.B<<=1;
	//set_pen_rgb(Pom.R,Pom.G,Pom.B,SkalaY-1,SSH_LINE_SOLID);  //Kolor post�pu na w�z�a

	//Pasek t�a
	set_pen_rgb(Col.R/2,Col.G/2,Col.B/2,SkalaY,SSH_LINE_SOLID);  //Przyciemniony kolor projektu
	line_d(SkalaX*X1,SkalaY*Y,SkalaX*X2,SkalaY*Y);

	//Pasek zaawansowania
	X2=X1+2*R*Zaaw;
	set_pen_rgb(Col.R,Col.G,Col.B,SkalaY-1,SSH_LINE_SOLID);  //Normalny kolor projektu
	line_d(SkalaX*X1,SkalaY*Y,SkalaX*X2,SkalaY*Y);
}

//Bardzo intensywnie uzywane  - Mo�e inliny kiedy� zrobie?)
double Swiat::SkalujX() { return SkalaX; }
double Swiat::SkalujY() { return SkalaY; }

/********************************************************************/
/*			          SPS  version 2011                             */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://wwww.iss.uw.edu.pl/borkowski/                */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/

