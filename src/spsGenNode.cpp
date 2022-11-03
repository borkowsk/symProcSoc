// //////////////////////////////////////////////////////////////////////////////
// Symulator Proces�w Sieciowych/Spolecznych (c) Instytut Studi�w Spo�ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
// //////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
// //////////////////////////////////////////////////////////////////////////////
//
// Definicje podstawowego typu realnego w�z�a (nie pure virtual jak WezelSieci) 27.10.2011
// //////////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <cassert>
#include <cfloat>
#include <cstdlib>
#include <cmath>
using namespace std;

#include "spsGenNode.h"

#include "wieloboki.hpp"
#include "sshutils.hpp"
#include "wb_ptr.hpp"
using namespace wbrtm;

GenerycznyWezelSieci::KonstruktorElementowModelu<GenerycznyWezelSieci> GenerycznyWezelSieci::WirtualnyKonstruktor("GenNode");

/*
class GenerycznyWezelSieci:public WezelSieci
{
  public:
	//Konstruktor  pobiera wszystkie potrzebne dane z listy string�w
	GenerycznyWezelSieci(const std::string* Lista,unsigned Ile);

  //Specyficzne dla W�z�a
  private:
  std::string*   Dane;
  double    W,X,Y,Z;
  DziedzinaWKolorze  Col;
  Wielobok* Ksztalt;
};
*/

bool GenerycznyWezelSieci::Poprawny()
//true je�li jest dobrze zdefiniowany
{
    return Ksztalt!=NULL;//Kszta�t musi byc! I co jeszcze???
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
			RR=1; //Jak za ma�e to chocia� "kropa"
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
		double Skala=this->R(0);  //Tyle powinno by�
		Skala=Skala/R; //Czyli jak przeliczy� (zwi�kszy� czy zmniejszy�)
		Ksztalt->Skaluj(Skala,Skala);
	}

	Pol=8; //Uda�o si� wczyta� 7 pol! Wskazujemy na nastepne
	return true;
}

bool GenerycznyWezelSieci::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	Dane.InicjujWgListy(Lista,Ile);
	if(Dane.Ile()<7) { Blad=Ile; return false; } //Za ma�o danych. Pierwsza kom. za.
	return _PrzeniesDaneNaPola(Blad);   //Interpretuje konieczne pola
}

double GenerycznyWezelSieci::R(double)
//Promie� otaczaj�cego okr�gu lub elipsy
{
	return 1+((JakieRwProcWidth/100)*ver*Waga); //R co najmniej 1!
}

bool GenerycznyWezelSieci::Trafiony(float sX,float sY)
//Np. do inspekcji myszk� - trafienie w otaczaj�ce ko�o
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
//Musi posiada� jakas nazwe
{
	if(Dane.Ile()>0)
	  return Dane[1].c_str();
	  else
	  return "<NONAME>";
}

void GenerycznyWezelSieci::_RuszProcesPriorytetowy()
//Posuni�cie do przodu najpilniejszego z proces�w
{
	unsigned ile=Swiat::IleProcesow(MojID());
	unsigned ktory=Swiat::NajpilniejszyProc(MojID(),NULL);
	if(ktory==Swiat::INV_INDEX) //Nie ma nic do roboty
			return; //To wychodzimy z pracy :-);

	Proces* Roboczy=Swiat::Proc(ktory,MojID());
	//Roboczy->  Mo�na by ustawia� porcj� czasu, ale og�lny proces nie ma (jeszcze) takiej opcji
	if(Roboczy)
		Roboczy->ChwilaDlaCiebie(); //Popchnij
}

void GenerycznyWezelSieci::_RuszKazdyProces()
//Daje szanse na ruch kazdemu procesowi
{
	unsigned NID=MojID();
	unsigned ile=Swiat::IleProcesow(NID);
	Proces*  Pr=NULL;
	for(unsigned i=0;i<ile;i++)
	  if((Pr=Swiat::Proc(i,NID))!=NULL )
		Pr->ChwilaDlaCiebie();
}

