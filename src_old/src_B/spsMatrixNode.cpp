////////////////////////////////////////////////////////////////////////////////
// Symulator Procesów Sieciowych/Spolecznych (c) Instytut Studiów Spo³ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
////////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
////////////////////////////////////////////////////////////////////////////////
//
// Definicje matrycowego typu wêz³a - czyli zawieraj¹cego tablicê "dziedin"
// która jest wizualizowana jako obrazek i mo¿e byæ obrazkiem zainicjowana
////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------

#pragma hdrstop

#include "spsMatrixNode.h"
#include "INCLUDE/platform.hpp"
#include "SYMSHELL/sshutils.hpp"

#include <stdlib.h>
#include <iostream>
#include <cassert>
using namespace std;

//---------------------------------------------------------------------------
//#pragma package(smart_init)
//  protected:
	//DziedzKol  		Col;   //Dziedziczony z ElementModelu - interpretowane jako t³o
	//std::string*	Dane;
	//unsigned		IleDanych; //Dziedziczony z  GenerycznyWezelSieci
	//double    		W,_X,_Y,_Z;
	//Wielobok* 		Ksztalt; //U¿ywany do wyœwietlania pojedynczego "piksela"
	//DziedzinaWKolorze*  Tablica;
	//unsigned 			Wysokosc;
	//unsigned 			Szerokosc;

WezelMacierzowy::WezelMacierzowy():	Tablica(0),//DziedzinaWKolorze*
									Wysokosc(0),Szerokosc(0)
{
}

WezelMacierzowy::KonstruktorElementowModelu<WezelMacierzowy> WezelMacierzowy::WirtualnyKonstruktor("MattNode");

DziedzKol& WezelMacierzowy::operator() (unsigned x,unsigned y)
//Dostêp do danych macierzy
{
	if(x<Szerokosc && y<Wysokosc && Tablica)
	{
		unsigned index=y*Szerokosc+x;
		return Tablica[index];
	}
	else
	{
		cerr<<_LPL("Niepoprawny dostep do ","Invalid index to ")<<Nazwa()<<" x:"<<x<<" y:"<<y<<endl;
		exit(-1);
	}
}

//Metoda pobiera wszystkie potrzebne dane z listy stringów. Jak blad to podaje ktora pozycja
bool WezelMacierzowy::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	Dane.InicjujWgListy(Lista,Ile);  //Przepisanie listy i ...
	if(Dane.Ile()<11)			//Sprawdzenie czy jest wystarczaj¹co danych do inicjacji
			{ Blad=Ile; return false; } //Za ma³o danych. Pierwsza kom. za.

	const char* astr;//Pozycja tekstu pomocniczego
	if((astr=strchr(Dane[8].c_str(),'*'))!=NULL) //Jeœli jest gwiazdka to bêdzie plik
	{   //Wiêc zawartosc macierzy z pliku
	   if(!_ZaladujKoloryZPliku(Dane[9].c_str(),astr+1))
			{ Blad=9; return false; }
	}
	else  //Je¿eli nie z pliku to pole 7 i 8 zawieraj¹ szerokoœæ i wysokoœæ
	{
		if(Dane.KonwertujDo(8,Szerokosc)!=-1 || Szerokosc==0)
			{ Blad=8; return false; }
		if(Dane.KonwertujDo(9,Wysokosc)!=-1 || Wysokosc==0)
			{ Blad=9; return false; }
		if(Tablica) delete [] Tablica; //Zwalnianie starej pamiêci
		Tablica=new DziedzinaWKolorze[Szerokosc*Wysokosc];
		//Kolor konieczny do inicjowania
		char* endptr=NULL;
		Col.ARGB=strtorgb(Dane[4].c_str(),&endptr);
		if(endptr!=NULL && *endptr!='\0')
					{ Blad=4; return false;}
		//W³asciwe inicjowanie tablicy
		for(unsigned i=0;i<Szerokosc*Wysokosc;i++)
			   Tablica[i]=Col;
	}

	{//Waga jest od razu potrzebna bo wg. niej oblicza siê promieñ
	double W;
	if(Dane.KonwertujDo(3,W)!=-1 || W<=0)
			{ Blad=3; return false;}
			else
			Waga=W;
	}
	//Wczytujemy pozosta³e pola (???) G³ownie chodzi o tworzenie wielok¹ta
	//Drobna sztuczka - na chwilê zmniejszamy wagê, ¿eby by³ mniejszy
	double ZapamietajW=Waga;
	Dane.PrzypiszZ(3, double(Waga/std::max(Szerokosc,Wysokosc))  );
	if(!_PrzeniesDaneNaPola(Blad))
			return false;
	Dane.PrzypiszZ(3, ZapamietajW );
	Waga=ZapamietajW;

	//Dodawanie szumu
	int RandPar;
	int poz;
	if((poz=Dane.KonwertujDo(10,RandPar))>=0)
			{ Blad=10; return false;}

	if(RandPar!=0) //Jakiœ szumek jest
	for(unsigned i=0;i<Szerokosc*Wysokosc;i++)
	{
		  Tablica[i].R=unsigned(Tablica[i].R*(1-2*DRAND_LOOP(RandPar)))%256;
		  Tablica[i].G=unsigned(Tablica[i].G*(1-2*DRAND_LOOP(RandPar)))%256;
		  Tablica[i].B=unsigned(Tablica[i].B*(1-2*DRAND_LOOP(RandPar)))%256;
	}

	Blad=11; //10 pól ju¿ jest wczytanych
	return true;   	//Teraz juz naprawdê wszystko gotowe
}

WezelMacierzowy::~WezelMacierzowy()
//Destruktor wirtualny
{
   if(Tablica) delete [] Tablica; //Zwalnianie pamiêci
}

	//Proste akcesory
double WezelMacierzowy::R(double Angle)
//Promieñ otaczaj¹cego okrêgu lub elipsy
{
	if(Waga>0)
	 return ((JakieRwProcWidth/100.0)*ver*Waga);//Idzie na ³atwiznê na razie
	 else
	 return 1; //jeden piksel !
}

bool WezelMacierzowy::Trafiony(float sX,float sY)
//Np. do inspekcji myszk¹ - Tu trafienie w prostok¹t.
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
