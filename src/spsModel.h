// //////////////////////////////////////////////////////////////////////////////
// Symulator Proces�w Sieciowych/Spolecznych (c) Instytut Studi�w Spo�ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
// //////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
// //////////////////////////////////////////////////////////////////////////////
//
// G��wne deklaracje parametr�w modelu, globalnych funkcji i struktur danych
// //////////////////////////////////////////////////////////////////////////////
#ifndef __SPS_MODEL_MAIN_HEADER_H__
#define __SPS_MODEL_MAIN_HEADER_H__

//Parametry konfiguracyjne SPS - mog� by� zmienione parametrami wywo�ania,
//a u�ywane od inicjalizacji `Swiata'

//Powinno by� raczej wewn�trz klasy Swiat (!?!?!)
const unsigned 	NumerKoloruTla=255+200;//const\y nie do zmieniania
extern int 		VerboseInput;//=1; 	//Parametr konfiguracyjny  VERBOSE
extern int 		   hor;//=790;      //Parametr konfiguracyjny  HORIZONTAL
extern int 		   ver;//=570;      //Parametr konfiguracyjny  VERTICAL
extern double   JakieRwProcWidth;//=5;   //Parametr konfiguracyjny RADIUS
extern unsigned DefMaxLiczbaWezlow;//=2  //Liczby od 0 do 10 oznaczaj� wielokrotno�� liczby pocz�tkowej...
extern unsigned DefMaxLiczbaLinkow;//=-1 //wi�ksze s� brane literalnie, -1 dla link�w oznacza graf pe�ny
extern unsigned DefMaxLiczbaKomunikatow; //=10*1024
extern unsigned DefLiczbaProcesowNaWezel;//=10*1024

#include "spsElement.h"  //Klasa bazowa dla wszystkich elementow modelu SPS

// Klasy bazowe dla bardziej konkretnych element�w modelu, ale nadal
// "pure virtual" - potrzebne tylko jako typy element�w tablic "�wiata"
// ////////////////////////////////////////////////////////////////////
class Komunikat; //Deklaracja zapowiadająca
class Proces;

class WezelSieci:public ElementModelu
{
  public:
	virtual ~WezelSieci(){}//Destruktor wirtualny, bo b�d� metody wirtualne
	virtual bool Poprawny()=0; //true je�li jest dobrze zdefiniowany. Jak nie to mo�e zosta� usuni�ty!
	virtual void Narysuj()=0;
	virtual void Wymazuj()=0;
	virtual void ChwilaDlaCiebie(){}; //Daje mu szanse na endogenne zmiany stan�w
//Specyficzne dla W�z�a
	virtual const char* Nazwa()=0;//Musi posiada� jakas nazwe
	virtual double ZOrder()=0; //Do ustalania kolejnosci wyswietlania
	virtual double X()=0; //X srodka w�z�a
	virtual double Y()=0; //Y �rodka w�z�a
	virtual double R(double angle)=0; //Promie� otaczaj�cego okr�gu lub elipsy
	virtual DziedzKol* Dziedzina()=0;
	virtual void InterpretujKomunikat(Komunikat* Co){}//Mo�e co� zrobi� z komunikatem, ale przechowywac tylko kopie
			//bo oryginal komunikatu jest zarz�dzany przez �wiat i mo�e znikn�� po powrocie z tej metody
};

const double PROCES_NAD_ZAAWANSOWANY=2.0;//Warto�c zaawansowania wymuszaj�ca skasowanie procesu przy najbli�szej okazji
class Proces:public ElementModelu
{
  public:
	Proces():_MojWezel(-1),_IleKrokowDoKonca(0){}//Konstruktor musi zapewni� domysln� NIEPOPRAWNOSC indeksu wez�a
	virtual ~Proces(){}//Destruktor wirtualny, bo b�d� metody wirtualne
	virtual bool Poprawny()=0; //true je�li jest dobrze zdefiniowany (wci�� istnieje link, nadawca i odbiorca etc...)
	virtual void Narysuj()=0;
	virtual void Wymazuj()=0;
	virtual void ChwilaDlaCiebie(){}; //Daje mu procesowi mo�liwo�� zmiany stan�w - zrobienia czego�
  //Specyficzne dla Procesu-projektu
	virtual bool InterpretujKomunikat(Komunikat* Co){return false;}//Mo�e co� zrobi� z komunikatem, ale przechowywac tylko kopie
			//bo oryginal komunikatu jest zarz�dzany przez �wiat i mo�e znikn�� po powrocie z tej metody
			//Zwraca bool

