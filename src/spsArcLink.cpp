/// \file
/// \brief Definicje metod linku ogólnego, ale rysowanego jako łuk
//         -------------------------------------------------------
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

#include "spsArcLink.h"
#include <cassert>
#include <cmath>
using namespace std;
//---------------------------------------------------------------------------
PowiazanieZagiete::KonstruktorElementowModelu<PowiazanieZagiete> PowiazanieZagiete::WirtualnyKonstruktor("arclink");

/// Nie musi robić nic - robotę wykonuje destruktor klasy bazowej
PowiazanieZagiete::~PowiazanieZagiete()
{
}

/// Domyślny konstruktor ustawiający pusty link z parametrem 1
PowiazanieZagiete::PowiazanieZagiete()

{
	parametr=1;		krokow=0;
	Xa=Ya=Xb=Yb=Xo=Yo=Promien=alfa=beta=0;
}

/// Konstruktor ustawiający jeszcze pusty link ALE Z PARAMETREM
PowiazanieZagiete::PowiazanieZagiete(double par)
{
	parametr=par;	krokow=0;
	Xa=Ya=Xb=Yb=Xo=Yo=Promien=alfa=beta=0;
}

/// Zmienia parametr więc i pomocnicze zmienne się zmieniają
void PowiazanieZagiete::UstawParametr(double par)
{
	parametr=par;	krokow=0;
	Xa=Ya=Xb=Yb=Xo=Yo=Promien=alfa=beta=0;
}

/// Metoda pobiera wszystkie potrzebne dane z listy stringów. Jak blad to podaje ktora pozycja
bool PowiazanieZagiete::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
   GenerycznePowiazanie::ZrobWgListy(Lista,Ile,Blad);
   // Teraz zostaje ustalenie parametru - jak jest zdefiniowany
   if(!Dane.KonwertujDo(6,parametr)) { Blad=6 ;return false;}
   return true;
}

const double Eps=1;

/// Liczy parametry łuku dla danej wartości parametru
/// pomocnicze pola zawierające niezbędne parametry łuku:
///	double Xa,Ya,Xb,Yb,Xo,Yo,Promien,alfa,beta;
void PowiazanieZagiete::_PoliczParametryLuku()
{
   WezelSieci* Pocz=Swiat::Wezel(_S);                                                          assert(Pocz!=NULL);
   WezelSieci* Koni=Swiat::Wezel(_T);                                                          assert(Koni!=NULL);
   Xa=Pocz->X();
   Ya=Pocz->Y();
   Xb=Koni->X();
   Yb=Koni->Y();

   if(Xa==Xb && Ya==Yb) //Bardzo złośliwa sytuacja
		{Promien=0;krokow=0; return;} //NIC SIĘ NIE DA ZROBIĆ

   // Liczenie współrzędnych środka okręgu, z którego bierzemy łuk
   if(fabs(Ya-Yb)<Eps) //Mniej niż minimalne, czyli pixel. Pewnie i tak nie będzie widać różnicy
   {
	  Xo=(Xa+Xb)/2;
	  Yo=(Ya+Yb)/2+parametr;
   }
   else
   if(fabs(Xa-Xb)<Eps) // j.w tylko, że w pionie
   {
	   Yo=(Ya+Yb)/2;
	   Xo=(Xa+Xb)/2+parametr;
   }
   else //Jednak trzeba trochę bardziej policzyć
   {
															                                           assert(Yb-Ya!=0);
															                                           assert(Xb-Xa!=0);
	 double t=-(Xb-Xa)/(Yb-Ya); 							                                           assert(t!=0);
	 double Xs=(Xa+Xb)/2;
	 double Ys=(Ya+Yb)/2;
	 double b=Ys-t*Xs;
	 double c=-t*Xa-Ya;
	 if( 2*t*Xo+b+c >0 )
	 {
		Xo=(parametr*sqrt(t*t+1)-b-c)/(2*t);
		Yo=t*Xo+b;
	 }
	 else
	 {
		Xo=(-parametr*sqrt(t*t+1)-b-c)/(2*t);
		Yo=t*Xo+b;
	 }
   }
														                                       assert(Xa!=Xo || Ya!=Yo);
   Promien=sqrt((Xa-Xo)*(Xa-Xo)+(Ya-Yo)*(Ya-Yo));   	                                       assert(Promien>0);
   alfa=atan2( (Xa-Xo),(Ya-Yo) );
   beta=atan2( (Xb-Xo),(Yb-Yo) );
 //											                        assert(Xa==Xo+Promien*cos(alfa));    //NIE HALO :-(
 //											                        assert(Ya==Yo+Promien*sin(alfa));
   krokow=Promien/10; //Na razie tak zgrubnie
}

/// Przelicza położenia wzdłuż linku:
/// Tak naprawdę to różni się tylko sposobem rysowania,
/// który zależy od funkcji liczącej punkty na łuku
void PowiazanieZagiete::PodajPozycje(double D,bool KierunekZwykly,double& X,double& Y,Komunikat* Messg)

{
   WezelSieci* Pocz=Swiat::Wezel(_S);
   WezelSieci* Koni=Swiat::Wezel(_T);
   if(Pocz->X()!=Xa || Pocz->Y()!=Ya || Koni->X()!=Xb || Koni->Y()!=Yb)
		   _PoliczParametryLuku(); //Któryś się przesunął albo zmienił się parametr
   if(krokow==0) //Coś się chyba nie udało
   {
	 GenerycznePowiazanie::PodajPozycje(D,KierunekZwykly,X,Y,Messg); //RACZEJ TYMCZASOWO
	 return;
   }

   // Właściwe obliczenie
   double angle;
   if(KierunekZwykly)
   {
   if(alfa>beta)
	angle=alfa-D*fabs(alfa-beta);
	else
	angle=alfa+D*fabs(beta-alfa);
   }
	else
   {
	angle=alfa+(1-D)*fabs(beta-alfa);
   }
   X=Xo+Promien*cos(angle);
   Y=Yo+Promien*sin(angle);
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
