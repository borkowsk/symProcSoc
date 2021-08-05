// Transfer technologii 2011 - w³aœciwa implementacja modelu     24-10-2011
////////////////////////////////////////////////////////////////////////////////
// Klasy modelu OPI - IMPLEMENTACJA TEGO CO BYÆ W NICH MUSI
// ze wzglêdu na wymagania interfaców pakietu SPS
////////////////////////////////////////////////////////////////////////////////

#include "opiKlasyWersja.hpp"
#include "INCLUDE/wb_smartlog.h"
using namespace wbrtm;

// OBIEKTY WIRTUALNYCH KONSTRUKTORÓW DLA KA¯DEJ Z KLAS MODELU
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Proces zajmuj¹cy siê produkcj¹ i wysy³¹niem towarów do odbiorcy lub na rynek
// Wysy³a pod koniec dzia³ania, a potem, jeœli wys³anie siê powiod³o to tworzy
// swoj¹ kopiê nieco bardziej efektywn¹ (szybsz¹)
////////////////////////////////////////////////////////////////////////////////
ProcesProdukcyjny::KonstruktorElementowModelu<ProcesProdukcyjny> ProcesProdukcyjny::WirtKonstr("ProductProc");

// Proces obci¹¿aj¹cy wêze³ i na koñcu wysy³aj¹cy raport do odbiorcy lub wêz³a
// nadrzêdnego (linkiem Adm.) i wzawiaj¹cy dzia³anie od poczatku bez nowego procesu
/////////////////////////////////////////////////////////////////////////////////////
ProcesRaportowany::KonstruktorElementowModelu<ProcesRaportowany> ProcesRaportowany::WirtKonstr("ReportProc");

// Proces badawczy produkuje wyniki naukowe do celów publikacji, patentowania,
// lub na czyjeœ zamówienie, st¹d ma w³aœciwoœæ "ZaplanowanyOdbiorca"
///////////////////////////////////////////////////////////////////////////////////////////////
ProcesBadawczy::KonstruktorElementowModelu<ProcesBadawczy> ProcesBadawczy::WirtKonstr("ResearchProc");

// Proces kreuj¹cy nowe linki - losuje wêz³y i wg. podobieñstwa tworzy link i
// wysy³a nim zajawkê o sobie, nadaje wagê proporcjonaln¹ do wagi wêz³a (albo...)
// Jak nie utworzy linku to to wysy³a przypomnienie do jakiegoœ z istniej¹cych
// powi¹zañ spo³ecznych zawieraj¹ce losowe bity ze swojej dziedziny.
////////////////////////////////////////////////////////////////////////////////
ProcesSpoleczny::KonstruktorElementowModelu<ProcesSpoleczny> ProcesSpoleczny::WirtKonstr("SocialProc");

// Proces poszukuj¹cy pomys³ów na nowe produkty i "odpalajacy" czasem procesy TT
//////////////////////////////////////////////////////////////////////////////////
ProcesPoszukiwanTT::KonstruktorElementowModelu<ProcesPoszukiwanTT>
								ProcesPoszukiwanTT::WirtKonstr("TTSearchProc");

// Proces przygotowuj¹cy nowy produkt - musi skompletowaæ wszystkie sk³adniki
//////////////////////////////////////////////////////////////////////////////////
ProcesTransferuTech::KonstruktorElementowModelu<ProcesTransferuTech>
								ProcesTransferuTech::WirtKonstr("TTransferProc");

// Proces zajmuj¹cy siê dawaniem grantów, dotacji lub po¿yczek na procent
/////////////////////////////////////////////////////////////////////////////////////
ProcesGrantowoPozyczkowy::KonstruktorElementowModelu<ProcesGrantowoPozyczkowy>
							ProcesGrantowoPozyczkowy::WirtKonstr("FinSourceProc");

//Klasa powi¹zania administracyjnego - przepuszcza w górê raporty,
// a w dó³ wytyczne oraz pieni¹dze
////////////////////////////////////////////////////////////////////////////////
PodlegloscOrganizacyjna::KonstruktorElementowModelu<PodlegloscOrganizacyjna>
	PodlegloscOrganizacyjna::WirtKonstr("AdminLink");

// Klasa kooperacji sformalizowanej - przepuszcza w raporty - efekty dzia³añ
// oraz pieni¹dze
////////////////////////////////////////////////////////////////////////////////
FormalnaKooperacja::KonstruktorElementowModelu<FormalnaKooperacja>
	FormalnaKooperacja::WirtKonstr("CoopLink");

