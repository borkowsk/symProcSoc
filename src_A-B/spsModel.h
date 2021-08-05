////////////////////////////////////////////////////////////////////////////////
// Symulator Procesów Sieciowych/Spolecznych (c) Instytut Studiów Spo³ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
////////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
////////////////////////////////////////////////////////////////////////////////
//
// G³ówne deklaracje parametrów modelu, globalnych funkcji i struktur danych
////////////////////////////////////////////////////////////////////////////////
#ifndef __SPS_MODEL_MAIN_HEADER_H__
#define __SPS_MODEL_MAIN_HEADER_H__

//Parametry konfiguracyjne SPS - mog¹ byæ zmienione parametrami wywo³ania,
//a u¿ywane od inicjalizacji `Swiata'

//Powinno byæ raczej wewn¹trz klasy Swiat (!?!?!)
const unsigned 	NumerKoloruTla=255+200;//const\y nie do zmieniania
extern int 		VerboseInput;//=1; 	//Parametr konfiguracyjny  VERBOSE
extern int 		   hor;//=790;      //Parametr konfiguracyjny  HORIZONTAL
extern int 		   ver;//=570;      //Parametr konfiguracyjny  VERTICAL
extern double   JakieRwProcWidth;//=5;   //Parametr konfiguracyjny RADIUS
extern unsigned DefMaxLiczbaWezlow;//=2  //Liczby od 0 do 10 oznaczaj¹ wielokrotnoœæ liczby pocz¹tkowej...
extern unsigned DefMaxLiczbaLinkow;//=-1 //wiêksze s¹ brane literalnie, -1 dla linków oznacza graf pe³ny
extern unsigned DefMaxLiczbaKomunikatow; //=10*1024
extern unsigned DefLiczbaProcesowNaWezel;//=10*1024

#include "spsElement.h"  //Klasa bazowa dla wszystkich elementow modelu SPS

//Klasy bazowe dla bardziej konkretnych elementów modelu, ale nadal
// "pure virtual" - potrzebne tylko jako typy elementów tablic "œwiata"
//////////////////////////////////////////////////////////////////////
class Komunikat;//Deklaracja zapowiadajaca
class Proces;

class WezelSieci:public ElementModelu
{
  public:
	virtual ~WezelSieci(){}//Destruktor wirtualny, bo bêd¹ metody wirtualne
	virtual bool Poprawny()=0; //true jeœli jest dobrze zdefiniowany. Jak nie to mo¿e zostaæ usuniêty!
	virtual void Narysuj()=0;
	virtual void Wymazuj()=0;
	virtual void ChwilaDlaCiebie(){}; //Daje mu szanse na endogenne zmiany stanów
//Specyficzne dla Wêz³a
	virtual const char* Nazwa()=0;//Musi posiadaæ jakas nazwe
	virtual double ZOrder()=0; //Do ustalania kolejnosci wyswietlania
	virtual double X()=0; //X srodka wêz³a
	virtual double Y()=0; //Y œrodka wêz³a
	virtual double R(double angle)=0; //Promieñ otaczaj¹cego okrêgu lub elipsy
	virtual DziedzKol* Dziedzina()=0;
	virtual void InterpretujKomunikat(Komunikat* Co){}//Mo¿e coœ zrobiæ z komunikatem, ale przechowywac tylko kopie
			//bo oryginal komunikatu jest zarz¹dzany przez Œwiat i mo¿e znikn¹æ po powrocie z tej metody
};

const double PROCES_NAD_ZAAWANSOWANY=2.0;//Wartoœc zaawansowania wymuszaj¹ca skasowanie procesu przy najbli¿szej okazji
class Proces:public ElementModelu
{
  public:
	Proces():_MojWezel(-1),_IleKrokowDoKonca(0){}//Konstruktor musi zapewniæ domysln¹ NIEPOPRAWNOSC indeksu wez³a
	virtual ~Proces(){}//Destruktor wirtualny, bo bêd¹ metody wirtualne
	virtual bool Poprawny()=0; //true jeœli jest dobrze zdefiniowany (wci¹¿ istnieje link, nadawca i odbiorca etc...)
	virtual void Narysuj()=0;
	virtual void Wymazuj()=0;
	virtual void ChwilaDlaCiebie(){}; //Daje mu procesowi mo¿liwoœæ zmiany stanów - zrobienia czegoœ
  //Specyficzne dla Procesu-projektu
	virtual bool InterpretujKomunikat(Komunikat* Co){return false;}//Mo¿e coœ zrobiæ z komunikatem, ale przechowywac tylko kopie
			//bo oryginal komunikatu jest zarz¹dzany przez Œwiat i mo¿e znikn¹æ po powrocie z tej metody
			//Zwraca bool

