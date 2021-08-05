
////////////////////////////////////////////////////////////////////////////////
// Symulator Procesów Sieciowych/Spolecznych (c) Instytut Studiów Spo³ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
////////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
////////////////////////////////////////////////////////////////////////////////
//
// Deklaracja/Definicja linku ogólnego, ale rysowanego jako ³uk
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
#ifndef spsArcLinkH
#define spsArcLinkH

#include "spsGenLink.h"

class PowiazanieZagiete:public GenerycznePowiazanie
{
  public:
	PowiazanieZagiete(); //Domyslny konstruktor ustawiaj¹cy pusty link
	PowiazanieZagiete(double par);//Konstruktor z ustawieniem parametru
	static KonstruktorElementowModelu<PowiazanieZagiete> WirtualnyKonstruktor;
	//Metoda pobiera wszystkie potrzebne dane z listy stringów. Jak blad to podaje ktora pozycja
	virtual bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);
	virtual ~PowiazanieZagiete();
	//Tak naprawdê to ró¿ni siê tylko sposobem rysowania
	virtual void Narysuj();
	virtual void Wymazuj();
	virtual void  PodajPozycje(double D,bool KierunekZwykly,
							   double& X,double& Y,Komunikat* K=NULL);//Przelicza polozenia wdluz linku
							   //Mo¿e uwzglêdniaæ te¿ specyfikê komunikatu
	void UstawParametr(double par);//Zmienia parametr wiêc i pomocnicze zmienne siê zmieniaja
	protected:
	void _PoliczParametryLuku();//Liczy parametry ³uku dla danej wartosci parametru
	double parametr;//Wartoœæ decyduj¹ca o sposobie zagiêcia
	private:
	//Pomocnicze pola zawieraj¹ce niezbêdne parametry ³uku
	double Xa,Ya,Xb,Yb,Xo,Yo,Promien,alfa,beta;
	unsigned krokow;//Ile odcinków ³uku przy rysowaniu ~ odlegloœæ PK/10 pix
};

//---------------------------------------------------------------------------

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
#endif