//Klasa powi¹zania towarzyskiego, zanikaj¹cego powoli gdy nie u¿ywany
// ASYMETRYCZNA - TAKIE LINKI S¥ POD DWA BO ZAUFANIE I WAGA POWI¥ZANIA
// ZAZWYCZAJ JEST RÓ¯NA Z PUNKTU WIDZENIA OBU PARTNERÓW!
////////////////////////////////////////////////////////////////////////////////
KontaktTowarzyski::KonstruktorElementowModelu<KontaktTowarzyski>
	KontaktTowarzyski::WirtKonstr("SocialLink");

//Klasa komunikatu oficjalnego - raporty (w górê), wytyczne w dó³ (podleglosci  adm.)
////////////////////////////////////////////////////////////////////////////////
KomunikacjaTowarzyska::KonstruktorElementowModelu<KomunikacjaTowarzyska>
	KomunikacjaTowarzyska::WirtKonstr("SocialInfo");

//Klasa komunikatu oficjalnego - raporty (w górê), wytyczne w dó³ (podleglosci  adm.)
////////////////////////////////////////////////////////////////////////////////
KomunikacjaOficjalna::KonstruktorElementowModelu<KomunikacjaOficjalna>
	KomunikacjaOficjalna::WirtKonstr("OficialInfo");

//Klasa komunikatu-pakietu do przesy³ania realnych produktów z badañ lub fabryk
// potrzebna g³ówne ze wzglêdu na odróznienie wizualizacyjne
////////////////////////////////////////////////////////////////////////////////
PaczkaProduktow::KonstruktorElementowModelu<PaczkaProduktow>
	PaczkaProduktow::WirtKonstr("PackInfo");

//Klasa komunikatu finansowego - obietnice finansowe i kredytowe, umowy, proformy, przelewy
////////////////////////////////////////////////////////////////////////////////
KomunikacjaFinansowa::KonstruktorElementowModelu<KomunikacjaFinansowa>
	KomunikacjaFinansowa::WirtKonstr("FinancInfo");

//Klasa dla œwiata - realizuje odpowiedzi na wolne poszukiwania, zapytania o produkty
// a tak¿e wymianê gotowych produktów na konkretn¹ kasê (czyli "zbyt")
////////////////////////////////////////////////////////////////////////////////
RynekZbytu::KonstruktorElementowModelu<RynekZbytu>
	RynekZbytu::WirtKonstr("MarketNode");

//Klasa dla PUBLIKATORA i UPATENTOWEGO - musi przyjmowac tylko komunikaty oficjalne
// a poza tym zachowuje siê podobnie do klasy bazowej
///////////////////////////////////////////////////////////////////////////////////
SystemInformacyjny::KonstruktorElementowModelu<SystemInformacyjny>
	SystemInformacyjny::WirtKonstr("InforNode");

//Klasa dla badacza i/lub zespo³u badawczego
////////////////////////////////////////////////////////////////////////////////
JednostkaBadawcza::KonstruktorElementowModelu<JednostkaBadawcza>
	JednostkaBadawcza::WirtKonstr("ResearchUNode");

//Klasa dla Instytutu, Wydzia³u, Uniwersytetu itp.
////////////////////////////////////////////////////////////////////////////////
AdministracjaNaukowa::KonstruktorElementowModelu<AdministracjaNaukowa>
	AdministracjaNaukowa::WirtKonstr("ResAdminNode");

//Klasa wykonwacy/producenta ostatecznego produktu
////////////////////////////////////////////////////////////////////////////////
Firma::KonstruktorElementowModelu<Firma>
	Firma::WirtKonstr("BusinessNode");

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
BOTT::KonstruktorElementowModelu<BOTT> BOTT::WirtKonstr("BOTTNode");
UOTT::KonstruktorElementowModelu<UOTT> UOTT::WirtKonstr("UOTTNode");


// Metody na potrzeby wczytywania i zapisywania plików danych
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
Komunikat* KomunikacjaTowarzyska::Klonuj(){return new KomunikacjaTowarzyska(this);}
Komunikat* KomunikacjaOficjalna::Klonuj(){return new KomunikacjaOficjalna(this);}
Komunikat* KomunikacjaFinansowa::Klonuj(){return new KomunikacjaFinansowa(this);}
Komunikat* PaczkaProduktow::Klonuj(){return new PaczkaProduktow(this);}

void JednostkaBadawcza::AktualizujListeDanych()
//Do zapisu i wyœwietlania inspekcyjnego
{
	unsigned Poz=8;           //PROWIZORKA!!! Na razie na "sile"
	//Finans. bie¿¹ce  = u float    FinanseBiezace; //Ile firma ma kasy aktualnie LUB UPROSZCZONA "ZAMO¯NOŒÆ"
	Dane.PrzypiszZ(Poz+8,FinanseSwobodne);
	//Finanse zaplan  = u float    FinanseZaplanowane;//Ile firma ma obiecanych œrodków kredytowych
	Dane.PrzypiszZ(Poz+9,FinanseObiecane );
	//D³ugi  = u float 	 Dlugi ;//Ile firma ma do sp³aty. Im wiêcej tym mniejsza szansa ¿e coœ dostanie nowego
	Dane.PrzypiszZ(Poz+10,Dlugi );
}