	virtual double   JakZaawansowany()=0;//0 po starcie, jak 1 to ju¿ zakoñczony
	virtual double   Priorytet()=0; //Im wy¿szy tym proces wiêcej dziala
	virtual unsigned Procesor(){return _MojWezel;} //Wêze³ który realizuje proces
			float    DoKonca(){ return _IleKrokowDoKonca;} //Ile do deadlinu
  private:
	friend class Swiat;
	unsigned _MojWezel; //To pole MUSI byæ dostêpne dla klasy ŒWIAT

  protected:
	float _IleKrokowDoKonca;//Ile kroków czasu zosta³o do deadlinu. Swiat:: odejmuje co krok.

	void __ChceWezelNr(unsigned Ktory)//Tylko do ustawiania przy wczytaniu -
	{  _MojWezel=Ktory; } //jedynie procedura inicjujaca Œwiat robi z tego u¿ytek!!!
};

class Powiazanie:public ElementModelu
{
  public:
	virtual ~Powiazanie(){}//Destruktor wirtualny, bo bêd¹ metody wirtualne
	virtual bool Poprawny()=0; //true jeœli jest dobrze zdefiniowany. Wci¹¿ istniej¹ oba konce itp.
	virtual void Narysuj()=0;
	virtual void Wymazuj()=0;
	virtual void ChwilaDlaCiebie(){}; //Daje mu szanse na endogenne zmiany stanów
  //Specyficzne dla Powi¹zania (linku)
	virtual unsigned Poczatek()=0; //Co jest koncem co poczatkiem mo¿e nie miec znaczenia
	virtual unsigned Koniec()=0;
	virtual bool  Kierunkowy()=0;
	virtual double ZOrder()=0; //Do ustalania kolejnosci wyswietlania
	virtual bool  Akceptacja(Komunikat* Co)=0;//Ustala czy dany komunikat mo¿e przejœæ tym ³¹czem
	virtual void  PodajPozycje(double D,bool KierunekZwykly,
							   double& X,double& Y,Komunikat* K=NULL)=0;//Przelicza polozenia wdluz linku
							   //Mo¿e uwzglêdniaæ te¿ specyfikê komunikatu
};

class Komunikat:public ElementModelu
{
  public:
	virtual ~Komunikat(){}//Destruktor wirtualny, bo bêd¹ metody wirtualne
	virtual bool Poprawny()=0; //true jeœli jest dobrze zdefiniowany (wci¹¿ istnieje link, nadawca i odbiorca etc...)
	virtual void Narysuj()=0;
	virtual void Wymazuj()=0;
	virtual void ChwilaDlaCiebie(){}; //Daje mu szanse na endogenne zmiany stanów
  //Specyficzne dla Komunikatu (Message)
	virtual float  JakDostawa()=0;//0 po starcie, jak 1 to ju¿ dotar³=0
	virtual unsigned Kanal()=0; //Po ktorym po³¹czeniu czyli linku
	virtual bool 	 KierunekZgodny()=0;//Czy idzie zgodnie z nominalnym kierunkiem linku czy pod pr¹d
	virtual unsigned Nadawca()=0; //Od kogo ...
	virtual unsigned Odbiorca()=0;//... do kogo.
	virtual Komunikat* Klonuj()=0; //Robi na stercie kopie komunikatu do przechowania lub wstawienia
	//Adresowanie w rózny sposóbw
	virtual bool 	Zwrotnie(float _Szybkosc=0)=0;//Mo¿na odwróciæ komunikat - zw³aszcza jak jest to klon
	virtual bool  	Zaadresuj(unsigned _Nadawca,unsigned _Odbiorca,float _Szybkosc=0)=0; //Zwracaj¹ true jak odnajdzie
	virtual bool  	Zaadresuj(unsigned _Powiazanie,bool KierunekZwykly,float _Szybkosc=0)=0;//...poprawny kana³ komunikacji
};

