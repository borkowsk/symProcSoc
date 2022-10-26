// Transfer technologii 2011 - w³aœciwa implementacja modelu     24-10-2011
////////////////////////////////////////////////////////////////////////////////
// Implementacja akcji klas w³aœciwego modelu
// Rynek, firma, kooperacja, produkty i pieni¹dze
//(bez technikaliów obslugi interfaceu )
////////////////////////////////////////////////////////////////////////////////

#include "opiKlasyWersja.hpp"
#include "INCLUDE/wb_smartlog.h"
using namespace wbrtm;

char* const  KONTO="Ruch konta"; //Marker komunikatu finansowego z przep³ywem
float  JEDNOSTKOWA_WYPLATA=100;  //Wartoœæ najmniejszej wp³aty/wyplaty. Sposób u¿ycia niejasny :-)
static const int LOCLOG=2; //Na jakim poziomie logowania domyœlnie zapisywaæ problemy z tego pliku

//Klasa wewnêtrzna skupiaj¹ca wspólne w³aœciwoœci wêz³ów z pracownikami (zespo³ów)
//////////////////////////////////////////////////////////////////////////////////
/*
class _ZespolRoboczy:public GenerycznyWezelSieci
{
 protected:
 friend class  ProcesProdukcyjny;
 friend class  ProcesRaportowany;
 friend class  ProcesSpoleczny;
//Ile Etatów	Efektywnosc	Aktywnoœæ	Centralnoœæ	Probiznesowosc	Prolegalnosc	Czasochlonnosc	Innowacyjnoœc	Finans. swobodne	Finanse zaplan	D³ug do nadrz.	Udzial w zysku TT	Eksperckoœæ w dziedzinie	Uwagi
 unsigned IleEtatow;//Ile etatów. Co najmniej 1, 0 - "duch" zespo³u - plan utworzenia
 float    Efektywnosc;//0..2. Czy dzia³a na 50% wydajnoœci czy na 150%. Zale¿y od motywacji
 float    Proaktywnosc;//Aktywnoœæ? 0..1. Podtrzymywanie aktywnoœci zewnêtrznej, tworzenie linków
 float    Centralnoœæ;//0..1. Jak bardzo aktywnoœæ (zewnêtrzna?) zale¿y od szefa/szefów
 float    Doswiadczenie;//POZIOM EKSPERTYZY? Jak tego u¿ywaæ? Mo¿e to samo co efektywnoœæ???
 float    Czasochlonnosc;//Jaki jest wspó³czynnik czasoch³onnoœci typowej dzia³alnoœci. Wygenerowania pojedynczego wyniku badañ?
 float    Prolegalnosc;//????Uczciwoœæ??? Jak ³atwo podj¹æ dzia³ania "szarostrefowe"
};
*/

PaczkaProduktow::PaczkaProduktow(const char* Nazwa,unsigned IleWPaczce,float CenaZaSztuke,unsigned KtoProducentem)
{
	Dane.Realokuj(12); Dane[9]=Nazwa;
	Cena=CenaZaSztuke;
	Ile=IleWPaczce;
	this->Producent=KtoProducentem;
}

PaczkaProduktow::PaczkaProduktow(const PaczkaProduktow* Wzor) //KONSTRUKTOR KOPIUJ¥CY
			:GeneryczneInfo(Wzor)
{
	Cena=Wzor->Cena;
	Ile=Wzor->Ile;
	Producent=Wzor->Producent;
}

const string& PaczkaProduktow::Rodzaj()
//Tu mo¿e byæ mark¹ czy nazw¹ handlow¹ - bo mo¿e byæ przesylany dalej!
{
	return Dane[9];
}

bool  PaczkaProduktow::UstawRodzaj(const char* Nowy)
//A jak typ nie udostêpnia zmiany to zwraca false
{
	Dane[9]=Nowy;
	return true;
}

KomunikacjaFinansowa::KomunikacjaFinansowa(const char* Kategoria,float JakaKwota)
//Konstruktor domyslny dla symulacyjnego tworzenia komunikatu finansowego
{
	Dane.Realokuj(12); Dane[9]=Kategoria; Kwota=JakaKwota;
}

