/// \file
/// \brief Główne deklaracje parametrów modelu, globalnych funkcji i interface-ów struktur danych
//         --------------------------------------------------------------------------------------
///
/// \details
///              ...
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011, wborkowski@uw.edu.pl
/// \date
///     2022.11.03 (last updated)
//*////////////////////////////////////////////////////////////////////////////////

#ifndef __SPS_MODEL_MAIN_HEADER_H__
#define __SPS_MODEL_MAIN_HEADER_H__

/// \brief Parametry konfiguracyjne SPS
/// \details mogą być zmienione parametrami wywołania programu,
///          a używane są od inicjalizacji obiektu klasy `Swiat`.
///          TODO: Powinny być raczej wewnątrz klasy `Swiat` (!?!?!)
const unsigned 	NumerKoloruTla=255+200; ///< \brief Indeksowany kolor tła \note const-y nie są do zmieniania!

extern int 		VerboseInput; ///< default=1;   \brief Parametr konfiguracyjny  VERBOSE
extern int 		   hor;       ///< default=790; \brief Parametr konfiguracyjny  HORIZONTAL
extern int 		   ver;       ///< default=570; \brief Parametr konfiguracyjny  VERTICAL
extern double   JakieRwProcWidth;      ///< default=5;  \brief Parametr konfiguracyjny RADIUS
extern unsigned DefMaxLiczbaWezlow;    ///< default=2;  \brief Ograniczenie na liczbę węzłów. Liczby od 0 do 10 oznaczają wielokrotność liczby początkowej...
extern unsigned DefMaxLiczbaLinkow;    ///< default=-1; \brief Ograniczenie na liczbę linków. -1 dla linków oznacza graf pełny
extern unsigned DefMaxLiczbaKomunikatow;    ///< default=10*1024; \brief Ograniczenie na liczbę komunikatów (połączenia ???)
extern unsigned DefLiczbaProcesowNaWezel;   ///< default=10*1024; \brief Ograniczenie na liczbę procesów węzła

/// Wartośc zaawansowania wymuszająca skasowanie procesu przy najbliższej okazji.
///          TODO: Powinno być raczej wewnątrz klasy `Proces` (!?!?!)
const double PROCES_NAD_ZAAWANSOWANY=2.0;

#include "spsElement.h"  //Klasa bazowa dla wszystkich elementów modelu SPS

// Klasy bazowe dla bardziej konkretnych elementów modelu, ale nadal
// "pure virtual" - potrzebne tylko jako typy elementów tablic "Świata"
// ////////////////////////////////////////////////////////////////////
class Komunikat; //Deklaracja zapowiadająca komunikat
class Proces;    //Deklaracja zapowiadająca proces

class WezelSieci:public ElementModelu
{
  public:
    // Z interfejsu wszelkich elementów
    // ////////////////////////////////
	virtual ~WezelSieci(){}    ///< \brief Destruktor wirtualny, bo są metody wirtualne
	virtual bool Poprawny()=0; ///< \brief Czy węzeł poprawny. Jak nie to może zostać usunięty! \return true jeśli jest dobrze zdefiniowany.
	virtual void Narysuj()=0;  ///< \brief Metoda rysująca
	virtual void Wymazuj()=0;  ///< \brief Metoda czyszcząca z wizualizacji
	virtual void ChwilaDlaCiebie(){}; ///< \brief Metoda zmiany stanu. Daje mu szanse na endogenne zmiany stan�w

    // Specyficzne tylko dla węzłów
    // ////////////////////////////
	virtual const char* Nazwa()=0;   ///< \brief Akcesor nazwy. Obiekt musi posiadać jakąś nazwę
	virtual double ZOrder()=0;       ///< \brief Akcesor kolejności wyświetlania
	virtual double X()=0;            ///< \brief Akcesor X środka węzła
	virtual double Y()=0;            ///< \brief Akcesor Y środka węzła
	virtual double R(double angle)=0;  ///< \brief Akcesor promienia otaczającego okręgu lub elipsy
	virtual DziedzKol* Dziedzina()=0;  ///< \brief Akcesor "dziedziny" kodowanej kolorystycznie

