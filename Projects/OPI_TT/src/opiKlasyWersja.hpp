// Transfer technologii 2011 - nag³ówk implementacji modelu     24-10-2011
////////////////////////////////////////////////////////////////////////////////
// Klasy do kolejnej startowej wersji modelu  (dla danych na poziomie wersji 1.61)
////////////////////////////////////////////////////////////////////////////////
#ifndef _opiKlasy_plik_naglowkowy_hpp_
#define _opiKlasy_plik_naglowkowy_hpp_

#include "spsModel.h"
#include "spsGenNode.h"
#include "spsGenProc.h"
#include "spsGenLink.h"
#include "spsParaLink.h"
#include "spsGenInfo.h"
#include "spsMatrixNode.h"
#include "INCLUDE/platform.hpp"
#include "INCLUDE/wb_ptr.hpp"
#include "INCLUDE/wb_bits.h"
using namespace wbrtm;

#include <stdlib.h>
#include <cassert>
#include <string>
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
using namespace std;

extern float  TOWARZYSKA_NAJMNIEJSZA_WAGA;//=0.01;// dla	bool KontaktTowarzyski::Poprawny();
extern float  JEDNOSTKOWA_WYPLATA;//=100;//Wartoœæ najmniejszej wp³aty/wyplaty. Sposób u¿ycia niejasny :-)

extern float  WAGA_NA_DZIENDOBRY;//=0.1;//Wspó³czynnik zmniejszania wagi przy pierwszym kontakcie - nie mo¿e byæ za du¿a
extern float  TEMPO_SPADKU_LINKU;// np. =0.01;//O jak¹ czêœæ link socjalny zanika przy nie u¿ywaniu
extern float  TEMPO_WZROSTU_LINKU;// np. =0.1;//O jak¹ czêœæ link socjalny roœnie przy odpowiedzi
extern const char* const KONTO;//="Ruch konta"; //Marker komunikatu finansowego z przep³ywem
extern const char* const DZIENDOBRY;//="Dzien dobry";//Marker otwieraj¹cego komunikatu spo³ecznego

//Klasa wewnêtrzna skupiaj¹ca wspólne w³aœciwoœci WEZLOW z pracownikami (zespo³ów)
//////////////////////////////////////////////////////////////////////////////////
class _ZespolRoboczy:public GenerycznyWezelSieci
{
 protected:
 friend class  ProcesProdukcyjny;
 friend class  ProcesRaportowany;
 friend class  ProcesBadawczy;
 friend class  ProcesSpoleczny;
 friend class  ProcesPoszukiwanTT;
 friend class  ProcesTransferuTech;
//Ile Etatów	Efektywnosc	Aktywnoœæ	Centralnosc	Probiznesowosc	Prolegalnosc	Czasochlonnosc	Innowacyjnoœc	Finans. swobodne	Finanse zaplan	D³ug do nadrz.	Udzial w zysku TT	Eksperckoœæ w dziedzinie	Uwagi
 unsigned IleEtatow;//Ile etatów. Co najmniej 1, 0 - "duch" zespo³u - plan utworzenia
 float    Efektywnosc;//0..2. Czy dzia³a na 50% wydajnoœci czy na 150%. Zale¿y od motywacji
 float    Doswiadczenie;//POZIOM EKSPERTYZY? Kolejny mno¿nik do liczby etatów
 float    Centralnosc;//0..1. Jak bardzo wydajnoœæ i ewentualnie aktywnoœæ zale¿y od szefa/szefów

 float    Proaktywnosc;//Aktywnoœæ? 0..1. Podtrzymywanie aktywnoœci zewnêtrznej, tworzenie linków  itp.
 float    Czasochlonnosc;//Jaki jest wspó³czynnik czasoch³onnoœci typowej dzia³alnoœci. Wygenerowania pojedynczego wyniku badañ?
 float    Prolegalnosc;//????Uczciwoœæ??? Jak ³atwo podj¹æ dzia³ania "szarostrefowe"
 //Liczba punktów
 unsigned Sukcesy;
 //Bezparametrowy konstruktor z wartoœciami domyslnymi
 _ZespolRoboczy(){IleEtatow=1;Efektywnosc=0.5;Doswiadczenie=0.1;Centralnosc=1;Proaktywnosc=0.5;Czasochlonnosc=31;Prolegalnosc=0.75;Sukcesy=0;}
 //Procedury podzialu czasu pracy na poszczególne procesy - do wywo³ywania w procedurach ChwilaDlaCiebie()
 void     _ZarzadcaProcesowLosowy(); //Wybiera proces do wykonania losowo anga¿uj¹c siê te¿ losowo
 void 	  _ZarzadcaProcesowLeniwy(); //Robi tylko najpilniejszy proces albo wcale
 void 	  _ZarzadcaProcesowFantazyjny();//Robi pilny i jak mu coœ zostaje si³ to jeszcze jakiœ
 void 	  _ZarzadcaProcesowFinezyjny();//Sprytnie dzieli czas ¿eby wszystko szlo do przodu
 void 	  _ZarzadcaProcesowSprawiedliwy();//Dzieli wszystkie si³y proporcjonalnie do priorytetów procesów co nie jest dobrym pomys³em
 public:
 virtual double R(double);//Promieñ otaczaj¹cego okrêgu lub elipsy - ze wzglêdu na du¿e wagi musi byc LOGARYTMICZNIE zalezny od wagi
 virtual bool Poprawny();//Sprawdza czy wszystkie wskaŸniki s¹ poprawnie wpisane (nie przypadkowe i nie 0)
 //Specjalne dla tego typu
 void ZmienWage(double mnoznik);//Zmienia bezpiecznie wagê powiazania. "mnoznik" mo¿e byæ wiêkszy lub mniejszy ni¿ 1
 //¯eby mogl rozliczac siê finansowo z procesami.
 virtual void PrzyjmijKase(float Suma)=0;//Suma mo¿e byæ ujemna. Mo¿e t¹ kasê "przepuœcic" ale proces jest OK
 virtual bool StacGoNaWydatek(float Suma)=0;//Sprawdza czy taki wydatek jest dopuszczalny
 //Do uzycia w ró¿nych sytuacjach - indeksy typowych aktorów
 static  unsigned DajRynek();//RYNEK albo MARKET
 static  unsigned DajUrzadPatentowy();//UPATENTOWY | URZ¥D PATENTOWY | patent office  | Patent Office
 static  unsigned DajPublikator();//PUBLIKATOR  //unsigned _ZespolRoboczy::DajPublikator();//PUBLIKATOR
 //Inne pomocnicze
 static float _PodobienstwoDziedzin(DziedzinaWKolorze A,DziedzinaWKolorze B);
 static float _KalkulujKosztProduktu(DziedzinaWKolorze D); //Furtka do "realistycznego" kalkulowania ceny produktu
	//Mo¿na uzale¿niæ od zawartoœci bitowej Dziedziny "D"!
	// np. KOSZT=D.IleBitow()*4+(D.R*0.5+D.G+D.B*2)/3.5;
	//Kosztuje liczba bitow i niektore bity oraz niektore dziedziny s¹ dro¿sze ni¿ inne
	//Model iloœciowy powinien siê temu przyjrzeæ.
	//Wa¿ny parametr to te¿:
	//ILE_RAZY_BADANIA_DROZSZE_NIZ_TOWAR
 };

