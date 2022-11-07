/// \file
/// \brief Klasa matrycowego typu węzła
///        ----------------------------
///
/// \details
///     Definicje matrycowego typu węzła - czyli zawierającego tablicę "dziedzin",
////    która jest wizualizowana jako obrazek i może być obrazkiem zainicjowana.
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
/// \date
///     2022.11.07 (last updated)
// //////////////////////////////////////////////////////////////////////////////

#ifndef spsMatrixNodeH
#define spsMatrixNodeH

#include "spsGenNode.h"

/// \brief Klasa matrycowego typu węzła
class WezelMacierzowy:public GenerycznyWezelSieci
{
    static KonstruktorElementowModelu<WezelMacierzowy> WirtualnyKonstruktor;
   public:

	/// \brief Konstruktor  domyślny  ustawia domyslne parametry
	WezelMacierzowy();
    virtual ~WezelMacierzowy(); ///<  \brief Destruktor wirtualny

    /// \brief Metoda pobiera wszystkie potrzebne dane z listy stringów. \return Jak błąd to podaje, która pozycja listy
	virtual bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);

    // Do wizualizacji
    // ///////////////
	virtual void Narysuj();    ///< \brief RYSOWANIE STANU WĘZŁA
	virtual void Wymazuj();    ///< \brief Czyszczenie miejsca po rysunku węzła

	// Proste akcesory
    // ///////////////
	virtual bool Trafiony(float X,float Y);   ///< \brief Sprawdzenie czy punkt w obrębie wizualizacji. \note Np. do inspekcji myszką
	virtual double R(double Angle);           ///< \brief Promień otaczającego okręgu lub elipsy itp...
    virtual DziedzKol& operator() (unsigned x,unsigned y); ///< \brief Dostęp do danych macierzy

    // DZIAŁANIA WĘZŁA
    // ///////////////
	virtual void InterpretujKomunikat(Komunikat* Co); ///< \brief Interpretuje komunikat \note Może jakimś procesem?
	virtual void ChwilaDlaCiebie(); ///< \brief Daje szanse na endogenne zmiany stanów (tu np. kolorów)


  protected:
	//DziedzKol  		Col;   //Dziedziczony z ElementModelu
	//Dane;				//Dziedziczony z  GenerycznyWezelSieci
	//double    		W,_X,_Y,_Z; //j.w.
	//Wielobok* 		Ksztalt; //Używany do wyświetlania pojedynczego "piksela"
	DziedzinaWKolorze*  Tablica;     ///< Tablica/lista kolorów
	unsigned 			Wysokosc;    ///< Wysokość macierzy kolorów
	unsigned 			Szerokosc;   ///< Szerokość macierzy kolorów

	// Procedury pomocnicze przydatne też dla klas potomnych
	// //////////////////////////////////////////////////////

    /// \brief Metoda ładująca dane z plików DAT (tab delimited), i ewentualnie graficznym PBM
    ///     \return "true" jeśli się uda, false jak nie (opis błędu może być na perror
    ///     \note   Implementacja metody jest w osobnym źródle więc można sobie przeimplementować
    /// \param Nazwa - po prostu ścieżka do pliku
    /// \param TekstPomocniczy to dane po gwiazdce (*) w komórce poprzedzajacej nazwę (???)
	virtual bool _ZaladujKoloryZPliku(const char* Nazwa,const char* TekstPomocniczy=NULL);

	/// \brief Funkcja przeszukiwania jako "bazy danych".
	/// \return Zwraca dziedzinę (kolor) elementu podobnego do szukanej dziedziny `D`
	/// \note  Może szukać losowo, albo liniowo, ale klasy potomne mogą zaimplementować coś lepszego.
	virtual DziedzinaWKolorze _ZnajdzNajpodobniejszy( DziedzinaWKolorze D,   ///< Poszukiwana dziedzina (kolor)
                                                      unsigned& Indeks,      ///< [out] gdzie znaleziono to co zwrócono
                                                      double&   Pobienstwo,  ///< [out] podobieństwo z D
                                                      unsigned IleProb=Swiat::INV_INDEX ///< ograniczenie liczby prób
                                                              );

    /// \brief Odpowiada na komunikat
	/// \note  Element będący podstawą może być wskazany lub losowy
	virtual bool _OdpowiedzLosowymBitem( Komunikat* Pyt,                    ///< Komunikat pytający
                                         unsigned Ktory=Swiat::INV_INDEX,   ///< z którego elementu odpowiedź
                                         bool AND_OR=true                   ///< operacja bitowa AND czy OR
                                                 );

    /// \brief Odpowiada na komunikat
	/// \note MOŻE BYĆ KOSZTOWNA! Jak nie podano liczby prób to przegląda całą zawartość macierzy!!!
	virtual bool _OdpowiedzNajpodobniejszym(Komunikat* Pyt,unsigned IleProb=-1);
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
