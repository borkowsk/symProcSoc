/// \file
/// \brief Główne definicje danych i funkcji świata
///        ----------------------------------------
///
/// \details
///      Czyli wszystkie te, które nie są w innych plikach jak np. Init-cjalizacja czy Stat-ystyka.
////
///      UŻYWAMY LINIOWEJ ORGANIZACJI KONTENERÓW, ALE NIELINIOWO JEJ UŻYWAMY.
///      MOŻNA JĄ ZASTĄPIĆ JAKĄŚ INNĄ, POD WARUNKIEM, ŻE GWARANTUJE NIEZMIENNE UCHWYTY W POSTACI
///      LICZB CAŁKOWITYCH.
///      (TODO) MOŻNA BY TO ZASTĄPIĆ JAKĄŚ INNĄ IMPLEMENTACJĄ
/// \note Cześć modelu może być w innych plikach, jak ten zanadto urośnie!
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
/// \date
///     2022.11.09 (last updated)
//*////////////////////////////////////////////////////////////////////////////////

#include "spsModel.h"

#include <cassert>
#include <iostream>
using namespace std;

#include "wb_smartlog.hpp"
#include "compatyb.h"
#include "lingo.hpp"

using namespace wbrtm;

///< Gdy dane s� w jednym to wszystkie inne "metody" mog� by� "static"
Swiat Swiat::Ten; ///<  Staramy si� wi�c mie� tylko jeden �wiat :-)


static const int LOGBASE=5; ///< Na jakim poziomie logowania domy�lnie zapisywa� zdarzenia wewn�trz prywatnych struktur danych

//  ZARZĄDZANIE STRUKTURAMI DANYCH Swiata
// //////////////////////////////////////

// Wskaźniki tablic
// ////////////////
static WezelSieci** _Wezly=NULL;
static Powiazanie** _Polaczenia=NULL;
static Komunikat** _Informacje=NULL;

// Zaalokowane rozmiary tablic
// ///////////////////////////
static unsigned _WezlyMax=0;
static unsigned _PolaczeniaMax=0;
static unsigned _InformacjeMax=0;

// Już użyte sloty
// ///////////////
static unsigned _UzyteWezly=0;
static unsigned _UzytePolaczenia=0;
static unsigned _UzyteInformacje=0;

// Zwolnione sloty (=NULL) do odnalezienia
// ///////////////////////////////////////
static unsigned _WolneWezly=0;
static unsigned _WolnePolaczenia=0;
static unsigned _WolneInformacje=0;


/// Dostępy do tablic mogą rzucić wyjątkiem, albo przerwać przy błędzie dostępu,
/// albo w "łagodnej" formie mogą zwrócić NULL, tak jak w przypadku pustego
/// czyli np. "uśmierconego" slotu w tablicy.
/// \return NULL jak `Który` poza zakresem albo węzeł zlikwidowany.
WezelSieci* Swiat::Wezel(unsigned Ktory)
{            						                                                               assert(_Wezly!=NULL);
									                                                    assert(Ktory!=Swiat::INV_INDEX);
	if(Ktory<_UzyteWezly)
		return _Wezly[Ktory];
		else
		return NULL;
}

/// \return NULL jak `Który` poza zakresem albo połączenie zlikwidowane.
Powiazanie* Swiat::Lacze(unsigned Ktory)
{            						                                                          assert(_Polaczenia!=NULL);
									                                                    assert(Ktory!=Swiat::INV_INDEX);
	if(Ktory<_UzytePolaczenia)
		return _Polaczenia[Ktory];
		else
		return NULL;
}

/// \return NULL jak `Który` poza zakresem albo komunikat już zlikwidowany.
Komunikat* Swiat::Info(unsigned Ktory)
{            						                                                          assert(_Informacje!=NULL);
									                                                    assert(Ktory!=Swiat::INV_INDEX);
	if(Ktory<_UzyteInformacje)
		return _Informacje[Ktory];
		else
		return NULL;
}

