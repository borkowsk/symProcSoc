/// \file
/// \brief Definicje podstawowego typu realnego węzła (nie pure virtual jak WezelSieci)
//         -----------------------------------------------------------------------------
///
/// \details
///              G...
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
/// \date
///     2022.11.07 (last updated)
// ///////////////////////////////////////////////////////////////////////////////////////
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
	//Konstruktor  pobiera wszystkie potrzebne dane z listy stringów
	GenerycznyWezelSieci(const std::string* Lista,unsigned Ile);

  //Specyficzne dla W�z�a
  private:
  std::string*   Dane;
  double    W,X,Y,Z;
  DziedzinaWKolorze  Col;
  Wielobok* Ksztalt;
};
*/

/// \return `true` jeżeli jest dobrze zdefiniowany. Musi posiadać zdefiniowany `Ksztalt`
bool GenerycznyWezelSieci::Poprawny()
{
    return Ksztalt!=NULL; //Kształt musi byc! I co jeszcze???
}

// Reszta to "technikalia"    ...
// ///////////////////////////////////////////////////////////////

/// Domyślny konstruktor ustala kolor  czarny a wszystkie parametry typu `float` na `-FLT_MAX`.
GenerycznyWezelSieci::GenerycznyWezelSieci():Ksztalt(NULL)
{
	Col.ARGB=0;
	Waga=_X=_Y=_Z=-FLT_MAX;
}

/// Interpretuje pola od 1 do 7 w `ZrobWgListy()`. Przykład danych do interpretacji poniżej:
/// | 0			    |   1			 |	2				 |  3	 |	  4    |	5 |	6	| 7
/// | typ(node/link)|	Nazwa/Zrodlo |	Wizualizacja/Cel |	Waga |	Kolor  | 	X |	Y	| Z	 | Pole1 | Pole2 | itd.
/// | gennode		|	Badacz A	 |		6			 |	0.5	 |	00FF00 |  100 |	20	| 3
/// ...
bool GenerycznyWezelSieci::_PrzeniesDaneNaPola(unsigned& Pol)
{
	//double    W,X,Y,Z;
	//DziedzinaWKolorze  Col;
	//Wielobok* Ksztalt;

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
			RR=1; //Jak za małe to chociaż "kropa"
	  Ksztalt=new Wielobok(Pom,RR); //`R` co najmniej 1!
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
		double Skala=this->R(0);  //Tyle powinno być
		Skala=Skala/R; //Czyli jak przeliczyć (zwiększyć czy zmniejszyć)
		Ksztalt->Skaluj(Skala,Skala);
	}

	Pol=8; //Udało się wczytać 7 pól! Wskazujemy na następne
	return true;
}

bool GenerycznyWezelSieci::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	Dane.InicjujWgListy(Lista,Ile);
	if(Dane.Ile()<7) { Blad=Ile; return false; } // Za mało danych. Pierwsza kom. za.
	return _PrzeniesDaneNaPola(Blad);   // Interpretuje konieczne pola
}

/// \return Promień otaczającego okręgu (lub elipsy przy nietypowym "aspect ratio")
double GenerycznyWezelSieci::R(double)
{
	return 1+((JakieRwProcWidth/100)*ver*Waga); //`R` co najmniej 1!
}

/// Oblicza trafienie w otaczające koło.
/// Np. do inspekcji myszką.
bool GenerycznyWezelSieci::Trafiony(float sX,float sY)
{
	double dist=distance(this->X(),sX,this->Y(),sY);
	if(dist<=this->R(0))
		return true;
		else
		return false;
}

/// Destruktor wirtualny usuwa Ksztalt.
GenerycznyWezelSieci::~GenerycznyWezelSieci()
{
   if(Ksztalt) delete Ksztalt;
}

/// Musi posiadać jakaś nazwę. A jak jednak nie to zwracane jest "<NONAME>".
const char* GenerycznyWezelSieci::Nazwa()
{
	if(Dane.Ile()>0)
	  return Dane[1].c_str();
	  else
	  return "<NONAME>";
}

/// Najprostszy sheduler priorytetowy.
/// Posunięcie do przodu jedynie najpilniejszego z procesów.
void GenerycznyWezelSieci::_RuszProcesPriorytetowy()
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