//Klasa interface'u Swiata - zarz¹dzaj¹ca danymi calej symulacji
///////////////////////////////////////////////////////////////////
//UWAGA - de facto SINGLETON!!! Dane s¹ tylko w jednej kopii - ka¿dy Œwiat ma do
//nich dostêp
//Chodzi o definicje interface'u, którego implementacje mo¿na ³atwo zmieniæ

class Swiat//:public ElementModelu - PO CO?
{

	//!!!
	//Wszystkie dane s¹ w jednej kopii poza deklaracj¹ klasy !!!
	//!!!
	Swiat();         //Konstruktor  prywatny - bo tylko jeden Œwiat
	public:
	//Staramy sie miec tylko jeden Swiat :-)
	static Swiat Ten; //!!!
	static const unsigned INVINDEX=unsigned(-1); //Marker wadliwych indeksów

	//Czêœæ wirtualna ze wzglêdu na dziedziczenie - mo¿e niepotrzebnie tak
	virtual ~Swiat();//Destruktor wirtualny
	//Wizualizacyjne - Co wyswietlaæ w ka¿dym kroku i definicja procedur
	static  bool WyswietlajWezly;
	static  bool WyswietlajProcesy;
	static  bool WyswietlajLinki;
	static  bool WyswietlajInformacje;
	//Które pole s³u¿y do opisu linków a które wêz³ów
	static  unsigned OpisWezlow;
	static  unsigned OpisLinkow;
	virtual void Narysuj(); //Odrysowanie ca³oœci
	virtual void Wymazuj(); //Wyczyszczenie caloœci - mo¿e byæ "na skróty"
	virtual void ChwilaDlaCiebie(){Krok_modelu();}; //Daje mu szanse na endogenne zmiany stanów

	//Poniewa¿ dane s¹ w jednej to wszystkie inne "metody" mog¹ byæ "static"
	///////////////////////////////////////////////////////////////////////////
	static bool Inicjalizacja(const char* PlikWejsciowy,const char DelimiterDanych);   //Wczytanie i/lub wylosowanie struktury modelu
	static bool Krok_modelu();     //Wykonanie kroku modelu - mo¿e byæ wizualizacja w trakcie
	static bool Wizualizacja_po_kroku(); //Wykonanie pe³nej wizualizacji modelu w danej chwili
	static bool Statystyka_po_kroku(); //Wyliczenie pe³nej statystyki modelu w danej chwili
	static bool Status_po_kroku();  //Na zakonczenie kroku trzeba odnowiæ graficzny wiersz statusu

	static double NumerKroku(); //Mo¿e kroki bêd¹ te¿ u³amkowe? A w ogóle double ma wiêcej cyfr znacz¹cych
	static ElementModelu* Inspekcja(int x,int y,bool Wizual=false);//Odnajduje element wg. pozycji pikselowej
							//i opcjonalnie od razu wyœwietla w oknie wizualizacji pe³ne informacje o nim

	// Dostêpy do tablic - mog¹ rzuciæ wyj¹tkiem albo przerwaæ przy b³êdzie dostêpu
	// albo w "³agodnej" formie mo¿e zwróciæ NULL, tak jak w przypadku pustego
	// czyli np. "usmierconego" slotu w tablicy
	static WezelSieci*  Wezel(unsigned Ktory);
	static Powiazanie*  Lacze(unsigned Ktory);
	static Komunikat*   Info(unsigned Ktory);
	static Proces*		Proc(unsigned Ktory,unsigned KtoryWezel=INVINDEX);//Zwi¹zane z wez³ami
						//!!!Indeksy procesów s¹ poprawne tylko tymczasowo

	//Pomocnicze funkcje wyszukiwawcze
	static unsigned 	ZnajdzIndeksWezla(const char* Nazwa); //Zwraca INVINDEX) jak nie znajdzie
	static unsigned     NajpilniejszyProc(unsigned KtoryWezel,unsigned* IleRealnie=NULL);//Daje indeks procesu o najwy¿szym priorytecie
															//Mo¿e zwrócic -1 jak nie ma procesów, a opcjonalnie ich liczbê na *IleRealnie
	//Aktualne rozmiary tablic - ale w ich obrêbie mog¹ byæ puste sloty
	static unsigned IleWezlow();
	static unsigned IlePowiazan();
	static unsigned IleInformacji();
	static unsigned IleProcesow(unsigned KtoryWezel=INVINDEX);//jak INVINDEX to ogolnie, jak numer wêz³a to lokalnie