/// \return -1(==UINT_MAX) jak nie znajdzie węzła o takiej `Nazwa`
unsigned 	Swiat::ZnajdzIndeksWezla(const char* Nazwa)
{
	if(strlen(Nazwa)==0)
			return -1;
	for(unsigned i=0;i<_UzyteWezly;i++)
		if(_Wezly[i]!=NULL
			&& strcasecmp(_Wezly[i]->Nazwa(),Nazwa)==0)
				return i;
	return -1; //Jak nie znajdzie
}


/// \return Aktualny rozmiar tablicy węzłów \note W obrębie tablic mogą być puste sloty
unsigned Swiat::IleWezlow()
{
	return _UzyteWezly;
}

/// \return Aktualny rozmiar tablicy powiązań \note W obrębie tablic mogą być puste sloty
unsigned Swiat::IlePowiazan()
{
	return _UzytePolaczenia;
}

/// \return Aktualny rozmiar tablicy komunikatów \note W obrębie tablic mogą być puste sloty
unsigned Swiat::IleInformacji()
{
	return _UzyteInformacje;
}

/// \return Liczba jeszcze dostępnych slotów w tablicy węzłów
unsigned Swiat::IleMoznaWezlow()
{
	return  (_WezlyMax-_UzyteWezly)+_WolneWezly;
}

/// \return Liczba jeszcze dostępnych slotów w tablicy powiązań
unsigned Swiat::IleMoznaPowiazan()
{
	return (_PolaczeniaMax-_UzytePolaczenia)+_WolnePolaczenia;
}

/// \return Liczba jeszcze dostępnych slotów w tablicy komunikatów
unsigned Swiat::IleMoznaInformacji()
{
	return (_InformacjeMax-_UzyteInformacje)+_WolneInformacje;
}

/// Funkcje wstawiające zwracają pozycje obiektu wstawionego i PRZEJMUJĄ ZARZĄD PAMIĘCIĄ
/// \note wstawiane obiekty muszą być alokowane na stercie (przez "new")
/// \param Co czyli wstawiany obiekt nie może być równy NULL
/// \param propozycja pozwala podpowiedzieć gdzie jest wolny slot. Jak nie ma sensu to procedura radzi sobie inaczej.
/// \return miejsce wstawienia lub (unsigned)-1 gdy z jakiegoś powodu się nie udało.
unsigned Swiat::WstawWezel(WezelSieci* Co,unsigned propozycja/*=-1*/)
{
	if(!Co) {  TLOG(0, << "Wezel == NULL!!!" ) return -1; /*Znaczy błąd!!!*/ }
												                                                       assert(Co!=NULL);
	if(! (propozycja<_UzyteWezly
		&& _Wezly[propozycja]!=NULL) )  //Szukamy tylko, jeśli propozycja nie ma sensu
	{
		if(_UzyteWezly<_WezlyMax) //Jest jeszcze miejsce
		{
			propozycja=_UzyteWezly++; //Bierzemy następny wolny
		}
		else  //Szukamy gdzie są te zwolnione
		if(_WolneWezly>0) //TODO TO JAKAŚ DZIWNA SPRAWA
		{
			propozycja=-1; //Gdyby nie było...
			for(unsigned i=0;i<_UzyteWezly;i++)
			if(_Wezly[i]==NULL)
			{
				propozycja=i;
				_WolneWezly--;
				break; //I nie ma co dalej szuka�!
			}
		}
	}

	if(propozycja<_WezlyMax) //a więc też nie -1, bo to naprawdę UINTMAX czy coś
	{
	  _Wezly[propozycja]=Co; //Przestaje być NULL
	  Co->_MojIndeks=propozycja;

	  Swiat::UwagaZmienionoStrukture();
	  return propozycja;   //TYLKO TU POPRAWNE WYJŚCIE
	}
	else
	{
		cerr<<endl<<_LPL("Brak miejsca na węzel!","Table to small. Can't insert a node.")<<endl;
		delete Co; //Usuwa ze sterty, bo przecież oddano mu już w zarząd!!!
		return -1; //Znaczy błąd!!!
	}
}