bool JednostkaBadawcza::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	unsigned Poz;
	if(!GenerycznyWezelSieci::ZrobWgListy(Lista,Ile,Poz)) {Blad=Poz; return false;} //Nie uda³o siê
 //Ile Etatów = unsigned IleEtatow;//Ile etatów. Co najmniej 1, 0 - "duch" zespo³u - plan utworzenia
	if(Dane.KonwertujDo(Poz+0,IleEtatow)!=-1) {Blad=Poz+0; return false;}
 //Efektywnosc = float    Efektywnosc;//0..2. Czy dzia³a na 50% wydajnoœci czy na 150%. Zale¿y od motywacji
	if(Dane.KonwertujDo(Poz+1,Efektywnosc)!=-1) {Blad=Poz+1; return false;}
 //Aktywnoœæ = float    Proaktywnosc;//Aktywnoœæ? 0..1. Podtrzymywanie aktywnoœci zewnêtrznej, tworzenie linków
	if(Dane.KonwertujDo(Poz+2,Proaktywnosc)!=-1) {Blad=Poz+2; return false;}
 //Centralnoœæ = float    Centralnoœæ;//0..1. Jak bardzo aktywnoœæ (zewnêtrzna?) zale¿y od szefa/szefów
	if(Dane.KonwertujDo(Poz+3,Centralnosc)!=-1) {Blad=Poz+3; return false;}
 //Probiznesowosc = float    Probiznesowosc;//0 - czysta nauka, 0.5-badania patentowalne/licenjonowalne, 1 - czysty biznes - szybkie wdra¿anie
	if(Dane.KonwertujDo(Poz+4,Probiznesowosc)!=-1) {Blad=Poz+4; return false;}
 //Prolegalnosc = float    Prolegalnosc;//????Uczciwoœæ??? Jak ³atwo podj¹æ dzia³ania "szarostrefowe"
	if(Dane.KonwertujDo(Poz+5,Prolegalnosc)!=-1) {Blad=Poz+5; return false;}
 //Czasochlonnosc = float    Czasochlonnosc;//Jaki jest wspó³czynnik czasoch³onnoœci typowej dzia³alnoœci. Wygenerowania pojedynczego wyniku badañ?
	if(Dane.KonwertujDo(Poz+6,Czasochlonnosc)!=-1) {Blad=Poz+6; return false;}
 //Innowacyjnoœc = float    Innowacyjnosc;//0..1. £atwoœæ startowania projektów bez inspiracji z zewn¹trz, zaciekawienie projektami nieco odleg³ymi od dotychczasowych
	if(Dane.KonwertujDo(Poz+7,Innowacyjnosc)!=-1) {Blad=Poz+7; return false;}
 //Finans. swobodne = float    FinanseSwobodne;//Rezerwy finansowe - w skali roku LUB UPROSZCZONA "ZAMO¯NOŒÆ"
	if(Dane.KonwertujDo(Poz+8,FinanseSwobodne)!=-1) {Blad=Poz+8; return false;}
 //Finanse zaplan = float    FinanseObiecane;//Zaplanowane do wydania, ale byæ mo¿e jeszcze nie na koncie (odnawiane/odbierane co 365 kroków)
	if(Dane.KonwertujDo(Poz+9,FinanseObiecane)!=-1) {Blad=Poz+9; return false;}
 //D³ug do nadrz. = float 	  Dlugi;//Ile zespó³ "wisi" swojemu wydzia³owi/instytutowi. Mo¿liwe tylko chwilowo (30 kroków?)
	if(Dane.KonwertujDo(Poz+10,Dlugi)!=-1) {Blad=Poz+10; return false;}
 //Udzial w zysku TT = float    UdzialWZysku;//0..1 Jaki procent zysku z innowacji pobiera (reszta idzie dla pracowników)
	if(Dane.KonwertujDo(Poz+11,UdzialWZysku)!=-1) {Blad=Poz+11; return false;}
 //Eksperckosc w dziedzinie = float    Doswiadczenie;//POZIOM EKSPERTYZY
	if(Dane.KonwertujDo(Poz+12,Doswiadczenie)!=-1) {Blad=Poz+12; return false;}  //Powinno byc 12!!!
   Blad=Poz+13;//OK
   return true;
}

