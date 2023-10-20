/// \file
/// \brief Implementacja matrycowego typu węzła
//         ------------------------------------
///
/// \details
///     Definicje matrycowego typu węzła - czyli zawierającego tablicę "dziedzin",
///     która jest wizualizowana jako obrazek i może być obrazkiem zainicjowana.
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011, wborkowski@uw.edu.pl
/// \date
///     2022.11.07 (last updated)
// ////////////////////////////////////////////////////////////////////////////////

#pragma hdrstop //???

#include "spsMatrixNode.h"
#include "sshutils.hpp"
#include "wb_ptr.hpp"
#include "wb_smartlog.hpp"
#include "lingo.hpp"

#include <stdlib.h>
#include <iostream>
#include <cassert>
using namespace std;
using namespace wbrtm;

//---------------------------------------------------------------------------
//#pragma package(smart_init)
//  protected:
	//DziedzKol  		Col;      //Dziedziczony z ElementModelu - interpretowane jako t�o
	//std::string*	Dane;
	//unsigned		IleDanych;    //Dziedziczony z  GenerycznyWezelSieci
	//double    		W,_X,_Y,_Z;
	//Wielobok* 		Ksztalt;  //U�ywany do wy�wietlania pojedynczego "piksela"
	//DziedzinaWKolorze*  Tablica;
	//unsigned 			Wysokosc;
	//unsigned 			Szerokosc;

WezelMacierzowy::KonstruktorElementowModelu<WezelMacierzowy> WezelMacierzowy::WirtualnyKonstruktor("MattNode");

WezelMacierzowy::WezelMacierzowy():	Tablica(0), //DziedzinaWKolorze*
									Wysokosc(0),Szerokosc(0)
{
}

/// Dostęp do danych macierzy sprawdza indeksy x i y oraz dostepnośc tablicy.
/// Jeśli jest to OK to wykonuje mnożenie i zwraca odpowienią pozycję z `Tablica`.
DziedzKol& WezelMacierzowy::operator() (unsigned x,unsigned y)
{
	if(x<Szerokosc && y<Wysokosc && Tablica)
	{
		unsigned index=y*Szerokosc+x;
		return Tablica[index];
	}
	else
	{
		cerr<<_LPL("Niepoprawny indeks do ","Invalid index to ")<<Nazwa()<<" x:"<<x<<" y:"<<y<<endl;
		exit(-1);
	}
}

//Funkcja przeszukiwania "bazy danych".
DziedzinaWKolorze WezelMacierzowy::_ZnajdzNajpodobniejszy(  DziedzinaWKolorze D,
															unsigned&   Indeks,
														    double&     WzglednePobienstwo,
															unsigned    IleProb )
{
	//static
	Indeks=Swiat::INV_INDEX; //Jakby nie znalazł
	WzglednePobienstwo=0;    //Podobienstwo mniejsze niż 0 być nie może ;=)
	DziedzinaWKolorze Wynik; //Na rezultat

	double IleBitowWzorca=D.IleBitow();  //Żeby tego ciągle nie powtarzać, ale czy potrzebne?
	unsigned ile=Szerokosc*Wysokosc;

    if(IleProb>ile) //Liniowo
	{
	   for(unsigned i=0;i<ile;i++)
	   {
		  DziedzinaWKolorze Roznica;
		  Roznica=D.ARGB ^ Tablica[i].ARGB;
		  double Podob=Roznica.IleBitow();
		  Podob=32-Podob;
		  Podob/=32;
		  if(Podob>WzglednePobienstwo)
		  {
			 Wynik=Tablica[i];
			 WzglednePobienstwo=Podob;
			 Indeks=i;
		  }
	   }
	}
	else //Losowo
	{
	   for(unsigned a=0;a<ile;a++)
	   {
		  unsigned i=RANDOM(ile);                			   	  assert(i<ile);
		  DziedzinaWKolorze Roznica;
		  Roznica=D.ARGB ^ Tablica[i].ARGB;
		  double Podob=Roznica.IleBitow();
		  Podob=32-Podob;
		  Podob/=32;
		  if(Podob>WzglednePobienstwo)
		  {
			 Wynik=Tablica[i];
			 WzglednePobienstwo=Podob;
			 Indeks=i;
		  }
	   }
	}
	//Coś się znanalzło
	if(IleBitowWzorca<=1)
			{
		   //	clog<<"?";//DEBUG
			}
	return Wynik;
}