//Klasa dla badacza, zespo³u badawczego
////////////////////////////////////////////////////////////////////////////////
class JednostkaBadawcza:public _ZespolRoboczy
{
	friend class ProcesBadawczy; //Musi mieæ dostêp do w³aœciwoœci, w celach kalkulacyjnych
	friend class ProcesTransferuTech;
	//Implementacja tego co musi byæ wg. interfejsu typu podstawowego wêz³a
	static KonstruktorElementowModelu<JednostkaBadawcza> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
  public:
	void ChwilaDlaCiebie(); //Endogenne zmiany stanów, zarz¹dzanie procesami itp.
	void InterpretujKomunikat(Komunikat* Co);//Przyjmowanie komunikatów
	JednostkaBadawcza(){}//Konstruktor domyœlny

	bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);
	void AktualizujListeDanych();//Do zapisu i wyœwietlania inspekcyjnego
	//Ruchy na koncie
	void PrzyjmijKase(float Suma) {FinanseSwobodne+=Suma;} //¯eby mogl rozliczac siê finansowo z procesami
	bool StacGoNaWydatek(float Suma){ return Suma<FinanseSwobodne;}//Sprawdza czy taki wydatek jest dopuszczalny
  protected://W³aœciwoœci obiektów klasy  i metody pomocnicze
 //Cechy:
 //////////////////////////////////
 // z klasy "ZespolRoboczy":
 //Ile Etatów	Efektywnosc	Aktywnoœæ	Centralnosc	Probiznesowosc	Prolegalnosc	Czasochlonnosc	Innowacyjnoœc	Finans. swobodne	Finanse zaplan	D³ug do nadrz.	Udzial w zysku TT	Eksperckoœæ w dziedzinie	Uwagi
 // inne - w³asne:
//Jedna skala czy dwie Pronaukowosc vs. Probiznesowsc
 float    Probiznesowosc;//0 - czysta nauka, 0.5-badania patentowalne/licenjonowalne, 1 - czysty biznes - szybkie wdra¿anie
 float    Innowacyjnosc;//0..1. £atwoœæ startowania projektów bez inspiracji z zewn¹trz, zaciekawienie projektami nieco odleg³ymi od dotychczasowych

 float    FinanseSwobodne;//Rezerwy finansowe - w skali roku LUB UPROSZCZONA "ZAMO¯NOŒÆ"
 float    FinanseObiecane;//Zaplanowane do wydania, ale byæ mo¿e jeszcze nie na koncie (odnawiane/odbierane co 365 kroków)
 float 	  Dlugi;//Ile zespó³ "wisi" swojemu wydzia³owi/instytutowi. Mo¿liwe tylko chwilowo (30 kroków?)
 float    UdzialWZysku;//0..1 Jaki procent zysku z innowacji pobiera (reszta idzie dla pracowników)

 //JAK IMPLEMENTOWAÆ MOTYWACJE?????????  --> Zmiany efektywnoœci, ale jaka metoda?
 //Projekty z listy aktywnych projektów
 //Kontakty z siatki
 //Co z kontaktami miêdzynarodowymi?
 //po³¹czenie z obiektem gridowym a la rynek?
};

//Klasa dla Instytutu, Wydzia³u, Uniwersytetu itp.
////////////////////////////////////////////////////////////////////////////////
class AdministracjaNaukowa:public _ZespolRoboczy
{
	friend class ProcesTransferuTech;
	//Implementacja tego co musi byæ wg. interfejsu typu podstawowego wêz³a
	static KonstruktorElementowModelu<AdministracjaNaukowa> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
  public:
	void ChwilaDlaCiebie(); //Endogenne zmiany stanów, zarz¹dzanie procesami itp.
	void InterpretujKomunikat(Komunikat* Co);//Przyjmowanie komunikatów
	AdministracjaNaukowa(){}//Konstruktor domyslny
	bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);
	void AktualizujListeDanych();//Do zapisu i wyœwietlania inspekcyjnego
	//Ruchy na koncie
	void PrzyjmijKase(float Suma) {FinanseSwobodne+=Suma;} //¯eby mogl rozliczac siê finansowo z procesami
	bool StacGoNaWydatek(float Suma){ return Suma<FinanseSwobodne;}//Sprawdza czy taki wydatek jest dopuszczalny
  protected://Wlasciwoœci obiektów klasy  i metody pomocnicze
 ///////////////////////////////////////////////////////////////////////////////
 // z KLASY: ZespolRoboczy
 // unsigned  IleEtatow;//Ile etatów. Co najmniej 1, 0 - "duch" zespo³u - plan utworzenia
 //float     Efektywnosc;//0..2. Czy dzia³a na 50% wydajnoœci czy na 150%. Zale¿y od motywacji
 //float     Proaktywnosc;//(Pro)aktywnoœæ. Prawdopodobieñstwo organizowania konferencji i innych eventów umo¿liwiaj¹cych kontakty miêdzy naukowcami i goœæmi (czyli co ile kroków nastêpuje taki event)
 //float     Centralnosc;//0..1. Jak bardzo aktywnoœæ (zewnêtrzna?) zale¿y od szefa/szefów
 //float     Doswiadczenie;//POZIOM EKSPERTYZY? Jak tego u¿ywaæ? Mo¿e to samo co efektywnoœæ???
 //float     Czasochlonnosc;// Ile czasu (kroków modelu) siê czeka z podjêciem decyzji
 //float     Prolegalnosc;//????Elastycznoœæ??? Jak ³atwo podj¹æ dzia³ania obchodz¹ce "bzdurne" przepisy

 float     Probiznesowosc;//0...1 Prawdopodobieñstwo podjêcia inicjatyw z wspó³udzia³em biznesu
 float     ProInwest;//0-1 Sk³onnoœæ inwestowania wolnych œrodków w rozwój zwi¹zany z TT

 float     FinanseSwobodne;//Rezerwy finansowe - w skali roku (odnawiane/odbierane co 365 kroków) LUB UPROSZCZONA "ZAMO¯NOŒÆ"
 float     FinanseObiecane;//Zaplanowane do wydania, ale byæ mo¿e jeszcze nie na koncie
 float 	   Dlugi;//Ile jednostka ju¿ "wisi". Np. na budowê, albo na pr¹d jak dotacja zaleg³a. Nie mo¿e wiêcej ni¿ urzêdowy limit
 float     UdzialWZysku;//0..1 Jaki procent zysku z innowacji pobiera (reszta idzie "w dó³" hierarchii)

 //float     UdzialPromo; //0-1 Jakie œrodki s¹ gotowi przeznaczyæ na marketing  (SK¥D JE BIOR¥?)
 //float     DecyzjaPatentowa;//Ile czasu  jest podejmowana taka decyzja: wdra¿amy vs. nie interesuje nas to
};