void AdministracjaNaukowa::AktualizujListeDanych()
//Do zapisu i wyœwietlania inspekcyjnego
{
	unsigned Poz=8;           //PROWIZORKA!!! Na razie na "sile"
	//Finans. bie¿¹ce  = u float    FinanseBiezace; //Ile firma ma kasy aktualnie LUB UPROSZCZONA "ZAMO¯NOŒÆ"
	Dane.PrzypiszZ(Poz+8,FinanseSwobodne);
	//Finanse zaplan  = u float    FinanseZaplanowane;//Ile firma ma obiecanych œrodków kredytowych
	Dane.PrzypiszZ(Poz+9,FinanseObiecane );
	//D³ugi  = u float 	 Dlugi ;//Ile firma ma do sp³aty. Im wiêcej tym mniejsza szansa ¿e coœ dostanie nowego
	Dane.PrzypiszZ(Poz+10,Dlugi );
}

bool AdministracjaNaukowa::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	unsigned Poz;
	if(!GenerycznyWezelSieci::ZrobWgListy(Lista,Ile,Poz)) {Blad=Poz; return false;} //Nie uda³o siê
	//Ile Etatów = unsigned  IleEtatow;//Ile etatów. Co najmniej 1, 0 - "duch" zespo³u - plan utworzenia
	if(Dane.KonwertujDo(Poz+0,IleEtatow)!=-1) {Blad=Poz+0; return false;}
	//Efektywnosc =  float     Efektywnosc;//0..2. Czy dzia³a na 50% wydajnoœci czy na 150%. Zale¿y od motywacji
	if(Dane.KonwertujDo(Poz+1,Efektywnosc)!=-1) {Blad=Poz+1; return false;}
	//Aktywnoœæ =  float     AktywnoscKonf;//(Pro)aktywnoœæ. Prawdopodobieñstwo organizowania konferencji i innych eventów umo¿liwiaj¹cych kontakty miêdzy naukowcami i goœæmi (czyli co ile kroków nastêpuje taki event)
	if(Dane.KonwertujDo(Poz+2,Proaktywnosc)!=-1) {Blad=Poz+2; return false;}
	//Centralnoœæ =  float     Centralnoœæ;//0..1. Jak bardzo aktywnoœæ (zewnêtrzna?) zale¿y od szefa/szefów
	if(Dane.KonwertujDo(Poz+3,Centralnosc)!=-1) {Blad=Poz+3; return false;}
	//Probiznesowosc =  float     Probiznesowosc;//0...1 Prawdopodobieñstwo podjêcia inicjatyw z wspó³udzia³em biznesu
	if(Dane.KonwertujDo(Poz+4,Probiznesowosc)!=-1) {Blad=Poz+4; return false;}
	//Prolegalnosc =  float     Prolegalnosc;//????Elastycznoœæ??? Jak ³atwo podj¹æ dzia³ania obchodz¹ce "bzdurne" przepisy
	if(Dane.KonwertujDo(Poz+5,Prolegalnosc)!=-1) {Blad=Poz+5; return false;}
	//Czasochlonnosc =  float     Czasochlonnosc;// Ile czasu (kroków modelu) siê czeka z podjêciem decyzji
	if(Dane.KonwertujDo(Poz+6,Czasochlonnosc)!=-1) {Blad=Poz+6; return false;}
	//ProTT =  float     ProInwest;//0-1 Sk³onnoœæ inwestowania wolnych œrodków w rozwój zwi¹zany z TT
	if(Dane.KonwertujDo(Poz+7,ProInwest)!=-1) {Blad=Poz+7; return false;}
	//Finans. swobodne =  float     FinanseSwobodne;//Rezerwy finansowe - w skali roku (odnawiane/odbierane co 365 kroków) LUB UPROSZCZONA "ZAMO¯NOŒÆ"
	if(Dane.KonwertujDo(Poz+8,FinanseSwobodne)!=-1) {Blad=Poz+8; return false;}
	//Finanse zaplan  = float     FinanseObiecane;//Zaplanowane do wydania, ale byæ mo¿e jeszcze nie na koncie
	if(Dane.KonwertujDo(Poz+9,FinanseObiecane)!=-1) {Blad=Poz+9; return false;}
	//D³ugi =  float 	   Dlugi;//Ile jednostka ju¿ "wisi". Np. na budowê, albo na pr¹d jak dotacja zaleg³a. Nie mo¿e wiêcej ni¿ urzêdowy limit
	if(Dane.KonwertujDo(Poz+10,Dlugi)!=-1) {Blad=Poz+10; return false;}
	//Udzial w zysku TT =  float     UdzialWZysku;//0..1 Jaki procent zysku z innowacji pobiera (reszta idzie "w dó³" hierarchii)
	if(Dane.KonwertujDo(Poz+11,UdzialWZysku)!=-1) {Blad=Poz+11; return false;}
	//Doœwiadczenie =  float     Doswiadczenie;//POZIOM EKSPERTYZY? Jak tego u¿ywaæ? Mo¿e to samo co efektywnoœæ???
	if(Dane.KonwertujDo(Poz+12,Doswiadczenie)!=-1) {Blad=Poz+12; return false;}
	Blad=Poz+13;//OK
	return true;
}

