////////////////////////////////////////////////////////////////////////////////
// Symulator Procesów Sieciowych/Spolecznych (c) Instytut Studiów Spo³ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
////////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
////////////////////////////////////////////////////////////////////////////////
//
// Definicje podstawowego typu realnego wêz³a (nie pure virtual jak WezelSieci)
////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <cassert>
#include <cfloat>
#include <cstdlib>
#include <cmath>
using namespace std;

#include "spsGenNode.h"

#include "MISCCLASSES/Wieloboki.hpp"
#include "SYMSHELL/sshutils.hpp"
#include "INCLUDE/wb_ptr.hpp"
using namespace wbrtm;

GenerycznyWezelSieci::KonstruktorElementowModelu<GenerycznyWezelSieci> GenerycznyWezelSieci::WirtualnyKonstruktor("GenNode");

/*
class GenerycznyWezelSieci:public WezelSieci
{
  public:
	//Konstruktor  pobiera wszystkie potrzebne dane z listy stringów
	GenerycznyWezelSieci(const std::string* Lista,unsigned Ile);

  //Specyficzne dla Wêz³a
  private:
  std::string*   Dane;
  double    W,X,Y,Z;
  DziedzinaWKolorze  Col;
  Wielobok* Ksztalt;
};
*/

bool GenerycznyWezelSieci::Poprawny()
//true jeœli jest dobrze zdefiniowany
{
    return Ksztalt!=NULL;//Kszta³t musi byc! I co jeszcze???
}

//Reszta to "technikalia"    ...
/////////////////////////////////////////////////////////////////
GenerycznyWezelSieci::GenerycznyWezelSieci():Ksztalt(NULL)
{
	Col.ARGB=0;
	Waga=_X=_Y=_Z=-FLT_MAX;
}

bool GenerycznyWezelSieci::_PrzeniesDaneNaPola(unsigned& Pol)
//Interpretuje pola od 1 do 7 w ZrobWgListy
{
	//double    W,X,Y,Z;
	//DziedzinaWKolorze  Col;
	//Wielobok* Ksztalt;
	//	0					1				2				  3		  4 	5	6	7
	//typ(node/link)	Nazwa/Zrodlo	Wizualizacja/Cel	Waga	Kolor 	X	Y	Z	Pole1	Pole2	Pole3	itd.
	//gennode			Badacz A			6				0.5		00FF00	100	20	3
	//...

	//Waga=atof(Dane[3].c_str());if(Waga<=0)
	if(Dane.KonwertujDo(3,Waga)!=-1) { Pol=3; return false;}
	//_X=atof(Dane[5].c_str());if(_X<=0)
	if(Dane.KonwertujDo(5,_X)!=-1){ Pol=5; return false;}
	//_Y=atof(Dane[6].c_str());if(_Y<=0)
	if(Dane.KonwertujDo(6,_Y)!=-1){ Pol=6; return false;}
	//_Z=atof(Dane[7].c_str());if(_Z<=0)
	if(Dane.KonwertujDo(7,_Z)!=-1){ Pol=7; return false;}

	char* endptr=NULL;
	Col.ARGB=strtorgb(Dane[4].c_str(),&endptr);
	if(endptr!=NULL && *endptr!='\0'){ Pol=4; return false;}


	if(isdigit(Dane[2][0]))
	{
	  int Pom;// =atoi(Dane[2].c_str());
	  if(Dane.KonwertujDo(2,Pom)!=-1)
					{ Pol=2; return false;}
	  double RR=R(0);
	  if(RR<1.0)
			RR=1; //Jak za ma³e to chocia¿ "kropa"
	  Ksztalt=new Wielobok(Pom,RR);//R co najmniej 1!
	}
	else
	{
		const Wielobok* Wzor=Wielobok::SprobujWielobok(Dane[2].c_str());
		if(Wzor==NULL)
			{ Pol=2; return false;}
		Ksztalt=new Wielobok(*Wzor);
		Ksztalt->Centruj();
		double  mX,mY,mxX,mxY,R;
		Ksztalt->Zakresy(mX,mY,mxX,mxY,R);
		double Skala=this->R(0);  //Tyle powinno byæ
		Skala=Skala/R; //Czyli jak przeliczyæ (zwiêkszyæ czy zmniejszyæ)
		Ksztalt->Skaluj(Skala,Skala);
	}

	Pol=8; //Uda³o siê wczytaæ 7 pol! Wskazujemy na nastepne
	return true;
}

bool GenerycznyWezelSieci::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	Dane.InicjujWgListy(Lista,Ile);
	if(Dane.Ile()<7) { Blad=Ile; return false; } //Za ma³o danych. Pierwsza kom. za.
	return _PrzeniesDaneNaPola(Blad);   //Interpretuje konieczne pola
}

double GenerycznyWezelSieci::R(double)
//Promieñ otaczaj¹cego okrêgu lub elipsy
{
	return 1+((JakieRwProcWidth/100)*ver*Waga); //R co najmniej 1!
}

bool GenerycznyWezelSieci::Trafiony(float sX,float sY)
//Np. do inspekcji myszk¹ - trafienie w otaczaj¹ce ko³o
{
	double dist=distance(this->X(),sX,this->Y(),sY);
	if(dist<=this->R(0))
		return true;
		else
		return false;
}

GenerycznyWezelSieci::~GenerycznyWezelSieci()
//Destruktor wirtualny
{
   if(Ksztalt) delete Ksztalt;
}

const char* GenerycznyWezelSieci::Nazwa()
//Musi posiadaæ jakas nazwe
{
	if(Dane.Ile()>0)
	  return Dane[1].c_str();
	  else
	  return "<NONAME>";
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