//Klasa wykonwacy/producenta ostatecznego produktu
////////////////////////////////////////////////////////////////////////////////
class Firma:public _ZespolRoboczy
{
	friend class ProcesTransferuTech;
	//Implementacja tego co musi byæ wg. interfejsu typu podstawowego wêz³a
	static KonstruktorElementowModelu<Firma> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
  public:
	void ChwilaDlaCiebie(); //Endogenne zmiany stanów, zarz¹dzanie procesami itp.
	void InterpretujKomunikat(Komunikat* Co);//Przyjmowanie komunikatów
	Firma(){}//Konstruktor domyslny
	bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);
	void AktualizujListeDanych();//Do zapisu i wyœwietlania inspekcyjnego
	//Ruchy na koncie
	void PrzyjmijKase(float Suma) {FinanseBiezace+=Suma;} //¯eby mogl rozliczac siê finansowo z procesami
	bool StacGoNaWydatek(float Suma){ return Suma<FinanseBiezace;}//Sprawdza czy taki wydatek jest dopuszczalny
  protected://Wlasciwoœci obiektów klasy i metody pomocnicze
 //////////////////////////////////////////////////////////////////////////////////////
  void _RozeslijNaleznosci(DziedzinaWKolorze Produkt,float& wplyw); //Wg. identyfikatora produktu szuka komu winien dywidende
 //Z KLASY: ZespolRoboczy
 // unsigned IleEtatow;//Ile etatów. Co najmniej 1, 0 - "duch" zespo³u - plan utworzenia
 //float    Efektywnosc;//0..2. Czy dzia³a na 50% wydajnoœci czy na 150%. Zale¿y od motywacji
 //float    Proaktywnosc; //Aktywnoœæ. 0-1 aktywnoœæ w tworzenia nowych znajomoœci biznesowych
 //float    Centralnosc;//0..1. Jak bardzo aktywnoœæ (zewnêtrzna?) zale¿y od szefa/szefów
 //float    Doswiadczenie;//POZIOM EKSPERTYZY? Jak tego u¿ywaæ? Mo¿e to samo co efektywnoœæ???
 //float    Czasochlonnosc;//Jaki jest wspó³czynnik czasoch³onnoœci typowej dzia³alnoœci (np. przygotowanie projektu oraz produktu)
 //float    Prolegalnosc;//?"Pragmatycznoœæ"? Jak ³atwo podj¹æ dzia³ania "szarostrefowe" i omijac "nie¿yciowe" przepisy

 float    ProAkademickosc; //0-1 chêæ nawi¹zywania kontaktów z badaczami i uniwersytetami
 float    ProInnowac;//Stosunek do nowoœci: 0-1 - 0..33% niewiele robi, 33-66% raczej blokuje innych,
					//powy¿ej - próbuje, wchodz¹c na coraz wczeœniejszym etapie (prawdopodobienstwo)

 float    FinanseBiezace; //Ile firma ma kasy aktualnie LUB UPROSZCZONA "ZAMO¯NOŒÆ"
 float    FinanseZaplanowane;//Ile firma ma obiecanych œrodków kredytowych
 float 	  Dlugi;//Ile firma ma do sp³aty. Im wiêcej tym mniejsza szansa ¿e coœ dostanie nowego
 float 	  LimitUdzialuInwestTT; //0-1 Jakie œrodki finansowe z zarobionych gotów przeznaczyæ na inwestycje TT

 //float 	  UdzialPromo; //0-1 Jakie œrodki sa gotowi przeznaczyæ na marketing
 //float Budzet
 //float Wiarygodnosc;//Cecha po³¹czenia? //0-1 na ile dana firma jest atrakcyjna
			//dla uniwersytetu oraz banków w kontekœcie podejmowania z ni¹ wspó³pracy
 //???float Wspolpraca_Konkurenci; //chêæ podejmowania wspó³pracy z innymi firmami 0-1
 //(te dwie zmienne powinny byæ od siebie niezale¿ne, relacja o sumie niezerowej
 //...
};

//Klasa organizatora wdrozenia innowacji (BOTT i UOTT)
////////////////////////////////////////////////////////////////////////////////
/*
class OrganizatorWdrozenia:public GenerycznyWezelSieci
{
  //DYLEMAT - CZY TO JEDEN TYP Z "SUWAKAMI" POZWALAJ¥CYMI ZMIENIÆ UOTT w niemal BOTT
  //czy dwa zupe³nie ró¿ne typy?
  //Ostatecznie przyjêto ¿e:
  //Czy BOTT=firma+OTT a UOTT=AdmNaukowa+OTT
};
*/

