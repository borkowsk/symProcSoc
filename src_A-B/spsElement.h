////////////////////////////////////////////////////////////////////////////////
// Symulator Procesów Sieciowych/Spolecznych (c) Instytut Studiów Spo³ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
////////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
////////////////////////////////////////////////////////////////////////////////
//
//Klasa bazowa definiuj¹ca najbardziej podstawowy interfejs do elementów modelu
// G³ównie chodzi o wizualizacje i inicjalizacje z pliku
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
#ifndef spsElementH
#define spsElementH

#include <string> //Musi byæ bo u¿ywany w deklaracjach

//Pomocnicza unia do manipulacji kolorami/dziedzinami elementow
union DziedzinaWKolorze
{
	unsigned ARGB;
	struct{     //Naiwna kolejnoœæ ARGB nie koniecznie musi dzia³ac
		unsigned B:8;
		unsigned G:8;
		unsigned R:8;
		unsigned A:8;
	};
	float F; //Jakby kto chcia³ widzieæ to jako Float
	bool operator == (const DziedzinaWKolorze& s) { return this->ARGB==s.ARGB;}
};
typedef DziedzinaWKolorze DziedzKol;

class Swiat; //Zapowiedz Swiata

class ElementModelu
{
	void operator = (const ElementModelu&); //Blokada bezpoœredniego przypisywania
 public:
	unsigned MojID(); //Zwraca Indeks ustawiony przez Œwiat albo jakiœ innych identyfikator wobec swiata

	ElementModelu();//Konstruktor domyœlny

	virtual ~ElementModelu();//Destruktor wirtualny, bo bêd¹ metody wirtualne
	virtual bool   Poprawny()=0; //true jeœli jest dobrze zdefiniowany. Jak nie to mo¿e zostaæ usuniêty!
	virtual double Waznosc() { return 1; } //Zwraca wspólczynnik wa¿noœci obiektu. Np. wagê wêz³a lub linku, priorytet procesu, etc.