/// Funkcje wstawiające zwracają pozycje obiektu wstawionego i PRZEJMUJĄ ZARZĄD PAMIĘCIĄ
/// \param Co czyli wstawiany obiekt nie może być równy NULL
/// \param propozycja pozwala podpowiedzieć gdzie jest wolny slot. Jak nie ma sensu to procedura radzi sobie inaczej.
/// \note wstawiane obiekty muszą być alokowane na stercie (przez "new")
/// \return miejsce wstawienia lub (unsigned)-1 gdy z jakiegoś powodu się nie udało.
unsigned Swiat::WstawLacze(Powiazanie* Co,unsigned propozycja/*=-1*/)
{
													                        assert(Co!=NULL); //Nie wolno tak kasować!!!
	if(! (propozycja<_UzytePolaczenia
		&& _Polaczenia[propozycja]!=NULL) )  //Szukamy tylko, jeśli propozycja nie ma sensu
	{
		if(_UzytePolaczenia<_PolaczeniaMax) //Jest jeszcze miejsce
		{
			propozycja=_UzytePolaczenia++; //Bierzemy następny wolny
		}
		else  //Szukamy gdzie są te zwolnione
		if(_WolnePolaczenia>0)
		{
			propozycja=-1; //Gdyby nie było...
			for(unsigned i=0;i<_UzytePolaczenia;i++)
			if(_Polaczenia[i]==NULL)
			{
				propozycja=i;
				_WolnePolaczenia--;
				break; //I nie ma co dalej szuka�!
			}
		}
	}

	if(propozycja<_PolaczeniaMax) //a więc też nie -1, bo to naprawdę UINTMAX czy coś
	{
	  _Polaczenia[propozycja]=Co; //Przestaje być NULL
	  Co->_MojIndeks=propozycja;

	  Swiat::UwagaZmienionoStrukture();
	  return propozycja;   //TYLKO TU POPRAWNE WYJŚCIE
	}
	else
	{
		cerr<<endl<<_LPL("Brak miejsca na link!","Table to small. Can't insert a link.")<<endl;
		delete Co; //Usuwa ze sterty, bo przecie� oddano mu ju� w zarz�d!!!
		return -1; //Znaczy błąd!!!
	}
}

/// Funkcje wstawiające zwracają pozycje obiektu wstawionego i PRZEJMUJĄ ZARZĄD PAMIĘCIĄ.
/// Musi sprawdzić czy komunikat jest OK.
/// \param Co czyli wstawiany obiekt nie może być równy NULL
/// \param propozycja pozwala podpowiedzieć gdzie jest wolny slot. Jak nie ma sensu to procedura radzi sobie inaczej.
/// \note wstawiane obiekty muszą być alokowane na stercie (przez "new")
/// \return miejsce wstawienia lub (unsigned)-1 gdy z jakiegoś powodu się nie udało.
unsigned Swiat::WstawInfo(Komunikat* Co,unsigned propozycja/*=-1*/)
{
													                        assert(Co!=NULL); //Nie wolno tak kasować!!!
	//Musi sprawdzić czy komunikat jest OK. Nieco redundantne
	//gdyż działa nawet jeśli ju� takie sprawdzenie wykona� kod zlecaj�cy
	unsigned Kanal=Co->Kanal();
	if(Kanal == INV_INDEX)
		  {	goto ERROR2; } //Jakby niezainicjowany, czyli bez informacji na log/cerr
	if(Kanal>=_UzytePolaczenia)
		  {	goto ERROR;  } //Nie ma takiego połaczenia/kanału/
	if(_Polaczenia[Kanal]==NULL)
		  {	goto ERROR; } //Kana� znikna�
	if(!(_Polaczenia[Kanal]->Akceptacja(Co)))
		   {  goto ERROR; } //Kana� nie przyjmuje komunikatu
	if(	!Co->Poprawny() )
		   {  goto ERROR; } //Jednak wadliwie skonstruowany

	//Teraz może już szukać miejsca w tablicach
	if(! (propozycja<_UzyteInformacje
		&& _Informacje[propozycja]!=NULL) )  //Szukamy tylko, jeśli propozycja nie ma sensu
	{
		if(_UzyteInformacje<_InformacjeMax) //Jest jeszcze miejsce
		{
			propozycja=_UzyteInformacje++; //Bierzemy nast�pny wolny
		}
		else  //Szukamy gdzie s� te zwolnione
		if(_WolneInformacje>0)
		{
			propozycja=-1; //Gdyby nie by�o...
			for(unsigned i=0;i<_UzyteInformacje;i++)
			if(_Informacje[i]==NULL)
			{
				propozycja=i;
				_WolneInformacje--;
				break; //I nie ma co dalej szuka�!
			}
		}
	}

	if(propozycja<_InformacjeMax) //a więc też nie -1, bo to naprawdę UINTMAX czy co�
	{
	  _Informacje[propozycja]=Co; //Przestaje być NULL
	  Co->_MojIndeks=propozycja;

	  //Swiat::UwagaZmienionoStrukture();  - TOPOLOGIA SIĘ NIE ZMIENIA OD WSTAWIENIA KOMUNIKATU
	  return propozycja;   //TYLKO TU POPRAWNE WYJŚCIE
	}
	else
	{
		TLOG(1, <<_LPL("BRAK MIEJSCA NA KOMUNIKAT!","NO SPACE IN MESSG POOL") )
	ERROR:
   /*	{ // CO TO ROBIŁO?
		clog<<endl<<_LPL("Nieudane wprowadzenie komunikatu!","Can't insert a message.")
				<<Co->WartoscPola(0)<<","<<Co->WartoscPola(1)<<" "<<Co->WartoscPola(2)<<endl;
		clog<<"\tlink: "<<Co->Kanal();
		Powiazanie* Kan=Swiat::Lacze(Co->Kanal());
		if(Kan)
		{
		WezelSieci* Start=Wezel(Kan->Poczatek());
		WezelSieci* End=Wezel(Kan->Koniec());
		clog<<" "<<(Start?Start->Nazwa():"???")<<(Kan->Kierunkowy()?"--->":"<-->")<<(End?End->Nazwa():"??? ");
		}
		clog<<endl;
	}  */
	ERROR2:
		delete Co; //Usuwa ze sterty, bo przecie� oddano mu ju� w zarz�d!!!
		return -1; //Znaczy błąd!!!
	}

	return 0; //TO NIGDY NIE ZACHODZI
}

