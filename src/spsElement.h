/// \file
/// \brief Klasa bazowa definiująca najbardziej podstawowy interfejs do elementów modelu
///        -----------------------------------------------------------------------------
///
/// \details
///              Głównie chodzi o wizualizacje i inicjalizację z pliku
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
/// \date
///     2022.11.04 (last updated)
// ///////////////////////////////////////////////////////////////////////////////////////

#ifndef spsElementH
#define spsElementH

#include <cassert>
#include <string> // Musi być, bo używany std::string w deklaracjach

/// \brief Pomocnicza unia do manipulacji kolorami/dziedzinami elementów
/// \note  TODO Wymaga sprawdzenia zgodności z kodowaniem kolorów w używanej bibliotece wizualizacyjnej!
/// \details constexpr - NIE DZIAŁA DLA TEJ UNII
///          UŻYJ: DziedzinaWKolorze::union_test_for_call_somewhere() w main() albo jakoś tak.
union DziedzinaWKolorze
{
	unsigned ARGB;  ///< \brief Pole zbiorcze \details Naiwna kolejność pól w ARGB nie koniecznie musi działać
	struct{
		unsigned B:8;  ///< byte Blue
		unsigned G:8;  ///< byte Green
		unsigned R:8;  ///< byte Red
		unsigned A:8;  ///< byte Alfa
	};
	float F;             ///< Jakby ktoś chciał widzieć to jako `float` (?) Tylko po co?

    /// \brief Konstruktor domyślny
    explicit DziedzinaWKolorze(): ARGB(0) {}

    /// \brief Konstruktor kopiujący
    //explicit
    DziedzinaWKolorze(const DziedzinaWKolorze&) = default;

    /// \brief Konstruktor z  liczby typu `unsigned`
    /// \details constexpr - NIE DZIAŁA DLA TEJ UNII
    ///          Use DziedzinaWKolorze::union_test_for_call_somewhere()
    constexpr explicit DziedzinaWKolorze(unsigned iARGB): ARGB(iARGB) {}

    /// \brief Operator przypisania liczby typu `unsigned`
    //explicit
    DziedzinaWKolorze& operator = (unsigned iARGB) { ARGB=iARGB; return *this; }

    /// \brief Trywialny operator porównania.
	bool operator == (const DziedzinaWKolorze& s) const
    { return this->ARGB==s.ARGB;}

    /// \brief  Zliczenie bitów ustawionych na 1
    /// \return Zwraca liczbę bitów 1 w tych danych
	unsigned IleBitow();

    /// \brief    Funkcja testująca konstrukcję tej uni.
    /// \details  Nie umiałem zrobić tego z użyciem static_assert
    static void union_test_for_call_somewhere()
    {
        DziedzinaWKolorze _wzorDziedziny{0x01020304};
        assert(_wzorDziedziny.A==1); //"Unexpected field order (ARGB.A)");
        assert(_wzorDziedziny.R==2); //"Unexpected field order (ARGB.R)");
        assert(_wzorDziedziny.G==3); //"Unexpected field order (ARGB.G)");
        assert(_wzorDziedziny.B==4); //"Unexpected field order (ARGB.B)");
    }
} ;

// To nie działa:
// error: accessing ‘DziedzinaWKolorze::<anonymous>’ member instead of initialized ‘DziedzinaWKolorze::ARGB’ member in constant expression
// (CLion, clang, C++12, UBUNTU 20.04)
//static_assert((DziedzinaWKolorze{0x01020304}).A==1); //"Unexpected field order (ARGB.A)");

/// \brief Skrót nazwy typu DziedzinaWKolorze
typedef DziedzinaWKolorze DziedzKol;

class Swiat; //Zapowiedz klasy świata

/// \brief Klasa definiująca interface do każdego elementu modelu
class ElementModelu
{
	void operator = (const ElementModelu&) = delete; ///< Blokada bezpośredniego przypisywania

 public:
    /// \brief Każdy element modelu musi znac swój własny identyfikator
	unsigned MojID(); ///< \details Zwraca Indeks ustawiony przez Swiat:: albo jakichś innych identyfikator wobec świata

	ElementModelu(); ///< \brief Konstruktor domyślny

	virtual ~ElementModelu();    ///< \brief Destruktor wirtualny, bo będą metody wirtualne