const string& KomunikacjaFinansowa::Rodzaj()//Gdzieœ w danych jest rodzaj tego komunikatu
{
	return Dane[9];
}

bool  KomunikacjaFinansowa::UstawRodzaj(const char* Nowy)
//A jak typ nie udostêpnia zmiany to zwraca false
{
	Dane[9]=Nowy;
	return true;
}


ProcesProdukcyjny:://ProcesProdukcyjny(unsigned IleSztuk,float JakaCena,unsigned IleWPaczce) // domyslnie JakaCena=1 , IleWPaczce=-1
ProcesProdukcyjny(const char* Nazwa, unsigned IleSztuk,float JakaCena,unsigned IleWPaczce):
			GenerycznyProces(Nazwa,365*10+DRAND()*10,365)
//Konstruktor z parametrami do tworzenia procesu w kodzie
{
	Zapotrzebowanie=IleSztuk; Cena=JakaCena; Wyslano=0;
	if(IleWPaczce!=-1)  WPaczke=IleWPaczce;
		   else 		WPaczke=IleSztuk/10; //Dzieli na 10 pak jak -1
}

ProcesProdukcyjny::ProcesProdukcyjny(const ProcesProdukcyjny* Wzor):
				GenerycznyProces(Wzor)
{
	Zapotrzebowanie=Wzor->Zapotrzebowanie;
	WPaczke=Wzor->WPaczke;
	Cena=Wzor->Cena;
	Wyslano=0;
}

ProcesProdukcyjny::ProcesProdukcyjny()
{
	Zapotrzebowanie=100;
	WPaczke=10;
	Cena=10;
	Wyslano=0;
}
	   //	unsigned Blad=0; //Dziedzina ju¿ jest taka jak macierzystego procesu
	   //	bool Result=Nowy->ZrobWgListy(&Dane[0],Dane.Ile(),Blad); //chodzi tez o nadanie wartoœci jak w macierzystym procesie
	   //																			assert(Result && Blad==8);//By³o poprawne kiedyœ, musi byæ i teraz


// WLASCIWA IMPLEMENTACJA TEGO CO ROBI¥ KLASY
////////////////////////////////////////////////////////////////////////////////

