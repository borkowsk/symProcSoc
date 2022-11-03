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
///     2022.11.03 (last updated)
// //////////////////////////////////////////////////////////////////////////////

#ifndef spsElementH
#define spsElementH

#include <string> //Musi być bo używany w deklaracjach

//Pomocnicza unia do manipulacji kolorami/dziedzinami elementow
union DziedzinaWKolorze
{
	unsigned ARGB;
	struct{     //Naiwna kolejność ARGB nie koniecznie musi działac
		unsigned B:8;
		unsigned G:8;
		unsigned R:8;
		unsigned A:8;
	};
	float F; //Jakby kto chciał widzieć to jako Float
	bool operator == (const DziedzinaWKolorze& s) { return this->ARGB==s.ARGB;}
	void operator = (unsigned ARGB) { this->ARGB=ARGB;} // explicit
	unsigned IleBitow(); //Zwraca liczbę bitów 1 w tych danych
};
typedef DziedzinaWKolorze DziedzKol;

class Swiat; //Zapowiedz Swiata

class ElementModelu
{
	void operator = (const ElementModelu&); //Blokada bezpośredniego przypisywania
 public:
	unsigned MojID(); //Zwraca Indeks ustawiony przez Świat albo jakiś innych identyfikator wobec swiata

	ElementModelu();//Konstruktor domyślny

	virtual ~ElementModelu();//Destruktor wirtualny, bo będą metody wirtualne
	virtual bool   Poprawny()=0; //true jeśli jest dobrze zdefiniowany. Jak nie to może zostać usunięty!
	virtual double Waznosc() { return 1; } //Zwraca wspólczynnik ważności obiektu. Np. wagę węzła lub linku, priorytet procesu, etc.

	//WSPARCIE WEJSCIA WYJSCIA i dostępu do danych tekstowych
	//Metoda pobiera wszystkie potrzebne dane z listy stringów. Jak blad to IlePol podaje ktora pozycja listy
	//Jak bez błedu to IlePol podaje pierwsze pole za listą
	virtual bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& IlePol)=0;
	//Jak ma jakieś dane to można przeczytać - i np. zwizualizować, oraz zmienić
	virtual unsigned 	IlePolDanych();  //Ile jest w tej chwili pól
	virtual const char* NazwaPola(unsigned Ktore);
	virtual const char* WartoscPola(unsigned Ktore);
	virtual bool 		UstawPole(unsigned Ktore,const char* NowaWartosc);
	//Metoda aktualizująca zapis stanu obiektu na jego liście danych. Po jej wykonaniu
	//pobierane pola muszą być już poprawne i można się nimi posłużyć do zapisu do pliku
	//W szczególności może realokować listę "Dane", jeśli nie miała wcześniej poprawnego rozmiaru
	virtual void AktualizujListeDanych();

	//Wspólny interfejs wizualizacji
	virtual void Narysuj()=0;
	virtual void Wymazuj()=0;
	virtual bool Trafiony(float X,float Y) { return false;} //Np. do inspekcji myszką


	virtual void ChwilaDlaCiebie()=0; //Daje mu szanse na endogenne zmiany stanów

	//Kolor czyli "dziedzinę" musi miec kazdy obiekt w używany w SPS
	//Może on znaczyć różne rzeczy, ale przede wszystkim jest potrzebny w każdej wizualizacji
	//Jego czytanie i pisanie można obudować w klasach potomnych w razie potrzeby
	virtual DziedzKol	PodajDziedzine();
	virtual void 	UstawDziedzine(const DziedzinaWKolorze& D);
	virtual void 	UstawDziedzine(unsigned R,unsigned G, unsigned B,unsigned A=0);

 public:
 //Klasa pomocnicza obslugujaca zarządzanie danymi tekstowymi obiektów modelu
 //a mogąca też przejąc zarządzanie ich konwersjami i przekształcenie w warianty
 class DaneLokalne
 {
	std::string* Dane;
	unsigned 	IleDanych;
	const DaneLokalne& operator = (const DaneLokalne& Wzor); //Prywatne przypisanie - BRAK!
 public:
	DaneLokalne():Dane(0),IleDanych(0){}
	~DaneLokalne();

	void InicjujWgListy(const std::string* Lista,unsigned Ile);
	void InicjujZDanych(const DaneLokalne& Wzor);
	void Realokuj(unsigned Ile);//Rozbudowuje lub skraca listę
	//Akcesory
	unsigned Ile() const { return IleDanych;}
	std::string& operator [] (unsigned index) const ;
	//Przypisania i konwersje na elementy listy
	//Zwracają true jak się udało
	bool PrzypiszZ(unsigned gdzie,double co);
	bool PrzypiszZ(unsigned gdzie,long   co);
	//Zwracają indeks znaku który nie pasował lub -1 jako sukces
	int KonwertujDo(unsigned zkad,float& naco);
	int KonwertujDo(unsigned zkad,double& naco);
	int KonwertujDo(unsigned zkad,int& naco);
	int KonwertujDo(unsigned zkad,unsigned& naco);
 };

 public:
// Interface konieczny do elastycznego wczytywania z tablicy stringów lub pliku
// !!! Ale nie róbcie tego sami w domu :-)
 class WirtualnyKonstruktor   //Klasa bazowa wirtualnych konstruktorów
 {
  protected:
	static void Zarejestruj(const char* NazwaKlasyKonstruowanej,WirtualnyKonstruktor* ); //Obiekt musi istnieć puki jest potrzebny
	static void Wyrejestruj(const char* NazwaKlasyKonstruowanej,WirtualnyKonstruktor* ); //A potem można wyrejestrować

  public:
	static const char* 			 ListaNazwTypow(void); //Generuje przeglądową listę nazw zarejestrowanych typów elementów
	static WirtualnyKonstruktor* DajWirtualnyKonstruktor(const char* NazwaKlasy); //Znajduje wg. zarejestrowanej nazwy typu

	virtual  const char* NazwaTypu()=0; //Nazwa typu dla tego obiektu konstruującego
	virtual  unsigned 	 IleNazwPol();//Dwa pola są obslugiwane nawet jak nie ma nazw!
	virtual  const char* NazwaPola(unsigned i);//Bezpieczniejsze w użyciu, a najlepiej uzywac "nazwy pola" z ElementModelu

	virtual  unsigned&    Flagi()=0; //32 bity na jakieś nieprzewidziane wcześniej informacje konfiguracyjne
	static const unsigned VFLAG=0x3; //Najmlodsze bity zarezerwowany na tryb wizualizacji
									 //* 0 nie powinien pokazywać obiektów danej klasy!
									 //* 1 tylko kropki
									 //* 2 domyslny sposób uproszczony
									 //* 3 sposob wirtualny, zwykle graficznie rozbudowany

	//NAJWAŻNIEJSZY ELEMENT -  TWORZENIE OBIEKTU ODPOWIEDNIEGO TYPU POCHODNEGO OD Element
	virtual  ElementModelu*  KonstruktorWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)=0;

  public:
	DaneLokalne Nazwy; //Nazwy pól, są też dostępne bezpośrednio, ale mogą być puste!!!
 };

//Kazda klasa potomna musi mieć klasę pełniąca rolę wirtualnego konstruktora tworzącego takie obiekty na stercie
//Obiekt takiego szablonu trzeba zadeklarować i zainicjowac jako pole static każdej klasy  potomnej dla "Element"
 template<class JakiElement>
 class KonstruktorElementowModelu:public  WirtualnyKonstruktor
 {
	std::string MojaNazwa;
	unsigned    FLAGI;
  public:
	KonstruktorElementowModelu(const char* NazwaKlasyKonstruowanej):MojaNazwa(NazwaKlasyKonstruowanej),FLAGI(VFLAG)
		{ Zarejestruj(MojaNazwa.c_str(),this); }

	~KonstruktorElementowModelu()  { Wyrejestruj(MojaNazwa.c_str(),this); }

	const char* NazwaTypu()	{ return MojaNazwa.c_str(); }
	unsigned&   Flagi()     { return FLAGI; }

	ElementModelu*  KonstruktorWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
	{
	   JakiElement* Tworzony=new JakiElement();
	   if(Tworzony->ZrobWgListy(Lista,Ile,Blad))
				 return Tworzony;
		  else  { delete Tworzony; return NULL;}
	}
 };

 // POLA chronione i publiczne stanowiące dane klasy Element
 ///////////////////////////////////////////////////////////////////////////////
 protected:
	friend class Swiat; //Ma dostęp do pól prywatnych - zwłaszcza do zapisu indeksu

	unsigned _MojIndeks;//Pole jest ustawiane przez Swiat(!!!)
				//Pozwala się obiektowi zlokalizować w strukturach Świata

	DziedzKol Col; //Każdy "Element" SPS musi mieć swój główny kolor, który może znaczyć różne rzeczy
				// ale przede wszystkim jest potrzebny w każdej wizualizacji
 public:
	//Obsluga danych tekstowych - jako udostępniony obiekt, co nie jest zbyt bezpieczne (!)
	DaneLokalne Dane; //I kazdy ElementModelu ma taki jeden obiekt  danych tekstowych

	//Obsluga wirtualnego kreowania
	//Dla usprawnienia klasa potomna może zwracać adres swojego wirtualnego konstruktora
	//albo NULL oznaczający że trzeba go szukać wg. zarejestrowanej nazwy
	//lub że go nie ma ... (co może stanowić problem)
	virtual WirtualnyKonstruktor* VKonstruktor();
	virtual unsigned              VWidocznosc(); //Bazująca na FLAGS informacja o widoczności
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