void Firma::AktualizujListeDanych()
//Do zapisu i wyœwietlania inspekcyjnego
{
	unsigned Poz=8;           //PROWIZORKA!!! Na razie na "sile"
	//Finans. bie¿¹ce  = u float    FinanseBiezace; //Ile firma ma kasy aktualnie LUB UPROSZCZONA "ZAMO¯NOŒÆ"
	Dane.PrzypiszZ(Poz+8,FinanseBiezace );
	//Finanse zaplan  = u float    FinanseZaplanowane;//Ile firma ma obiecanych œrodków kredytowych
	Dane.PrzypiszZ(Poz+9,FinanseZaplanowane );
	//D³ugi  = u float 	 Dlugi ;//Ile firma ma do sp³aty. Im wiêcej tym mniejsza szansa ¿e coœ dostanie nowego
	Dane.PrzypiszZ(Poz+10,Dlugi );
}

bool Firma::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	unsigned Poz;
	if(!GenerycznyWezelSieci::ZrobWgListy(Lista,Ile,Poz))  {Blad=Poz; return false;} //Nie uda³o siê
	//Ile Etatów  = unsigned IleEtatow;//Ile etatów. Co najmniej 1, 0 - "duch" zespo³u - plan utworzenia
	if(Dane.KonwertujDo(Poz+0,IleEtatow)!=-1) {Blad=Poz+0; return false;}
	//Efektywnosc  = u float    Efektywnosc;//0..2. Czy dzia³a na 50% wydajnoœci czy na 150%. Zale¿y od motywacji
	if(Dane.KonwertujDo(Poz+1,Efektywnosc )!=-1) {Blad=Poz+1; return false;}
	//Aktywnoœæ  = u float    Proaktywnosc; //Aktywnoœæ. 0-1 aktywnoœæ w tworzenia nowych znajomoœci biznesowych
	if(Dane.KonwertujDo(Poz+2,Proaktywnosc )!=-1) {Blad=Poz+2; return false;}
	//Centralnoœæ  = u float    Centralnoœæ;//0..1. Jak bardzo aktywnoœæ (zewnêtrzna?) zale¿y od szefa/szefów
	if(Dane.KonwertujDo(Poz+3,Centralnosc )!=-1) {Blad=Poz+3; return false;}
	//Pronaukowosc  = u float    ProAkademickosc; //0-1 chêæ nawi¹zywania kontaktów z badaczami i uniwersytetami
	if(Dane.KonwertujDo(Poz+4,ProAkademickosc )!=-1) {Blad=Poz+4; return false;}
	//Prolegalnosc  = u float    Prolegalnosc;//?"Pragmatycznoœæ"? Jak ³atwo podj¹æ dzia³ania "szarostrefowe" i omijac "nie¿yciowe" przepisy
	if(Dane.KonwertujDo(Poz+5,Prolegalnosc )!=-1) {Blad=Poz+5; return false;}
	//Czasochlonnosc  = u float    Czasochlonnosc;//Jaki jest wspó³czynnik czasoch³onnoœci typowej dzia³alnoœci (np. przygotowanie projektu oraz produktu)
	if(Dane.KonwertujDo(Poz+6,Czasochlonnosc )!=-1) {Blad=Poz+6; return false;}
	//Proinnowacyjnosc  = u float    ProInnowac;//Stosunek do nowoœci: 0-1 - 0..33% niewiele robi, 33-66% raczej blokuje innych, powy¿ej - próbuje, wchodz¹c na coraz wczeœniejszym etapie (prawdopodobienstwo)
	if(Dane.KonwertujDo(Poz+7,ProInnowac )!=-1) {Blad=Poz+7; return false;}
	//Finans. bie¿¹ce  = u float    FinanseBiezace; //Ile firma ma kasy aktualnie LUB UPROSZCZONA "ZAMO¯NOŒÆ"
	if(Dane.KonwertujDo(Poz+8,FinanseBiezace )!=-1) {Blad=Poz+8; return false;}
	//Finanse zaplan  = u float    FinanseZaplanowane;//Ile firma ma obiecanych œrodków kredytowych
	if(Dane.KonwertujDo(Poz+9,FinanseZaplanowane )!=-1) {Blad=Poz+9; return false;}
	//D³ugi  = u float 	 Dlugi ;//Ile firma ma do sp³aty. Im wiêcej tym mniejsza szansa ¿e coœ dostanie nowego
	if(Dane.KonwertujDo(Poz+10,Dlugi )!=-1) {Blad=Poz+10; return false;}
	//Limit TT  = u float 	  LimitUdzialuInwestTT; //0-1 Jakie œrodki finansowe z zarobionych gotów przeznaczyæ na inwestycje TT
	if(Dane.KonwertujDo(Poz+11,LimitUdzialuInwestTT )!=-1) {Blad=Poz+11; return false;}
	//Znajomoœæ bran¿y  = u float    Doswiadczenie;//POZIOM EKSPERTYZY? Jak tego u¿ywaæ? Mo¿e to samo co efektywnoœæ???
	if(Dane.KonwertujDo(Poz+12,Doswiadczenie )!=-1) {Blad=Poz+12; return false;}
	Blad=Poz+13;//OK
	return true;
}