void ProcesProdukcyjny::ChwilaDlaCiebie()
{
	if(!this->Poprawny() || this->JakZaawansowany()>=PROCES_NAD_ZAAWANSOWANY)
					return; //Bo mo¿e ju¿ zakoñczony!!! A to by mno¿y³ procesy
						//Sheduler nie powinien do tego dopuœciæ, ale...

	GenerycznyProces::ChwilaDlaCiebie(); //Popchniête liczniki

	double Zaaw=JakZaawansowany(); //Musi wiedzieæ jak jest teraz zaawansowany

	if(0.985<Zaaw) //Finalizowanie procesu - wysy³anie produktu na rynek
		_WyslijProduktyNaRynek();

	if(Zaaw>=1) //Jest ju¿ zakonczony - samowzbudza reprodukcjê
	{
		WezelSieci* MojProcesor=Swiat::Wezel(this->Procesor());

		if(DoKonca()<-7)//SpóŸni³ siê wiêcej ni¿ tydzieñ  - KOMUNIKAT
		{
		   IFTL(LOCLOG+1)	clog<<endl<<"\""<<MojProcesor->Nazwa()<<"\" "<<_LPL("z poslizgiem zakonczyl proces","fin.proc.")<<' '<<Dane[1]<<':'<<hex<<PodajDziedzine().ARGB<<" ("<<-DoKonca()<<_LPL(" krokow po terminie"," steps after deadline")<<")"<<endl;
		   TLOG(LOCLOG, <<"\""<<MojProcesor->Nazwa()<<"\" "<<_LPL("z poslizgiem zakonczyl proces","fin.proc.")<<'\t'<<Dane[1]<<":\t"<<hex<<PodajDziedzine().ARGB<<"\t ("<<-DoKonca()<<_LPL(" krokow po terminie"," steps after deadline")<<")" )
		}

		//Tworzymy nowy proces produkcji bêd¹cy niemal kopi¹, tylko mo¿e trochê mniej czasoch³onny i kosztowny
		//==========================================================================================================
		wb_ptr<ProcesProdukcyjny> Nowy( new ProcesProdukcyjny(this) );	//(IleSztuk,JakaCena,IleWPaczce);//Nowa wersja, ale parametry siê i tak zama¿¹ zaraz

		//Ale niektóre trzeba zmieniæ, ¿eby by³ postêp
		double Zaangazowanie=JakPracochlonny()*0.999;//IleEtatow/ile_proc? //  Czasochlonnosc ukryta w pierwotnym procesie
		double IleCzasu=365;//Domyœlny czas przygotowania produktu - raczej za du¿y
		Nowy->WPaczke=WPaczke; //Tego akurat nie, ale wartoœæ domyœlna nie musi byæ poprawna
		Nowy->Cena=_ZespolRoboczy::_KalkulujKosztProduktu(this->PodajDziedzine())*1.20; //Nowa kalkulacja ceny z 20% zyskiem
		if(Nowy->Cena>=Cena)//Starszy produkt mo¿na i trzeba by sprzedawaæ taniej
			Nowy->Cena*=0.99;
		Nowy->Zapotrzebowanie=Zapotrzebowanie*1.1; //Jakaœ szansa na wzrost, bo wracaj¹ce paczki zawsze obni¿aj¹ zapotrzebowanie
		if(Nowy->Zapotrzebowanie<WPaczke) //Ju¿ siê bardzo s³abo sprzedaje
		{
			if(DRAND()<0.5)
				Nowy->Zapotrzebowanie=WPaczke; //Produkt kolekcjonerski?
				else
				{//A mo¿e ca³kiem zrezygnowaæ? Mo¿na jeœli s¹ jakieœ inne procesy produkcji na tym wêŸle (Firmie)
					IFTL(LOCLOG) clog<<endl<<"\""<<MojProcesor->Nazwa()<<"\" "<<_LPL("PORZUCIL proces","abandon the proc.")<<' '<<Nowy->Dane[1]<<':'<<hex<<Nowy->PodajDziedzine().ARGB<<dec<<endl;
					TLOG(LOCLOG, <<"\""<<MojProcesor->Nazwa()<<"\" "<<_LPL("PORZUCIL proces","abandon the proc.")<<'\t'<<Nowy->Dane[1]<<":\t"<<hex<<Nowy->PodajDziedzine().ARGB<<dec )
					Wymazuj();  //Informacja na wyœwietlaniu
					UznajZaZakonczony();//Jawnie nie usuwamy bo to grozi katastrof¹ gdy¿ wywo³uje "delete"
				}
		}

		//Trzeba sprawdziæ by wydolnoœæ firmy i z tego policzyæ czas
		_ZespolRoboczy* ProcesorZR=dynamic_cast<_ZespolRoboczy*>(MojProcesor);
		if(ProcesorZR) //Uda³o siê sprowadziæ do tego "mianownika"
		{
			//Mo¿na lepiej zaplanowaæ czas wykonania nowej wersji procesu //=(Zaangazowanie/IleEtatow)*ile_proc*1.1;//10% rezerwy
			unsigned ile_proc=Swiat::IleProcesow(Procesor()); //S¹ tam te¿ puste!
			IleCzasu=(Zaangazowanie/ProcesorZR->IleEtatow)*ile_proc*1.1;//10% rezerwy
			ProcesorZR->Doswiadczenie*=1.01;//Wzrost doœwiadczenia
			if(ProcesorZR->Doswiadczenie>1) ProcesorZR->Doswiadczenie=1; //Ale w tym modelu nie wiêcej ni¿ do 1

			Nowy->WymaganaIloscPracy=Zaangazowanie;
			Nowy->_IleKrokowDoKonca=IleCzasu;
			Nowy->TmpPorcjaPracy=1;
			Nowy->PracaDotychczasowa=0;

			IFTL(LOCLOG+1) clog<<endl<<"\""<<MojProcesor->Nazwa()<<"\" "<<_LPL("ustalil nowy proces","set new proc.")<<' '<<Nowy->Dane[1]<<':'<<hex<<Nowy->PodajDziedzine().ARGB<<" Zapotrz.:"<<dec<<Nowy->Zapotrzebowanie<<" Zaang.:"<<Zaangazowanie<<endl;
			TLOG(LOCLOG, <<"\""<<MojProcesor->Nazwa()<<"\" "<<_LPL("ustalil nowy proces","set new proc.")<<'\t'<<Nowy->Dane[1]<<":\t"<<hex<<Nowy->PodajDziedzine().ARGB<<"\tZapotrz.:\t"<<dec<<Nowy->Zapotrzebowanie<<"\tZaang.:\t"<<Zaangazowanie )

			if(Swiat::WstawProc(Nowy.give(),Procesor())!=Swiat::INVINDEX) //I procesor te¿...
							;//Uda³o sie
				else
				TLOG(LOCLOG, <<"Nie udane odpalenie procesu na "<<"\""<<MojProcesor->Nazwa()<<"\" " )
		}
		else
		{
			//Jakaœ wtopa - PROCES PRODUKCJI na NIEW£AŒCIWYM TYPE WÊZ£A  - nie ponawiamy!!!
			cerr<<endl<<"PROCES PRODUKCJI na NIEWLASCIWYM TYPE WEZ£A "<<"\""<<MojProcesor->Nazwa()<<"\" TYP: "<<MojProcesor->WartoscPola(0)<<endl;
			TLOG(0, <<"PROCES PRODUKCJI na NIEW£AŒCIWYM TYPE WÊZ£A "<<"\t\""<<MojProcesor->Nazwa()<<"\"\tTYP:\t"<<MojProcesor->WartoscPola(0)  )
		}

		//Teraz uznajemy aktualny za gotowy do usuniecia
		Wymazuj();  //Informacja na wyœwietlaniu
		UznajZaZakonczony();//Jawnie nie usuwamy bo to grozi katastrof¹ gdy¿ wywo³uje "delete"

	}
}