    ///  \brief Metoda interpretacji komunikatu
    /// \details Może zareagować, może coś zrobić z komunikatem, ale przechowywać może tylko kopie
    ///          komunikatu bo oryginał jest zarządzany przez świat i może zniknąć po powrocie z tej metody.
    /// \note    Wersja domyślna nie robi w ogóle nic.
	virtual void InterpretujKomunikat(Komunikat* Co){}

};


/// \brief Klasa procesu wykonywanego na węźle
/// \details Może to być tylko atrapa procesu, ale klasy potomne mogą wykonywać jakieś realne działania.
class Proces:public ElementModelu
{
  public:
    // To co byc musi
    // ///////////////
	Proces():_MojWezel(-1),_IleKrokowDoKonca(0){} ///< Konstruktor musi zapewnić domyślną NIEPOPRAWNOŚĆ indeksu węzła
	virtual ~Proces(){}                           ///< Destruktor wirtualny, bo są metody wirtualne
	virtual bool Poprawny()=0;        ///< \brief test poprawności \return `true` jeśli jest dobrze zdefiniowany
    virtual void Narysuj()=0;         ///< \brief Metoda rysująca
    virtual void Wymazuj()=0;         ///< \brief Metoda czyszcząca z wizualizacji
	virtual void ChwilaDlaCiebie(){}; ///< \brief Możliwość zmiany stanu procesu albo zrobienia czegoś realnego

    // Specyficzne dla Procesu-projektu
    // ////////////////////////////////
	virtual double   JakZaawansowany()=0;  ///< \brief Stopień zaawansowania procesu. \return 0 po starcie, a jak 1 to zakończony
	virtual double   Priorytet()=0;        ///< \brief Priorytet procesu. Im wyższa wartość tym proces więcej działa.
	virtual unsigned Procesor(){return _MojWezel;} ///< \brief Indeks węzła który realizuje proces
			float    DoKonca(){ return _IleKrokowDoKonca;} ///< \brief Ile (kroków) do oczekiwanego konca.

    /// \brief Przetwarzanie komunikatu skierowanego do tego procesu
    /// \details Tak jak węzeł, może coś zrobić z komunikatem, ale przechowywać może tylko kopie
    ///          bo oryginał komunikatu może zniknąć po powrocie z tej metody.
    /// \return  Zwraca `true`, jeśli przetworzył komunikat
    virtual bool InterpretujKomunikat(Komunikat* Co){return false;}

  private: // POLA
	friend class Swiat;
	unsigned _MojWezel; //To pole MUSI być jakoś dostępne dla klasy `SWIAT`

  protected:
	float _IleKrokowDoKonca; //Ile kroków czasu zostało do deadlinu. `Swiat::` odejmuje co krok.

	void __ChceWezelNr(unsigned indeks) // Tylko do ustawiania przy wczytaniu -
	{  _MojWezel=indeks; }              // jedynie procedura inicjująca świat robi z tego użytek!!!
};

/// \brief Interface powiązania elementów
class Powiazanie:public ElementModelu
{
  public:
	virtual ~Powiazanie(){}           ///< \brief Destruktor wirtualny
	virtual bool Poprawny()=0;        ///< \brief Test poprawności. \return true je�li jest dobrze zdefiniowany.
    virtual double ZOrder()=0;        ///< \brief Do ustalania kolejnosci wyswietlania
    virtual void Narysuj()=0;         ///< \brief Metoda rysująca
    virtual void Wymazuj()=0;         ///< \brief Metoda czyszcząca z wizualizacji
	virtual void ChwilaDlaCiebie(){}; ///< \brief Daje mu szanse na endogenne zmiany stanów

    // Specyficzne dla Powiązania (linku)
    // //////////////////////////////////
	virtual unsigned Poczatek()=0;    ///< \brief Co jest początkiem połączenia \return indeks węzła
	virtual unsigned Koniec()=0;      ///< \brief Co jest koncem  połączenia \return indeks węzła
	virtual bool  Kierunkowy()=0;     ///< \brief Czy połączenie symetryczne? Wtedy niema znaczenia co jest końcem co początkiem.

