////////////////////////////////////////////////////////////////////////////////
// Symulator Procesów Sieciowych/Spolecznych (c) Instytut Studiów Spo³ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
////////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
////////////////////////////////////////////////////////////////////////////////
//
// Deklaracja podstawowego typu realnego linku (nie pure virtual jak Powiazanie)
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
#ifndef spsGenLinkH
#define spsGenLinkH

#include "spsModel.h" //Tu s¹ deklaracje potrzebnych typow

class GenerycznePowiazanie:public Powiazanie
{
  public:
	GenerycznePowiazanie(); //Domyslny konstruktor ustawiaj¹cy pusty link
	static KonstruktorElementowModelu<GenerycznePowiazanie> WirtualnyKonstruktor;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtualnyKonstruktor;}
	virtual bool Poprawny(); //true jeœli jest dobrze zdefiniowany
	virtual double Waznosc() { return Waga; } //Zwraca wspólczynnik wa¿noœci obiektu. Np. wagê wêz³a lub linku, priorytet procesu, etc.
	//Metoda pobiera wszystkie potrzebne dane z listy stringów. Jak blad to podaje ktora pozycja
	virtual bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);
	virtual ~GenerycznePowiazanie();//Destruktor wirtualny, bo bêd¹ metody wirtualne
	virtual void ChwilaDlaCiebie(); //Daje mu szanse na endogenne zmiany stanów
	virtual void Narysuj();
	virtual void Wymazuj();
  //Specyficzne dla Powi¹zania (linku)
	virtual double ZOrder() {return _Z;} //Do ustalania kolejnosci wyswietlania
	virtual unsigned Poczatek();
	virtual unsigned Koniec();
	virtual bool  Kierunkowy() { return false;}
	virtual bool  Akceptacja(Komunikat* Co);//To akurat pozwala ka¿demu POPRAWNEMU komunikatowi mo¿e przejœæ tym ³¹czem
	virtual void  PodajPozycje(double D,bool KierunekZwykly,
							   double& X,double& Y,Komunikat* K=NULL);//Przelicza polozenia wdluz linku
							   //Mo¿e uwzglêdniaæ te¿ specyfikê komunikatu

	//Zakres w jakim musi siê zmieœciæ wizualizacja linków
	//Ale mo¿e byæ jeszcze przeskalowane przez odpowiednie parametry klasy Swiat
	static float MINIMALNA_GRUBOSC_LINKU;
	static float MAKSYMALNA_GRUBOSC_LINKU;

  protected: //Pola
	//DziedzKol  		Col;   //Dziedziczone z ElementModelu
	//DaneLokalne		Dane;      //Dane tekstowe
	unsigned    	_S,_T;
	float         	_Z;
	float 			Waga;
};

class GenerycznePowiazanieSkierowane:public GenerycznePowiazanie
{
	public:
	static KonstruktorElementowModelu<GenerycznePowiazanieSkierowane> WirtualnyKonstruktor;
		virtual bool  Kierunkowy() { return true;}
};

//------------------------------------------------------------------------------

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
