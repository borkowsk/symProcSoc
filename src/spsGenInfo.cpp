/// \file
/// \brief Implementacja klasy podstawowego komunikatu realizująca interface Komunikat
///        ---------------------------------------------------------------------------
///
/// \details
///
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
/// \date
///     2022.11.07 (last updated)
// /////////////////////////////////////////////////////////////////////////////////////
//#pragma hdrstop
#include <iostream>
#include <cassert>
#include <string>
#include <cfloat>
using namespace std;

#include "spsGenInfo.h"
#include "sshutils.hpp"

//---------------------------------------------------------------------------
#pragma package(smart_init)   /*
	std::string*	    Dane;      //Ciąg danych do interpretacji
		unsigned		IleDanych; //Ile jest tych danych
		unsigned 	    Link;      //Używany kanał komunikacji
		bool 			Kierunek;  //Czy zgodnie z kanałem, czy "pod prąd"
		unsigned 		LicznikZycia;
		float			Predkosc;  //Jak szybko ma dotrzeć
		DziedzKol  		Col;       //Komunikaty też mają kolorki oczywiście...
		*/

GeneryczneInfo::KonstruktorElementowModelu<GeneryczneInfo> GeneryczneInfo::WirtualnyKonstruktor("GenInfo");

/// Rodzaj niedookreślonego komunikatu. Klasy potomne i tak przesłaniają tę funkcje
const string& GeneryczneInfo::Rodzaj()
{
   static const string _What("???");
	return _What;
}

/// A jak typ nie udostępnia zmiany to zwraca `false`
bool  GeneryczneInfo::UstawRodzaj(const char* )
{
    return false;
}

/// `true` jeśli kanał jest dobrze zdefiniowany (wciąż istnieje link, nadawca i odbiorca)
bool GeneryczneInfo::Poprawny()
{
	Powiazanie* Lacze;
	return (Link!=Swiat::INV_INDEX)
		&& (0<Predkosc && Predkosc<=1)
		&& (Lacze=Swiat::Lacze(Link))!=NULL
		&& (Swiat::Wezel(Lacze->Poczatek())!=NULL)
		&& (Swiat::Wezel(Lacze->Koniec())!=NULL
 //		&& !(Lacze->Kierunkowy() && !Kierunek) //NIEBEZPIECZNE - Niektóre łącza są kierunkowe,
 //												ale akceptują też komunikaty zwrotne idące pod prąd
		);
}

/// Kopiujący jest potrzebny tylko `Klonuj()`
GeneryczneInfo::GeneryczneInfo(const GeneryczneInfo* Wzor)
{
	Dane.InicjujZDanych(Wzor->Dane);
	Link=Wzor->Link;
	Kierunek=Wzor->Kierunek;
	Predkosc=Wzor->Predkosc;
	Col=Wzor->Col;
	LicznikZycia=0;
}

/// Pole Dane() - konstruktor domyślny wywołany automatycznie
GeneryczneInfo::GeneryczneInfo():
		Link(-1),Kierunek(false),
		Predkosc(0),LicznikZycia(0)
{

}

/// Destruktor wirtualny, bo sx metody wirtualne
/// \return -1 żeby był na pewno "niepoprawny"
GeneryczneInfo::~GeneryczneInfo()
{
	Link=-1; //żeby był na pewno "niepoprawny"
}

/// Robi na stercie kopie komunikatu do przechowania lub wstawienia - wirtualny re-konstruktor
Komunikat* GeneryczneInfo::Klonuj()
{
   return new GeneryczneInfo(this);
}