class BOTT:public Firma
{  //Implementacja tego co musi byæ wg. interfejsu typu podstawowego wêz³a
	static KonstruktorElementowModelu<BOTT> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
  public:
	void ChwilaDlaCiebie(); //Endogenne zmiany stanów, zarz¹dzanie procesami itp.
	void InterpretujKomunikat(Komunikat* Co);//Przyjmowanie komunikatów
	BOTT(){}//Konstruktor domyslny
	bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);
  protected://Wlasciwoœci obiektów klasy i metody pomocnicze
  //////////////////////////////////////////////////////////////////////////////
  //Ile Etatów	Efektywnosc	Aktywnoœæ	Centralnosc	Pronaukowosc	Prolegalnosc	Czasochlonnosc	Proinnowacyjnosc	Finans. bie¿¹ce	Finanse zaplan	D³ugi	Limit ryzyk. TT	EksperckoscTT	Uwagi
  //float 	  LimitUdzialuInwestTT; //0-1 Jakie œrodki finansowe na RYZYKOWNE inwestycje TT
  // float EksperckoscTT;=DOSWIADCZENIE//Dodatkowy wspó³czynnik 0..2 skutecznoœci TT wynikaj¹cy z doœwiadczenia
  // float AktywnoscInform;//Aktywne poszukiwanie informacji "bazodanowej" (rynek, bazy, patenty etc)
   // ... po prostu 0.5 normalnej aktywnoœci???
 //Kontakty z siatki
 //Projekty z listy aktywnych projektów
 //Nowe znajomoœci przez polecenie, konferencje, poszukiwanie/nawi¹zywanie kontaktów
 //Ocena projektu?  bezpoœrednia i poœrednia
};

class UOTT:public AdministracjaNaukowa
{  //Implementacja tego co musi byæ wg. interfejsu typu podstawowego wêz³a
	static KonstruktorElementowModelu<UOTT> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
  public:
	void ChwilaDlaCiebie(); //Endogenne zmiany stanów, zarz¹dzanie procesami itp.
	void InterpretujKomunikat(Komunikat* Co);//Przyjmowanie komunikatów
	UOTT(){}//Konstruktor domyslny
	bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);
  protected://Wlasciwoœci obiektów klasy i metody pomocnicze
  //////////////////////////////////////////////////////////////////////////////
  //Ile Etatów	Efektywnosc	Aktywnoœæ	Centralnosc	Probiznesowosc	Prolegalnosc	Czasochlonnosc	ProTT	Finans. swobodne	Finanse zaplan	D³ug do nadrz.	Udzial w zysku TT	EksperckoscTT	Uwagi
   //float    UdzialWZysku;//0..1 Jaki procent zysku z innowacji pobiera i przekszta³ca w motywacjê pracowników
   //float EksperckoscTT;==Doswiadczenie//0..2 wspó³czynnik skutecznoœci TT wynikaj¹cy z doœwiadczenia
   //float AktywnoscInform;//Aktywne poszukiwanie informacji "bazodanowej" (rynek, bazy, patenty etc)
   // ... po prostu 0.5 normalnej aktywnoœci???
 //Kontakty z siatki
 //Projekty z listy aktywnych projektów
 //Ocena projektu??? - poœrednia przez wewn¹trz uczelnianych ekspertów lub zewnêtrzne firmy
 //Trwa to d³u¿ej
};


//Klasa powi¹zania administracyjnego - przepuszcza w górê raporty,
// a w dó³ wytyczne oraz pieni¹dze
////////////////////////////////////////////////////////////////////////////////
class PodlegloscOrganizacyjna:public GenerycznePowiazanie
{  //Implementacja tego co musi byæ wg. interfejsu typu podstawowego linku
	static KonstruktorElementowModelu<PodlegloscOrganizacyjna> WirtKonstr;
	//Dla usprawnienia klasa potomna mo¿e zwracaæ adres swojego wirtualnego konstruktora
	//Choæ mo¿na te¿ nie definiowaæ  i polegac na gorszej funkcji z klasy bazowej
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
  public:
	//void ChwilaDlaCiebie(); //Endogenne zmiany stanów linku - tu brak ...
	bool  Kierunkowy() { return true;}  //Wizualnie zachowuje siê jak kierunkowy
	bool  Akceptacja(Komunikat* Co);//Ale selekcja komunikatów jest z³o¿ona - mog¹ iœæ pod pr¹d
	PodlegloscOrganizacyjna(){}//Konstruktor domyslny

	bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);
  protected://W³aœciwoœci obiektów klasy  i metody pomocnicze
  //... ???
};


// Klasa kooperacji sformalizowanej - przepuszcza w raporty - efekty dzia³añ
// oraz pieni¹dze
////////////////////////////////////////////////////////////////////////////////
class FormalnaKooperacja:public PowiazaniePaboliczne
{  //Implementacja tego co musi byæ wg. interfejsu typu podstawowego linku
	friend class KonstruktorElementowModelu<FormalnaKooperacja>;
	static KonstruktorElementowModelu<FormalnaKooperacja> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
  public:
	FormalnaKooperacja(unsigned Inicjator,unsigned Kooperator,float Waga/*=Udzial=1*/,unsigned Termin=(10*365));//Konstruktor domyœlny
	void ChwilaDlaCiebie(); //Jak "Wykonany" to z czasem siê coraz bardziej wygina, a jak termi spadnie do 0 to znika
	bool  Akceptacja(Komunikat* Co);//Selekcja komunikatów
	bool  Poprawny();//Jak po terminie to znika
	bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);
	void Narysuj();//Przechwycenie rysowania dla debugingu
	//Akcesory
	unsigned DajTermin(){ return Termin;}//Dostep do terminu
	void     UstawTermin(unsigned NowyTermin){Termin = NowyTermin;}
	bool     CzyWykonany(){ return Wykonany;}
	void     UstawWykonano(bool TakNie=true){Wykonany=TakNie;  }
	float 	 JakiUdzial(){ if(Waga<1) return Waga; else return 1;} //Waga linku kooperacyjnego jest mierzona udzialem w zyskach!!!
	//W³aœciwoœci obiektów klasy  i metody pomocnicze
  protected:
   FormalnaKooperacja(){Wykonany=0;Termin=(10*365);}//Konstruktor domyœlny
   bool   Wykonany;//Czy ju¿ zosta³ zrealizowany i tylko p³yn¹ dywidendy
   int    Termin;//Maleje z ka¿dym krokiem i jak 0 to link znika - koniec umowy
};


