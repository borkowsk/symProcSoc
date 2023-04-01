/// \file
/// \brief Implementacja generycznego procesu realizującego interface Proces
///        -----------------------------------------------------------------
///
/// \details
///              G...
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
/// \date
///     2022.11.07 (last updated)
// ///////////////////////////////////////////////////////////////////////////////////////

#pragma hdrstop

#include "spsGenProc.h"
#include "sshutils.hpp"

#include <iostream>
#include <cassert>
#include <cstring>
using namespace std;

#pragma package(smart_init) //???
/*
protected:
	int 	 Prior;              // Jaki ma priorytet działania
	unsigned WymaganyCzasPracy;  // Ile czasu ("roboczogodzin") musi działać, żeby być gotowym?
	unsigned DotychczasowyCzas;  // Ile już działał...
*/

//static              KonstruktorElementowModelu<GenerycznyProces> WirtualnyKonstruktor;
	GenerycznyProces::KonstruktorElementowModelu<GenerycznyProces> GenerycznyProces::WirtualnyKonstruktor("GenProc");

/// Konstruktor domyślny musi zapewnić żeby proces z raz chociaż dostał czas i mogł się zmienić
GenerycznyProces::GenerycznyProces():Prior(1),WymaganaIloscPracy(1),PracaDotychczasowa(0),TmpPorcjaPracy(1)
{_IleKrokowDoKonca=1;}

/// Konstruktor kopiujący wzór podajny jako wskaźnik.
/// `_IleKrokowDoKonca` raczej trzeba to zmienić/uściślić potem.
GenerycznyProces::GenerycznyProces(const GenerycznyProces* Wzor):
				   WymaganaIloscPracy(Wzor->WymaganaIloscPracy),
				   Prior(Wzor->Prior),PracaDotychczasowa(0),TmpPorcjaPracy(Wzor->TmpPorcjaPracy)
{
	if(TmpPorcjaPracy>0)
		_IleKrokowDoKonca=WymaganaIloscPracy/TmpPorcjaPracy; //Raczej trzeba to zmienić potem
	 Dane.InicjujZDanych(Wzor->Dane);
	 Col=(Wzor->Col); //!!! DZIEDZINA!
}

/// Konstruktor do normalnego tworzenia normalnie.
GenerycznyProces::GenerycznyProces(const char* Nazwa,float KoniecznaPraca,float DoDeadlinu):
				   WymaganaIloscPracy(KoniecznaPraca),
				   Prior(1),PracaDotychczasowa(0),TmpPorcjaPracy(1)
{
	_IleKrokowDoKonca=DoDeadlinu;
	Dane.Realokuj(7);
	Dane[1]=Nazwa;
}

/// Musi miec nazwę. \return "<???>" jak jednak nie ma.
const char* GenerycznyProces::Nazwa()
{
	if(Dane.Ile()>2)
		return Dane[1].c_str();
		else
		return "<???>";
}

/// Destruktor wirtualny jawnie nie ma nic do roboty.
GenerycznyProces::~GenerycznyProces()
{
}

/// Im WYŻSZY priorytet tym proces WIĘCEJ ma szans na działanie.
/// Ćwierć dnia pracy jednej osoby to minimalna porcja pracy (`TmpPorcjaPracy`)
double   GenerycznyProces::Priorytet()
{
	double IleDoZrobienia=WymaganaIloscPracy-PracaDotychczasowa;

	if(TmpPorcjaPracy<=0.25) // Ćwierć czy pół dnia praco-osoby to minimum?
					TmpPorcjaPracy=0.25;

	double NaIleKrokow=IleDoZrobienia/TmpPorcjaPracy;

	double NewPrior=-1;
	if(_IleKrokowDoKonca<=0)
	{                            // A ten jest już po terminie
		NewPrior=IleDoZrobienia; // NaIleKrokow???
			                     // Chciałby więcej niż można?
	}
	else
	{
		NewPrior=NaIleKrokow/_IleKrokowDoKonca; // A ten rozsądnie planowany
			                             // Ile czasu pracy chciałby na dziś?
	}

	Prior=NewPrior;

	return Prior;
}