bool GenerycznyWezelSieci::_KomunikatDoProcesow(Komunikat* Co)
//Obrobienie komunikatu przez pierwszy ch�tny proces
{                                           					assert(Co->Kanal()!=Swiat::INV_INDEX);
	unsigned NID=MojID();
	unsigned ile=Swiat::IleProcesow(NID);
	Proces*  Pr=NULL;
	for(unsigned i=0;i<ile;i++)
	  if((Pr=Swiat::Proc(i,NID))!=NULL)
	  {                                                         assert(Co->Kanal()!=Swiat::INV_INDEX);
		 if(Pr->InterpretujKomunikat(Co) )
		 {              //Tu ju� komunikat mo�e by� ziszczony
			return true;//Jak kt�ry� proces uzna� obr�bk� za zako�czon�
		 }
	  }
																assert(Co->Kanal()!=Swiat::INV_INDEX);
	return false; //Zaden si� nie przyzna�
}

void GenerycznyWezelSieci::_KomunikatPrzekazLosowo(Komunikat* Co,double P)
//Przekazuje komunikat losowo i z prawdopodobie�stwem P namna�a
//Dosy� to kosztowne, ale "generic node" nie mo�e mie� pomocniczych struktur
//bo jest baz� do dziedziczenia dla bardziej u�ytecznych typ�w w�z��w
{
	//Przygotowanie listy powi�za� zdatnych do przesy�ania
	wb_dynarray<unsigned> ListaPow;
	unsigned IlePowFakt=0;
	ListaPow.alloc(Swiat::IlePowiazan());
	//Przepisywanie na list�
	for(unsigned i=0;i<Swiat::IlePowiazan();i++)
	{
		Powiazanie* Pom=Swiat::Lacze(i);
		if( Pom!=NULL  //Mo�e by� pusty slot
			&& ( (Swiat::Wezel(Pom->Poczatek())==this) //Gdzie jest pocz�tkiem
			|| (!Pom->Kierunkowy() && Swiat::Wezel(Pom->Koniec())==this)) //Albo koncem linku obustronnego
			)
			ListaPow[IlePowFakt++]=i;
	}

	if(IlePowFakt==0) //To s� tylko takie kt�rymi mo�na co� dosta�!!!
			return; //To si� wys�a� nie da nic.


	// PR�BY WYSY�ANIA
	//cout<<endl<<"Wezel: "<<Dane[1]<<" Linkow: "<<IlePowFakt<<endl;
	unsigned wyslano=0;
	unsigned probowano=0;
	wb_ptr<Komunikat> Klon( Co->Klonuj() );

	do
	{
	  unsigned a=RANDOM(IlePowFakt);
										assert(a<IlePowFakt);
	  if(ListaPow[a]==Swiat::INV_INDEX)
				continue; //Trafiony ju� u�ywany slot

	  Powiazanie* Pom=Swiat::Lacze(ListaPow[a]);
	  bool Kierunek=Swiat::Wezel(Pom->Poczatek())==this;
	  Klon->Zaadresuj(ListaPow[a],Kierunek,0.01+DRAND()*0.2);

	  if( Pom->Akceptacja( Klon.get_ptr_val() )  )  //Jawnie sprawdza akceptacje
	  {
		 //cout<<Pom->Poczatek()<<"->"<<Pom->Koniec()<<' '<<Kierunek<<endl;
		 if(Swiat::WstawInfo(Klon.give())!=Swiat::INV_INDEX)
				wyslano++;//OK. Wstawi� klon komunikatu trac�c z "zarz�du"

		 ListaPow[a]=Swiat::INV_INDEX;//Blokada tego ��cza

		 if( DRAND()<P )
			Klon.take(Co->Klonuj()); //B�dzie potrzebna kolejna kopia
			else
			return; //Lub konczy...
	  }
	  probowano++;
	}
	while(probowano<IlePowFakt);
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

