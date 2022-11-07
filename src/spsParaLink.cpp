////////////////////////////////////////////////////////////////////////////////
// Symulator Proces�w Sieciowych/Spolecznych (c) Instytut Studi�w Spo�ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
///////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
////////////////////////////////////////////////////////////////////////////////
//
// Deklaracja/Definicja linku og�lnego, ale rysowanego jako �uk
////////////////////////////////////////////////////////////////////////////////

#pragma hdrstop

#include "spsParaLink.h"
#include <cassert>
#define _USE_MATH_DEFINES /*For MSVC*/
#include <cmath>
#include <iostream>
using namespace std;

#include "wb_smartlog.hpp"
using namespace wbrtm;

//---------------------------------------------------------------------------
PowiazanieParaboliczne::KonstruktorElementowModelu<PowiazanieParaboliczne>
						PowiazanieParaboliczne::WirtualnyKonstruktor("ParaLink");
PowiazanieParaboliczneSkierowane::KonstruktorElementowModelu<PowiazanieParaboliczneSkierowane>
			PowiazanieParaboliczneSkierowane::WirtualnyKonstruktor("DirParaLink");


bool PowiazanieParaboliczne::Poprawny()
{
	if(fabs(parametr)>10)
			{
				TLOG(0, <<"PowiazaniePaboliczne::Parametr="<<parametr<<" ZA DU�Y???" )
				return false;
			}
	return GenerycznePowiazanie::Poprawny();
}

PowiazanieParaboliczne::~PowiazanieParaboliczne()
{
   //Nie musi robi� nic - robot� wykonuje destruktor klasy bazowej
}

PowiazanieParaboliczne::PowiazanieParaboliczne()
//Domyslny konstruktor ustawiaj�cy pusty link
{
	parametr=1;		krokow=0;
	Xa=Ya=Xb=Yb=Promien=alfa=0;
}

PowiazanieParaboliczne::PowiazanieParaboliczne(double par)
//Domyslny konstruktor ustawiaj�cy pusty link Z PARAMETREM
{
	parametr=par;	krokow=0;
	Xa=Ya=Xb=Yb=Promien=alfa=0;
}

void PowiazanieParaboliczne::UstawWygiecie(double par)
//Zmienia parametr wi�c i pomocnicze zmienne si� musz� zmienic
{
	parametr=par;	krokow=0;
	Xa=Ya=Xb=Yb=Promien=alfa=0;
}

//Metoda pobiera wszystkie potrzebne dane z listy string�w. Jak blad to podaje ktora pozycja
bool PowiazanieParaboliczne::ZrobWgListy(const std::string* Lista, unsigned Ile, unsigned& Blad)
{
   if(!GenerycznePowiazanie::ZrobWgListy(Lista,Ile,Blad))
						 return false;
   //Teraz zostaje ustalenie parametru - jak jest zdefiniowany
   if(Dane.KonwertujDo(6,parametr)!=-1)
				{ Blad=6 ;return false;}
   Blad=7;//6 pole uda�o si� wczyta�
   return true;
}

const double Eps=1;

void PowiazanieParaboliczne::_PoliczParametryLuku()//Liczy parametry �uku dla danej wartosci parametru
//Pomocnicze pola zawieraj�ce niezb�dne parametry �uku
//	double Xa,Ya,Xb,Yb,Xo,Yo,Promien,alfa,beta;
{
   WezelSieci* Pocz=Swiat::Wezel(_S);          assert(Pocz!=NULL);
   WezelSieci* Koni=Swiat::Wezel(_T);          assert(Koni!=NULL);
   Xa=Pocz->X();
   Ya=Pocz->Y();
   Xb=Koni->X();
   Yb=Koni->Y();

   if(Xa==Xb && Ya==Yb) //Bardzo z�o�liwa sytuacja
		{Promien=0;krokow=0; return;} //NIC SIE NIE DA ZROBIC

   //Liczenie wsp�rz�dnych �rodka okr�gu z kt�rego bierzemy �uk

   if(fabs(Xa-Xb)<Eps)// tylko �e w pionie
   {
		  if(Ya<Yb)	alfa=M_PI/2;
		  else      alfa=1.5*M_PI;
   }
   else //Jednak trzeba troch� bardziej policzy�
   {
	   //alfa=atan2( (Xa-Xb),(Ya-Yb) );// nie dzia�a  ?????
	   //alfa=atan2( (Xb-Xa),(Yb-Ya) );// nie dzia�a ?????
	   //!!!! atan2( Y , X ) - funkcja wymaga najpierw Y, potem X!!!!!!!!!!!!!
	   //Procedura do korekty, ale na razie dzia�a, cho� nie wiem dlaczego!
											if((Xb-Xa)==0 && (Ya-Yb)==0)
											{
												clog<<"Zbyt krotki link? X1:"<<Xa<<" Y1"<<Ya<<endl;
												Promien=0;krokow=0; return;
											}

	   alfa=atan2( (Xb-Xa),(Ya-Yb) )-M_PI/2;//?????  TAK DZIA�A!  WHY????
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

//Tak naprawd� to r�ni si� tylko sposobem rysowania
//Kt�ry zalezy od funkcji licz�cej punkty na �uku
void PowiazanieParaboliczne::PodajPozycje(double D, bool KierunekZwykly, double& X, double& Y, Komunikat* Messg)
//Przelicza polozenia wdluz linku
{
   WezelSieci* Pocz=Swiat::Wezel(_S);
   WezelSieci* Koni=Swiat::Wezel(_T);
   if(Pocz->X()!=Xa || Pocz->Y()!=Ya || Koni->X()!=Xb || Koni->Y()!=Yb)
		   _PoliczParametryLuku();//Kt�rys si� przesuna� albo zmienil si� parametr
   if(krokow==0) //Co� si� chyba nie uda�o
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

   //Obr�t UWAGA! 90 stopni (alfa=M_PI) jest w d� a 270 w g�r�

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