/// Sheduler w ogóle bezpriorytetowy.
/// Daje szanse na ruch każdemu procesowi
void GenerycznyWezelSieci::_RuszKazdyProces()
{
	unsigned NID=MojID();
	unsigned ile=Swiat::IleProcesow(NID);
	Proces*  Pr=NULL;
	for(unsigned i=0;i<ile;i++)
	  if((Pr=Swiat::Proc(i,NID))!=NULL )
		Pr->ChwilaDlaCiebie();
}

/// Obrobienie komunikatu przez pierwszy chętny proces.
/// \return `false` jeśli żaden proces się nie przyznał do tego komunikatu
bool GenerycznyWezelSieci::_KomunikatDoProcesow(Komunikat* Co)
{                                           					                  assert(Co->Kanal()!=Swiat::INV_INDEX);
	unsigned NID=MojID();
	unsigned ile=Swiat::IleProcesow(NID);
	Proces*  Pr=NULL;
	for(unsigned i=0;i<ile;i++)
	  if((Pr=Swiat::Proc(i,NID))!=NULL)
	  {                                                                           assert(Co->Kanal()!=Swiat::INV_INDEX);
		 if(Pr->InterpretujKomunikat(Co) )
		 {               //Tu już komunikat może być zniszczony
			return true; //Jak któryś proces uzna obróbkę za zakończoną
		 }
	  }
																                  assert(Co->Kanal()!=Swiat::INV_INDEX);
	return false; // Żaden się nie przyznał
}

/// Przekazuje komunikat losowo i z prawdopodobieństwem P namnaża go.
/// Dosyć to kosztowne, ale "generic node" nie może mieć pomocniczych struktur
/// bo jest bazą do dziedziczenia dla bardziej użytecznych typów węzłów.
void GenerycznyWezelSieci::_KomunikatPrzekazLosowo(Komunikat* Co,double P)
{
	//Przygotowanie listy linków zdatnych do przesyłania
	wb_dynarray<unsigned> ListaPow;
	unsigned IlePowFakt=0;
	ListaPow.alloc(Swiat::IlePowiazan());
	//Przepisywanie na listy
	for(unsigned i=0;i<Swiat::IlePowiazan();i++)
	{
		Powiazanie* Pom=Swiat::Lacze(i);
		if( Pom!=NULL  //Może być pusty slot
			&& ( (Swiat::Wezel(Pom->Poczatek())==this) //Gdzie jest? Za początkiem???
			|| (!Pom->Kierunkowy() && Swiat::Wezel(Pom->Koniec())==this)) //Albo końcem linku obustronnego
			)
			ListaPow[IlePowFakt++]=i;
	}

	if(IlePowFakt==0) // To są tylko takie, którymi można coś sięgnąć!!!
			return; //Nie ma którędy wysyłać


	// PRÓBY WYSYŁANIA
    // ////////////////

	//cout<<endl<<"Wezel: "<<Dane[1]<<" Linkow: "<<IlePowFakt<<endl;
	unsigned wyslano=0;
	unsigned probowano=0;
	wb_ptr<Komunikat> Klon( Co->Klonuj() );

	do
	{
	  unsigned a=RANDOM(IlePowFakt);
										assert(a<IlePowFakt);
	  if(ListaPow[a]==Swiat::INV_INDEX)
				continue; //Trafiony już używany slot

	  Powiazanie* Pom=Swiat::Lacze(ListaPow[a]);
	  bool Kierunek=Swiat::Wezel(Pom->Poczatek())==this;
	  Klon->Zaadresuj(ListaPow[a],Kierunek,0.01+DRAND()*0.2);

	  if( Pom->Akceptacja( Klon.get_ptr_val() )  )  //Jawnie sprawdza akceptacje
	  {
		 //cout<<Pom->Poczatek()<<"->"<<Pom->Koniec()<<' '<<Kierunek<<endl;
		 if(Swiat::WstawInfo(Klon.give())!=Swiat::INV_INDEX)
				wyslano++; //OK. Wstawia klon komunikatu tracąc go z "zarządu"

		 ListaPow[a]=Swiat::INV_INDEX; //Chwilowa blokada tego łącza

		 if( DRAND()<P )
			Klon.take(Co->Klonuj()); //Będzie potrzebna kolejna kopia
			else
			return; //Lub kończy...
	  }
	  probowano++; //zliczanie ile prób
	}
	while(probowano<IlePowFakt);
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


