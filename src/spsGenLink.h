/// \file
/// \brief Deklaracja podstawowego typu realnego linku (nie pure virtual jak Powiazanie)
///        -----------------------------------------------------------------------------
///
/// \details
///              ...
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
/// \date
///     2022.11.03 (last updated)
//*////////////////////////////////////////////////////////////////////////////////

#ifndef spsGenLinkH
#define spsGenLinkH

#include "spsModel.h" //Tu są deklaracje potrzebnych typów

/// \brief link ogólny, symetryczny, rysowanego jako linia
class GenerycznePowiazanie:public Powiazanie
{
  public:
	GenerycznePowiazanie(); ///< Domyslny konstruktor ustawiający pusty link
    /// \brief Powiązany obiekt wirtualnego konstruktora
	static KonstruktorElementowModelu<GenerycznePowiazanie> WirtualnyKonstruktor;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtualnyKonstruktor;}

    /// \brief   Czytanie danych obiektu
    /// \details Metoda pobiera wszystkie potrzebne dane z listy stringów. Jak błąd to podaje która pozycja
	virtual bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);

    /// \brief Destruktor wirtualny. \details bo są inne metody wirtualne
	virtual ~GenerycznePowiazanie();

    /// \brief Sprawdzanie poprawności
    /// \return true jeśli jest dobrze zdefiniowany
    virtual bool Poprawny();

    /// \brief Współczynnik ważności obiektu.
    /// \details Znaczenie zależy od kontekstu. Współczynnik może oznaczać
    ///          Np. wagę węzła lub linku, priorytet procesu, etc.
    virtual double Waznosc() { return Waga; }

    /// \brief Metoda dająca =szanse na endogenne zmiany stanów obiektów
    virtual void ChwilaDlaCiebie();

    /// \brief rysowanie \details Od zwykłego powiązania różni się tylko sposobem rysowania
	virtual void Narysuj();

    /// \brief czyszczenie z ekranu
	virtual void Wymazuj();

    // Specyficzne dla Powiązania (linku)
    // //////////////////////////////////

	virtual double ZOrder() {return _Z;} ///< \brief Do ustalania kolejności wyświetlania

    virtual unsigned Poczatek();         ///< \brief Indekst węzła źródłowego (???)

    virtual unsigned Koniec();           ///< \brief Indeks węzła docelowego (???)

	virtual bool  Kierunkowy() { return false;} /// \brief Czy link kierunkowy czy symetryczny? \return false (symetryczny)

    ///< \brief   Czy komunikat pasuje do łącza?
    ///< \details Ta wersja akurat pozwala każdemu POPRAWNEMU komunikatowi przejść tym łączem
    virtual bool  Akceptacja(Komunikat* Co);

    /// \brief   Pozycja na połączeniu
    /// \details Przelicza położenie wzdłuż linku
    ///          Może uwzględniać też specyfikę komunikatu
    virtual void  PodajPozycje(double D,                        ///< Pozycja między źródłem a celem
                               bool KierunekZwykly,             ///< Czy kierunek zwykły czy odwrotny
                               double& X,                       ///< [out] X wynikowe
                               double& Y,                       ///< [out] Y wynikowe
                               Komunikat* K=NULL                ///< opcjonalnie link do komunikatu, jeśli jakaś specyfika.
    );

	/// \brief Zakres w jakim musi się zmieścić wizualizacja linków
	/// \details Ale może być jeszcze przeskalowane przez odpowiednie parametry klasy Swiat
	static float MINIMALNA_GRUBOSC_LINKU,MAKSYMALNA_GRUBOSC_LINKU;

  protected: //Pola
	//DziedzKol  		Col;       // Dziedziczone z ElementModelu
	//DaneLokalne		Dane;      // Dane tekstowe
	unsigned    	_S,_T;  // INDEKSY ŹRÓDŁA I CELU
	float         	_Z;     // Z-order
	float 			Waga;   ///< Znaczenie linku
};

/// \brief link ogólny, skierowany, rysowanego jako strzałka
class GenerycznePowiazanieSkierowane:public GenerycznePowiazanie
{
	public:
    /// \brief Powiązany obiekt wirtualnego konstruktora
	static KonstruktorElementowModelu<GenerycznePowiazanieSkierowane> WirtualnyKonstruktor;
    ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtualnyKonstruktor;}

    /// \brief Czy link kierunkowy czy symetryczny?
    /// \return true (asymetryczny)
    virtual bool  Kierunkowy() { return true;}
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
