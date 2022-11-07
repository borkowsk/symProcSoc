/// \file
/// \brief Definicje podstawowego typu realnego linku (nie pure virtual jak Powiazanie)
///        -----------------------------------------------------------------------------
///
/// \details
///              ...
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
/// \date
///     2022.11.07 (last updated)
//*////////////////////////////////////////////////////////////////////////////////

#include "spsGenLink.h"

#include <iostream>
#include <cassert>
#include <string>
#include <cfloat>
#include <cstdlib>
#include <cmath>
using namespace std;

#include "sshutils.hpp"

// Wirtualne konstruktory obu definiowanych tu typów
// /////////////////////////////////////////////////
GenerycznePowiazanie::KonstruktorElementowModelu<GenerycznePowiazanie>
        GenerycznePowiazanie::WirtualnyKonstruktor("GenLink");
GenerycznePowiazanieSkierowane::KonstruktorElementowModelu<GenerycznePowiazanieSkierowane>
        GenerycznePowiazanieSkierowane::WirtualnyKonstruktor("DirGenLink");

/// Zakres, w jakim musi się zmieścić wizualizacja linków.
/// Jednakże może być jeszcze przeskalowane przez odpowiednie parametry klasy `Swiat::`
float GenerycznePowiazanie::MINIMALNA_GRUBOSC_LINKU=0.05;
/// Domyślne wartości dają maksymalnie szerokie linki
float GenerycznePowiazanie::MAKSYMALNA_GRUBOSC_LINKU=0.5;


// Specyficzne dla Powiązania (linku)
// //////////////////////////////////
/*
  private: //Pola
  std::string*	Dane;
  unsigned		IleDanych;
  double 		W;
  unsigned    	_S,_T;
  DziedzKol  	Col;
*/

/// \return `true` jeśli jest dobrze zdefiniowany
bool GenerycznePowiazanie::Poprawny()
{
	return Swiat::Wezel(_S)!=NULL && Swiat::Wezel(_T)!=NULL;
}

/// Domyślna implementacja pozwala każdemu POPRAWNEMU komunikatowi przejść tym łączem
bool  GenerycznePowiazanie::Akceptacja(Komunikat* Co)
{
	unsigned _N=Co->Nadawca();
	unsigned _O=Co->Odbiorca();
	if(	(_N==_S && _O==_T)     //Klasy potomne mogą być skierowane
		|| ( !Kierunkowy() && _N==_T && _O==_S) )
			return true;
			else
			return false;
}

/// Oblicza położenia wzdłuż linku. Nie uwzględnia specyfiki komunikatu bo nie może jej znać
/// Używa `virtual bool  Kierunkowy();`
void  GenerycznePowiazanie::PodajPozycje(double D,bool KierunekZwykly,double& X,double& Y,Komunikat*)
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

  /// Domyślny konstruktor ustawia pusty link o kolorze 0 i indeksach _S i _T `== -1`
  GenerycznePowiazanie::GenerycznePowiazanie() :  Waga(1)
  {
	  Col.ARGB = 0;
	  _S = _T = -1;
  }

  /// Metoda pobiera wszystkie potrzebne dane z listy stringów.
  bool GenerycznePowiazanie::ZrobWgListy( const std::string* Lista,
                                          unsigned Ile, unsigned& Blad)
  {
	  Dane.InicjujWgListy(Lista,Ile);
	  if(Dane.Ile()<5) { Blad=Ile; return false; } //Za mało danych. Pierwsza kom. za.
	//std::string*	Dane;
	//unsigned		IleDanych;
	//unsigned    	W,_S,_T;
	//float 		_Z;
	//DziedzKol  	Col;
	//	0					1				2				  3		  4 	5
	// typ(node/link)	Nazwa/Zrodlo	Wizualizacja/Cel	Waga	Kolor 	Z	Pole1	Pole2	Pole3	itd.
	// genlink				Wydzia� 			Uniwerek	1			0   0
	// genlink				Badacz A			Badacz B	0.5		00FF00   1
	//...
	//unsigned long strtoul(const char *s, char **endptr,int base);
	//int atoi(const char *s);
	//double atof(const char *s);
	if(Dane.KonwertujDo(3,Waga)!=-1) //	Waga=atof(Dane[3].c_str());
		{ Blad=3; return false;}

	char* endptr=NULL;
	Col.ARGB=strtorgb(Dane[4].c_str(),&endptr);
	if(endptr!=NULL && *endptr!='\0')
		{ Blad=4; return false;}

	if(Dane.KonwertujDo(5,_Z)!=-1)  	//_Z=atof(Dane[5].c_str());
		 { Blad=5; return false;}

	_S=Swiat::ZnajdzIndeksWezla(Dane[1].c_str());
	if(_S==Swiat::INV_INDEX)
		{ Blad=1; return false;}

	_T=Swiat::ZnajdzIndeksWezla(Dane[2].c_str());
	if(_T==Swiat::INV_INDEX)
		{ Blad=2; return false;}

	Blad=6; //Udało się wczytać do pola 5.

	return true;
  }

  /// Destruktor wirtualny, bo ma metody wirtualne
  GenerycznePowiazanie::~GenerycznePowiazanie()
  {
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