// Specyficzne dla Procesu-projektu
// ////////////////////////////////

/// \return 0 po starcie, jak 1 to już zakończony. Może też zwrócić wartość `PROCES_NAD_ZAAWANSOWANY`.
/// \note Jeśli `TmpPorcjaPracy<0` to proces markowany do likwidacji (`PROCES_NAD_ZAAWANSOWANY`)
double   GenerycznyProces::JakZaawansowany()
{
	if(TmpPorcjaPracy<0) //WYMUSZANIE ZAKOŃCZENIA
		return  PROCES_NAD_ZAAWANSOWANY;
		else
		if(WymaganaIloscPracy>0)
			return  double(PracaDotychczasowa)/double(WymaganaIloscPracy);
				else
				return 1.0001; // Odrobinę większe niż 1 to proces przeterminowany. Gotowy do kasacji.
}

/// Daje procesowi możliwość zmiany stanu albo realnie zrobienia czegoś
void GenerycznyProces::ChwilaDlaCiebie()
{
   PracaDotychczasowa+=TmpPorcjaPracy; //Dolicza porcje pracy na to wywołanie

   if(_IleKrokowDoKonca<-1)
   {
	  unsigned   MyPrId=this->Procesor();
	  WezelSieci* MyPr=Swiat::Wezel(MyPrId);
//	  std::cout<<endl<<"Proces:"<<Dane[1]<<"@"<<MyPr->Nazwa()<<_LPL(" - limit czasu"," - time limit")<<' '<<_CzasuDoKonca<<endl;
   }
}

/// \return `true` jeśli jest dobrze zdefiniowany. Tu jeśli wciąź istnieje procesor.
bool GenerycznyProces::Poprawny()

{
	unsigned procesorek=Procesor();
	return procesorek!=Swiat::INV_INDEX && Swiat::Wezel(procesorek) != NULL;
}

/// Metoda pobiera wszystkie potrzebne dane z listy stringów. Jak błąd to podaje, która pozycja listy.
/// |		0	   |	   1   |	2	  |		3	  |	   4	         |	 5	        |	6        |   7      |
///	| #typ procesu |	Nazwa  | Procesor |	Priorytet |	  Kolor 	     | Zaawansowany |	Wymagany | Deadline |
///	| genproc	   |      BST  | Badacz A |	   2	  | argb(1,64,10,32) |	50	        |   100      |  100     |
bool GenerycznyProces::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	Dane.InicjujWgListy(Lista,Ile);
	if(Dane.Ile()<7) { Blad=Ile; return false; } //Za mało danych. Pierwsza kom. za.

	char* endptr=NULL;
	Col.ARGB=strtorgb(Dane[4].c_str(),&endptr);
	if(endptr!=NULL && *endptr!='\0')
		{ Blad=4; return false;}

	unsigned JakiProcesor=-1; //Tymczasowy indeks procesora
	if(!Dane.KonwertujDo(2,JakiProcesor))
	{
		JakiProcesor=Swiat::ZnajdzIndeksWezla(Dane[2].c_str());
		if(JakiProcesor==Swiat::INV_INDEX) { Blad=2;return false;}
	}

    __ChceWezelNr(JakiProcesor);

	//I reszta danych
	if(!Dane.KonwertujDo(3,Prior)) { Blad=3; return false;}
	if(!Dane.KonwertujDo(5,PracaDotychczasowa)) { Blad=5; return false;}
	if(!Dane.KonwertujDo(6,WymaganaIloscPracy)) { Blad=6; return false;}
	if(!Dane.KonwertujDo(7,_IleKrokowDoKonca)) 		{ Blad=7; return false;}  //Czas do konca jest polem z klasy "Proces" !!!
	Blad=8; //Udało się wczytać pole nr 7
	return true;
}

//	float WymaganaIloscPracy;   //Ile "roboczogodzin" musi działać, żeby się zakończyć
//	float PracaDotychczasowa;   //Ile już zrobiono
//	float CzasuDoKonca;         //Ile kroków czasu zostało do deadline-u


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