bool ProcesProdukcyjny::InterpretujKomunikat(Komunikat * Co)
//Gdy paczki wracaj¹ mo¿e je odes³ac ze zmniejszon¹ cen¹
//a jak to nie pomo¿e to zmniejszyæ "Zapotrzebowanie" na przysz³oœæ
{
	if(Co->PodajDziedzine()==PodajDziedzine()) //Interesuj¹ go komunikatyz jego dziedziny
	{
	  PaczkaProduktow* Zwrot=dynamic_cast<PaczkaProduktow*>(Co);
	  if(Zwrot!=NULL) //I na razie jedynie wtedy gdy to zwrot jego produktu
	  {
		float WJakiejBylCenie=Zwrot->CenaZaSzt();
		if(WJakiejBylCenie<Cena/3) //Jeœli nie uda³o siê sprzedaæ za 1/3 aktualnej ceny to rezygnujemy
		{
			Zapotrzebowanie-=Zwrot->IleSztuk();  //Zmniejszamy trochê zapotrzebowanie (o 1 czy o wielkoœæ paczki?)
			if(Zapotrzebowanie<=0)
								Zapotrzebowanie=1; //Bo nie wiem co siê stanie w procesie jak to spadnie do 0!!!
			return true; //Ale komunikat obsluzony
		}
		//Mo¿na jeszcze spróbowaæ sprzedaæ taniej w mniejszych partiach
		Zwrot->Cena*=0.75;//Za 3/4 poprzedniej ceny
		Zwrot->Ile/=2;//I dwie mniejsze paczki
		if(Zwrot->Ile<1)
			 Zwrot->Ile=1; //Nie da³o siê podzieliæ zawartoœci
		Komunikat* PaP=Zwrot->Klonuj(); //Dziedzina z klonowania  paczki
		PaP->Zwrotnie(0.012);//Powinno zadzia³aæ
		if(Swiat::WstawInfo(PaP)!=Swiat::INVINDEX && Zwrot->Ile>1) //Jak zosta³a druga porcja
		{
		  PaP=Zwrot->Klonuj(); //Dziedzina z klonowania  paczki
		  PaP->Zwrotnie(0.015);
		  Swiat::WstawInfo(PaP);//Ale juz nie sprawdzamy (dla skrócenia kodu)
		}
		return true;   // komunikat obsluzony
	  }
	}

	return false;
}

