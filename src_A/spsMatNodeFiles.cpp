////////////////////////////////////////////////////////////////////////////////
// Symulator Procesów Sieciowych/Spolecznych (c) Instytut Studiów Spo³ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
////////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
////////////////////////////////////////////////////////////////////////////////
//
// Wczytywanie z plików dla matrycowego typu wêz³a -
// czyli zawieraj¹cego tablicê "dziedzin"
// - wizualizowana jako obrazek i ewentualnie obrazkiem zainicjowana
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------

#pragma hdrstop

#include "spsMatrixNode.h"
#include "INCLUDE/platform.hpp"
#include "MISCCLASSES/TabelaTabDeli.h"
#include "SYMSHELL/sshutils.hpp"
#include <iostream>
#include <cassert>
//#include <cstring>
//#include <stdlib.h>

using namespace std;

//------------------------------------------------------------------------------
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



bool WezelMacierzowy::_ZaladujKoloryZPliku(const char* NazwaPliku,const char* TekstPomocniczy/*=NULL*/)
//Procedura ³aduj¹ca dane
//zwraca "true" jeœli siê uda, false jak nie (komunikat mo¿e byæ na perror)
//z plików DAT (tab delimited), i ewentualnie graficznym PBM
// - jest w osobnym Ÿródle wiêc mo¿na przeimplementowaæ
{
	//Na razie tylko czytanie z pliku DAT - tab delimited
	 TabelaTabDelimited PlikDat;   //Po u¿yciu automatycznie dealokowane
	 if(!PlikDat.WczytajZPliku(NazwaPliku))
		{cerr<<endl<<_LPL("Nieudane wczytywanie z pliku","Failed to read the file")<<" '"<<NazwaPliku<<"'"<<endl; return false;}
	 unsigned Szerokosc=PlikDat.IleKolumn();
	 unsigned Wysokosc=PlikDat.IleWierszy();
	 unsigned index=0;
	 if(Tablica) delete [] Tablica; //Zwalnianie starej pamiêci
	 Tablica=new DziedzinaWKolorze[Szerokosc*Wysokosc];
	 this->Szerokosc=Szerokosc;
	 this->Wysokosc=Wysokosc;
	 for(unsigned w=0;w<Wysokosc;w++)
		for(unsigned s=0;s<Szerokosc;s++)
		{
			char* endptr=NULL;
			Tablica[index].ARGB=strtorgb(PlikDat(w,s).c_str(),&endptr);
			if(endptr!=NULL && *endptr!='\0')
				{ cerr<<_LPL("Plik","File")<<"'"<<NazwaPliku<<"'"<<endl
					<<_LPL("Niepoprawna komorka","Invalid cell")<<" : "<<char('A'+s)<<' '<<w+1<<endl; return false;}
			index++;
		}
	 return true;//Doszed³ tu wiêc siê uda³o
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