bool BOTT::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	return Firma::ZrobWgListy(Lista,Ile,Blad);
}
bool UOTT::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	return AdministracjaNaukowa::ZrobWgListy(Lista,Ile,Blad);
}
bool PodlegloscOrganizacyjna::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	return GenerycznePowiazanie::ZrobWgListy(Lista,Ile,Blad);
}
bool FormalnaKooperacja::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	return PowiazaniePaboliczne::ZrobWgListy(Lista,Ile,Blad);
}
bool KontaktTowarzyski::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	return PowiazaniePaboliczneSkierowane::ZrobWgListy(Lista,Ile,Blad);
}
bool KomunikacjaTowarzyska::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	return GeneryczneInfo::ZrobWgListy(Lista,Ile,Blad);
}
bool KomunikacjaOficjalna::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	return GeneryczneInfo::ZrobWgListy(Lista,Ile,Blad);
}
bool KomunikacjaFinansowa::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	return GeneryczneInfo::ZrobWgListy(Lista,Ile,Blad);
}
bool PaczkaProduktow::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	return GeneryczneInfo::ZrobWgListy(Lista,Ile,Blad);
}
bool RynekZbytu::ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad)
{
	return WezelMacierzowy::ZrobWgListy(Lista,Ile,Blad);
}

// DODATKOWE IMPLEMENTACJE WIZUALIZACJI!!!
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//Chwilowo tutaj ró¿ne wizualizacje komunikatów zrobione proceduralnie.
//Na razie SPS tylko to umo¿liwia, ale docelowo powinny byæ obiekty wizualizacyjne
//u¿ywaj¹ce po prostu obrazków lub animacji - jak ¿ó³wie w Logomocji czy StarLogo

#include "SYMSHELL/symshell.h"

double _ZespolRoboczy::R(double KAT_NIEISTOTNY)
//Promieñ otaczaj¹cego zespól roboczy okrêgu lub elipsy musi byæ bardziej zmyœlnie
//zalezny od wagi, poniewa¿ te wagi bywaj¹ bardzo du¿e w trakcie symulacji
{
	double JakWazny=this->Waznosc();
	if(JakWazny<=1)
	{
	   JakWazny=1+((JakieRwProcWidth/100)*ver*JakWazny);
	   return JakWazny;
	}
	else
	{
	   double LogWazny=log10(JakWazny);
	   JakWazny=1+((JakieRwProcWidth/100)*ver*(1+LogWazny));
	   return JakWazny;
	}
}

void FormalnaKooperacja::Narysuj()
{
	PowiazaniePaboliczne::Narysuj();//Przechwycenie rysowania dla debugingu
}

void KomunikacjaTowarzyska::RysujKsztalt(float X,float Y,float Rad,unsigned R,unsigned G,unsigned B)
{
	set_brush_rgb(R,G,B);//Podstawa
	set_pen_rgb(R/2,G/2,B/2,0,SSH_LINE_SOLID); //Ciemne  //Free_style_pen
	//set_pen_rgb(R*2%256,G*2%256,B*2%256,0,SSH_LINE_SOLID); //Kontur
	//fill_rect_d(X-Rad,Y-Rad,X+Rad,Y+Rad);
	//set_brush_rgb(R,G,B);
	fill_circle_d(X,Y,Rad);
	set_pen_rgb(min(R*2,255u),min(G*2,255u),min(B*2,255u),0,SSH_LINE_SOLID); //JASNE - Free_style_pen
	int Left=X-Rad+1;
	int Right=X+Rad-1;
	if(this->Rodzaj().find('?'))
	{
	   line_d(Left,Y-Rad/2,Right,Y+Rad/2);
	   line_d(Left,Y+Rad/2,Right,Y-Rad/2);
	}
	else
	{
	for(unsigned i=0;i<unsigned(Rad);i++)
		if(i%2==0)
			//line_d(X-Rad+1,Y-Rad+i,X+Rad-1,Y-Rad+i);
			line_d(Left,Y-Rad/2+i,Right,Y-Rad/2+i);
	}
}

