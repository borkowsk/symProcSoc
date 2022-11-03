// //////////////////////////////////////////////////////////////////////////////
// Symulator Proces�w Sieciowych/Spolecznych (c) Instytut Studi�w Spo�ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
// //////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
// //////////////////////////////////////////////////////////////////////////////
//
//
// //////////////////////////////////////////////////////////////////////////////
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
	std::string*	Dane;      //Ci�g danych do interpretacji
		unsigned		IleDanych; //Ile jest tych danych
		unsigned 	    Link;      //U�ywany kana� komunikacji
		bool 			Kierunek;  //Czy zgodnie z kana�em czy "pod pr�d"
		unsigned 		LicznikZycia;
		float			Predkosc; //Jak szybko ma dotrze�
		DziedzKol  		Col;//Komunikaty te� maj� kolorki oczywi�cie...
		*/

GeneryczneInfo::KonstruktorElementowModelu<GeneryczneInfo> GeneryczneInfo::WirtualnyKonstruktor("GenInfo");

const string& GeneryczneInfo::Rodzaj()
{
   static const string _What("???");//Rodzaj nieodokre�lonego komunikatu. Klasy potomne i tak przes�aniaj� t� funkcje
	return _What;
}

bool  GeneryczneInfo::UstawRodzaj(const char* )
//A jak typ nie udost�pnia zmiany to zwraca false
{
    return false;
}

bool GeneryczneInfo::Poprawny()
//true je�li kanal jest dobrze zdefiniowany (wci�� istnieje link, nadawca i odbiorca)
{
	Powiazanie* Lacze;
	return (Link!=Swiat::INV_INDEX)
		&& (0<Predkosc && Predkosc<=1)
		&& (Lacze=Swiat::Lacze(Link))!=NULL
		&& (Swiat::Wezel(Lacze->Poczatek())!=NULL)
		&& (Swiat::Wezel(Lacze->Koniec())!=NULL
 //		&& !(Lacze->Kierunkowy() && !Kierunek) //NIEBEZPIECZNE - Niekt�re ��cza s� kierunkowe
 //												ale akceptuj� te� komunikaty zwrotne id�ce pod pr�d
		);
}

GeneryczneInfo::GeneryczneInfo(const GeneryczneInfo* Wzor)
//Kopiuj�cy jest potrzebny tylko Klonuj()
{
	Dane.InicjujZDanych(Wzor->Dane);
	Link=Wzor->Link;
	Kierunek=Wzor->Kierunek;
	Predkosc=Wzor->Predkosc;
	Col=Wzor->Col;
	LicznikZycia=0;
}

GeneryczneInfo::GeneryczneInfo(): //Dane() - konstruktor wywo�any automatycznie
		Link(-1),Kierunek(false),
		Predkosc(0),LicznikZycia(0)
{

}

GeneryczneInfo::~GeneryczneInfo()
//Destruktor wirtualny, bo s� metody wirtualne
{
	Link=-1; //Zeby by� na pewno "niepoprawny"
}

Komunikat* GeneryczneInfo::Klonuj()
//Robi na stercie kopie komunikatu do przechowania lub wstawienia - wirtualny re-konstruktor
{
   return new GeneryczneInfo(this);
}

//Metoda pobiera wszystkie potrzebne dane z listy string�w. Jak blad to podaje ktora pozycja
bool 	GeneryczneInfo::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	Dane.InicjujWgListy(Lista,Ile);
	if(Dane.Ile()<4) { Blad=Ile; return false; } //Za ma�o danych. Pierwsza kom. za.
//   0             1           2              3           4           5       6       7      ...
//#typ infor.	Zrodlo		Cel/nr linku	Pr�dko��	Kolor 		Licz	Pole1	Pole2	Pole3	Pole4	Pole5	Pole6	Pole7	Pole8	Pole9	Pole10	itd. Czyli komentarz
//geninfo		Badacz A	Wydzia�			0.1		rgb(200,100,50)	0
//geninfo		*LINK*NR*	2				0.1			x10FFAA		0.5
	char* endptr=NULL;
	if(Dane.KonwertujDo(3,Predkosc)!=-1) {Blad=3; return false;}
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
	if(dPom>1)  LicznikZycia=dPom; //Jak ma nie by� koniec to daj >1
		else   LicznikZycia=dPom/Predkosc;

	Col.ARGB=strtorgb(Dane[4].c_str(),&endptr);
	if(endptr!=NULL && *endptr!='\0')
		{ Blad=4; return false;}

	if(Dane[1].c_str()[0]=='*' || Dane[1].c_str()[1]=='*')  //Dos�ownie podany numer linku-kanalu
	{
	   unsigned Pom;
		  //	unsigned strtol(Dane[2].c_str(),&endptr,10);
	   if(Dane.KonwertujDo(2,Pom)!=-1)
				{ Blad=2; return false; }
	   if(Pom>=Swiat::IleMoznaPowiazan() || Swiat::Lacze(Pom)==NULL) //Ale user m�g� si� pomyli�!
				{ Blad=2; return false; }
	   Link=Pom; //Jak OK to zapisujemy.
	   if(!this->Poprawny())
				{ Blad=1; return false; }
	}
	else    //Gdy musi poszuka�
	{
		unsigned _S=Swiat::ZnajdzIndeksWezla(Dane[1].c_str());
		if(_S==Swiat::INV_INDEX)
			{ Blad=1; return false;}
		unsigned _T=Swiat::ZnajdzIndeksWezla(Dane[2].c_str());
		if(_T==Swiat::INV_INDEX)
			{ Blad=2; return false;}
		unsigned ll=LicznikZycia; //Bo si� zmieni w funkcji
		if(!Zaadresuj(_S,_T,Predkosc))
			{ Blad=2; return false;}
		LicznikZycia=ll;
	}
	Blad=6; //Uda�o si� wczyta� pole o indeksie 5
	return true;
}