    /// \brief Ustala czy dany komunikat może przejść tym łączem
    /// \details Przelicza położenia wzdłuż linku
    ///          Może ponadto uwzględniać specyfikę komunikatu
	virtual bool  Akceptacja(Komunikat* Co)=0;

	virtual void  PodajPozycje(double D,bool KierunekZwykly,
							   double& X,double& Y,Komunikat* K=NULL)=0;
};

/// \brief Interface komunikatu
class Komunikat:public ElementModelu
{
  public:
	virtual ~Komunikat(){}      ///< \brief Destruktor wirtualny, bo są metody wirtualne
    ///< \brief Test poprawności.
	virtual bool Poprawny()=0;  ///< \return true jeśli jest dobrze zdefiniowany (wciąż istnieje nadawca i odbiorca etc...)
    virtual void Narysuj()=0;         ///< \brief Metoda rysująca
    virtual void Wymazuj()=0;         ///< \brief Metoda czyszcząca z wizualizacji
    virtual void ChwilaDlaCiebie(){}; ///< \brief Daje mu szanse na endogenne zmiany stanów

    // Specyficzne dla Komunikatu (Message)
    // ////////////////////////////////////
    virtual const std::string& Rodzaj()=0;    ///< \brief Jak nic nie ma to zwraca "???"
    virtual bool  UstawRodzaj(const char* )=0;    ///< \brief A jak typ nie udostępnia zmiany to zwraca false
	virtual float  JakDostawa()=0;    ///< \brief 0 po starcie, jak 1 to już dotarł
	virtual float  JakiTermin()=0;    ///< \brief W ile kroków powinien zostać dostarczony?
	virtual unsigned Kanal()=0;       ///< \brief Po ktorym połczeniu biegnie komunikat \return indeks linku
	virtual bool 	 KierunekZgodny()=0;      ///< \brief Czy idzie zgodnie z nominalnym kierunkiem linku czy pod prąd
	virtual unsigned Nadawca()=0;     ///< \brief Od kogo komunikat \return indeks nadawcy
	virtual unsigned Odbiorca()=0;    ///< \brief Do kogo komunikat \return indeks odbiorcy
	virtual Komunikat* Klonuj()=0;    ///< \brief Robi na stercie kopie komunikatu do przechowania lub wstawienia

	// Adresowanie w różne sposoby
    // ////////////////////////////
	virtual bool 	Zwrotnie(float _Szybkosc=0)=0;    ///< \brief Można odwrócić komunikat - zwłaszcza jak jest to klon

    ///< \brief  Adresowanie komunikatu - od kogo do kogo
    ///< \return Zwraca `true` jak odnajdzie oba węzły i poprawnie zaadresuje
	virtual bool  	Zaadresuj(  unsigned _Nadawca,   ///< indeks nadawcy
                                unsigned _Odbiorca,  ///< indeks odbiorcy
                                float _Szybkosc=0    ///< zadana prędkość dostawy
                                        )=0;

    ///< \brief Adresowanie komunikatu - przez poprawny kanał komunikacji
    ///< \return Zwraca `true` jak odnajdzie link i poprawnie zaadresuje
	virtual bool  	Zaadresuj( unsigned _Powiazanie, ///< indeks powiązania
                               bool  KierunekZwykly, ///< kierunek użycia powiązania
                               float   _Szybkosc=0   ///< zadana prędkość dostawy
                                       )=0;
};

/// \brief Klasa interface-u świata. Zarządza danymi całej symulacji
/// \details
///     UWAGA - Wszystkie dane tej klasy są static czyli tylko w jednej kopii.
///     Każdy potomna klasa świata ma do nich dostęp, więc w praktyce
///     oczekujemy że każdy świat będzie singletonem w tej aplikacji.
/// \note
///     Zamiast tworzyć klasę potomną można
///     każdą metodę klasy `Swiat::`  podmienić na poziomie linkera,
///     o ile nie zmieniają się pola.
///     TODO: Zapewnić żeby każda metoda była w odrębnej jednostce kompilacji
class Swiat     //:public ElementModelu - TODO: CZY ŚWIAT MOŻE BYĆ ELEMENTEM ŚWIATA - PO CO?
{
protected:
    /// \brief Konstruktor ukryty - bo tylko jeden świat
	Swiat();

public:
    /// \brief Skalowanie współrzędnych
	static double SkalujX(); ///< TODO Bardzo często używane. Może inline-y zrobić?
	static double SkalujY(); ///< TODO Bardzo często używane. Może inline-y zrobić?