    /// \brief Test poprawności stanu obiektu
	virtual bool   Poprawny()=0; ///< \return true jeśli jest dobrze zdefiniowany. Jak nie to może zostać usunięty!

    /// \brief Wartość ważności obiektu (waga, priorytet etc.)
	virtual double Waznosc() { return 1; } ///< \return współczynnik ważności obiektu.
                                           ///< Np. wagę węzła lub linku, priorytet procesu, etc.

	// WSPARCIE WEJŚCIA WYJŚCIA i dostępu do danych tekstowych
    // ///////////////////////////////////////////////////////

    /// \brief Wypełnianie obiektu z listy wartości tekstowych
    /// \details
	///        Metoda pobiera wszystkie potrzebne dane z listy stringów. Jak błąd, to `IlePol` podaje pozycję na liście
	///        Jak bez błędu to IlePol podaje pierwsze pole za listą
	virtual bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& IlePol)=0;

    /// \brief Ile jest pól obiektu (w tej chwili?)
	/// \details
	///       Jak obiekt ma jakieś dane to można je przeczytać żeby np. zwizualizować, ale można też zmienić.
	virtual unsigned 	IlePolDanych();

    /// \brief Nazwa pola o danym indeksie \param Ktore - indeks pola.
	virtual const char* NazwaPola(unsigned Ktore);

    /// \brief Wartość pola w postaci tekstowej \param Ktore - indeks pola.
    virtual const char* WartoscPola(unsigned Ktore);

    /// \brief Nadawanie wartości polu
    /// \param Ktore - indeks pola.
    /// \param NowaWartosc - nowa wartość w postaci tekstowej
    /// \return `false` jeśli nie można wykonać konwersji lub z innego powodu nie da się ustawić wartości tego pola.
    virtual bool 		UstawPole(unsigned Ktore,const char* NowaWartosc);

    /// \brief Aktualizacja tekstowego dostępu do pól obiektu
    /// \details
	///     Metoda aktualizująca zapis stanu obiektu na jego liście danych. Po jej wykonaniu
	///     pobierane pola muszą być już poprawne i można się nimi posłużyć do zapisu do pliku
	///     W szczególności może realokować listę "Dane", jeśli nie miała wcześniej poprawnego rozmiaru.
	virtual void AktualizujListeDanych();

	// Wspólny interfejs wizualizacji
    // //////////////////////////////
	virtual void Narysuj()=0; ///< \brief Narysowanie obiektu
	virtual void Wymazuj()=0; ///< \brief W miarę dokładne wymazanie obiektu

    /// \brief Test czy pukt/piksel jest w obiekcie \details Np. do inspekcji myszką
	/// \note  TODO: Dlaczego parametry `float`?
	virtual bool Trafiony(float X,float Y) { return false;}

    /// \brief Przydział czasu CPU na zmianę stanu obiektu
    /// \details Daje mu szanse na endogenne zmiany stanów, co nie znaczy że muszą one zajść
	virtual void ChwilaDlaCiebie()=0;

    /// \brief Podaje "dziedzinę" danego obiektu
    /// \details
	///     Kolor czyli "dziedzinę" musi miec kazdy obiekt w używany w SPS
	///     Może on znaczyć różne rzeczy, ale przede wszystkim jest potrzebny w każdej wizualizacji
	///     Jego czytanie i pisanie można obudować w klasach potomnych w razie potrzeby
	virtual DziedzKol	PodajDziedzine();

    /// \brief Ustawia "dziedzinę" danego obiektu na podstawie dziedziny innego
	virtual void 	UstawDziedzine(const DziedzinaWKolorze& D);

    /// \brief Ustawia "dziedzinę" danego obiektu na podstawie składowych RGBA
	virtual void 	UstawDziedzine(unsigned R,unsigned G, unsigned B,unsigned A=0);

 public:

 /// \brief Klasa obsługi nazwanych danych obiektu
 /// \details
 ///     Klasa pomocnicza obsługująca zarządzanie danymi tekstowymi obiektów modelu
 ///     a mogąca też przejąc zarządzanie ich konwersjami i przekształcenie w warianty.
 class DaneLokalne
 {
	std::string* Dane;          ///< ???
	unsigned 	 IleDanych;     ///< ???

	const DaneLokalne& operator = (const DaneLokalne& Wzor) = delete; ///< przypisania BRAK!
 public:
	DaneLokalne():Dane(0),IleDanych(0){}        ///< \brief Jedyny konstruktor tworzący pusty obiekt
	~DaneLokalne();                             ///< \brief Destruktor nie jest wirtualny

    /// \brief Wypełnienie obiektu danymi z listy
	void InicjujWgListy(const std::string* Lista,unsigned Ile);

     /// \brief Wypełnienie obiektu kopiowanymi danymi
	void InicjujZDanych(const DaneLokalne& Wzor);

    /// \brief Metoda która rozszerza lub skraca listę
	void Realokuj(unsigned Ile);

	// Akcesory
    // ////////

	unsigned Ile() const { return IleDanych;}           ///< \brief Ile ma zapamiętanych danych
	std::string& operator [] (unsigned index) const ;   ///< \brief Dostęp do konkretnej danej

	// Przypisania i konwersje na elementy listy
	// Zwracają true jak się udało
    // //////////////////////////////////////////

    /// \brief Przypisywanie danej z konwersją z typu `double` \return true jak się udało
	bool PrzypiszZ(unsigned gdzie,double co);

    /// \brief Przypisywanie danej z konwersją z typu `long`  \return true jak się udało
	bool PrzypiszZ(unsigned gdzie,long   co);

	//Zwracają indeks znaku, który nie pasował lub -1 jako sukces

    /// \brief konwersja pola na `float` \return -1 jak sukces albo indeks znaku gdzie błąd
	int KonwertujDo(unsigned zkad,float& naco);

    /// \brief konwersja pola na `double` \return -1 jak sukces albo indeks znaku gdzie błąd
	int KonwertujDo(unsigned zkad,double& naco);

    /// \brief konwersja pola na `int` \return -1 jak sukces albo indeks znaku gdzie błąd
	int KonwertujDo(unsigned zkad,int& naco);

    /// \brief konwersja pola na `unsigned` \return -1 jak sukces albo indeks znaku gdzie błąd
	int KonwertujDo(unsigned zkad,unsigned& naco);
 };

 /// \brief   Klasa bazowa wirtualnych konstruktorów elementów modelu
 /// \details
 ///          Interface konieczny do elastycznego wczytywania
 ///          Bardziej złożony od tego z wb_rtm. Pozwala na czytanie z liku lub z tablicy stringów.
 /// \note Ale nie róbcie tego sami w domu :-)
 class WirtualnyKonstruktor
 {
  protected:
    /// \brief Rejestrator wirtualnego konstruktora danej klasy \note Obiekt musi istnieć puki jest potrzebny
    /// \details Chroniony, bo oczekujemy  ze używany tylko w konstruktorach klas potomnych
	static void Zarejestruj(const char* NazwaKlasyKonstruowanej,WirtualnyKonstruktor* );

    /// \brief De-rejestrator wirtualnego konstruktora danej klasy \note Jak już niepotrzebny to można wyrejestrować
    /// \details Chroniony, bo oczekujemy  ze używany tylko w destruktorach klas potomnych
	static void Wyrejestruj(const char* NazwaKlasyKonstruowanej,WirtualnyKonstruktor* );

  public:
    /// \brief Lista typów \details Generuje przeglądową listę nazw zarejestrowanych typów elementów
	static const char* 			 ListaNazwTypow();

    /// \brief Wirtualny konstruktor dla typu o znanej nazwie \details Znajduje wg. __zarejestrowanej__ nazwy typu
	static WirtualnyKonstruktor* DajWirtualnyKonstruktor(const char* NazwaKlasy);


	virtual  const char* NazwaTypu()=0; ///< \brief Nazwa typu obsługiwanego dla tego obiektu wirtualnego konstruktora
	virtual  unsigned 	 IleNazwPol();  ///< \brief Ile jest nazwanych pól \note Dwa pola są obslugiwane nawet jak nie ma nazw!

    /// \details Bezpieczniejsze w użyciu, a najlepiej używać "nazwy pola" z ElementModelu
	virtual  const char* NazwaPola(unsigned i);

    /// \brief Słowo flag danego typu.
    /// \details
    ///     32 bity na jakieś nieprzewidziane wcześniej informacje konfiguracyjne.
    ///     Najmłodsze bity zarezerwowany na tryb wizualizacji:
    ///     * 0 nie powinien pokazywać obiektów danej klasy!
    ///     * 1 tylko jako kropki
    ///     * 2 domyślny sposób uproszczony
    ///     * 3 sposób wirtualny, zwykle graficznie rozbudowany
	virtual  unsigned&    Flagi()=0;

	static const unsigned VFLAG=0x3; ///< Stała maskująca bity wizualizacji na słowie flag konfiguracji typu.


	/// \brief NAJWAŻNIEJSZA METODA -  TWORZENIE OBIEKTU ODPOWIEDNIEGO TYPU POCHODNEGO OD Element
	virtual  ElementModelu*  KonstruktorWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)=0;

  public:
	DaneLokalne Nazwy; ///< Nazwy pól, są też dostępne bezpośrednio, ale mogą być puste!!!
 };

 /// \brief Szablon wirtualnych konstruktorów
 /// \details
 ///     Każda klasa potomna musi mieć klasę pełniąca rolę wirtualnego konstruktora tworzącego takie obiekty na stercie.
 ///     Obiekt takiego szablonu trzeba zadeklarować i zainicjować jako pole static każdej klasy  potomnej
 ///     dla klasy `ElementModelu::`.
 template<class ObslugiwanyTyp>
 class KonstruktorElementowModelu:public  WirtualnyKonstruktor
 {
	std::string MojaNazwa;  ///< Nazwa obsługiwanego typu
	unsigned    FLAGI;      ///< Flagi konfiguracji obsługiwania typu, zwłaszcza konfiguracja wizualizacji

  public:
	KonstruktorElementowModelu(const char* NazwaKlasyKonstruowanej):MojaNazwa(NazwaKlasyKonstruowanej),FLAGI(VFLAG)
		{ Zarejestruj(MojaNazwa.c_str(),this); }

	~KonstruktorElementowModelu()  { Wyrejestruj(MojaNazwa.c_str(),this); }

    /// \brief Nazwa typu obsługiwanego dla tego obiektu wirtualnego konstruktora
	const char* NazwaTypu() override { return MojaNazwa.c_str(); }

    /// \brief Słowo flag danego typu.
	unsigned&   Flagi() override     { return FLAGI; }

    /// \brief Wirtualna konstrukcja na podstawie tekstowej listy parametrów
	ElementModelu*  KonstruktorWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad) override
	{
	   auto Tworzony=new ObslugiwanyTyp();
	   if(Tworzony->ZrobWgListy(Lista,Ile,Blad))
				 return Tworzony;
		  else  { delete Tworzony; return NULL;}
	}
 };

 // POLA chronione i publiczne stanowiące dane klasy ElementModelu
 // //////////////////////////////////////////////////////////////
 protected:
	friend class Swiat;  ///< Świat ma dostęp do pól prywatnych - zwłaszcza konieczne do zapisu!

	unsigned _MojIndeks; ///< \brief Liczbowy identyfikator obiektu w świecie symulacji
	                     ///<  Pole jest ustawiane przez Swiat(!!!)
				         ///<  Pozwala się obiektowi zlokalizować w strukturach Świata.

	DziedzKol Col; ///< \brief "Dziedzina" czyli kolor
	               ///<   Każdy "Element" SPS musi mieć swój główny kolor, który może znaczyć różne rzeczy,
				   ///<   ale przede wszystkim jest potrzebny w każdej wizualizacji
 public:
	/// \brief Tekstowe dane lokalne
    /// \details
    ///     Obsługa danych tekstowych - jako udostępniony obiekt, co nie jest zbyt bezpieczne (TODO ?!)
    ///     Kazdy ElementModelu ma taki jeden obiekt danych tekstowych, ale jego wypełnienie jest "leniwe".
	DaneLokalne Dane;

	/// \brief Obsluga wirtualnego kreowania
	/// \details
	///     Dla usprawnienia klasa potomna może zwracać adres swojego wirtualnego konstruktora
	///     albo NULL oznaczający że trzeba go szukać wg. zarejestrowanej nazwy
	///     lub że go nie ma ... (co może stanowić problem)
	/// \return Wirtualny konstruktor danej klasy albo NULL oznaczający że trzeba jednak szukać po nazwie.
	virtual WirtualnyKonstruktor* VKonstruktor();

    /// \brief   Informacja o wizualizacji
    /// \details Bazująca na KonstruktorElementowModelu< __obsługiwany_typ__ >::FLAGS
	virtual unsigned              VWidocznosc();
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
#endif    // opiElementH