bool 	GeneryczneInfo::Zwrotnie(float _Szybkosc)
//Mo�na odwr�ci� komunikat - zw�aszcza jak jest to klon
{
	Powiazanie* P=Swiat::Lacze(this->Link);
	if(P==NULL)
		   {  this->Link=-1; return false; }//Ale nie mo�na gdy w mi�dzyczasie przesta� istnie�
	if(P->Kierunkowy())
		   {  this->Link=-1; return false; } //No i nie mo�na "pod pr�d" linkiem kierunkowym
	Kierunek=!Kierunek; //Po proistu zmiana kierunku
	if(_Szybkosc!=0)
		Predkosc=_Szybkosc;//A je�li jest r�wne 0 to zostawia to co by�o
	if(Predkosc<=0) //A jak wysz�o mniejsze bad� r�wne 0 to nadaje predko�� awaryjn�
	{
		Predkosc=0.000000001;//Awaryjna pr�dko�c komunikatu
		//cerr<<
	}
	LicznikZycia=0;
	//if(P->Akceptacja()    ????  Raczej w momencie wstawiania
	return true;
}

//Specyficzne dla Komunikatu (Message)
bool  	 GeneryczneInfo::Zaadresuj(unsigned _Powiazanie,bool KierunekZwykly,float _Szybkosc)
{
	Powiazanie* P=Swiat::Lacze(_Powiazanie);
	if(P==NULL)
			return false; //W mi�dzyczasie przesta� istnie�
	//To zbyt restrykcyjne!!!
	//Lepiej jak decyduje wirtialna "Akceptacja" przy wstawianiu
	//if(!KierunekZwykly && P->Kierunkowy())
	//			return false; //Nie mo�na "pod pr�d" linkiem kierunkowym

	//Jak formalnie OK to mo�emy spr�bowa� adresowania
	Link=_Powiazanie;
	Kierunek=KierunekZwykly;
	if(_Szybkosc!=0)
		Predkosc=_Szybkosc;//A je�li r�wne to zostawia
	if(Predkosc<=0)
		Predkosc=0.000000001;//Awaryjna pr�dko�c komunikatu
	LicznikZycia=0;
	//if(P->Akceptacja()    ---> Sprawdzana w momencie wstawiania
	return true;
}

bool  	GeneryczneInfo::Zaadresuj(unsigned _Nadawca,unsigned _Odbiorca,float _Szybkosc)
{
	for(unsigned i=0;i<Swiat::IlePowiazan();i++)
	{
		Powiazanie* P=Swiat::Lacze(i);
		if( P!=NULL )
		{  //Link bezkierunkowy lub kierunkowy zgodny
		   if(P->Poczatek()==_Nadawca
		   && P->Koniec()==_Odbiorca
		   && Zaadresuj(i,true,_Szybkosc) 	)
				return true; //Uda�o si� w zwyk�y spos�b
		   //Link bezkierunkowy niezgodny
		   if(P->Poczatek()==_Odbiorca
			 && P->Koniec()==_Nadawca
			 && (!P->Kierunkowy())    //Teoretycznie wi�cej link�w bezkierunkowych ni� z danym odbiorc�!?
			 && Zaadresuj(i,false,_Szybkosc)  )
				return true; //Uda�o si� w odwrotnym kierunku
		}
	}
	return false;//Jak doszed� do konca p�tli to znaczy �e si� nie uda�o
}

unsigned  GeneryczneInfo::Nadawca()
//Od kogo ...
{
   Powiazanie* P=Swiat::Lacze(Link);
   if(P==NULL) return -1;
		return (Kierunek?P->Poczatek():P->Koniec());
}

unsigned GeneryczneInfo::Odbiorca()
//... do kogo.
{
   Powiazanie* P=Swiat::Lacze(Link);
   if(P==NULL) return -1;
		return (Kierunek?P->Koniec():P->Poczatek());
}

float  GeneryczneInfo::JakDostawa()
//0 po starcie, jak 1 to ju� dotar�
{               assert(0<Predkosc && Predkosc<=1);
	  double krokow=1/Predkosc;
	  double zaaw=LicznikZycia/krokow; //Jak >1 to przeterminowany
	  if(zaaw<=1.00000)
		return zaaw;
		else
		return 1.001;//�eby si� nie pokazywa� bardzo za celem jak si� przeterminuje
}

float  GeneryczneInfo::JakiTermin()
//W ile krok�w powinien zosta� dostarczony?
{                     	assert(0<Predkosc && Predkosc<=1);
	return 1.0/Predkosc;
}

void GeneryczneInfo::ChwilaDlaCiebie()
//Daje jakies szanse na endogenne zmiany stan�w - np. zbli�a do dostawy
{
	LicznikZycia++;
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