    /// \note
    ///        Wszystkie dane są w jednej kopii na aplikacje.
	static Swiat Ten; 	///< Staramy sie mieć tylko jeden `Swiat` :-)

	static const unsigned INV_INDEX=unsigned(-1); ///< Marker wadliwych indeksów

	/// \brief Destruktor wirtualny
	virtual ~Swiat();

	/// \brief Parametry wizualizacyjne - Co wyświetlać w każdym kroku
	static  bool WyswietlajWezly;      ///<  \brief  Czy wizualizować węzły?
	static  bool WyswietlajProcesy;    ///<  \brief  Czy wizualizować procesy?
	static  bool WyswietlajLinki;      ///<  \brief  Czy wizualizować powiązania?
	static  bool WyswietlajInformacje; ///<  \brief  Czy wizualizować dodatkowe informacje?
    static  bool ForceTolerant;        ///<  Przy wczytywaniu danych tylko ostrzega o niepoprawnych linkach
                                       ///<  TODO ? i innych błędach niekrytycznych ?
	// Które pole elementów służy do opisu linków, a które węzłów?
	static  unsigned OpisWezlow;       ///< \brief Indeks pola opisu węzłów
	static  unsigned OpisLinkow;       ///< \brief Indeks pola opisu powiązań

    // Główne procedury wizualizacyjne
	virtual void Narysuj(); ///< \brief Odrysowanie całości świata
	virtual void Wymazuj(); ///< \brief Wyczyszczenie całości świata - może być "na skróty"

    /// \brief Czas na zmianę, czyli krok modelu
    /// \details Daje światu szanse na endogenne zmiany stanów jego składowych
	virtual void ChwilaDlaCiebie(){Krok_modelu();};

	// Ponieważ dane są static to wszystkie "metody" mogą być "static"
	// /////////////////////////////////////////////////////////////////////////

    /// \brief Wczytanie i/lub wylosowanie struktury modelu
    /// \param PlikWejsciowy - nazwa pliku wejściowego, tekstowego we właściwym formacie
    /// \param DelimiterDanych - delimiter pól w pliku wejściowym, np '\t',',',';',':'
	static bool Inicjalizacja(const char* PlikWejsciowy,const char DelimiterDanych);

    /// \brief Wykonanie kroku modelu. \note Wizualizacja może być realizowana (częściowo?) w trakcie tej zmiany
	static bool Krok_modelu();

    /// \brief Wykonanie pełnej wizualizacji stanu modelu w danej chwili
	static bool Wizualizacja_po_kroku();

    /// \brief   Wyliczenie pełnej statystyki modelu w danej chwili
    static bool Statystyka_po_kroku();

    /// \brief   Na zakończenie kroku trzeba odnowić graficzny wiersz statusu
	static bool Status_po_kroku();

    /// \brief   Numer kroku w postaci liczby typu double!
    /// \details Może kroki będą też ułamkowe? A w ogóle double ma więcej cyfr znaczycych
	static double NumerKroku();

    /// \brief    Zbadanie elementu pod kursorem myszy
    /// \details  Odnajduje element wg. pozycji pikselowej
    ///           i opcjonalnie od razu wyświetla w oknie wizualizacji pełne informacje o nim.
	static ElementModelu* Inspekcja(int x,int y,bool Wizual=false);

    // Pomocnicze funkcje wyszukiwawcze
    /// \brief  Wyszukuje indeks węzła po nazwie, co może być dosyć kosztowne
    /// \return Zwraca INVINDEX jak nie znajdzie takiego
    static unsigned 	ZnajdzIndeksWezla(const char* Nazwa);