// UWAGA NA USUWANIE! GENERALNIE DOSYĆ DROGIE
// //////////////////////////////////////////


/// (TODO) Tańsze nieco są funkcje usuwające wg. indeksu (lub uchwytu, zależnie od implementacji)
///     * Usuwanie węzła usuwa też wszystkie związane z nim łącza i komunikaty.
/// \warning NIE ZAIMPLEMENTOWANE!
void Swiat::UsunWezel(unsigned Numer)
{
    TLOG(0, << "Jeszcze nie zaiplementowane" )
							                                                assert("Jeszcze nie zaiplementowane"==NULL);
	Swiat::UwagaZmienionoStrukture();
}

///     * Usuwanie łącza usuwa związane z nim komunikaty
void Swiat::UsunLacze(unsigned Numer)
{
	if(_Polaczenia[Numer]==NULL) //już skasowany
				return ; //Nic nie robimy
	else
	{
	   delete _Polaczenia[Numer];
	   _Polaczenia[Numer]=NULL;
	   _WolnePolaczenia++;      //Jest jeden wolny slot więcej
	}

	// Usuwamy komunikaty używające tego łącza
	for(unsigned i=0;i<_UzyteInformacje;i++)
		if(_Informacje[i]!=NULL
		&& _Informacje[i]->Kanal()==Numer)
			Swiat::UsunInfo(i);

	Swiat::UwagaZmienionoStrukture();
}

///   * Usuwanie komunikatu jest proste, bo niczemu w strukturze nie wolno się do niego odnosić
void Swiat::UsunInfo(unsigned i)
{
	delete (_Informacje[i]);
	_Informacje[i]=NULL;
	_WolneInformacje++; //Jest teraz wolny slot
   //	Swiat::UwagaZmienionoStrukture();  - komunikat nie nalezy do topologii sieci
}