/// Metoda pobiera wszystkie potrzebne dane z listy stringów.
bool WezelMacierzowy::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	Dane.InicjujWgListy(Lista,Ile);  //Przepisanie listy i ...
	if(Dane.Ile()<11)			     //Sprawdzenie, czy jest wystarczająco danych do inicjacji
			{ Blad=Ile; return false; } //Za mało danych. Pierwsza kom. za.

	const char* astr; //Pozycja tekstu pomocniczego
	if((astr=strchr(Dane[8].c_str(),'*'))!=NULL) //Jeśli jest gwiazdka to będzie plik
	{   //Więc zawartość macierzy z pliku
	   if(!_ZaladujKoloryZPliku(Dane[9].c_str(),astr+1))
			{ Blad=9; return false; }
	}
	else  //Jeżeli nie z pliku to pole 7 i 8 zawierają szerokość i wysokość
	{
		if(Dane.KonwertujDo(8,Szerokosc)!=-1 || Szerokosc==0)
			{ Blad=8; return false; }
		if(Dane.KonwertujDo(9,Wysokosc)!=-1 || Wysokosc==0)
			{ Blad=9; return false; }
		if(Tablica) delete [] Tablica; //Zwalnianie starej pamięci
		Tablica=new DziedzinaWKolorze[Szerokosc*Wysokosc];
		//Kolor konieczny do inicjowania
		char* endptr=NULL;
		Col.ARGB=strtorgb(Dane[4].c_str(),&endptr);
		if(endptr!=NULL && *endptr!='\0')
					{ Blad=4; return false;}
		//Własciwe inicjowanie tablicy
		for(unsigned i=0;i<Szerokosc*Wysokosc;i++)
			   Tablica[i]=Col;
	}

	{ //Waga jest od razu potrzebna, bo wg. niej oblicza się promień
	double W;
	if(Dane.KonwertujDo(3,W)!=-1 || W<=0)
			{ Blad=3; return false;}
			else
			Waga=W;
	}

	//Wczytujemy pozostałe pola (???) Głownie chodzi o tworzenie wizualizacji
	//Drobna sztuczka - na chwilę zmniejszamy wagę, żeby był mniejszy
	double ZapamietajW=Waga;
	double JakaWagaJednostki=double( Waga/std::max(Szerokosc,Wysokosc) );
	if(JakaWagaJednostki<0.1)
			JakaWagaJednostki=0.1; //Awaryjny rozmiar jednostki jak za mała

	Dane.PrzypiszZ(3, JakaWagaJednostki );
	if(!_PrzeniesDaneNaPola(Blad))
			return false;
	Dane.PrzypiszZ(3, ZapamietajW );
	Waga=ZapamietajW;

	//Dodawanie szumu
	int RandPar;
	int poz;
	if((poz=Dane.KonwertujDo(10,RandPar))>=0)
			{ Blad=10; return false;}

	if(RandPar!=0) //Jakiś szumek jest
	for(unsigned i=0;i<Szerokosc*Wysokosc;i++)
	{
		  Tablica[i].R=unsigned(Tablica[i].R*(1-2*DRAND_LOOP(RandPar)))%256;
		  Tablica[i].G=unsigned(Tablica[i].G*(1-2*DRAND_LOOP(RandPar)))%256;
		  Tablica[i].B=unsigned(Tablica[i].B*(1-2*DRAND_LOOP(RandPar)))%256;
	}

	Blad=11;        // 10 pól juź jest wczytanych
	return true;   	// Teraz już naprawdę wszystko gotowe
}

/// Destruktor wirtualny - Zwalnianie pamięci pola `Tablica`
WezelMacierzowy::~WezelMacierzowy()
{
   if(Tablica) delete [] Tablica; //Zwalnianie pamięci
}

	// Proste akcesory
    // ///////////////

