////////////////////////////////////////////////////////////////////////////////
// Symulator Procesów Sieciowych/Spolecznych (c) Instytut Studiów Spo³ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
////////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
////////////////////////////////////////////////////////////////////////////////
//
// Definicje matrycowego typu wêz³a - czyli zawieraj¹cego tablicê "dziedin"
// która jest wizualizowana jako obrazek i mo¿e byæ obrazkiem zainicjowana
////////////////////////////////////////////////////////////////////////////////
#ifndef spsMatrixNodeH
#define spsMatrixNodeH

#include "spsGenNode.h"

class WezelMacierzowy:public GenerycznyWezelSieci
{
   public:
	//Konstruktor  ustawia domyslne
	WezelMacierzowy();
	static KonstruktorElementowModelu<WezelMacierzowy> WirtualnyKonstruktor;
	//Metoda pobiera wszystkie potrzebne dane z listy stringów. Jak blad to podaje ktora pozycja
	virtual bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);
	virtual ~WezelMacierzowy();//Destruktor wirtualny
	virtual void Narysuj();
	virtual void Wymazuj();
	//Proste akcesory
	virtual bool Trafiony(float X,float Y);//Np. do inspekcji myszk¹
	virtual double R(double Angle); //Promieñ otaczaj¹cego okrêgu lub elipsy itp...
	virtual void InterpretujKomunikat(Komunikat* Co);//Interpretuje komunikat
	virtual void ChwilaDlaCiebie(); //Daje jakies szanse na endogenne zmiany stanów - tu koloru
	virtual DziedzKol& operator() (unsigned x,unsigned y); //Dostêp do danych macierzy
  protected:
	//DziedzKol  		Col;   //Dziedziczony z ElementModelu
	//Dane;				//Dziedziczony z  GenerycznyWezelSieci
	//double    		W,_X,_Y,_Z; //j.w.
	//Wielobok* 		Ksztalt; //U¿ywany do wyœwietlania pojedynczego "piksela"
	DziedzinaWKolorze*  Tablica;
	unsigned 			Wysokosc;
	unsigned 			Szerokosc;
	//Procedury pomocnicze przydatne te¿ dla klas potomnych
	//////////////////////////////////////////////////////////////////////////
	virtual bool _ZaladujKoloryZPliku(const char* Nazwa,const char* TekstPomocniczy=NULL);//Procedura ³aduj¹ca dane
				//zwraca "true" jeœli siê uda, false jak nie (komunikat mo¿e byæ na perror)
				//z plików DAT (tab delimited), i ewentualnie graficznym PBM
				// - jest w osobnym Ÿródle wiêc mo¿na przeimplementowaæ
				//TekstPomocniczy to dane po gwiazdce (*) w komórce poprzedzajacej nazwê
	//Funkcja przeszukiwania "bazy danych". Mo¿e losowo, albo liniowo, ale potomne mog¹ zaiplementowac coœ lepszego
	virtual DziedzinaWKolorze _ZnajdzNajpodobniejszy(DziedzinaWKolorze D,unsigned& Indeks,double& WzglednePobienstwo,unsigned IleProb=Swiat::INVINDEX);
	//...Element bêd¹cy podstw¹ mo¿e byæ wskazany lub losowy
	virtual bool _OdpowiedzLosowymBitem(Komunikat* Pyt,unsigned Ktory=Swiat::INVINDEX,bool AND_OR=true);
	//MOZE BYÆ KOSZTOWNA! Jak nie podano liczby prób to przegl¹da ca³¹ zawartoœæ!!!
	virtual bool _OdpowiedzNajpodobniejszym(Komunikat* Pyt,unsigned IleProb=-1);
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