bool ProcesProdukcyjny::_WyslijProduktyNaRynek()
//Pomocnicza procedura wysy³ania na rynek przy koñcu procesu produkcji
{
	if(Wyslano>=Zapotrzebowanie)
						return true; //Ju¿ wszystko wys³ano!

	unsigned NID=Procesor();//Nadawc¹ jest wêze³ "procesor" tego procesu
	WezelSieci* We=Swiat::Wezel(NID);

	unsigned ile_wyslac=this->WPaczke;
	double Zaaw=JakZaawansowany(); //Musi wiedzieæ jak jest teraz zaawansowany
	if(Zaaw>=1)//Chyba ostatnia szansa - ca³a reszta
			ile_wyslac=Zapotrzebowanie-Wyslano;
	Wyslano+=ile_wyslac;

	//Produkcja kosztuje. Odliczamy to w tym mniejscu
	_ZespolRoboczy* ProcesorZR=dynamic_cast<_ZespolRoboczy*>(We);
	if( ProcesorZR!=NULL ) //O ile wêzel bierze pod uwagê koszty
	{
		float koszt=_ZespolRoboczy::_KalkulujKosztProduktu(this->PodajDziedzine())*ile_wyslac;
		if(!ProcesorZR->StacGoNaWydatek(koszt))
						return false;//Przerywamy
		ProcesorZR->PrzyjmijKase( - koszt);//Tu to proces przyjmuje kasê a nie wêze³
	}

	//Pakujemy paczke
	wb_ptr<PaczkaProduktow> PaP( new PaczkaProduktow(We->Nazwa(),ile_wyslac,Cena,NID) );
	PaP->UstawDziedzine(this->PodajDziedzine()); //Zawartosc z dziedziny konkretnego procesu produkcji

	//Znajdujemy wlaœciwego odbiorce
	std::string NazwaOdbiorcy;
	if(IlePolDanych()<9 || strlen(WartoscPola(8))==0)
		NazwaOdbiorcy="Market";
		else
		NazwaOdbiorcy=WartoscPola(8);

	unsigned Odbiorca=Swiat::ZnajdzIndeksWezla(NazwaOdbiorcy.c_str());
	if(Odbiorca==Swiat::INVINDEX)
	{
		//Nie ma ju¿ dostêpnego odbiorcy!!!   PaP usunie paczkê na koncu metody
		IFTL(LOCLOG) clog<<endl<<"Proces \""<<Nazwa()<<"\" utracil odbiorce \""<<NazwaOdbiorcy.c_str()<<"\""<<endl;
		TLOG(1,      <<"Proces \""<<Nazwa()<<"\" utraci³ odbiorcê \""<<NazwaOdbiorcy.c_str()<<"\""  )
		if(NazwaOdbiorcy=="Market")//Jak odbiorc¹ by³ ogólny rynek
			 UznajZaZakonczony(); //To proces ju¿ niepotrzebny wiêc do USUNIÊCIA
			 else //W przeciwnym razie mo¿na jeszcze sprobowaæ sprzedawaæ na rynek
			 Dane[8]="Market";
		return false; //Nie uda³o siê...
	}

	if(PaP->Zaadresuj(NID,Odbiorca,0.02) //Jak siê uda poprawnie zaadresowaæ
	   &&  (Swiat::WstawInfo(PaP.give())!=Swiat::INVINDEX) )    //To wysy³amy...
					;//OK
			else //Mo¿e siê np. zdarzyæ gdy system uleg³ zawa³owi komunikatów
			{
			Zapotrzebowanie-=ile_wyslac;//Wiêc trzeba siê stabilizowaæ
			if(Zapotrzebowanie<5) Zapotrzebowanie=5;
			TLOG( 1, <<"Nie uda³o siê wys³aæ paczki do \""<<NazwaOdbiorcy.c_str()<<"\". Mo¿e za du¿o komunikatow?" )
			}

	return true; //Chyba uda³o siê
}