/// Promień otaczającego okręgu
double WezelMacierzowy::R(double Angle)
{
	if(Waga>0)
	 return ((JakieRwProcWidth/100.0)*ver*Waga); //Idzie na łatwiznę na razie
	 else
	 return 1; //jeden piksel !
}

/// Tu trafienie w prostokat!
bool WezelMacierzowy::Trafiony(float sX,float sY)
{
	 double StartX,StartY,EndX,EndY,Step;
	 double R=this->R(0);
	 if(Szerokosc>=Wysokosc)
	 {
		Step=(2*R)/Szerokosc;
	 }
	 else
	 {
		 Step=(2*R)/Wysokosc;
	 }
	 StartX=X()-0.5*Step*(Szerokosc);
	 StartY=Y()-0.5*Step*(Wysokosc);
	 EndX=X()+0.5*Step*(Szerokosc);
	 EndY=Y()+0.5*Step*(Wysokosc);
	 if(StartX<=sX
	 && sX<=EndX
	 && StartY<=sY
	 && sY<=EndY)
		return true;
	 else
		return false;
}

/// Element będący podstawą odpowiedzi może być wskazany za pomocą `Który` lub zostanie wylosowany.
bool WezelMacierzowy::_OdpowiedzLosowymBitem(Komunikat* Pyt,unsigned Ktory,bool AND_OR)
{
	wb_ptr<Komunikat> Klon( Pyt->Klonuj() ); //Jak się z jakichś powodów nie uda wysłać to przygotowywany komunikat znika dzięki wb_ptr

	if(Klon->Zwrotnie()) //O ile uda się adresowanie zwrotne
		{
			DziedzKol Pom=Pyt->PodajDziedzine(); //Jaka jest dziedzina komunikatu
			unsigned i=Ktory;
			if(i==Swiat::INV_INDEX)
						RANDOM(Szerokosc*Wysokosc);  //Jakiś losowy element
			if(AND_OR)
				Pom.ARGB=Tablica[i].ARGB & Pom.ARGB; // Część wspólna pytania i odpowiedzi
			else
				Pom.ARGB=Tablica[i].ARGB | Pom.ARGB; // Suma logiczna pytania i odpowiedzi
			Klon->UstawDziedzine(Pom);
			return Swiat::WstawInfo(Klon.give())!=Swiat::INV_INDEX; //Wstawia klon komunikatu - tracąc z "zarządu"
		}
	return false;
}

/// MOŻE BYĆ KOSZTOWNA! Jak nie podano liczby prób to przegląda całą zawartość!!!
bool WezelMacierzowy::_OdpowiedzNajpodobniejszym(Komunikat* Pyt,unsigned IleProb)
{
										                                          //  assert("NIE ZAIMPLEMENTOWANE!!!");
	DziedzinaWKolorze D;
	unsigned Indeks;
    double WzglednePobienstwo;
	D=_ZnajdzNajpodobniejszy(Pyt->PodajDziedzine(),Indeks,WzglednePobienstwo,IleProb);//unsigned IleProb)<==oznacza przeszukanie liniowe

	Komunikat* ODP=Pyt->Klonuj();
	string Pom=Pyt->Rodzaj()+".ODP";
	ODP->UstawRodzaj(Pom.c_str());
	ODP->UstawDziedzine(D);
	if(ODP->Zwrotnie()
		&& Swiat::WstawInfo(ODP)!=Swiat::INV_INDEX)
			 return true;
		else
		TLOG(1, <<"Nie da się wysłać odpowiedzi na komunikat rodzaju \""<<Pyt->Rodzaj()
                <<"\" D:"<<hex<<Pyt->PodajDziedzine().ARGB<<" Do "<<Pyt->Nadawca()  )
	return false;
}

/* *******************************************************************/
/*			            SPS  version 2022                            */
/* *******************************************************************/
/*             THIS CODE IS DESIGNED & COPYRIGHT  BY:                */
/*              W O J C I E C H   B O R K O W S K I                  */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*        RG:https://www.researchgate.net/profile/Wojciech-Borkowski */
/*        GitHub: https://github.com/borkowsk                        */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/

