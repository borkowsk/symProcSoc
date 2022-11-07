/// \file
/// \brief Definicja linku ogólnego, ale rysowanego jako łuk paraboliczny
///        --------------------------------------------------------------
///
/// \details
///              ...
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
/// \date
///     2022.11.07 (last updated)
//*////////////////////////////////////////////////////////////////////////////////


#pragma hdrstop

#include "spsParaLink.h"
#include <cassert>
#define _USE_MATH_DEFINES /*For MSVC*/
#include <cmath>
#include <iostream>
using namespace std;

#include "wb_smartlog.hpp"
using namespace wbrtm;

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

/// Nie musi robić nic - robotę wykonuje destruktor klasy bazowej
PowiazanieParaboliczne::~PowiazanieParaboliczne()
{
}

/// Domyślny konstruktor tworzący pusty link o parametrze =1.
PowiazanieParaboliczne::PowiazanieParaboliczne()
{
	parametr=1;		krokow=0;
	Xa=Ya=Xb=Yb=Promien=alfa=0;
}

/// Domyślny konstruktor ustawiający pusty link Z PARAMETREM
PowiazanieParaboliczne::PowiazanieParaboliczne(double par)
{
	parametr=par;	krokow=0;
	Xa=Ya=Xb=Yb=Promien=alfa=0;
}

/// Zmienia parametr więc i pomocnicze zmienne się muszą zmienic, ale dzieje się to leniwie. Tu sa tylko zerowane.
void PowiazanieParaboliczne::UstawWygiecie(double par)
{
	parametr=par;	krokow=0;
	Xa=Ya=Xb=Yb=Promien=alfa=0;
}

/// Metoda pobiera wszystkie potrzebne dane z listy stringów.
/// Używa metody `GenerycznePowiazanie::ZrobWgListy` a potem tylko czyta pole 6. zawierające parametr łuku.
bool PowiazanieParaboliczne::ZrobWgListy(const std::string* Lista, unsigned Ile, unsigned& Blad)
{
   if(!(Lista,Ile,Blad))
						 return false;

   //Teraz zostaje ustalenie parametru - jak jest zdefiniowany
   if(Dane.KonwertujDo(6,parametr)!=-1)
				{ Blad=6 ;return false;}

   Blad=7; //6 pole udało się wczytać
   return true;
}

const double Eps=1;

/// Liczy dane łuku dla danej wartości parametru.
/// Pomocnicze pola zawierające niezbędne parametry łuku to:
///	double Xa,Ya,Xb,Yb,Xo,Yo,Promien,alfa,beta;
void PowiazanieParaboliczne::_PoliczParametryLuku()
{
   WezelSieci* Pocz=Swiat::Wezel(_S);                                                          assert(Pocz!=NULL);
   WezelSieci* Koni=Swiat::Wezel(_T);                                                          assert(Koni!=NULL);
   Xa=Pocz->X();
   Ya=Pocz->Y();
   Xb=Koni->X();
   Yb=Koni->Y();

   if(Xa==Xb && Ya==Yb) //Bardzo przykra sytuacja
		{Promien=0;krokow=0; return;} //NIC SIĘ NIE DA ZROBIĆ

   //Liczenie współrzędnych Środka okręgu, z którego bierzemy łuk

   if(fabs(Xa-Xb)<Eps)// tylko �e w pionie
   {
		  if(Ya<Yb)	alfa=M_PI/2;
		  else      alfa=1.5*M_PI;
   }
   else //Jednak trzeba trochę bardziej policzyć
   {
	   //alfa=atan2( (Xa-Xb),(Ya-Yb) );// nie działa  ?????
	   //alfa=atan2( (Xb-Xa),(Yb-Ya) );// nie działa ?????
	   // !!! atan2( Y , X ) - funkcja wymaga najpierw Y, potem X!!!!!!!!!!!!!
	   //Procedura do korekty, ale na razie działa, choć nie wiem dlaczego!
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
   Promien=sqrt((Xa-Xb)*(Xa-Xb)+(Ya-Yb)*(Ya-Yb));   	                                              assert(Promien>0);
   krokow=Promien/5;//Na razie tak zgrubnie
   if(krokow<3) krokow=3;
}

/// Przelicza położenia wzdłuż linku.
/// Tak naprawdę ta klas od klasy bazowej różni się tylko sposobem rysowania,
/// który zależy właśnie od tej funkcji liczącej punkty na łuku.
void PowiazanieParaboliczne::PodajPozycje(double D, bool KierunekZwykly, double& X, double& Y, Komunikat* Messg)
{
   WezelSieci* Pocz=Swiat::Wezel(_S);
   WezelSieci* Koni=Swiat::Wezel(_T);
   if(Pocz->X()!=Xa || Pocz->Y()!=Ya || Koni->X()!=Xb || Koni->Y()!=Yb)
		   _PoliczParametryLuku(); //Któryś się przesunął albo zmienił się parametr

   if(krokow==0) //Cos się chyba nie udało
   {
	GenerycznePowiazanie::PodajPozycje(D,KierunekZwykly,X,Y,Messg);//RACZEJ TYMCZASOWO
	return;
   }

   // Właściwe obliczenie paraboli
   if(!KierunekZwykly)
				D=1-D;
   X=Xa+D*Promien;
   D=(D-0.5);
   double Przeliczony=-parametr*Promien;
   Y=Ya+(-Przeliczony*D*D+Przeliczony*0.25);
   //Y=Ya;

   //Obrót UWAGA! 90 stopni (alfa=M_PI) jest w dół a 270 w górę

   D=sqrt((Xa-X)*(Xa-X)+(Ya-Y)*(Ya-Y));
   X-=Xa;
   Y-=Ya;
   double Xf=X*cos_alfa-Y*sin_alfa;
   double Yf=X*sin_alfa+Y*cos_alfa;
   X=Xf+Xa;
   Y=Yf+Ya;

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