/// Metoda pobiera wszystkie potrzebne dane z listy stringów. 
/// Jak napotka błąd to podaje na `Blad`, która to pozycja.
///     - Jak za mało danych to pozycja: Pierwsza kom. za. listą.
/// |  0         |     1      |    2         |     3      |      4          | 5    |   6   |    7      ...
/// |#typ infor. | Zrodlo	  |	Cel/nr linku |	Predkosc  |	Kolor 		    | Licz | Pole1 |  Pole2	 | itd | Jeszcze komentarz
/// | geninfo	 | Badacz A	  | Wydział		 |	0.1		  | rgb(200,100,50)	| 0    |
/// | geninfo	 | *LINK*NR*  | 1112		 |	0.1	      |	x10FFAA		    | 0.5  |
bool 	GeneryczneInfo::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	Dane.InicjujWgListy(Lista,Ile);
	if(Dane.Ile()<4) { Blad=Ile; return false; } //Za mało danych. Pozycja: Pierwsza kom. za.

	char* endptr=NULL;
	if(Dane.KonwertujDo(3,Predkosc)!=-1) {Blad=3; return false;} // pole 3.
	if(Predkosc<0)
	{
		Kierunek=false;
		Predkosc=-Predkosc;//Normalny znak
	}
	else   Kierunek=true;

	double dPom;//Licz-nik.
	if(Dane.KonwertujDo(5,dPom)!=-1) {Blad=5; return false;}
	if(dPom<0)
		{ Blad=5; return false;}
	if(dPom>1)  LicznikZycia=dPom; //Jak to nie był koniec to daj >1 (???)
		else   LicznikZycia=dPom/Predkosc;

	Col.ARGB=strtorgb(Dane[4].c_str(),&endptr);
	if(endptr!=NULL && *endptr!='\0')
		{ Blad=4; return false;}

	if(Dane[1].c_str()[0]=='*' || Dane[1].c_str()[1]=='*')  //Dosłownie podany numer linku-kanału
	{
	   unsigned Pom;
		  //	unsigned strtol(Dane[2].c_str(),&endptr,10);
	   if(Dane.KonwertujDo(2,Pom)!=-1)
				{ Blad=2; return false; }
	   if(Pom>=Swiat::IleMoznaPowiazan() || Swiat::Lacze(Pom)==NULL) //Ale user mógł się pomylić!
				{ Blad=2; return false; }
	   Link=Pom; //Jak OK to zapisujemy.
	   if(!this->Poprawny())
				{ Blad=1; return false; }
	}
	else    //Gdy musi poszukać
	{
		unsigned _S=Swiat::ZnajdzIndeksWezla(Dane[1].c_str());
		if(_S==Swiat::INV_INDEX)
			{ Blad=1; return false;}
		unsigned _T=Swiat::ZnajdzIndeksWezla(Dane[2].c_str());
		if(_T==Swiat::INV_INDEX)
			{ Blad=2; return false;}
		unsigned ll=LicznikZycia; //Bo six zmieni w funkcji
		if(!Zaadresuj(_S,_T,Predkosc))
			{ Blad=2; return false;}
		LicznikZycia=ll;
	}
	Blad=6; //Udaxo six wczytax pole o indeksie 5
	return true;
}

/// Można odwrócić komunikat - zwłaszcza jak jest to klon
bool 	GeneryczneInfo::Zwrotnie(float _Szybkosc)
{
	Powiazanie* P=Swiat::Lacze(this->Link);
	if(P==NULL)
		   {  this->Link=-1; return false; } // Ale nie można, gdy w międzyczasie przestał istnieć

	if(P->Kierunkowy())
		   {  this->Link=-1; return false; } // No i nie można "pod prąd" linkiem kierunkowym (jednak nie?)

	Kierunek=!Kierunek; //Po prostu zmiana kierunku
	if(_Szybkosc!=0)
		Predkosc=_Szybkosc; //A jeśli jest równe 0 to zostawia to co było
	if(Predkosc<=0) //A jak wyszło mniejsze lub równe 0 to nadaje prędkość awaryjną
	{
		Predkosc=0.000000001; //Awaryjna prędkość komunikatu
		//cerr<<
	}
	LicznikZycia=0;
	//if(P->Akceptacja()    ????  Raczej w momencie wstawiania
	return true;
}

// Specyficzne dla Komunikatu (Message)
// ////////////////////////////////////