	//Liczba jeszcze dostêpnych slotow
	static 	unsigned IleMoznaWezlow();
	static 	unsigned IleMoznaPowiazan();
	static 	unsigned IleMoznaInformacji();
	static  unsigned IleMoznaProcesow(unsigned KtoryWezel=INVINDEX);//jak INVINDEX to ogolnie, jak numer wêz³a to lokalnie

	//UWAGA NA ZMIANY STRUKTURY!
	//Niektóre dane do wizualizacji mog¹ byæ liczone rzadziej ni¿ co krok/klatkê obrazu -
	//np. usuniêcie/dodanie wez³a, usuniecie/dodanie linku
	//ale czasem te zmiany s¹ w obrêbie obiektow podrzêdnych i wtedy trzeba poinformowaæ
	//klasê "Œwiata" ¿e wszystko trzeba policzyæ od nowa.
	//Dotyczy to na pewno zmian wartoœci "ZOrder"
	static void UwagaZmienionoStrukture(); //Wstawianie i usuwanie wezlow jest ...

	//Funkcje wstawiaj¹ce zwracaj¹ pozycje wstawienia i PRZEJMUJ¥ ZARZ¥D PAMIÊCI¥
	// - wstawiane elementy musz¹ byæ alokowane na stercie (przez "new")
	//Jak siê nie uda z jakiœ powodów wstawiæ to wstawiany obiekt jest deletowany!
	//Nie jest wiêc bezpiecznie u¿ywaæ roboczego wskaŸnika po wstawieniu
	//Pole "propozycja" pozwala zaoszczêszczêdziæ szukania miejsca jeœli trafia
	static unsigned WstawWezel(WezelSieci* Jaki,unsigned propozycja=INVINDEX);
	static unsigned WstawLacze(Powiazanie* Jaki,unsigned propozycja=INVINDEX);
	static unsigned WstawInfo(Komunikat* Jaki,unsigned propozycja=INVINDEX);
	static unsigned WstawProc(Proces* Jaki,unsigned KtoryWezel);//Musi byæ jasne który wêze³

	//UWAGA NA USUWANIE!  GENERALNIE DOSYÆ DROGIE
	//* Usuwanie wêz³a usuwa te¿ wszystkie zwi¹zane z nim ³¹cza, komunikaty i procesy
	//* Usuwanie ³¹cza usuwa zwi¹zane z nim komunikaty

	//Tansze funkcje usuwaj¹ce wg. indeksu (lub uchwytu, zale¿nie od implementacji
	static void UsunWezel(unsigned Ktory);
	static void UsunLacze(unsigned Ktory);
	static void UsunInfo(unsigned Ktory);
	static void UsunProc(unsigned Ktory,unsigned KtoryWezel=INVINDEX);

	//Funkcja usuwaj¹ce wg. wskaŸnika - kosztowne bo szukaj¹
	static unsigned UsunWezel(WezelSieci* Jaki);
	static unsigned UsunLacze(Powiazanie* Jaki);
	static unsigned UsunInfo(Komunikat* Jaki);
	static unsigned UsunProc(Proces* Jaki,unsigned KtoryWezel=INVINDEX);

	private:  //Zmiany rozmiarów struktur potrzebne s¹ tylko wewnêtrznie (na razie?)
	static bool _UstalLiczbeWezlow(unsigned IleMax);
	static bool _UstalLiczbeLaczy(unsigned IleMax);
	static bool _UstalLiczbeInfo(unsigned IleMax);
	static bool _UstalPoczLiczbeProc(unsigned IleMaxDlaWezla);
};


//Inne Globalne funkcje - jeœli zwracaj¹ false to znaczy ¿e siê coœ nie uda³o
bool konfiguruj(const char* parametr,const char* wartosc=0); //Przetwarza pojedynczy "configpar" o postaci XXX=YYY lub XXX i YYY jako drugi
bool czytaj_parametry(int argc,const char* argv[]); //Przetwarza ca³¹ liste parametrow
bool podaj_nazwe_pliku(char* Bufor,unsigned size,const char* Monit="Nazwa pliku");

//RANDOMIZER MUSI BYÆ WYBRANY RAZ NA APLIKACJE!
#define USES_STDC_RAND
#include "INCLUDE/Random.h"

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
#endif //__SPS_MODEL_MAIN_HEADER_H__ - KONIEC NAG£ÓWKA