void Firma::InterpretujKomunikat(Komunikat* Co)
//Przyjmowanie komunikatów
{                                                                         assert(Co->Kanal()!=Swiat::INVINDEX);
	KomunikacjaFinansowa* KFI=dynamic_cast<KomunikacjaFinansowa*>(Co);
	if(KFI!=NULL)
	{
		if(KFI->Rodzaj()==KONTO)
		{
		   float wplyw=KFI->JakaKwota(); //Tu mo¿e zwiêkszyæ swój fundusz
		   _RozeslijNaleznosci(KFI->PodajDziedzine(),wplyw); //Ale mo¿e jest winien oplaty licencyjne
		   this->FinanseBiezace+=wplyw; //To co zosta³o mo¿na wsadziæ an w³asne konto
		   if(Dlugi>0)//Jak trzeba jakoœ wyrównaæ d³ug
		   {
				double kwota=0.01*Dlugi; //Nie wiêcej ni¿ 1% na raz ¿eby nie utraciæ p³ynnoœci finansowej
				if(kwota<FinanseBiezace) //I o ile siê w ogóle uda
				{
					Dlugi-=kwota;
					FinanseBiezace-=kwota;
				}
		   }
		}
		else //Jakiœ inny komunikat finansowy
		{
			//MO¯E JAKIŒ MOJ PROCES BEDZIE ZAINTERESOWANY? (na dole generyczne przegladanie)
		}
	}
	else
	if(Co->Rodzaj()=="?SZUKAM.PRODUCENTA")
	{
		DziedzinaWKolorze D=PodajDziedzine(); //Czy siê to miesci w naszej branzy
		D=D.ARGB & Co->PodajDziedzine().ARGB;
		D.A=0;//To nas nie interesuje. Chwilowo?
		if(D.IleBitow()>0            //Jakies wspolne bity
		&& _ZespolRoboczy::_PodobienstwoDziedzin(D,PodajDziedzine())>DRAND())    //I ewentualnie prawdopodobienstwo
		{
			Komunikat* Klon=Co->Klonuj();
			Klon->UstawRodzaj("WYPRODUKUJE!");

			if(  ! Klon->Zwrotnie()  )//Jak towarzyski to musialby istnieæ zwrotny link
			{   //A jak nie to tworzymy towarzyski do jednostki wskazanej w Co
				unsigned IndInny=Co->Nadawca();
				KomunikacjaTowarzyska* KT=dynamic_cast<KomunikacjaTowarzyska*>(Co);
				if(KT) IndInny=KT->OKimTaGadka();

				wb_ptr<KontaktTowarzyski>  Nowy( new KontaktTowarzyski(MojID(),IndInny,WAGA_NA_DZIENDOBRY,DRAND())  );
				Nowy->UstawDziedzine(D);
				unsigned Kanal=Swiat::WstawLacze(Nowy.give());       		assert(Kanal!=Swiat::INVINDEX);
				Klon->Zaadresuj(Kanal,true,0.1);		//Tu siê b³êdu juz nie spodziewam...
			}

			KomunikacjaTowarzyska* KlonT=dynamic_cast<KomunikacjaTowarzyska*>(Klon);
			if(KlonT) KlonT->OKimTaGadka()=MojID();
			if(Swiat::WstawInfo(Klon)==Swiat::INVINDEX)
					TLOG(0,<<"ODPOWIEDZ NIE WYSLANA" )
					else
					return ;  //Czyli obsluzyl?
		}
	}
	else
	{                             										assert(Co->Kanal()!=Swiat::INVINDEX);
	//Obróbka domyœlna
	GenerycznyWezelSieci::InterpretujKomunikat(Co);
	}
}