	virtual double   JakZaawansowany()=0;//0 po starcie, jak 1 to ju� zako�czony
	virtual double   Priorytet()=0; //Im wy�szy tym proces wi�cej dziala
	virtual unsigned Procesor(){return _MojWezel;} //W�ze� kt�ry realizuje proces
			float    DoKonca(){ return _IleKrokowDoKonca;} //Ile do deadlinu
  private:
	friend class Swiat;
	unsigned _MojWezel; //To pole MUSI by� dost�pne dla klasy �WIAT

  protected:
	float _IleKrokowDoKonca;//Ile krok�w czasu zosta�o do deadlinu. Swiat:: odejmuje co krok.

	void __ChceWezelNr(unsigned Ktory)//Tylko do ustawiania przy wczytaniu -
	{  _MojWezel=Ktory; } //jedynie procedura inicjujaca �wiat robi z tego u�ytek!!!
};

class Powiazanie:public ElementModelu
{
  public:
	virtual ~Powiazanie(){}//Destruktor wirtualny, bo b�d� metody wirtualne
	virtual bool Poprawny()=0; //true je�li jest dobrze zdefiniowany. Wci�� istniej� oba konce itp.
	virtual void Narysuj()=0;
	virtual void Wymazuj()=0;
	virtual void ChwilaDlaCiebie(){}; //Daje mu szanse na endogenne zmiany stan�w
  //Specyficzne dla Powi�zania (linku)
	virtual unsigned Poczatek()=0; //Co jest koncem co poczatkiem mo�e nie miec znaczenia
	virtual unsigned Koniec()=0;
	virtual bool  Kierunkowy()=0;
	virtual double ZOrder()=0; //Do ustalania kolejnosci wyswietlania
	virtual bool  Akceptacja(Komunikat* Co)=0;//Ustala czy dany komunikat mo�e przej�� tym ��czem
	virtual void  PodajPozycje(double D,bool KierunekZwykly,
							   double& X,double& Y,Komunikat* K=NULL)=0;//Przelicza polozenia wdluz linku
							   //Mo�e uwzgl�dnia� te� specyfik� komunikatu
};

//class string;//U�ywany tylko jako rezultat metod pure-virtual
class Komunikat:public ElementModelu
{
  public:
	virtual ~Komunikat(){}//Destruktor wirtualny, bo b�d� metody wirtualne
	virtual const std::string& Rodzaj()=0;  //Jak nic nie ma to zwraca "???"
	virtual bool  UstawRodzaj(const char* )=0;//A jak typ nie udost�pnia zmiany to zwraca false
	virtual bool Poprawny()=0; //true je�li jest dobrze zdefiniowany (wci�� istnieje link, nadawca i odbiorca etc...)
	virtual void Narysuj()=0;
	virtual void Wymazuj()=0;
	virtual void ChwilaDlaCiebie(){}; //Daje mu szanse na endogenne zmiany stan�w
  //Specyficzne dla Komunikatu (Message)
	virtual float  JakDostawa()=0;//0 po starcie, jak 1 to ju� dotar�=0
	virtual float  JakiTermin()=0;//W ile krok�w powinien zosta� dostarczony?
	virtual unsigned Kanal()=0; //Po ktorym po��czeniu czyli linku
	virtual bool 	 KierunekZgodny()=0;//Czy idzie zgodnie z nominalnym kierunkiem linku czy pod pr�d
	virtual unsigned Nadawca()=0; //Od kogo ...
	virtual unsigned Odbiorca()=0;//... do kogo.
	virtual Komunikat* Klonuj()=0; //Robi na stercie kopie komunikatu do przechowania lub wstawienia
	//Adresowanie w r�zny spos�bw
	virtual bool 	Zwrotnie(float _Szybkosc=0)=0;//Mo�na odwr�ci� komunikat - zw�aszcza jak jest to klon
	virtual bool  	Zaadresuj(unsigned _Nadawca,unsigned _Odbiorca,float _Szybkosc=0)=0; //Zwracaj� true jak odnajdzie
	virtual bool  	Zaadresuj(unsigned _Powiazanie,bool KierunekZwykly,float _Szybkosc=0)=0;//...poprawny kana� komunikacji
};

//Klasa interface'u Swiata - zarz�dzaj�ca danymi calej symulacji
///////////////////////////////////////////////////////////////////
//UWAGA - de facto SINGLETON!!! Dane s� tylko w jednej kopii - ka�dy �wiat ma do
//nich dost�p
//Chodzi o definicje interface'u, kt�rego implementacje mo�na �atwo zmieni�

class Swiat//:public ElementModelu - PO CO?
{
	//!!!
	//Wszystkie dane s� w jednej kopii poza deklaracj� klasy !!!
	//!!!
	Swiat();         //Konstruktor  prywatny - bo tylko jeden �wiat
	public:
	static double SkalujX(); //Bardzo cz�sto uzywane
	static double SkalujY(); //Mo�e inliny kiedy� zrobie?
	//Staramy sie miec tylko jeden Swiat :-)
	static Swiat Ten; //!!!
	static const unsigned INVINDEX=unsigned(-1); //Marker wadliwych indeks�w

	//Cz�� wirtualna ze wzgl�du na dziedziczenie - mo�e niepotrzebnie tak
	virtual ~Swiat();//Destruktor wirtualny
	//Wizualizacyjne - Co wyswietla� w ka�dym kroku i definicja procedur
	static  bool WyswietlajWezly;
	static  bool WyswietlajProcesy;
	static  bool WyswietlajLinki;
	static  bool WyswietlajInformacje;
	//Kt�re pole s�u�y do opisu link�w a kt�re w�z��w
	static  unsigned OpisWezlow;
	static  unsigned OpisLinkow;
	virtual void Narysuj(); //Odrysowanie ca�o�ci
	virtual void Wymazuj(); //Wyczyszczenie calo�ci - mo�e by� "na skr�ty"
	virtual void ChwilaDlaCiebie(){Krok_modelu();}; //Daje mu szanse na endogenne zmiany stan�w

	//Poniewa� dane s� w jednej to wszystkie inne "metody" mog� by� "static"
	///////////////////////////////////////////////////////////////////////////
	static bool ForceTolerant;//Przy wczytywaniu danych tylko ostrzega o niepoprawnych linkach
	static bool Inicjalizacja(const char* PlikWejsciowy,const char DelimiterDanych);   //Wczytanie i/lub wylosowanie struktury modelu
	static bool Krok_modelu();     //Wykonanie kroku modelu - mo�e by� wizualizacja w trakcie
	static bool Wizualizacja_po_kroku(); //Wykonanie pe�nej wizualizacji modelu w danej chwili
	static bool Statystyka_po_kroku(); //Wyliczenie pe�nej statystyki modelu w danej chwili
	static bool Status_po_kroku();  //Na zakonczenie kroku trzeba odnowi� graficzny wiersz statusu

	static double NumerKroku(); //Mo�e kroki b�d� te� u�amkowe? A w og�le double ma wi�cej cyfr znacz�cych
	static ElementModelu* Inspekcja(int x,int y,bool Wizual=false);//Odnajduje element wg. pozycji pikselowej
							//i opcjonalnie od razu wy�wietla w oknie wizualizacji pe�ne informacje o nim

	// Dost�py do tablic - mog� rzuci� wyj�tkiem albo przerwa� przy b��dzie dost�pu
	// albo w "�agodnej" formie mo�e zwr�ci� NULL, tak jak w przypadku pustego
	// czyli np. "usmierconego" slotu w tablicy
	static WezelSieci*  Wezel(unsigned Ktory);
	static Powiazanie*  Lacze(unsigned Ktory);
	static Komunikat*   Info(unsigned Ktory);
	static Proces*		Proc(unsigned Ktory,unsigned KtoryWezel=INVINDEX);//Zwi�zane z wez�ami
						//!!!Indeksy proces�w s� poprawne tylko tymczasowo

	//Pomocnicze funkcje wyszukiwawcze
	static unsigned 	ZnajdzIndeksWezla(const char* Nazwa); //Zwraca INVINDEX) jak nie znajdzie
	static unsigned     NajpilniejszyProc(unsigned KtoryWezel,unsigned* IleRealnie=NULL,bool Posortuj=false);//Daje indeks procesu o najwy�szym priorytecie
															//Mo�e zwr�cic -1 jak nie ma proces�w, a opcjonalnie ich liczb� na *IleRealnie
	//Aktualne rozmiary tablic - ale w ich obr�bie mog� by� puste sloty
	static unsigned IleWezlow();
	static unsigned IlePowiazan();
	static unsigned IleInformacji();
	static unsigned IleProcesow(unsigned KtoryWezel=INVINDEX);//jak INVINDEX to ogolnie, jak numer w�z�a to lokalnie

