////////////////////////////////////////////////////////////////////////////////
// Symulator Procesów Sieciowych/Spolecznych (c) Instytut Studiów Spo³ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
////////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
////////////////////////////////////////////////////////////////////////////////
//
// Definicje podstawowego typu realnego linku (nie pure virtual jak Powiazanie)
////////////////////////////////////////////////////////////////////////////////////////
#include "spsGenLink.h"

#include <iostream>
#include <cassert>
#include <string>
#include <cfloat>
#include <cstdlib>
#include <cmath>
using namespace std;

#include "SYMSHELL/sshutils.hpp"

//Zakres w jakim musi siê zmieœciæ wizualizacja linków
//Ale mo¿e byæ jeszcze przeskalowane przez odpowiednie parametry klasy Swiat
//Domyslne wartosæi daj¹ maksymalnie szerokie linki
float GenerycznePowiazanie::MINIMALNA_GRUBOSC_LINKU=0.05;
float GenerycznePowiazanie::MAKSYMALNA_GRUBOSC_LINKU=0.5;

GenerycznePowiazanie::KonstruktorElementowModelu<GenerycznePowiazanie>
							GenerycznePowiazanie::WirtualnyKonstruktor("GenLink");
GenerycznePowiazanieSkierowane::KonstruktorElementowModelu<GenerycznePowiazanieSkierowane>
				GenerycznePowiazanieSkierowane::WirtualnyKonstruktor("DirGenLink");


//Specyficzne dla Powi¹zania (linku)
/*
  private: //Pola
  std::string*	Dane;
  unsigned		IleDanych;
  double 		W;
  unsigned    	_S,_T;
  DziedzKol  	Col;
*/

bool GenerycznePowiazanie::Poprawny()
//true jeœli jest dobrze zdefiniowany
{
	return Swiat::Wezel(_S)!=NULL && Swiat::Wezel(_T)!=NULL;
}

bool  GenerycznePowiazanie::Akceptacja(Komunikat* Co)
//Ta implementacja pozwala ka¿demu POPRAWNEMU komunikatowi mo¿e przejœæ tym ³¹czem
{
	unsigned _N=Co->Nadawca();
	unsigned _O=Co->Odbiorca();
	if(	(_N==_S && _O==_T)     //Klasy potomne mog¹ byæ skierowane
		|| ( !Kierunkowy() && _N==_T && _O==_S) )
			return true;
			else
			return false;
}

//virtual bool  Kierunkowy()=0;
void  GenerycznePowiazanie::PodajPozycje(double D,bool KierunekZwykly,double& X,double& Y,Komunikat*)
//Oblicza polozenia wdluz linku. Nie uwzglêdnia specyfiki komunikatu bo nie mo¿e jej znaæ
{
	WezelSieci *_P, *_K;
	if(KierunekZwykly)
	{
		_P=Swiat::Wezel(_S);
		_K=Swiat::Wezel(_T);
	}
	else
	{
		_P=Swiat::Wezel(_T);
		_K=Swiat::Wezel(_S);
	}

	X=(1-D)*_P->X()+(D)*_K->X();
	Y=(1-D)*_P->Y()+(D)*_K->Y();
}

  unsigned GenerycznePowiazanie::Poczatek()
  {
	  return _S;
  }

  unsigned GenerycznePowiazanie::Koniec()
  {
      return _T;
  }


  GenerycznePowiazanie::GenerycznePowiazanie() :  Waga(1)
  // Domyslny konstruktor ustawiaj¹cy pusty link
  {
	  Col.ARGB = 0;
	  _S = _T = -1;
  }

  bool GenerycznePowiazanie::ZrobWgListy(const std::string* Lista,
											unsigned Ile, unsigned& Blad)
  // Metoda pobiera wszystkie potrzebne dane z listy stringów. Jak blad to podaje ktora pozycja
  {
	  Dane.InicjujWgListy(Lista,Ile);
	  if(Dane.Ile()<5) { Blad=Ile; return false; } //Za ma³o danych. Pierwsza kom. za.
	//std::string*	Dane;
	//unsigned		IleDanych;
	//unsigned    	W,_S,_T;
	//float 		_Z;
	//DziedzKol  	Col;
	//	0					1				2				  3		  4 	5
	//typ(node/link)	Nazwa/Zrodlo	Wizualizacja/Cel	Waga	Kolor 	Z	Pole1	Pole2	Pole3	itd.
	//genlink				Wydzia³ 			Uniwerek	1			0   0
	//genlink				Badacz A			Badacz B	0.5		00FF00   1
	//...
	//unsigned long strtoul(const char *s, char **endptr,int base);
	//int atoi(const char *s);
	//double atof(const char *s);
	if(Dane.KonwertujDo(3,Waga)!=-1)//	Waga=atof(Dane[3].c_str());
		{ Blad=3; return false;}

	char* endptr=NULL;
	Col.ARGB=strtorgb(Dane[4].c_str(),&endptr);
	if(endptr!=NULL && *endptr!='\0')
		{ Blad=4; return false;}

	if(Dane.KonwertujDo(5,_Z)!=-1)  	//_Z=atof(Dane[5].c_str());
		 { Blad=5; return false;}

	_S=Swiat::ZnajdzIndeksWezla(Dane[1].c_str());
	if(_S==Swiat::INVINDEX)
		{ Blad=1; return false;}

	_T=Swiat::ZnajdzIndeksWezla(Dane[2].c_str());
	if(_T==Swiat::INVINDEX)
		{ Blad=2; return false;}

	Blad=6;//Uda³o siê wczytaæ do indeksu 5

	return true;
  }

  GenerycznePowiazanie::~GenerycznePowiazanie()
  // Destruktor wirtualny, bo bêd¹ metody wirtualne
  {
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