void Firma::ChwilaDlaCiebie()
{
	if(Doswiadczenie<0.33)
		_ZarzadcaProcesowFantazyjny();//Mocno niedoœwiadczona firma
	else
		_ZarzadcaProcesowFinezyjny();
		//_ZarzadcaProcesowSprawiedliwy(); //le dzia³a :-(

	//Mo¿e przeliczyæ swoj¹ wagê
	//float    FinanseBiezace; //Ile firma ma kasy aktualnie LUB UPROSZCZONA "ZAMO¯NOŒÆ"
	//float    ;//Ile firma ma obiecanych œrodków kredytowych
	//float 	  Dlugi;//Ile firma ma do sp³aty. Im wiêcej tym mniejsza szansa ¿e coœ dostanie nowego
	double bilans=(FinanseBiezace+FinanseZaplanowane)-Dlugi;
	bilans*=this->Doswiadczenie;
	if(bilans>1)
		bilans=log10(bilans);
		else
		bilans=0.9;
	bilans/=3;//Typowe sumy s¹ 10^6 wiêc by³oby za du¿o
	Waga=(0.99*Waga)+(0.01*bilans);  //Wêdrowna œrednia
				//Na razie dzia³a tylko na d³ugoœæ procesów bo rozmiar wielok¹tów zosta³
				//zaplanowany niestety jako sta³y
}

void RynekZbytu::ChwilaDlaCiebie()
//Proces dystrybucji produktów,
//oraz(?) zmiany potrzeb w mechanizmie wp³ywu i epidemi
{
	GenerycznyWezelSieci::ChwilaDlaCiebie();
}

DziedzinaWKolorze RynekZbytu::_DaSieSprzedac(const DziedzinaWKolorze CoZaProdukt)
{
	 DziedzinaWKolorze D;
	 unsigned Indeks; double WzglednePobienstwo;
	 D=_ZnajdzNajpodobniejszy(CoZaProdukt,Indeks,WzglednePobienstwo);//unsigned IleProb)<==oznacza przeszukanie liniowe

	 if( pow(WzglednePobienstwo,3) > DRAND() ) //Musi byæ nieliniow, ¿eby siê naprawdê liczy³o podobienstwo do niszy
		return D;     //Da siê
		else
		{
			if(DRAND()<0.1)//Z rzadka nisza siê blokuje, zwlaszcza gdy produkt nietrafiony
			{
				Tablica[Indeks].A=RANDOM(256); Tablica[Indeks].R=RANDOM(256);
				Tablica[Indeks].G=RANDOM(256); Tablica[Indeks].B=RANDOM(256);
			}
			D.ARGB=0;
			return D;  //Tym razdem nie
		}
}

void RynekZbytu::InterpretujKomunikat(Komunikat* Co)
//Przyjmowanie komunikatów typu PACZKA i zwracanie zap³aty za nie
{
	PaczkaProduktow* Paka=dynamic_cast<PaczkaProduktow*>(Co);
	if(Paka!=NULL)
	{
		if(_DaSieSprzedac(Paka->PodajDziedzine()).ARGB>0 )        //W _DaSie mo¿e byæ losowoœæ. Czêœæ paczek idzie wtedy na przemia³!
		{
	   //Trzeba za to zaplaciæ
		float WartoscZaplaty=Paka->IleSztuk()*Paka->CenaZaSzt();
		KomunikacjaFinansowa* Zaplata=new KomunikacjaFinansowa(KONTO,WartoscZaplaty);
		Zaplata->UstawDziedzine(Paka->PodajDziedzine()); //Za jaki produkt ta faktura

		if(Zaplata->Zaadresuj(Co->Kanal(),!Co->KierunekZgodny(),0.03)
			&& (Swiat::WstawInfo(Zaplata)!=Swiat::INVINDEX) )
				;//OK
				else
				{
				TLOG( 1, <<"Nie uda³o siê wys³aæ zap³aty za paczkê "<<Paka->Rodzaj()<<"do wêz³a "<<Paka->Nadawca()  )
				}
		}
		else //Nie uda³o siê sprzedaæ, zwracamy
		{
			Komunikat* Zwrot=Paka->Klonuj();//Mog³by jakoœ zaznaczaæ ¿e t zwrot?
			if(Zwrot->Zwrotnie()
			 && Swiat::WstawInfo(Zwrot)!=Swiat::INVINDEX)
				;//OK
				else
				TLOG(1, <<"Nie uda³o siê wys³aæ zap³aty za paczkê "<<hex<<Co->PodajDziedzine().ARGB<<" Do "<<Co->Nadawca()  )
		}
	}
	else
	if(Co->Rodzaj()=="TEST")   //Niezale¿nie od typu...
	{
		DziedzinaWKolorze D=Co->PodajDziedzine();
		unsigned Indeks; double WzglednePobienstwo;
		D=_ZnajdzNajpodobniejszy(D,Indeks,WzglednePobienstwo);//unsigned IleProb)<==oznacza przeszukanie liniowe
		if(WzglednePobienstwo>0.75)//Tylko naprawdê podobne i przy uproszczonej metryce nie jednobitowe
		{
			Komunikat* ODP=Co->Klonuj();
			ODP->UstawRodzaj("TEST.OK");
			ODP->UstawDziedzine(D);
			if(ODP->Zwrotnie()
			 && Swiat::WstawInfo(ODP)!=Swiat::INVINDEX)
			 ;
			 else
			 TLOG(1, <<"Nie uda³o siê wys³aæ odpowiedzi na TEST rynkowy D:"<<hex<<Co->PodajDziedzine().ARGB<<" Do "<<Co->Nadawca()  )
		}
	}
	else
	//Inny rodzaj komunikatu lub nie sprzedana paczka
	WezelMacierzowy::InterpretujKomunikat(Co);
}