	//Liczba jeszcze dost�pnych slotow
	static 	unsigned IleMoznaWezlow();
	static 	unsigned IleMoznaPowiazan();
	static 	unsigned IleMoznaInformacji();
	static  unsigned IleMoznaProcesow(unsigned KtoryWezel=INVINDEX);//jak INVINDEX to ogolnie, jak numer w�z�a to lokalnie

	//UWAGA NA ZMIANY STRUKTURY!
	//Niekt�re dane do wizualizacji mog� by� liczone rzadziej ni� co krok/klatk� obrazu -
	//np. usuni�cie/dodanie wez�a, usuniecie/dodanie linku
	//ale czasem te zmiany s� w obr�bie obiektow podrz�dnych i wtedy trzeba poinformowa�
	//klas� "�wiata" �e wszystko trzeba policzy� od nowa.
	//Dotyczy to na pewno zmian warto�ci "ZOrder"
	static void UwagaZmienionoStrukture(); //Wstawianie i usuwanie wezlow jest ...

	//Funkcje wstawiaj�ce zwracaj� pozycje wstawienia i PRZEJMUJ� ZARZ�D PAMI�CI�
	// - wstawiane elementy musz� by� alokowane na stercie (przez "new")
	//Jak si� nie uda z jaki� powod�w wstawi� to wstawiany obiekt jest deletowany!
	//Nie jest wi�c bezpiecznie u�ywa� roboczego wska�nika po wstawieniu
	//Pole "propozycja" pozwala zaoszcz�szcz�dzi� szukania miejsca je�li trafia
	static unsigned WstawWezel(WezelSieci* Jaki,unsigned propozycja=INVINDEX);
	static unsigned WstawLacze(Powiazanie* Jaki,unsigned propozycja=INVINDEX);
	static unsigned WstawInfo(Komunikat* Jaki,unsigned propozycja=INVINDEX);
	static unsigned WstawProc(Proces* Jaki,unsigned KtoryWezel);//Musi by� jasne kt�ry w�ze�

	//UWAGA NA USUWANIE!  GENERALNIE DOSY� DROGIE
	//* Usuwanie w�z�a usuwa te� wszystkie zwi�zane z nim ��cza, komunikaty i procesy
	//* Usuwanie ��cza usuwa zwi�zane z nim komunikaty

	//Tansze funkcje usuwaj�ce wg. indeksu (lub uchwytu, zale�nie od implementacji
	static void UsunWezel(unsigned Ktory);
	static void UsunLacze(unsigned Ktory);
	static void UsunInfo(unsigned Ktory);
	static void UsunProc(unsigned Ktory,unsigned KtoryWezel=INVINDEX);

	//Funkcja usuwaj�ce wg. wska�nika - kosztowne bo szukaj�
	static unsigned UsunWezel(WezelSieci* Jaki);
	static unsigned UsunLacze(Powiazanie* Jaki);
	static unsigned UsunInfo(Komunikat* Jaki);
	static unsigned UsunProc(Proces* Jaki,unsigned KtoryWezel=INVINDEX);

	static bool _UstalLiczbeInfo(unsigned IleMax);  //W celu zmiany w konfiguruj
	private:  //Zmiany rozmiar�w struktur potrzebne s� tylko wewn�trznie (na razie?)
	static bool _UstalLiczbeLaczy(unsigned IleMax);
	static bool _UstalLiczbeWezlow(unsigned IleMax);

	static bool _UstalPoczLiczbeProc(unsigned IleMaxDlaWezla);
};


//Inne Globalne funkcje - je�li zwracaj� false to znaczy �e si� co� nie uda�o
bool konfiguruj(const char* parametr,const char* wartosc=0); //Przetwarza pojedynczy "configpar" o postaci XXX=YYY lub XXX i YYY jako drugi
bool czytaj_parametry(int argc,const char* argv[]); //Przetwarza ca�� liste parametrow
bool podaj_nazwe_pliku(char* Bufor,unsigned size,const char* Monit="Nazwa pliku");

//RANDOMIZER MUSI BY� WYBRANY RAZ NA APLIKACJE!
#define USES_STDC_RAND
#include "random.h"

/* *******************************************************************/
/*			          SPS  version 2011                              */
/* *******************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                  */
/*            W O J C I E C H   B O R K O W S K I                    */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego        */
/*        WWW:  http://wwww.iss.uw.edu.pl/borkowski/                 */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/
#endif //__SPS_MODEL_MAIN_HEADER_H__ - KONIEC NAG��WKA