bool  	 GeneryczneInfo::Zaadresuj(unsigned _Powiazanie,bool KierunekZwykly,float _Szybkosc)
{
	Powiazanie* P=Swiat::Lacze(_Powiazanie);
	if(P==NULL)
			return false; //W międzyczasie przestał istnieć

	// TODO To zbyt restrykcyjne!!!
	//Lepiej jak decyduje wirtualna "Akceptacja" przy wstawianiu
	//if(!KierunekZwykly && P->Kierunkowy())
	//			return false; //Nie moxna "pod prxd" linkiem kierunkowym

	//Jak formalnie OK to możemy spróbować adresowania
	Link=_Powiazanie;
	Kierunek=KierunekZwykly;
	if(_Szybkosc!=0)
		Predkosc=_Szybkosc; //A jeśli równe to zostawia
	if(Predkosc<=0)
		Predkosc=0.000000001; //Awaryjna prędkość komunikatu
	LicznikZycia=0;
	//if(P->Akceptacja()    ---> Sprawdzana w momencie wstawiania
	return true;
}

///  Musi sam znaleźć jakieś powiązanie wśród linków klasy `Swiat::`:
///     - dla każdego kolejnego łącza...
///     - Najpierw próbuje adres jakby to był link bezkierunkowy lub kierunkowy zgodny
///     - A jak się nie uda to jakby link bezkierunkowy niezgodny
///     - Jak dojdzie do końca listy linków nic odpowiedniego nie znajdując to znaczy że nie da się
///       nadać takiego komunikatu.
bool  	GeneryczneInfo::Zaadresuj(unsigned _Nadawca,unsigned _Odbiorca,float _Szybkosc)
{
	for(unsigned i=0;i<Swiat::IlePowiazan();i++) // Szuka po wszystkich linkach Świata!!!
	{
		Powiazanie* P=Swiat::Lacze(i);
		if( P!=NULL )
		{  //Link bezkierunkowy lub kierunkowy zgodny
		   if(P->Poczatek()==_Nadawca
		   && P->Koniec()==_Odbiorca
		   && Zaadresuj(i,true,_Szybkosc) 	)
				return true; //Udało się w zwykły sposób

		   //Link bezkierunkowy niezgodny
		   if(P->Poczatek()==_Odbiorca
			 && P->Koniec()==_Nadawca
			 && (!P->Kierunkowy())    //Teoretycznie więcej linków bezkierunkowych niż z danym odbiorcą!?
			 && Zaadresuj(i,false,_Szybkosc)  )
				return true; //Udało się w odwrotnym kierunku
		}
	}
	return false; //Jak doszedł do konca pętli, to znaczy, że się nie udało
}

/// Od kogo, czyli nadawca komunikatu z uwzględnieniem kierunku na linku
unsigned  GeneryczneInfo::Nadawca()
{
   Powiazanie* P=Swiat::Lacze(Link);
   if(P==NULL) return -1;
		return (Kierunek?P->Poczatek():P->Koniec());
}

/// Do kogo, czyli odbiorca komunikatu z uwzględnieniem kierunku na linku
unsigned GeneryczneInfo::Odbiorca()

{
   Powiazanie* P=Swiat::Lacze(Link);
   if(P==NULL) return -1;
		return (Kierunek?P->Koniec():P->Poczatek());
}

/// Zwraca `0` tuż po starcie, a jak zwraca `1` to już komunikat dotarł.
/// Jak przeterminowany to `1.001`, żeby się nie pokazywał bardzo za celem
float  GeneryczneInfo::JakDostawa()
{               assert(0<Predkosc && Predkosc<=1);
	  double krokow=1/Predkosc;
	  double zaaw=LicznikZycia/krokow; //Jak >1 to przeterminowany
	  if(zaaw<=1.00000)
		return zaaw;
		else
		return 1.001; //żeby się nie pokazywał bardzo za celem jak się przeterminuje
}

/// \return W ciągu ilu kroków powinien zostać dostarczony?
float  GeneryczneInfo::JakiTermin()
{                     	assert(0<Predkosc && Predkosc<=1);
	return 1.0/Predkosc;
}

/// Metoda daje  szanse na endogenne zmiany stanów.
/// Ale ta bazowa jedynie inkrementuje `LicznikZycia`.
void GeneryczneInfo::ChwilaDlaCiebie()
{
	LicznikZycia++;
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