///  Funkcja usuwająca wg. wskaźnika jest dodatkowo kosztowna bo musi szukać.
unsigned Swiat::UsunWezel(WezelSieci* Jaki)
{
	for(unsigned i=0;i<_UzyteWezly;i++)
		if(_Wezly[i]==Jaki)
		{
			UsunWezel(i); //Usuwa znaleziony
			return i;
		}
	return -1;
}

///  Funkcja usuwająca wg. wskaźnika jest dodatkowo kosztowna bo musi szukać.
unsigned Swiat::UsunLacze(Powiazanie* Jaki)
{
	for(unsigned i=0;i<_UzytePolaczenia;i++)
		if(_Polaczenia[i]==Jaki)
		{
			UsunLacze(i);//Usuwa znaleziony
			return i;
		}
	return -1;
}

///  Funkcja usuwająca wg. wskaźnika jest dodatkowo kosztowna bo musi szukać.
unsigned Swiat::UsunInfo(Komunikat* Jaki)
{
	for(unsigned i=0;i<_UzyteInformacje;i++)
		if(_Informacje[i]==Jaki)
		{
			UsunInfo(i);//Usuwa znaleziony
			return i;
		}
	return -1;
}

///  KONSTRUKTOR ŚWIATA WŁAŚCIWIE NIE MA NIC WAZNEGO DO ZROBIENIA (???)
///  Prawdziwa robota robi się gdzie indziej i kiedy indziej.
///  Konstruktor jest  prywatny, bo jest tylko jeden świat.
Swiat::Swiat()
{
#ifndef NDEBUG
	// Konstruktor globalnej zmiennej nie bardzo może pisać przez `cout`, bo `cout` może nie być jeszcze zainicjowane!
    // fprintf(stderr,"%s\n","DEBUG: Inicjalizacja struktur danych");
#endif
}

typedef WezelSieci* ptrWezelSieci;
typedef Powiazanie* ptrPowiazanie;
typedef Komunikat*  ptrKomunikat;

/// Procedura konfiguracji nowego, PUSTEGO świata.
/// \warning Na razie można wywołać tylko raz! (???)
bool Swiat::_UstalLiczbeWezlow(unsigned IleMax)
{	                                                                                               assert(_Wezly==NULL);
	_Wezly=new ptrWezelSieci[IleMax];
	_WezlyMax=IleMax;
	_UzyteWezly=0;
	Swiat::UwagaZmienionoStrukture();
	return true;
}

/// Procedura konfiguracji nowego, PUSTEGO świata.
/// \warning Na razie można wywołać tylko raz! (???)
bool Swiat::_UstalLiczbeLaczy(unsigned IleMax)
{	                                                                                          assert(_Polaczenia==NULL);
	_Polaczenia=new ptrPowiazanie[IleMax];
	_PolaczeniaMax=IleMax;
	_UzytePolaczenia=0;
	Swiat::UwagaZmienionoStrukture();
	return true;
}

/// Procedura konfiguracji nowego, PUSTEGO świata.
/// \warning Na razie można wywołać tylko raz! (???)
bool Swiat::_UstalLiczbeInfo(unsigned IleMax)
{                                                                                             assert(_Informacje==NULL);
	if(_UzyteInformacje!=0)
			return false; //Ju� nie mo�na!!!
	if(_Informacje!=NULL)
			delete [] _Informacje;
	_Informacje=new ptrKomunikat[IleMax];
	_InformacjeMax=IleMax;
	_UzyteInformacje=0;
	Swiat::UwagaZmienionoStrukture();
	return true;
}

/// Destruktor wirtualny powinien posprzątać, ale to czasochłonne,
/// a prawdopodobnie zbędne, bo destruktory obiektów świata i tak by pewnie zwalniały
/// pamięć i powiadamiały się wzajemnie o likwidacji. A przecież to i tak KONIEC!
/// Lepiej jak "świat" zostanie oddany w całości systemowi na koniec programu bez zawracania sobie głowy destruktorami.
/// \note TODO Gdyby chcieć sprawdzić ewentualne błędy alokacji i wskaźników to trzeba by jednak taką destrukcje zrobić.
Swiat::~Swiat()
{
#ifndef NDEBUG
 //	fprintf(stderr,"%s\n","DEBUG: Destrukcja struktur danych");
	cout<<endl;
#endif
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


