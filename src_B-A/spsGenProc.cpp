////////////////////////////////////////////////////////////////////////////////
// Symulator Procesów Sieciowych/Spolecznych (c) Instytut Studiów Spo³ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
////////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
////////////////////////////////////////////////////////////////////////////////
//
//---------------------------------------------------------------------------
#pragma hdrstop

#include "spsGenProc.h"
#include "SYMSHELL/sshutils.hpp"
//---------------------------------------------------------------------------
#include "INCLUDE/platform.hpp"
#include <iostream>
#include <cassert>
#include <cstring>
using namespace std;

#pragma package(smart_init)
/*
protected:
	int 	 Prior;//Jaki ma priorytet dzia³ania
	unsigned WymaganyCzasPracy;//Ile czasu ("roboczogodzin") musi dzia³aæ ¿eby byæ gotowy
	unsigned DotychczasowyCzas;//Ile ju¿ dzia³a³...
*/

//static              KonstruktorElementowModelu<GenerycznyProces> WirtualnyKonstruktor;
	GenerycznyProces::KonstruktorElementowModelu<GenerycznyProces> GenerycznyProces::WirtualnyKonstruktor("GenProc");

GenerycznyProces::GenerycznyProces():Prior(1),WymaganaIloscPracy(1),PracaDotychczasowa(0),TmpPorcjaPracy(1)
//Konstruktor musi zapewniæ ¿eby proces z raz dosta³ czas i mog³ siê zmienic
{_IleKrokowDoKonca=1;}

GenerycznyProces::GenerycznyProces(const GenerycznyProces* Wzor):
				   WymaganaIloscPracy(Wzor->WymaganaIloscPracy),
				   Prior(Wzor->Prior),PracaDotychczasowa(0),TmpPorcjaPracy(Wzor->TmpPorcjaPracy)
{
	if(TmpPorcjaPracy>0)
		_IleKrokowDoKonca=WymaganaIloscPracy/TmpPorcjaPracy; //Raczej trzeba to zmienieæ  potem
	 Dane.InicjujZDanych(Wzor->Dane);
	 Col=(Wzor->Col); //!!! DZIEDZINA!
}

GenerycznyProces::GenerycznyProces(const char* Nazwa,float KoniecznaPraca,float DoDeadlinu):
				   WymaganaIloscPracy(KoniecznaPraca),
				   Prior(1),PracaDotychczasowa(0),TmpPorcjaPracy(1)
//I do tworzenia normalnie
{
	_IleKrokowDoKonca=DoDeadlinu;
	Dane.Realokuj(7);
	Dane[1]=Nazwa;
}

const char* GenerycznyProces::Nazwa()
{
	if(Dane.Ile()>2)
		return Dane[1].c_str();
		else
		return "<???>";
}

GenerycznyProces::~GenerycznyProces()
//Destruktor wirtualny
{
}

double   GenerycznyProces::Priorytet()
//Im wy¿szy tym proces wiêcej dziala
{
	double IleDoZrobienia=WymaganaIloscPracy-PracaDotychczasowa;

	if(TmpPorcjaPracy<=0.25)//Cwierc czy po³ dnia pracy osoby to minimum
					TmpPorcjaPracy=0.25;

	double NaIleKrokow=IleDoZrobienia/TmpPorcjaPracy;

	double NewPrior=-1;
	if(_IleKrokowDoKonca<=0)
	{        //A ten jest ju¿ po terminie
		NewPrior=IleDoZrobienia; //NaIleKrokow???
			//Chcia³by wiêcej ni¿ mo¿na
	}
	else
	{
		NewPrior=NaIleKrokow/_IleKrokowDoKonca; //A ten rozs¹dnie planowany
			//Ile czasu pracy chcia³by na dziœ
	}

	Prior=NewPrior;

	return Prior;
}

//Specyficzne dla Procesu-projektu
double   GenerycznyProces::JakZaawansowany()
//0 po starcie, jak 1 to ju¿ zakoñczony
{
	if(TmpPorcjaPracy<0) //WYMUSZANIE ZAKOÑCZENIA
		return  PROCES_NAD_ZAAWANSOWANY;
		else
		if(WymaganaIloscPracy>0)
			return  double(PracaDotychczasowa)/double(WymaganaIloscPracy);
				else
				return 1.0001;//¯eby wiêksze od 1
}

void GenerycznyProces::ChwilaDlaCiebie()
//Daje procesowi mo¿liwoœæ zmiany stanów - zrobienia czegoœ
{
   PracaDotychczasowa+=TmpPorcjaPracy; //Dolicza porcje pracy na to wywo³anie

   if(_IleKrokowDoKonca<-1)
   {
	  unsigned   MyPrId=this->Procesor();
	  WezelSieci* MyPr=Swiat::Wezel(MyPrId);
//	  std::cout<<endl<<"Proces:"<<Dane[1]<<"@"<<MyPr->Nazwa()<<_LPL(" - limit czasu"," - time limit")<<' '<<_CzasuDoKonca<<endl;
   }
}

bool GenerycznyProces::Poprawny()
//true jeœli jest dobrze zdefiniowany (wci¹¿ istnieje procesor)
{
	unsigned procesorek=Procesor();
	return procesorek!=Swiat::INVINDEX && Swiat::Wezel(procesorek)!=NULL;
}

//Metoda pobiera wszystkie potrzebne dane z listy stringów. Jak blad to podaje ktora pozycja listy
bool GenerycznyProces::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	Dane.InicjujWgListy(Lista,Ile);
	if(Dane.Ile()<7) { Blad=Ile; return false; } //Za ma³o danych. Pierwsza kom. za.
//		0			   1		2			3		   4		5				6           7
//	#typ procesu	Nazwa	Procesor	Priorytet	Kolor 	Zaawansowany	Wymagany      Deadline
//	genproc	         BST	Badacz A	2	argb(1,64,10,32)	50	          100           100
	char* endptr=NULL;
	Col.ARGB=strtorgb(Dane[4].c_str(),&endptr);
	if(endptr!=NULL && *endptr!='\0')
		{ Blad=4; return false;}

	unsigned JakiProcesor=-1;//Tymczasowy indeks procesora
	if(!Dane.KonwertujDo(2,JakiProcesor))
	{
		JakiProcesor=Swiat::ZnajdzIndeksWezla(Dane[2].c_str());
		if(JakiProcesor==Swiat::INVINDEX) { Blad=2;return false;}
	}
	__ChceWezelNr(JakiProcesor);
	//I reszta danych
	if(!Dane.KonwertujDo(3,Prior)) { Blad=3; return false;}
	if(!Dane.KonwertujDo(5,PracaDotychczasowa)) { Blad=5; return false;}
	if(!Dane.KonwertujDo(6,WymaganaIloscPracy)) { Blad=6; return false;}
	if(!Dane.KonwertujDo(7,_IleKrokowDoKonca)) 		{ Blad=7; return false;}  //Czas do konca jest polem z klasy "Proces" !!!
	Blad=8; //Uda³o siê wczytac pole nr 7
	return true;
}

//	float WymaganaIloscPracy;//Ile "roboczogodzin" musi dzia³aæ ¿eby byæ gotowy
//	float PracaDotychczasowa;//Ile ju¿ zrobiono
//	float CzasuDoKonca;//Ile kroków czasu zosta³o do deadlinu



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

