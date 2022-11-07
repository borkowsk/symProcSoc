/// \file
/// \brief Klasa podstawowego typu realnego węzła (nie pure virtual jak WezelSieci)
///        -----------------------------------------------------------------------------
///
/// \details
///              G...
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
/// \date
///     2022.11.07 (last updated)
// ///////////////////////////////////////////////////////////////////////////////////////

#ifndef spsGenNodeH
#define spsGenNodeH

#include <string>
#include "spsModel.h" // Tu są deklaracje potrzebnych typów

class Wielobok; // Używa wewnętrznie, ale nie ma powodu tu wnikać w szczegóły

/// \brief Klasa podstawowego typu realnego węzła (nie pure virtual jak WezelSieci)
class GenerycznyWezelSieci:public WezelSieci
{
    static KonstruktorElementowModelu<GenerycznyWezelSieci> WirtualnyKonstruktor;
  public:
    ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtualnyKonstruktor;}

	GenerycznyWezelSieci();             ///< \brief Konstruktor  ustawia domyślne
    virtual ~GenerycznyWezelSieci();    ///< \brief Destruktor wirtualny

    // Ogólne akcesory wymagane
    // ////////////////////////
	virtual bool Poprawny();                    ///< \brief Test poprawności \return true jeśli jest dobrze zdefiniowany
	virtual double  Waznosc() { return Waga; }  ///< \brief Po prostu waga innymi słowy
	/// \brief Metoda pobiera wszystkie potrzebne dane z listy stringów. \note Jak błąd to podaje która pozycja
	virtual bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);

    // Do wizualizacji
    // ///////////////
	virtual void Narysuj();      ///< \brief Wizializacja węzła
	virtual void Wymazuj();      ///< \brief Wymazanie miejsca po wizualizacji
	virtual const char* Nazwa(); ///< \brief Podaje nzawę węzła \note Musi posiadać jakaś nazwę!

	// Proste akcesory ważne dla węzła
    // ///////////////////////////////
	virtual bool Trafiony(float X,float Y);  ///< \brief Czy punkt w obrębie węzła? \note Np. do inspekcji myszką
	virtual double ZOrder() {return _Z;}     ///< \brief Współrzędna Z \details Do ustalania kolejności wyświetlania
	virtual double X() { return _X; }        ///< \brief Współrzędna X
	virtual double Y() { return _Y; }        ///< \brief Współrzędna Y
	virtual double R(double Angle);          ///< \brief Promień otaczającego okręgu lub elipsy
	virtual DziedzKol* Dziedzina() { return &Col;}           ///< \brief Dziedzina czyli kolor węzła

    // DZIAŁANIA WĘZŁA
    // ///////////////

    /// \brief Okazja na przetwarzanie \note Daje jakieś szanse na endogenne zmiany stanów
	virtual void ChwilaDlaCiebie();

    /// \brief Przetworzenie komunikatu do węzła
    /// \note  Próbuje obrobić procesami, a jak się nie uda to
    ///        "brodkastuje" komunikat losowo do jednego lub dwu sąsiadów
    ///        (oczywiście jeśli ich ma)
	virtual void InterpretujKomunikat(Komunikat* Co);

  protected:
	//DziedzKol  		Col;   //Dziedziczony z ElementModelu
	//DaneLokalne		Dane;  //Dane tekstowe z ElementModelu
	float    		_X;         ///< Współrzędna X
    float    		_Y;         ///< Współrzędna Y
	float			_Z;         ///< Współrzędna Z
	float			Waga;       ///<  Ważność węzła
	Wielobok* 		Ksztalt;    ///<  Kształt węzła przy wizualizacji

	// Metody pomocnicze dla klas domyślnej akcji ChwilaDlaCiebie() i dla klas potomnych
    // //////////////////////////////////////////////////////////////////////////////////
    virtual bool _PrzeniesDaneNaPola(unsigned& Blad); ///< \brief Interpretuje pola od 1 do 7 w ZrobWgListy()

	virtual bool _KomunikatDoProcesow(Komunikat* Co);    ///< \brief Obrobienie komunikatu przez pierwszy chętny proces

    /// \brief Przekazuje komunikat losowo i z prawdopodobieństwem P namnaża
    /// \note  Dosyć to kosztowna funkcja, zwłaszcza póki (TODO!) 'Swiat' nie ma cache-owania linków wezła!
	virtual void _KomunikatPrzekazLosowo(Komunikat* Co,double P);

    virtual void _RuszProcesPriorytetowy(); ///< \brief Posunięcie do przodu najpilniejszego z procesów
    virtual void _RuszKazdyProces();        ///< \brief Wywołuje każdemu procesowi ChwilaDlaCiebie()

    /// \brief   Jakiś zarządca procesów misi być zawsze dostępny
    void          ZarzadcaProcesowPrymitywny() { _RuszKazdyProces(); }
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