	//WSPARCIE WEJSCIA WYJSCIA i dostêpu do danych tekstowych
	//Metoda pobiera wszystkie potrzebne dane z listy stringów. Jak blad to IlePol podaje ktora pozycja listy
	//Jak bez b³edu to IlePol podaje pierwsze pole za list¹
	virtual bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& IlePol)=0;
	//Jak ma jakieœ dane to mo¿na przeczytaæ - i np. zwizualizowaæ, oraz zmieniæ
	virtual unsigned 	IlePolDanych();  //Ile jest w tej chwili pól
	virtual const char* NazwaPola(unsigned Ktore);
	virtual const char* WartoscPola(unsigned Ktore);
	virtual bool 		UstawPole(unsigned Ktore,const char* NowaWartosc);
	//Metoda aktualizuj¹ca zapis stanu obiektu na jego liœcie danych. Po jej wykonaniu
	//pobierane pola musz¹ byæ ju¿ poprawne i mo¿na siê nimi pos³u¿yæ do zapisu do pliku
	//W szczególnoœci mo¿e realokowaæ listê "Dane", jeœli nie mia³a wczeœniej poprawnego rozmiaru
	virtual void AktualizujListeDanych();

	//Wspólny interfejs wizualizacji
	virtual void Narysuj()=0;
	virtual void Wymazuj()=0;
	virtual bool Trafiony(float X,float Y) { return false;} //Np. do inspekcji myszk¹


	virtual void ChwilaDlaCiebie()=0; //Daje mu szanse na endogenne zmiany stanów

	//Kolor czyli "dziedzinê" musi miec kazdy obiekt w u¿ywany w SPS
	//Mo¿e on znaczyæ ró¿ne rzeczy, ale przede wszystkim jest potrzebny w ka¿dej wizualizacji
	//Jego czytanie i pisanie mo¿na obudowaæ w klasach potomnych w razie potrzeby
	virtual DziedzKol	PodajDziedzine();
	virtual void 	UstawDziedzine(const DziedzinaWKolorze& D);
	virtual void 	UstawDziedzine(unsigned R,unsigned G, unsigned B,unsigned A=0);

 public:
 //Klasa pomocnicza obslugujaca zarz¹dzanie danymi tekstowymi obiektów modelu
 //a mog¹ca te¿ przej¹c zarz¹dzanie ich konwersjami i przekszta³cenie w warianty
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
	void Realokuj(unsigned Ile);//Rozbudowuje lub skraca listê
	//Akcesory
	unsigned Ile() const { return IleDanych;}
	std::string& operator [] (unsigned index) const ;
	//Przypisania i konwersje na elementy listy
	//Zwracaj¹ true jak siê uda³o
	bool PrzypiszZ(unsigned gdzie,double co);
	bool PrzypiszZ(unsigned gdzie,long   co);
	//Zwracaj¹ indeks znaku który nie pasowa³ lub -1 jako sukces
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
	static void Zarejestruj(const char* NazwaKlasyKonstruowanej,WirtualnyKonstruktor* ); //Obiekt musi istnieæ puki jest potrzebny
	static void Wyrejestruj(const char* NazwaKlasyKonstruowanej,WirtualnyKonstruktor* ); //A potem mo¿na wyrejestrowaæ

  public:
	static const char* 			 ListaNazwTypow(void); //Generuje przegl¹dow¹ listê nazw zarejestrowanych typów elementów
	static WirtualnyKonstruktor* DajWirtualnyKonstruktor(const char* NazwaKlasy); //Znajduje wg. zarejestrowanej nazwy typu

	virtual  const char* NazwaTypu()=0; //Nazwa typu dla tego obiektu konstruuj¹cego
	virtual  unsigned 	 IleNazwPol();//Dwa pola s¹ obslugiwane nawet jak nie ma nazw!
	virtual  const char* NazwaPola(unsigned i);//Bezpieczniejsze w u¿yciu, a najlepiej uzywac "nazwy pola" z ElementModelu

	virtual  unsigned&    Flagi()=0; //32 bity na jakieœ nieprzewidziane wczeœniej informacje konfiguracyjne
	static const unsigned VFLAG=0x3; //Najmlodsze bity zarezerwowany na tryb wizualizacji
									 //* 0 nie powinien pokazywaæ obiektów danej klasy!
									 //* 1 tylko kropki
									 //* 2 domyslny sposób uproszczony
									 //* 3 sposob wirtualny, zwykle graficznie rozbudowany

	//NAJWA¯NIEJSZY ELEMENT -  TWORZENIE OBIEKTU ODPOWIEDNIEGO TYPU POCHODNEGO OD Element
	virtual  ElementModelu*  KonstruktorWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)=0;

  public:
	DaneLokalne Nazwy; //Nazwy pól, s¹ te¿ dostêpne bezpoœrednio, ale mog¹ byæ puste!!!
 };

//Kazda klasa potomna musi mieæ klasê pe³ni¹ca rolê wirtualnego konstruktora tworz¹cego takie obiekty na stercie
//Obiekt takiego szablonu trzeba zadeklarowaæ i zainicjowac jako pole static ka¿dej klasy  potomnej dla "Element"
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

 // POLA chronione i publiczne stanowi¹ce dane klasy Element
 ///////////////////////////////////////////////////////////////////////////////
 protected:
	friend class Swiat; //Ma dostêp do pól prywatnych - zw³aszcza do zapisu indeksu

	unsigned _MojIndeks;//Pole jest ustawiane przez Swiat(!!!)
				//Pozwala siê obiektowi zlokalizowaæ w strukturach Œwiata

	DziedzKol Col; //Ka¿dy "Element" SPS musi mieæ swój g³ówny kolor, który mo¿e znaczyæ ró¿ne rzeczy
				// ale przede wszystkim jest potrzebny w ka¿dej wizualizacji
 public:
	//Obsluga danych tekstowych - jako udostêpniony obiekt, co nie jest zbyt bezpieczne (!)
	DaneLokalne Dane; //I kazdy ElementModelu ma taki jeden obiekt  danych tekstowych

	//Obsluga wirtualnego kreowania
	//Dla usprawnienia klasa potomna mo¿e zwracaæ adres swojego wirtualnego konstruktora
	//albo NULL oznaczaj¹cy ¿e trzeba go szukaæ wg. zarejestrowanej nazwy
	//lub ¿e go nie ma ... (co mo¿e stanowiæ problem)
	virtual WirtualnyKonstruktor* VKonstruktor() { return NULL; }
 	virtual unsigned              VWidocznosc(); //Bazuj¹ca na FLAGS informacja o widocznosci
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
#endif    // opiElementH