    /// \brief Podaje indeks procesu o najwyższym priorytecie w danym węźle
    /// \details Może zwrócić -1 jak nie ma procesów, a opcjonalnie ich liczbę na parametrze *IleRealnie
    static unsigned     NajpilniejszyProc( unsigned KtoryWezel,         ///< indeks węzła
                                           unsigned* IleRealnie=NULL,   ///< [out] ile w sumie procesów na tym węźle
                                           bool Posortuj=false          ///< CO TO TODO???
                                                   );

    /// \note
	///     Dostępy do tablic - mogą rzucić wyjątkiem, albo przerwać przy błędzie dostępu,
	///     albo w "łagodnej" formie mogą zwrócić NULL, tak jak w przypadku pustego
	///     czyli np. "uśmierconego" slotu w tablicy.
	static WezelSieci*  Wezel(unsigned Ktory); ///< zmienia indeks węzła na wskaźnik do węzła \return może zwrócić NULL
	static Powiazanie*  Lacze(unsigned Ktory); ///< zmienia indeks łącza na wskaźnik do niego \return może zwrócić NULL
	static Komunikat*   Info(unsigned Ktory);  ///< zmienia indeks komunikatu na wskaźnik do niego \return może zwrócić NULL

    /// \brief Zmiana indeksu procesu w danych węźle na wskaźnik do procesu.
    /// \details Procesy są związane z węzłami, choć teoretycznie można sobie wyobrazić ich migrację.
    /// \note Indeksy procesów są poprawne tylko tymczasowo. Każde wywołanie scheduler-a może je zmienić
	static Proces*		Proc(  unsigned Ktory,                ///< indeks węzła
                               unsigned KtoryWezel=INV_INDEX  ///< indeks procesu
                                       );

	// Aktualne rozmiary tablic, ale w tym obrębie mogą być puste sloty
	static unsigned IleWezlow();                ///< \brief Ile jest zajętych slotów węzłów.
	static unsigned IlePowiazan();              ///< \brief Ile jest zajętych slotów powiązań
	static unsigned IleInformacji();            ///< \brief Ile jest zajętych slotów komunikatów

    /// \brief Ile jest zajętych slotów procesów
    /// \param KtoryWezel ==INVINDEX to ogólnie, jak indeks węzła to lokalnie
	static unsigned IleProcesow( unsigned KtoryWezel=INV_INDEX );

	//Liczba jeszcze w ogóle dostępnych slotow
	static 	unsigned IleMoznaWezlow();          ///< \brief Ile jest w ogóle slotów węzłów.
	static 	unsigned IleMoznaPowiazan();        ///< \brief Ile jest w ogóle slotów powiązań
	static 	unsigned IleMoznaInformacji();      ///< \brief Ile jest w ogóle slotów komunikatów

    /// \brief Ile jest w ogóle slotów procesów
    /// \param KtoryWezel ==INVINDEX to ogólnie, jak indeks węzła to lokalnie
	static  unsigned IleMoznaProcesow(unsigned KtoryWezel=INV_INDEX);

	/// \note UWAGA NA ZMIANY STRUKTURY!
	/// \brief Notyfikacja wstawiania i usuwania węzłów
	/// \details
	///     Niektóre dane do wizualizacji mogą być liczone rzadziej niż co krok/klatkę obrazu:
	///     np. gdy tylko usunięcie/dodanie wezła, usuniecie/dodanie linku powoduje ich zmianę.
	///     Czasem te zmiany są w obrębie obiektów podrzędnych i wtedy trzeba poinformować
	///     klasę "świata", że wszystko trzeba sprawdzić, ewentualnie przeliczyć od nowa.
	///     Dotyczy to na pewno zmian wartości "ZOrder".
	static void UwagaZmienionoStrukture();

