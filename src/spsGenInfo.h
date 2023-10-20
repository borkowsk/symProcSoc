/// \file
/// \brief Klasa podstawowego komunikatu realizująca interface Komunikat
//         -------------------------------------------------------------
///
/// \details
///
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
/// \date
///     2022.11.07 (last updated)
// //////////////////////////////////////////////////////////////////////////////

#ifndef spsGenInfoH
#define spsGenInfoH

#include "spsModel.h" //Tu są deklaracje potrzebnych typów

/// \brief Klasa podstawowego komunikatu realizująca interface Komunikat
class GeneryczneInfo:public Komunikat
{
    static KonstruktorElementowModelu<GeneryczneInfo> WirtualnyKonstruktor;
  public:
    ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtualnyKonstruktor;}

	GeneryczneInfo();             ///< Domyślny  konstruktor tworzący pusty komunikat
    virtual ~GeneryczneInfo();    ///< Destruktor wirtualny, bo są metody wirtualne
    virtual bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad); ///< Wypełnianie z listy stringów
                                                          ///< Metoda pobiera wszystkie potrzebne dane z listy stringów.
                                                          ///< Jak błąd to podaje która pozycja

	virtual bool Poprawny();                             ///< \brief test poprawności
                                                         ///< \return true jeśli kanał jest dobrze zdefiniowany
                                                         ///< (wciąż istnieje link, nadawca i odbiorca etc...

    /// \brief Po prostu jak szybko ma dotrzeć. Trochę jak priorytet.
	virtual double  Waznosc() { return Predkosc; }


    // Wizualizacja
    // ////////////
	virtual void Narysuj();     ///< \brief Rysowanie reprezentacji graficznej komunikatu
	virtual void Wymazuj();     ///< \brief Wymazanie miejsca po rysunku komunikatu

    /// \brief Przydział czasu obliczeniowego \note Daje szanse na endogenne zmiany stanów - np. zbliża do dostarczenia
	virtual void ChwilaDlaCiebie();

    // Specyficzne dla Komunikatu (Message)
    // ////////////////////////////////////

    /// \brief Tekstowe określenie rodzaju komunikatu
    /// \note klasy potomne mogą mieć różne rodzaje interpretacji DziedzinyWKolorze komunikatu
    virtual bool  UstawRodzaj(const char* );

    /// \brief Sprawdzenie rodzaju komunikatu ustawionego za pomocą UstawRodzaj()
    virtual const std::string& Rodzaj();

    // Akcesory danych komunikatu
    // //////////////////////////
	virtual unsigned  Kanal() { return Link;} /// \brief Po którym połączeniu czyli linku idzie komunikat
	virtual bool 	  KierunekZgodny(){return Kierunek;} /// \brief Czy idzie zgodnie z nominalnym kierunkiem linku czy pod prąd
	virtual unsigned  Nadawca(); /// \brief  Od kogo komunikat \return identyfikator węzła
	virtual unsigned Odbiorca(); /// \brief  Do kogo komunikat \return identyfikator węzła
	virtual float  JakDostawa(); /// \brief Postęp dostarczania \return 0 po starcie, jak 1 to już dotarł
	virtual float  JakiTermin(); /// \brief W ile kroków powinien zostać dostarczony?

	// Specyficzne dla generycznego komunikatu
    // ///////////////////////////////////////
	virtual Komunikat* Klonuj(); ///< \brief Robi na stercie kopie komunikatu do przechowania lub wstawienia
	bool 	Zwrotnie(float _Szybkosc=0);///< \brief Można odwrócić komunikat - zwłaszcza jak jest to klon

	/// \brief Adresowanie komunikatu z podaniem nadawcy i odbiorcy \return true jak jest to poprawne
	bool  	Zaadresuj(unsigned _Nadawca,unsigned _Odbiorca,float _Szybkosc=0);
    /// \brief Adresowanie komunikatu z podaniem kanału czyli powiązania \return true jak poprawny kanał komunikacji
	bool  	Zaadresuj(unsigned _Powiazanie,bool KierunekZwykly,float _Szybkosc=0);

  protected:
	/// Rysowanie kształtu zależnego od typu potomnego, ale z uwzględnieniem wielkości i koloru)
	virtual void RysujKsztalt(float X,float Y,float Rad,unsigned R,unsigned G,unsigned B);

    /// \brief Chroniony konstruktor kopiujący
    /// \note jest potrzebny dla Klonuj(), ale tez dla klas potomnych
	GeneryczneInfo(const GeneryczneInfo* Wzor);

    // Pola
    // /////
    //DziedzKol  		Col;   //Dziedziczony z ElementModelu
    //DaneLokalne		Dane;  //Dziedziczony z ElementModelu
    unsigned 	    Link;      ///< Używany kanał komunikacji
    bool 			Kierunek;  ///< Czy przekazywanie zgodnie z kanałem czy "pod prąd"
    float			Predkosc;  ///< Jak szybko ma dotrzeć, czyli priorytet
    unsigned 		LicznikZycia; ///< ???
};

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
#endif