//Klasa powi¹zania towarzyskiego, zanikaj¹cego powoli gdy nie u¿ywany
// ASYMETRYCZNA - TAKIE LINKI S¥ POD DWA BO ZAUFANIE I WAGA POWI¥ZANIA
// ZAZWYCZAJ JEST RÓ¯NA Z PUNKTU WIDZENIA OBU PARTNERÓW!
//Komentarz na temat negatywnoœci (do rozwa¿enia) - hamuj¹ca rola wêz³a w przekazie
//informacji. Jak nie lubiê bo zazdroszczê, to mu nie powiem,
//chocia¿ jesteœmy powi¹zani.
////////////////////////////////////////////////////////////////////////////////
class KontaktTowarzyski:public PowiazaniePaboliczneSkierowane
{  //Implementacja tego co musi byæ wg. interfejsu typu podstawowego linku
	static KonstruktorElementowModelu<KontaktTowarzyski> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
  public:
	void ChwilaDlaCiebie(); //Endogenne zmiany stanów linku - zanikanie...
	bool Poprawny(); //true jeœli jest dobrze zdefiniowany. Wci¹¿ istniej¹ oba konce itp.
					// i WAGA jest wiêksza ni¿ TOWARZYSKA_NAJMNIEJSZA_WAGA
	bool  Akceptacja(Komunikat* Co);//Selekcja komunikatów - podstawowa i wzrost wagi
	KontaktTowarzyski(){}//Konstruktor domyslny
	KontaktTowarzyski(unsigned Kogo,unsigned ZKim,double JakaWaga,double JakieWygiecie);
	bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);
	//Specjalne dla tego typu
	void ZmienWage(double mnoznik);//Zmienia bezpiecznie wagê powiazania. "mnoznik" mo¿e byæ wiêkszy lub mniejszy ni¿ 1
  protected://Wlasciwoœci obiektów klasy  i metody pomocnicze
	//float Zaufanie == Waga linku . Dlatego skierowany bo zaufanie mo¿e byæ asymetryczne
	//float TempoZanikania; //Czasem szybkie, czasem wolne, ró¿ne dla ró¿nych osób
	//Parametr wygiêcia linku jest te¿ skorelowane z tempem "parowania" nieu¿ywanego
	//linku socjalnego - im bardziej "poboczny" czyli wygiêty link tym szybciej zanika
};


//Klasa komunikatu oficjalnego - raporty (w górê), wytyczne w dó³ (podleglosci  adm.)
////////////////////////////////////////////////////////////////////////////////
class KomunikacjaTowarzyska:public GeneryczneInfo
{  //Implementacja tego co musi byæ wg. interfejsu typu podstawowego linku
	friend class KonstruktorElementowModelu<KomunikacjaTowarzyska>;
	static KonstruktorElementowModelu<KomunikacjaTowarzyska> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
 public:
	Komunikat* Klonuj(); //Robi na stercie kopie komunikatu do przechowania lub wstawienia
	virtual bool  UstawRodzaj(const char* );
	const string& Rodzaj();//Gdzieœ w danych jest rodzaj tego komunikatu

	//Konstruktor wymaga kategori i opcjonalnie numeru wezla o którym mowa
	KomunikacjaTowarzyska(const char* Rodzaj,unsigned OKogoChodzi=-1);
	unsigned&     OKimTaGadka() {return OKim;}  //Na temat kogo jest ten komunikat - mo¿e o nadawcy, a mo¿e nie!

	bool Zwrotnie(float Szybkosc=0);//Zwrotne adresowanie sluzy do pogrubiania linku na którego kom. odpowiadamy
	bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad); //Jak generyczny

  protected://Wlasciwoœci obiektów klasy  i metody pomocnicze
	void RysujKsztalt(float X,float Y,float Rad,unsigned R,unsigned G,unsigned B); //Rysowanie kszta³tu zale¿nego od typu potomnego,
	unsigned OKim;
	KomunikacjaTowarzyska(){OKim=-1;}//Konstruktor domyœlny - nie wiadomo o kim mowa
	KomunikacjaTowarzyska(const KomunikacjaTowarzyska* Wzor):GeneryczneInfo(Wzor)
		{OKim=Wzor->OKim;}//KONSTRUKTOR KOPIUJ¥CY przenosi te¿ numer "obgadywanego" wêz³¹

};

//Klasa komunikatu oficjalnego - raporty (w górê), wytyczne w dó³ (podleglosci  adm.)
////////////////////////////////////////////////////////////////////////////////
class KomunikacjaOficjalna:public GeneryczneInfo
{  //Implementacja tego co musi byæ wg. interfejsu typu podstawowego linku
	friend class KonstruktorElementowModelu<KomunikacjaOficjalna>;
	static KonstruktorElementowModelu<KomunikacjaOficjalna> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
 public:
	Komunikat* Klonuj(); //Robi na stercie kopie komunikatu do przechowania lub wstawienia
	virtual bool  UstawRodzaj(const char* );
	const string& Rodzaj();//Gdzieœ w danych jest rodzaj tego komunikatu

	//Konstruktor z ustalonym rodzajem komunikatu
	KomunikacjaOficjalna(const char* Rodzaj,unsigned Autor);
	unsigned	KtoJestAutorem(){ return Autor;}

	bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);

  protected://Wlasciwoœci obiektów klasy  i metody pomocnicze
	void RysujKsztalt(float X,float Y,float Rad,unsigned R,unsigned G,unsigned B); //Rysowanie kszta³tu zale¿nego od typu potomnego,
	KomunikacjaOficjalna(){Autor=-1;}//Konstruktor domyœlny ukryty, ¿eby byle kto go nie u¿ywa³ :-)
	KomunikacjaOficjalna(const KomunikacjaOficjalna* Wzor):GeneryczneInfo(Wzor) //KONSTRUKTOR KOPIUJ¥CY
						{Autor=Wzor->Autor;}
   //POLE:
   unsigned Autor; //Kto jest autorem
  //JAKIE RODZAJE:
  //wytyczne w dó³, raporty w górê,
  //proœba o konsultacje prawn¹, o zgodê na kontakt/wspolprace
  //odpowiedzi na to...
  //...CO JESZCZE???
};

//Klasa komunikatu-pakietu do przesy³ania realnych produktów z badañ lub fabryk
// potrzebna g³ówne ze wzglêdu na odróznienie wizualizacyjne
////////////////////////////////////////////////////////////////////////////////
class PaczkaProduktow:public GeneryczneInfo
{
	//Implementacja tego co musi byæ wg. interfejsu typu podstawowego linku
	friend class KonstruktorElementowModelu<PaczkaProduktow>;
	static KonstruktorElementowModelu<PaczkaProduktow> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
  public:
	virtual bool  UstawRodzaj(const char* );
	const string& Rodzaj();//Gdzieœ w danych jest rodzaj tego komunikatu
	Komunikat* Klonuj(); //Robi na stercie kopie komunikatu do przechowania lub wstawienia

	PaczkaProduktow(const char* Nazwa,unsigned IleWPaczce,float CenaZaSztuke,unsigned Producent);
	unsigned IleSztuk() {return Ile;}
	float    CenaZaSzt() {return Cena;}

	bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);

  protected://Wlasciwoœci obiektów klasy  i metody pomocnicze
	//Wlasciwoœci obiektów klasy  i metody pomocnicze
	void RysujKsztalt(float X,float Y,float Rad,unsigned R,unsigned G,unsigned B); //Rysowanie kszta³tu zale¿nego od typu potomnego,

	PaczkaProduktow(){Ile=1;Cena=1;Producent=-1;}//Konstruktor domyœlny ukryty
	PaczkaProduktow(const PaczkaProduktow* Wzor);//KONSTRUKTOR KOPIUJ¥CY
  //POLE - LICZBA W PACZCE, CENA, ADRES PRODUCENTA
	unsigned Producent;
	unsigned Ile;
	float    Cena;
  //JAKIE RODZAJE:
  //* Dziedzina to produkt ktorego dotyczy.
  	friend class ProcesProdukcyjny; //Chyba musi miec dostêp do wnêtrza paczki
};

//Klasa komunikatu finansowego - obietnice finansowe i kredytowe, umowy, proformy, przelewy
////////////////////////////////////////////////////////////////////////////////
class KomunikacjaFinansowa:public GeneryczneInfo
{  //Implementacja tego co musi byæ wg. interfejsu typu podstawowego linku
	friend class KonstruktorElementowModelu<KomunikacjaFinansowa>;
	static KonstruktorElementowModelu<KomunikacjaFinansowa> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
 public:
	Komunikat* Klonuj(); //Robi na stercie kopie komunikatu do przechowania lub wstawienia
	virtual bool  UstawRodzaj(const char* );
	const string& Rodzaj();//Gdzieœ w danych jest rodzaj tego komunikatu

	KomunikacjaFinansowa(const char* Kategoria,float JakaKwota);//Konstruktor domyslny
	float JakaKwota() { return Kwota;}

	bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);
  protected://Wlasciwoœci obiektów klasy  i metody pomocnicze
	void RysujKsztalt(float X,float Y,float Rad,unsigned R,unsigned G,unsigned B); //Rysowanie kszta³tu zale¿nego od typu potomnego,
	KomunikacjaFinansowa(){Kwota=0;}//Sprywatyzowany konstruktor domyslny
	KomunikacjaFinansowa(const KomunikacjaFinansowa* Wzor):GeneryczneInfo(Wzor)
		{}//KONSTRUKTOR KOPIUJ¥CY
	//JAKIE RODZAJE:
	float Kwota;
	//obietnice (propozale umów) finansowe i kredytowe,
	//umowy, proformy,
	//P³ace, przelewy
	//Raporty/wyniki/transfer wiedzy na koniec umowy o finansowanie badañ
	//Partie produktów na rynek
	//pieniadze za produkty z powrotem do producenta
};

//Klasa dla œwiata - realizuje odpowiedzi na wolne poszukiwania, zapytania o produkty
// a tak¿e wymianê gotowych produktów na konkretn¹ kasê (czyli "zbyt")
////////////////////////////////////////////////////////////////////////////////
class RynekZbytu:public WezelMacierzowy
{  //Implementacja tego co musi byæ wg. interfejsu typu podstawowego wêz³a
	static KonstruktorElementowModelu<RynekZbytu> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
  public:
	RynekZbytu(){}//Konstruktor domyœlny
	void ChwilaDlaCiebie(); //Endogenne zmiany stanów, zarz¹dzanie procesami itp.
	void InterpretujKomunikat(Komunikat* Co);//Przyjmowanie komunikatów
	bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);
  protected://Wlasciwoœci obiektów klasy  i metody pomocnicze
	DziedzinaWKolorze _DaSieSprzedac(const DziedzinaWKolorze CoZaProdukt); //Jak 0 to nie...
  //GRID NISZ RYNKOWYCH:
  //CO ROBI:
  //* realizuje odpowiedzi na swobodne poszukiwania inspiracji - losowo z filtrem
  //* zapytania o zbywalnoœæ/zapotrzebowanie na produkty z danym pomys³em
  //	* Czy jest nisza rynkowa? Jak dok³adne jest dopasowanie?
  //	* Czy jest pusta czy zajêta i jak dok³adnie zajêta?
  //* a tak¿e realizuje wymianê gotowych produktów
  // na konkretn¹ kasê (czyli "zbyt")
  // Kolor (DZIEDZINA) rynku okreœla preferencje
  //float SilaPreferencji;//0..1 - jak bardzo preferencja zmienia rozk³ad (?)
  //albo wczytany grid o okreœlonej zawartoœci
};


//Klasa dla PUBLIKATORA i UPATENTOWEGO - musi przyjmowac tylko komunikaty oficjalne
// a poza tym zachowuje siê podobnie do klasy bazowej
///////////////////////////////////////////////////////////////////////////////////
class SystemInformacyjny:public WezelMacierzowy
{   //Implementacja tego co musi byæ wg. interfejsu typu podstawowego wêz³a
	static KonstruktorElementowModelu<SystemInformacyjny> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
  public:
	SystemInformacyjny(){}//Konstruktor domyœlny
	//void ChwilaDlaCiebie(); //Endogenne zmiany stanów, zarz¹dzanie procesami itp. - jak klasa bazowa?
	void InterpretujKomunikat(Komunikat* Co);//Przyjmowanie komunikatów
	//bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);   - jak klasa bazowa?
};

// Proces kreuj¹cy nowe linki - losuje wêz³y i wg. podobieñstwa tworzy link i
// wysy³a nim zajawkê o sobie, nadaje wagê proporcjonaln¹ do wagi wêz³a (albo...)
// Jak nie utworzy linku to to wysy³a przypomnienie do jakiegoœ z istniej¹cych
// powi¹zañ spo³ecznych zawieraj¹ce losowe bity ze swojej dziedziny.
// Co jakiœ czas aktualizuje dziedzinê na podstawie dziedziny swojego wêz³a.
// Jego procedura przechwytywania komunikatów zajmuje siê odpowiadaniem na "zaczepki"
// o ile pochodz¹ od kogoœ znanego, lub wartego poznania. Kieruje siê tu wag¹ linku
// spo³ecznego.
////////////////////////////////////////////////////////////////////////////////
class ProcesSpoleczny:public GenerycznyProces
{
   //Implementacja tego co musi byæ wg. interfejsu typu Proces
	static KonstruktorElementowModelu<ProcesSpoleczny> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
	public:
		 ProcesSpoleczny(){}//Konstruktor domyslny wykonuje robotê za pomoc¹ domyœnego GenerycznyProces()

