////////////////////////////////////////////////////////////////////////////////
// Symulator Procesów Sieciowych/Spolecznych (c) Instytut Studiów Spo³ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
////////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
////////////////////////////////////////////////////////////////////////////////
//
// Deklaracja podstawowego typu realnego wêz³a (nie pure virtual jak WezelSieci)
////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
#ifndef spsGenNodeH
#define spsGenNodeH

#include <string>
#include "spsModel.h" //Tu s¹ deklaracje potrzebnych typow

class Wielobok; //U¿ywa wewnetrznie, ale nie ma powodu tu wnikaæ w szczegoly

class GenerycznyWezelSieci:public WezelSieci
{
  public:
	//Konstruktor  ustawia domyslne
	GenerycznyWezelSieci();
	static KonstruktorElementowModelu<GenerycznyWezelSieci> WirtualnyKonstruktor;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtualnyKonstruktor;}
	virtual bool Poprawny(); //true jeœli jest dobrze zdefiniowany
	virtual double  Waznosc() { return Waga; }  //Po prostu waga innymi s³owy
	//Metoda pobiera wszystkie potrzebne dane z listy stringów. Jak blad to podaje ktora pozycja
	virtual bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);
	virtual ~GenerycznyWezelSieci();//Destruktor wirtualny
	virtual void Narysuj();
	virtual void Wymazuj();
	virtual const char* Nazwa();//Musi posiadaæ jakas nazwe
	//Proste akcesory
	virtual bool Trafiony(float X,float Y); //Np. do inspekcji myszk¹
	virtual double ZOrder() {return _Z;} //Do ustalania kolejnosci wyswietlania
	virtual double X() { return _X; }
	virtual double Y() { return _Y; }
	virtual double R(double Angle); //Promieñ otaczaj¹cego okrêgu lub elipsy
	virtual DziedzKol* Dziedzina() { return &Col;}
	virtual void ChwilaDlaCiebie(); //Daje jakies szanse na endogenne zmiany stanów - tu koloru
	virtual void InterpretujKomunikat(Komunikat* Co); //Próbuje obrobiæ procesami, a jak siê nie uda to
									//"Brodkastuje" komunikat losowo do jednego lub dwu s¹siadów jak ma
  protected:
	//DziedzKol  		Col;   //Dziedziczony z ElementModelu
	//DaneLokalne		Dane;      //Dane tekstowe
	float    		_X,_Y;
	float			_Z;
	float			Waga;
	Wielobok* 		Ksztalt;
	//Metody pomocnicze dla klas domyœlnej akcji ChwilaDlaCiebie() i dla klas potomnych
	virtual void _RuszProcesPriorytetowy();//Posuniêcie do przodu najpilniejszego z procesów
	virtual void _RuszKazdyProces();//Wywoluje ka¿demu procesowi ChwilaDlaCiebie()
	void          ZarzadcaProcesowPrymitywny() { _RuszKazdyProces(); }
	virtual bool _PrzeniesDaneNaPola(unsigned& Blad);//Interpretuje pola od 1 do 7 w ZrobWgListy
	virtual bool _KomunikatDoProcesow(Komunikat* Co);//Obrobienie komunikatu przez pierwszy chêtny proces
	virtual void _KomunikatPrzekazLosowo(Komunikat* Co,double P);//Przekazuje komunikat losowo i z prawdopodobieñstwem P namna¿a
							//Dosyæ to kosztowna funkcja, zw³aszcza puki 'Swiat' nie ma cachowania linków wez³a
};



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