void Firma::_RozeslijNaleznosci(DziedzinaWKolorze Produkt,float& wplyw)
//Wg. identyfikatora produktu szuka komu winien dywidende
{
   float IleWydane=wplyw;//Wydane
   if(wplyw<1) //No grosze to ju¿ ignorujemy!!!!
			return;
   unsigned ile_linkow=Swiat::IlePowiazan();
   Powiazanie* Link;
   FormalnaKooperacja* FK=NULL;
   for(unsigned i=0;i<ile_linkow;i++)
   if((Link=Swiat::Lacze(i))!=NULL
   &&  Link->PodajDziedzine()==Produkt  //Jest zwi¹zany z tym produktem
   && (Link->Poczatek()==MojID() ) //  || Link->Koniec()==MojID()  ZAPÊTLA!!!
   && (FK=dynamic_cast<FormalnaKooperacja*>(Link))!=NULL
   )
   {
		//Trzeba bêdzie za to zaplaciæ
		float WartoscZaplaty=wplyw*FK->JakiUdzial();
		KomunikacjaFinansowa* Zaplata=new KomunikacjaFinansowa(KONTO,WartoscZaplaty);
		Zaplata->UstawDziedzine(Produkt); //Za jaki produkt ta kasiora
		bool Kierunek=FK->Poczatek()==MojID();
		if(Zaplata->Zaadresuj(i,Kierunek,0.03)
		&& (Swiat::WstawInfo(Zaplata)!=Swiat::INVINDEX) )
			{    //Posz³o chyba?
				if(IleWydane>WartoscZaplaty)//Placiæ trzeba, nawet jak zabraknie bo umowa g³upia
					IleWydane-=WartoscZaplaty;
				else
					Dlugi+=WartoscZaplaty;//Powieksza d³ug, mo¿e potem siê wyrówna
			}
			else
			TLOG(1, <<"\""<<Nazwa()<<"\" nie uda³o siê wys³aæ dywidendy za produkt D:"<<hex<<Produkt.ARGB<<dec<<" "<<WartoscZaplaty )
   }
}

/********************************************************************/
/*			          OPI  version 2011                             */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://wwww.iss.uw.edu.pl/borkowski/                */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/


/*
DziedzinaWKolorze RynekZbytu::_DaSieSprzedac(const DziedzinaWKolorze CoZaProdukt)
{
	 DziedzinaWKolorze D;
	 D=CoZaProdukt;
	 unsigned ile_bitow=bits(D.A)+bits(D.R)+bits(D.G)+bits(D.B);
	 //Wersja uproszczona - im wiêcej bitów tym wiêksza szansa - losowe testowanie da³oby coœ zbli¿onego
	 //Chodzi o to ¿e nie zawsze uda siê sprzedaæ i im bardziej wyspecjalizowany produkt tym trudniej
	 if(double(ile_bitow)/32.0 > DRAND() )
		return D;
		else
		{D.ARGB=0;return D;}
}
*/