void KomunikacjaOficjalna::RysujKsztalt(float X,float Y,float Rad,unsigned R,unsigned G,unsigned B)
{
	if(this->Rodzaj()=="RAPORT")
		set_brush_rgb(255,255,255);//Bia³a kartka
		else
        set_brush_rgb(R,G,B);//Kartka w kolorze tematycznym
	fill_rect_d(X-Rad,Y-Rad,X+Rad,Y+Rad);
	set_pen_rgb(R/2,G/2,B/2,0,SSH_LINE_SOLID); //Ciemne  //Free_style_pen
	for(unsigned i=0;i<unsigned(2*Rad);i++)
		if(i%2==0)
			line_d(X-Rad+1,Y-Rad+i,X+Rad-1,Y-Rad+i);
	//set_brush_rgb(R,G,B);
	//fill_circle_d(X,Y,Rad);
}

void KomunikacjaFinansowa::RysujKsztalt(float X,float Y,float Rad,unsigned R,unsigned G,unsigned B)
{
	float L;
	if(Kwota>1)
		L=log10(double(this->Kwota));  //Uzaleznienie wielkoœci od kwoty
		else
		L=0.5;//Czasem tu siê coœ dzieje choæ nie powinno
	if(L<2) Rad/=2; //Drobne kwoty
		else
		Rad*=L-1;//Typowa kwota to 100!
	set_brush_rgb(255,255,255);//Bia³a kartka
	fill_rect_d(X-Rad,Y-Rad,X+Rad,Y+Rad);
	set_pen_rgb(R/2,G/2,B/2,0,SSH_LINE_SOLID); //Ciemne  //Free_style_pen
	for(unsigned i=Rad;i<unsigned(2*Rad);i++) //Rzut ¿eby nie "parska³"
		if(i%2==0)
			line_d(X-Rad+1,Y-Rad+i,X+Rad-1,Y-Rad+i);
	set_pen_rgb(R/2,G/2,B/2,0,SSH_LINE_SOLID);  //Free_style_pen
	set_brush_rgb(255,255,0); //Z³oto
	fill_circle_d(X,Y,Rad/2);
}

void PaczkaProduktow::RysujKsztalt(float X,float Y,float Rad,unsigned R,unsigned G,unsigned B)
{                            						assert(this->Ile>=1);
    float L;
	if(Ile>1)
		L=log10(double(this->Ile));  //Uzaleznienie wielkoœci od kwoty
		else
		L=0.5;//Czasem tu siê coœ dzieje choæ nie powinno
	if(L<1) Rad/=2;   //Typowy rozmiar to 10
		else
		Rad*=L;
	set_brush_rgb(R,G,B);
	//Paczka
	fill_rect_d(X-Rad,Y-Rad,X+Rad,Y+Rad);
	//fill_circle_d(X,Y,Rad);
	set_pen_rgb(R/3,G/3,B/3,0,SSH_LINE_SOLID);  //Free_style_pen
	//Sznurek
	line_d(X-Rad,Y,X+Rad,Y);
	line_d(X,Y-Rad,X,Y+Rad);
}


void ProcesTransferuTech::Narysuj()
//Ma uzupelnione rysowanie
{
	double SkalaX=Swiat::SkalujX();
	double SkalaY=Swiat::SkalujY();

	GenerycznyProces::Narysuj();
	WezelSieci* Mw=Swiat::Wezel(Procesor());
	if(Mw==NULL)
		cerr<<endl<<"Proces na nieistniej¹cym wezle"<<endl;
	unsigned ile=Swiat::IleProcesow(Procesor());

	double R=Mw->R(1.5*M_PI);
	double Y=Mw->Y()-0.1*R+this->MojID();//Indeks procesu na wêŸle
	double X1=Mw->X()-R;
	double X2=X1+2*R;

	for(unsigned i=0;i<sizeof(Fragmenty)/sizeof(Fragmenty[0]);i++)
	{
		if(Fragmenty[i].Check)
		{        //Fragmenty[i].Check=true;//oznacza ¿e bit niepotrzebny!
				 //Fragmenty[i].Wykonawca=-1;//Zaden wykonawca a odhaczone
			if(Fragmenty[i].Wykonawca!=-1)
			   set_pen_rgb(255,255,255,1,SSH_LINE_SOLID);//Gotowe - bia³y!
			else
			   set_pen_rgb(0,255,255,1,SSH_LINE_SOLID);  //Niepotrzebne - Cyjan
		}
		else
			set_pen_rgb(0,0,0,1,SSH_LINE_SOLID);  //Czarny
		line_d(SkalaX*X1-i-1,SkalaY*Y-1,SkalaX*X1-i-1,SkalaY*Y+1); //Linia bitowa na check
	}

	if( Fragmenty[PRODUCENT].Check )    //Gdzie zaznaczamy kto wyprodukuje
	{
		set_pen_rgb(255,255,255,1,SSH_LINE_SOLID);
		line_d(SkalaX*X2+2,SkalaY*Y-1,SkalaX*X2+4,SkalaY*Y+2); //Linia bitowa na check
	}
	if( Fragmenty[FINANSOWANIE].Check ) //Gdzie zaznaczamy kto to finansuje
	{
		set_pen_rgb(255,0,255,1,SSH_LINE_SOLID);
		line_d(SkalaX*X2+0,SkalaY*Y-1,SkalaX*X2+2,SkalaY*Y+2); //Linia bitowa na check
	}
}

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

