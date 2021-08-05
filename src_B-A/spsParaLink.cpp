////////////////////////////////////////////////////////////////////////////////
// Symulator Procesów Sieciowych/Spolecznych (c) Instytut Studiów Spo³ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
///////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
////////////////////////////////////////////////////////////////////////////////
//
// Deklaracja/Definicja linku ogólnego, ale rysowanego jako ³uk
////////////////////////////////////////////////////////////////////////////////

#pragma hdrstop

#include "spsParaLink.h"
#include <cassert>
#define _USE_MATH_DEFINES /*For MSVC*/
#include <cmath>
#include <iostream>
using namespace std;

#include "INCLUDE/wb_smartlog.h"
using namespace wbrtm;

//---------------------------------------------------------------------------
PowiazaniePaboliczne::KonstruktorElementowModelu<PowiazaniePaboliczne>
						PowiazaniePaboliczne::WirtualnyKonstruktor("ParaLink");
PowiazaniePaboliczneSkierowane::KonstruktorElementowModelu<PowiazaniePaboliczneSkierowane>
			PowiazaniePaboliczneSkierowane::WirtualnyKonstruktor("DirParaLink");


bool PowiazaniePaboliczne::Poprawny()
{
	if(fabs(parametr)>10)
			{
				TLOG(0, <<"PowiazaniePaboliczne::Parametr="<<parametr<<" ZA DU¯Y???" )
				return false;
			}
	return GenerycznePowiazanie::Poprawny();
}

PowiazaniePaboliczne::~PowiazaniePaboliczne()
{
   //Nie musi robiæ nic - robotê wykonuje destruktor klasy bazowej
}

PowiazaniePaboliczne::PowiazaniePaboliczne()
//Domyslny konstruktor ustawiaj¹cy pusty link
{
	parametr=1;		krokow=0;
	Xa=Ya=Xb=Yb=Promien=alfa=0;
}

PowiazaniePaboliczne::PowiazaniePaboliczne(double par)
//Domyslny konstruktor ustawiaj¹cy pusty link Z PARAMETREM
{
	parametr=par;	krokow=0;
	Xa=Ya=Xb=Yb=Promien=alfa=0;
}

void PowiazaniePaboliczne::UstawWygiecie(double par)
//Zmienia parametr wiêc i pomocnicze zmienne siê musz¹ zmienic
{
	parametr=par;	krokow=0;
	Xa=Ya=Xb=Yb=Promien=alfa=0;
}

//Metoda pobiera wszystkie potrzebne dane z listy stringów. Jak blad to podaje ktora pozycja
bool PowiazaniePaboliczne::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
   if(!GenerycznePowiazanie::ZrobWgListy(Lista,Ile,Blad))
						 return false;
   //Teraz zostaje ustalenie parametru - jak jest zdefiniowany
   if(Dane.KonwertujDo(6,parametr)!=-1)
				{ Blad=6 ;return false;}
   Blad=7;//6 pole uda³o siê wczytaæ
   return true;
}

const double Eps=1;

void PowiazaniePaboliczne::_PoliczParametryLuku()//Liczy parametry ³uku dla danej wartosci parametru
//Pomocnicze pola zawieraj¹ce niezbêdne parametry ³uku
//	double Xa,Ya,Xb,Yb,Xo,Yo,Promien,alfa,beta;
{
   WezelSieci* Pocz=Swiat::Wezel(_S);          assert(Pocz!=NULL);
   WezelSieci* Koni=Swiat::Wezel(_T);          assert(Koni!=NULL);
   Xa=Pocz->X();
   Ya=Pocz->Y();
   Xb=Koni->X();
   Yb=Koni->Y();

   if(Xa==Xb && Ya==Yb) //Bardzo z³oœliwa sytuacja
		{Promien=0;krokow=0; return;} //NIC SIE NIE DA ZROBIC

   //Liczenie wspó³rzêdnych œrodka okrêgu z którego bierzemy ³uk

   if(fabs(Xa-Xb)<Eps)// tylko ¿e w pionie
   {
		  if(Ya<Yb)	alfa=M_PI/2;
		  else      alfa=1.5*M_PI;
   }
   else //Jednak trzeba trochê bardziej policzyæ
   {
	   //alfa=atan2( (Xa-Xb),(Ya-Yb) );// nie dzia³a  ?????
	   //alfa=atan2( (Xb-Xa),(Yb-Ya) );// nie dzia³a ?????
	   //!!!! atan2( Y , X ) - funkcja wymaga najpierw Y, potem X!!!!!!!!!!!!!
	   //Procedura do korekty, ale na razie dzia³a, choæ nie wiem dlaczego!
											if((Xb-Xa)==0 && (Ya-Yb)==0)
											{
												clog<<"Zbyt krotki link? X1:"<<Xa<<" Y1"<<Ya<<endl;
												Promien=0;krokow=0; return;
											}

	   alfa=atan2( (Xb-Xa),(Ya-Yb) )-M_PI/2;//?????  TAK DZIA£A!  WHY????
	  /* if(alfa<0)
		{
			alfa=2*M_PI-alfa-M_PI/2;
		}
		else	alfa+=M_PI/2;      */
   }

   cos_alfa=cos(alfa);
   sin_alfa=sin(alfa);
   Promien=sqrt((Xa-Xb)*(Xa-Xb)+(Ya-Yb)*(Ya-Yb));   	assert(Promien>0);
   krokow=Promien/5;//Na razie tak zgrubnie
   if(krokow<3) krokow=3;
}

//Tak naprawdê to ró¿ni siê tylko sposobem rysowania
//Który zalezy od funkcji licz¹cej punkty na ³uku
void PowiazaniePaboliczne::PodajPozycje(double D,bool KierunekZwykly,double& X,double& Y,Komunikat* Messg)
//Przelicza polozenia wdluz linku
{
   WezelSieci* Pocz=Swiat::Wezel(_S);
   WezelSieci* Koni=Swiat::Wezel(_T);
   if(Pocz->X()!=Xa || Pocz->Y()!=Ya || Koni->X()!=Xb || Koni->Y()!=Yb)
		   _PoliczParametryLuku();//Którys siê przesuna³ albo zmienil siê parametr
   if(krokow==0) //Coœ siê chyba nie uda³o
   {
	GenerycznePowiazanie::PodajPozycje(D,KierunekZwykly,X,Y,Messg);//RACZEJ TYMCZASOWO
	return;
   }

   //Wlasciwe obliczenie paraboli
   if(!KierunekZwykly)
				D=1-D;
   X=Xa+D*Promien;
   D=(D-0.5);
   double Przeliczony=-parametr*Promien;
   Y=Ya+(-Przeliczony*D*D+Przeliczony*0.25);
   //Y=Ya;

   //Obrót UWAGA! 90 stopni (alfa=M_PI) jest w dó³ a 270 w górê

   D=sqrt((Xa-X)*(Xa-X)+(Ya-Y)*(Ya-Y));
   X-=Xa;
   Y-=Ya;
   double Xf=X*cos_alfa-Y*sin_alfa;
   double Yf=X*sin_alfa+Y*cos_alfa;
   X=Xf+Xa;
   Y=Yf+Ya;

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