	void ChwilaDlaCiebie(); //Tworzenie nowych kontaktów i podtrzymywanie starych
							//Co jakiœ czas (7 dni?) zaczyna siê od pocz¹tku
	bool InterpretujKomunikat(Komunikat* Co);//Ewentualnie odpowiada na komunikat
	private:
	float Aktywnosc; //Pobierana z odpowiedniej wartoœci wêzla
	//Procedury pomocnicze skracajace zapis tego co istotne
	bool _WyslijAutoprezentacje(unsigned kanal,bool calaprawda); //Wysy³ania informacji o sobie
	unsigned _StworzKanal(unsigned IndInny,float Waga,float Parametr=0);//I nowy kanal z ju¿ obliczona wag¹
														//Jak Parametr =0 to jest losowanie ale z dodatnich!
	public:
	//Pomocnicza funkcja statyczna do u¿ycia gdzieb¹dŸ - znajduje zadane powi¹zanie spoleczne
	static  unsigned _JestPowiazanySocjalnie(unsigned Startowy,unsigned Docelowy);
};

// Proces obci¹¿aj¹cy wêze³ i na koñcu wysy³aj¹cy raport do odbiorcy lub wêz³a
// nadrzêdnego (linkiem Adm.) i wzawiaj¹cy dzia³anie od poczatku bez nowego procesu
/////////////////////////////////////////////////////////////////////////////////////
class ProcesRaportowany:public GenerycznyProces
{
   //Implementacja tego co musi byæ wg. interfejsu typu Proces
	static KonstruktorElementowModelu<ProcesRaportowany> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
	public:
		 ProcesRaportowany(){}//Konstruktor domyslny wykonuje robotê za pomoc¹ domyœnego GenerycznyProces()

	void ChwilaDlaCiebie(); //Endogenne zmiany stanów, popychanie pracy do przodu itp.
							//Na koniec wysy³a raport i zaczyna siê od pocz¹tku
	bool InterpretujKomunikat(Komunikat* Co);//Przesyla dalej raporty, wysy³a kase, inkasuje kasê
	private:
	bool _WyslijRaportOdbiorcy(); //Pomocnicza procedura wysy³ania raportu, gdy proces gotowy
};

// Proces zajmuj¹cy siê dawaniem grantów, doatacji lub po¿yczek na procent
/////////////////////////////////////////////////////////////////////////////////////
class ProcesGrantowoPozyczkowy:public GenerycznyProces
{
   //Implementacja tego co musi byæ wg. interfejsu typu Proces
	static KonstruktorElementowModelu<ProcesGrantowoPozyczkowy> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
	public:
		 ProcesGrantowoPozyczkowy();//Konstruktor domyslny wykonuje robotê za pomoc¹ domyœnego GenerycznyProces()

	void ChwilaDlaCiebie(); //Endogenne zmiany stanów, popychanie pracy do przodu itp.
	bool InterpretujKomunikat(Komunikat* Co);//Odbiera propozycje, a przydziela kasê
	private:
	//Wewnetrzne w³aœciwoœci. Mo¿e u¿ywaæ te¿ cech wêz³a, np. jego sk³onnoœci do ryzyka
	float Fundusz; //Findusze do podzia³u
	float OczekiwanyZwrot;//0 - dotacja, do 100% po¿yczki czêœcowo zwrotne, pow 100% pozyczki komercyjne
};

// Proces badawczy produkuje wyniki naukowe do celów publikacji, patentowania,
// lub na czyjeœ zamówienie, st¹d ma w³aœciwoœæ "ZaplanowanyOdbiorca".
// Mo¿e siê wznawiaæ orza generowaæ inne procesy badawcze!!!
////////////////////////////////////////////////////////////////////////////////
class ProcesBadawczy:public GenerycznyProces
{
	friend class JednostkaBadawcza; //¯eby móg³ sobie sam poustawiaæ pola jak go tworzy od nowa
   //Implementacja tego co musi byæ wg. interfejsu typu Proces
	friend class KonstruktorElementowModelu<ProcesBadawczy>;
	static KonstruktorElementowModelu<ProcesBadawczy> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
	public:
	//Konstruktor z parametrami do tworzenia procesu w kodzie
	enum EfektNaukowy { NIEWIEM=-1, PUBLIKACJA, PATENT, RAPORT };
	ProcesBadawczy(const char* Nazwa,EfektNaukowy RodzajProd=NIEWIEM,unsigned ZaplanowanyOdbiorca=-1);//Mo¿e od razu znaæ odbiorce i rodzaj produktu albo nie

	void ChwilaDlaCiebie(); //Endogenne zmiany stanów, popychanie pracy do przodu itp.
							//Na koniec tworzy swoj¹ kopie i sibie przeznacza do skasowania
	bool InterpretujKomunikat(Komunikat* Co);//Przyjmowanie komunikatów
	protected: //Ukryty przed innymi konstruktor domyslny wykonuje robotê za pomoc¹ domyœnego GenerycznyProces()
	ProcesBadawczy(){ ZaplanowanyEfekt=NIEWIEM; ZaplOdbiorca=-1;}
	virtual float _KalkulujKosztBadan(); //Furtka do "realistycznego" kalkulowania kosztów badañ
	bool _WyslijInformacje(unsigned link=-1); //Do rozsylania informacji ¿e siê czyms takim zajmujemy - socjalna lub oficjalna
	bool _WyslijPublikacje(); //Pomocnicza procedura publikowania - domyslnie szuka wêz³a PUBLIKATOR
	bool _WyslijPatent();     //Pomocnicza procedura publikowania - domyslnie szuka wêz³a UPATENTOWY, publikacja wyklucza patent
	bool _WyslijRaport();     //Pomocnicza procedura raportowania - do odbiorcy albo administracji jak brak
	//G³ówna specyficzna zmienna
	private:
	EfektNaukowy ZaplanowanyEfekt;//Jakiego rodzaju efekt ma tu byæ
	unsigned     ZaplOdbiorca;//Czy znany jest odbiorca i kto to
};

