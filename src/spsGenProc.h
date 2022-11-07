/// \file
/// \brief Klasa generycznego procesu realizującego interface Proces
///        ---------------------------------------------------------
///
/// \details
///              G...
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
/// \date
///     2022.11.07 (last updated)
// ///////////////////////////////////////////////////////////////////////////////////////

#ifndef spsGenProcH
#define spsGenProcH

#include "spsModel.h" //Tu są deklaracje potrzebnych typów

/// \brief Klasa generycznego procesu realizującego interface Proces
class GenerycznyProces:public Proces
{
	static KonstruktorElementowModelu<GenerycznyProces> WirtualnyKonstruktor;
  public:
    virtual ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtualnyKonstruktor;}

    /// \brief Konstruktor do tworzenia normalnie
    GenerycznyProces(const char* Nazwa,     ///< Nazwa procesu
                     float KoniecznaPraca,  ///< Ile pracy do wykonania
                     float DoDeadlinu       ///< Ile czasu na to
    );

    GenerycznyProces(); ///< \brief Konstruktor domyślny musi tylko zapewnić sensowne powstanie
	GenerycznyProces(const GenerycznyProces* Wzor); ///< \brief Konstruktor "kopiujący ze wskaźnika"
	virtual ~GenerycznyProces(); ///< \brief Destruktor wirtualny, bo są metody wirtualne

    // Akcesory
    // ////////
    virtual bool Poprawny(); ///< \brief Test poprawności \return true jeśli jest dobrze zdefiniowany (wciąż istnieje procesor)
	virtual double  Waznosc() { return Prior; }  ///< \brief Po prostu priorytet innymi słowy
	virtual const char* Nazwa(); ///< \brief Po prostu nazwa procesu

    /// \brief Metoda pobiera wszystkie potrzebne dane z listy stringów. \return Jak błąd to podaje, która pozycja listy
	virtual bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);

    // Do wizualizacji
    // ///////////////
	virtual void Narysuj();           ///< \brief RYSOWANIE STANU PROCESU
	virtual void Wymazuj();           ///< \brief Czyszczenie miejsca po rysunku stanu

    // DZIAŁANIE
    // /////////
	virtual void ChwilaDlaCiebie();   ///< \brief Daje mu procesowi możliwość zmiany stanów - zrobienia czegoś

    // Specyficzne dla Procesu-projektu
    // //////////////////////////////////
	virtual double   JakPracochlonny() {return  WymaganaIloscPracy;} ///< \brief Robotożerność procesu
	virtual double   Priorytet();                                    ///< \brief Im wyższy tym proces więcej działa
	virtual double   JakZaawansowany();                              ///< \return 0 po starcie, jak 1 to już zrobiony.
                                                                     ///< PROCES_NAD_ZAAWANSOWANY - gotowy do usunięcia
	virtual void     UznajZaZakonczony() { TmpPorcjaPracy=-1; } ///< WSTAWIENIE TU WARTOŚCI UJEMNEJ (<0) OZNACZA KONIEC.

  protected:
	float WymaganaIloscPracy; ///< Ile "roboczogodzin" musi działać, żeby być gotowy
	float PracaDotychczasowa; ///< Ile już zrobiono
  public:
	float  Prior;   ///< Jaki ma priorytet działania (miał  w poprzednim obliczeniu),
				    ///< ale też można "ręcznie" zmniejszać lub zwiększać jak potrzeba

	float  TmpPorcjaPracy; ///< O ile "roboczogodzin" posuwa się w każdym kroku czasu.
						   ///< Zamiast parametru do ChwilaDlaCiebie()
						   ///< WSTAWIENIE TU WARTOŚCI UJEMNEJ (<0) OZNACZA KONIEC:
						   ///< JakZaawansowany() zwraca wtedy wartość PROCES_NAD_ZAAWANSOWANY.
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