	/// \note
	///     Funkcje wstawiające elementy zwracają pozycje wstawienia i PRZEJMUJĄ ZARZĄD PAMIĘCIĄ OBIEKTU!
	///     Wstawiane elementy muszą być alokowane na stercie (przez "new").
	///     Jak się nie uda z jakiś powodów wstawić to i tak wstawiany obiekt jest deletowany!
	///     Nie jest więc bezpiecznie używać roboczego wskaźnika po wstawieniu.
	///     \param propozycja -  pozwala zaoszczędzić szukania miejsca na liście, jeśli trafia.
	static unsigned WstawWezel(WezelSieci* Jaki,unsigned propozycja=INV_INDEX); ///< \brief Wstawianie
	static unsigned WstawLacze(Powiazanie* Jaki,unsigned propozycja=INV_INDEX); ///< \brief Wstawianie
	static unsigned WstawInfo(Komunikat* Jaki,unsigned propozycja=INV_INDEX);   ///< \brief Wstawianie

    /// \brief Wstawianie procesu na węzeł
    /// \param Jaki - wskaźnik na nowo zaalokowany i przygotowany proces
    /// \param KtoryWezel - Musi być jasne który węzeł
	static unsigned WstawProc(Proces* Jaki,unsigned KtoryWezel);

	/// \note UWAGA USUWANIE JEST GENERALNIE DOSYĆ DROGIE!
	/// \details
    ///    Tańsze są funkcje usuwające wg. indeksu (lub uchwytu, zależnie od implementacji)
    ///    Funkcje usuwające wg. nazwy muszą najpierw zlokalizować obiekt.
	///    - Usuwanie węzła usuwa też wszystkie związane z nim łącza, komunikaty i procesy
	/// \brief Usuwanie węzła wg. indeksu.
	static void UsunWezel(unsigned Ktory);

    /// \brief Usuwanie łącza wg. indeksu.
    /// \details
    ///    - Usuwanie łącza usuwa związane z nim komunikaty
	static void UsunLacze(unsigned Ktory);

    /// \brief Usuwanie komunikatu wg. indeksu.
    static void UsunInfo(unsigned Ktory);

    /// \brief Usuwanie procesu wg. indeksu. z węzłem wg. indeksu.
    /// \param KtoryWezel - indeks węzła
    /// \param Ktory      - indeks procesu
    static void UsunProc(unsigned Ktory,unsigned KtoryWezel);

    /// \details
	/// - Funkcja usuwające wg. wskaźnika węzła - kosztowne bo najpierw lokalizują
	static unsigned UsunWezel(WezelSieci* Jaki);    ///< \brief Usuwanie węzła wg. wskaźnika
	static unsigned UsunLacze(Powiazanie* Jaki);    ///< \brief Usuwanie łącza wg. wskaźnika
	static unsigned UsunInfo(Komunikat* Jaki);      ///< \brief Usuwanie komunikat wg. wskaźnika

    /// \brief Usuwanie procesu wg. wskaźnika
    /// \param Jaki - wskaźnik do procesu
    /// \param KtoryWezel  - indeks węzła
	static unsigned UsunProc(Proces* Jaki,unsigned KtoryWezel);

	static bool _UstalLiczbeInfo(unsigned IleMax);  ///< W celu zmiany w konfiguruj

	private:
    // Zmiany rozmiarów struktur potrzebne są tylko wewnętrznie (na razie?)
	static bool _UstalLiczbeLaczy(unsigned IleMax);
	static bool _UstalLiczbeWezlow(unsigned IleMax);
	static bool _UstalPoczLiczbeProc(unsigned IleMaxDlaWezla);
};


// Globalne funkcje. Jeśli zwracają `false` to znaczy, że się cos nie udało
// /////////////////////////////////////////////////////////////////////////
bool konfiguruj(const char* parametr,const char* wartosc=0); ///< Przetwarza pojedynczy "configpar" o postaci XXX=YYY lub XXX i YYY jako drugi
bool czytaj_parametry(int argc,const char* argv[]);          ///< Przetwarza całą liste parametrów wywołania programu

/// \brief    Dialog wyboru pliku.
/// \details  Może być w konsoli tekstowej lub dopasowany do używanej platformy.
bool podaj_nazwe_pliku(char* Bufor,unsigned size,const char* Monit="Nazwa pliku");

/// RANDOMIZER MUSI BYĆ WYBRANY RAZ NA APLIKACJE!
#define USES_STDC_RAND
#include "random.h"

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
#endif //__SPS_MODEL_MAIN_HEADER_H__ - KONIEC NAGŁÓWKA