// Proces zajmuj¹cy siê produkcj¹ i wysy³¹niem towarów do odbiorcy lub na rynek
// Wysy³a pod koniec dzia³ania, a potem, jeœli wys³anie siê powiod³o to tworzy
// swoj¹ kopiê nieco bardziej efektywn¹ (szybsz¹)
////////////////////////////////////////////////////////////////////////////////
class ProcesProdukcyjny:public GenerycznyProces
{
   //Implementacja tego co musi byæ wg. interfejsu typu Proces
	friend class KonstruktorElementowModelu<ProcesProdukcyjny>;
	static KonstruktorElementowModelu<ProcesProdukcyjny> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
	public:
	//Konstruktor z parametrami do tworzenia procesu w kodzie
	ProcesProdukcyjny(const char* Nazwa, unsigned IleSztuk,float JakaCena,unsigned IleWPaczce=-1);//Dzieli na 10 pak jak -1
	ProcesProdukcyjny(const ProcesProdukcyjny* Wzor);
	void ChwilaDlaCiebie(); //Endogenne zmiany stanów, popychanie pracy do przodu itp.
							//Na koniec tworzy swoj¹ kopie i sibie przeznacza do skasowania
	bool InterpretujKomunikat(Komunikat* Co);//Przyjmowanie komunikatów
	protected: //Ukryty przed innymi konstruktor domyslny wykonuje robotê za pomoc¹ domyœnego GenerycznyProces()
	ProcesProdukcyjny();
	bool _WyslijProduktyNaRynek(); //Pomocnicza procedura wysy³ania na rynek przy koñcu procesu produkcji
	//G³ówna specyficzna zmienna
	private:
	unsigned Zapotrzebowanie; //Na ile sztuk produktu szacujemy zapotrzebowanie
	float    Cena;	  //Jaka cena producenta za sztukê
	unsigned WPaczke; //Po ile sztuk siê miesci w paczce
	unsigned Wyslano; //Ile juz posz³o
};

// Proces poszukuj¹cy pomys³ów na nowe produkty i "odpalajacy" czasem procesy TT
////////////////////////////////////////////////////////////////////////////////
class ProcesPoszukiwanTT:public GenerycznyProces
{
   //Implementacja tego co musi byæ wg. interfejsu typu Proces
	static KonstruktorElementowModelu<ProcesPoszukiwanTT> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
	public:
		 ProcesPoszukiwanTT();//Konstruktor domyslny wykonuje robotê za pomoc¹ domyœnego GenerycznyProces()
	bool InterpretujKomunikat(Komunikat* Co);//Pobiera ró¿ne komunikaty i ³¹czy z nich pomys³y
									//Tak ¿eby by³y zgodne z profilem firmy. Od czasu do czasu odpala
									//zwyk³y ProcesTT
	void ChwilaDlaCiebie(); //Endogenne zmiany stanów - nigdy siê nie koñczy...
	private:
	//Pomocnicze procedury i cechy procesu...
	void _SprobujOdpalicProcesTT(DziedzinaWKolorze Wzorzec,unsigned Wynalazca=-1);
};

// Proces przygotowuj¹cy nowy produkt - musi skompletowaæ wszystkie sk³adniki
//////////////////////////////////////////////////////////////////////////////////
class ProcesTransferuTech:public GenerycznyProces
{
   //Implementacja tego co musi byæ wg. interfejsu typu Proces
	friend class KonstruktorElementowModelu<ProcesTransferuTech>;
	static KonstruktorElementowModelu<ProcesTransferuTech> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
	public:
	ProcesTransferuTech(const char* Nazwa,DziedzinaWKolorze Pomysl,unsigned AutorPomyslu,float IleCzasu);
	//Endogenne zmiany stanów - koñczy siê sukcesem a  lbo pora¿k¹ (z braku czasu lub œrodków)
	//Tu wysy³a ró¿ne zapytania w zaleznoœci od stanu checklisty. Sykces jest wtedy jak odchaczy wszystko
	void ChwilaDlaCiebie();
	//Poszukuje odpowiedzi na swoje pytania. W razie sukcesu odchacza,
	//a w przypadku koniecznoœci ustalenia formalej koopereacji tworzy linki kooperacyjne.
	//W razie pora¿ki zamienia linki koop. na s³abe powi¹zania socjalne (gdy ich brak)
	//W razie sukcesu przetwarza linki koop. mocne socjalne (lub wzmacnia)
	//I jeœli nie dzia³a na bezpoœrednim producencie to sieæ kooperacji mo¿e przerzuciæ na produ
	//na producenta bo jest konieczna do placenia oplat licencyjnych z zysku.
	bool InterpretujKomunikat(Komunikat* Co);
	void Narysuj();//Ma uzupelnione rysowanie
	static double SkalkulujKosztyProjektu(DziedzinaWKolorze D);
	private:
	ProcesTransferuTech(); //Konstruktor domyslny wykonuje robotê za pomoc¹ domyœnego GenerycznyProces()
	void RemanentPoczatkowy();//Sprawdzenie czego robiæ nie trzeba
	bool RemanentCzySukces();//Sprawdzenie czy siê uda³o
	void ObsluzPorazke(); //W razie pora¿ki zamienia linki koop. na s³abe powi¹zania socjalne (gdy ich brak) lub obni¿a wagi
	void ObsluzSukces();  //Przekazuje produkt do produkcji i przeksztalca sieæ linków
	DziedzinaWKolorze WzorzecZapotrzebowania();//Ktorych wyników naukowych wci¹¿ brakuje
	void SprawdzajZapotrzebowania();  //Szprawdza czego potrzebuje
	bool ZarejestrujCoSiePrzyda(DziedzinaWKolorze& D,unsigned Badacz); //Co z badañ badacza siê przyda
	bool OdhaczWyniki(DziedzinaWKolorze& D,unsigned Badacz); //Badacz powinien siê zgadzaæ z zamowieniem, ale jak uprzedzi³ to jego zysk (czesciowy?)
	void _RozeslijPoLinkachSocjalnych(Komunikat* Wzor); //Mno¿y komunikat po linkach socjalnych
	//Pola chekclisty
	unsigned AutorPomyslu;
	struct ElementListy {
	double Kiedy;unsigned Wykonawca;bool Check;
	ElementListy(){Kiedy=-1;Wykonawca=-1;Check=false;}
	};
	static const unsigned PRODUCENT=24;    //Gdzie zaznaczamy kto wyprodukuje
	static const unsigned FINANSOWANIE=25; //Gdzie zaznaczamy kto to finansuje
	ElementListy Fragmenty[26];//Sk¹d s¹ poszczególne fragmenty/elementy produktu
};

/********************************************************************/
/*			          OPI  version 2011                             */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://wwww.iss.uw.edu.pl/borkowski/                */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
#endif