/*
#include "spsModel.h"
#include "spsGenNode.h"
#include "spsGenProc.h"
#include "spsGenLink.h"
#include "spsParaLink.h"
#include "spsGenInfo.h"
#include "spsMatrixNode.h"
#include "INCLUDE/platform.hpp"
#include "INCLUDE/wb_bits.h"
#include "INCLUDE/wb_ptr.hpp"
using namespace wbrtm;

#include <stdlib.h>
#include <cassert>
#include <string>
#include <iostream>
using namespace std;
*/

/*  STARY KOD Z "JEDNOSTKI NAUKOWEJ"
		else  //Obsluga konca procesu sprawozdawczego
		{
			DziedzinaWKolorze D=Roboczy->PodajDziedzine();
			std::string Nazwa=Roboczy->Dane[1];

			//Konczymy i ewentualnie tworzymy jakiœ nowy proces gdy obici¹¿eniowo-sprawozdawczy
			c----<<endl<<this->Nazwa()<<' '<<_LPL("zakonczyl proces","fin.proc.")<<' '<<Nazwa<<' '<<Roboczy->DoKonca()<<' ';

			if(Nazwa=="OBCI¥¯ENIE")
			{
				double Zaangazowanie=Roboczy->JakPracochlonny();//Jak pracoch³onne to dzia³anie
				//Delikatne obni¿anie obci¹¿enia jak siê nie wyrabiaj¹
				if(Roboczy->DoKonca()<-7)
				{
					Zaangazowanie*=0.99;
					c----<<" z mocna OBSUWA!!!"<<endl;
				}
				else c----<<endl;
				//Usuwamy bo siê skoñczyl
				Roboczy->Wymazuj();
				Roboczy->UznajZaZakonczony();

				//Tworzymy nowy
				Roboczy=new GenerycznyProces(Nazwa.c_str(),Zaangazowanie,365);  //356 dni oczywiscie
				Roboczy->UstawDziedzine(D); //Dziedzina poprzedniego procesu
				Swiat::WstawProc(Roboczy,MojID());
				c----<<this->Nazwa()<<' '<<_LPL("ustalil nowy proces","set new proc.")<<' '<<Nazwa<<' ';
				c----<<"Zaang.:"<<Zaangazowanie<<"=="<<(Zaangazowanie/(IleEtatow*365))*100<<"% wydolnosci"<<endl;
			}
			else
			{
				//Usuwamy proces inny ni¿ OBCIAZENIE bo siê skonczyl
				Roboczy->Wymazuj();
				Roboczy->UznajZaZakonczony();
				//Swiat::UsunProc(ktory,MojID());
			}

			//Generuje raport dla administracji
			KomunikacjaOficjalna* KO=new KomunikacjaOficjalna();
			KO->UstawDziedzine(D);
			//Dosyæ skomplikowane szukanie adresata. Trzeba by mo¿e zrobiæ lepsz¹ obslugê linków w³asnych wêz³a
			unsigned Kanal=-1;
			for(unsigned i=0;i<Swiat::IlePowiazan();i++)
			{
				PodlegloscOrganizacyjna* P=dynamic_cast<PodlegloscOrganizacyjna*>(Swiat::Lacze(i));
				if(P!=NULL && P->Poczatek()==MojID())
					{Kanal=i;break;}
			}
			if(Kanal!=Swiat::INVINDEX
				&& KO->Zaadresuj(Kanal,true,0.05)
				&& (Swiat::WstawInfo(KO)!=Swiat::INVINDEX) )
					;//OK
		}
*/


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
