// Transfer technologii 2011
///////////////////////////////////////////////////////////////
// Funkcje obliczania statystyk dla Œwiata  - wyodrêbnione
// ¿eby by³o ³atwo znaleŸæ i rozbudowywaæ
////////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <map>
#include <list>
#define _USE_MATH_DEFINES /*For MSVC*/
#include <math.h>
#include <string.h>
#include <time.h>
using namespace std;

#include "INCLUDE/platform.hpp"
#include "spsModel.h"

/*
GenNode;MattNode;ResAdminNode;ResearchUNode;BusinessNode;MarketNode;UOTTNode;BOTTNode; InforNode;
GenInfo;SocialInfo;FinancInfo;PackInfo;OficialInfo;
GenLink;DirGenLink;DirParaLink;ParaLink;SocialLink;AdminLink;CoopLink;
GenProc;SocialProc;ReportProc;ProductProc;   ResearchProc;  FinSourceProc;  TTSearchProc;  TTransferProc;
*/

extern char PlikWejsciowy[256];

unsigned STAT_HISTOGRAM_WAZNOSCI=5;//Liczba przedzia³ów histogramów "wa¿noœci"
unsigned MAX_WARTOSC_POWIAZANIA=1;
unsigned MAX_WARTOSC_WEZLA=STAT_HISTOGRAM_WAZNOSCI;//Do ewentualnej zmiany przy starcie programu lub z pliku .dat

//Widoczny te¿ na zewn¹trz strumien logu statystycznego.
//¯eby w razie NIEZBÊDNEJ potrzeby mo¿na by³o cos do niego dopisaæ
ofstream StatLog;
bool StatLogPoprawny=false; //Informacja ¿e ju¿ mo¿na u¿ywaæ StatLog
bool StatLogExists = false;
string StatLogPath;

// Wyliczenie pe³nej statystyki modelu w danym kroku
///////////////////////////////////////////////////////////////////////////////

void InitStatLog() {
	string NazwaPliku = PlikWejsciowy;

	time_t rawtime;
	struct tm * timeinfo;
	char buffer [80];
	
	time( &rawtime );
	timeinfo = localtime( &rawtime );
	strftime(buffer,80," %d-%m-%Y %H.%M.%S",timeinfo);

	int LastSlash = NazwaPliku.length() - 1;
	int DotPos = NazwaPliku.length() - 1;

	while (LastSlash >= 0) {
		if (((char)NazwaPliku[LastSlash]) == ((char)'\\')) {
			break;
		}
		else {
			LastSlash--;
		}
	}

	while (DotPos >= 0) {
		if (((char)NazwaPliku[DotPos]) == ((char)'.')) {
			break;
		}
		else {
			DotPos--;
		}
	}

	if (DotPos > LastSlash) {
		NazwaPliku = NazwaPliku.substr(LastSlash + 1, DotPos - LastSlash - 1);
	}
	else {
		NazwaPliku = NazwaPliku.substr(LastSlash + 1, NazwaPliku.length() - LastSlash);
	}

	StatLogPath = "WynikiOpiTT";
	system(("mkdir "+StatLogPath).c_str());
    StatLogPath += "/";
    StatLogPath += NazwaPliku;
    StatLogPath += buffer;
    StatLogPath += ".sdat";

    StatLog.open(StatLogPath.c_str());
		
	StatLog << "NrKroku\t";

	StatLog << "AdminLink\t";
	StatLog << "CoopLink\t";
	StatLog << "DirGenLink\t";
	StatLog << "DirParaLink\t";
	StatLog << "GenLink\t";
	StatLog << "ParaLink\t";
	StatLog << "SocialLink\t";

	StatLog << "BOTTNode\t";
	StatLog << "BusinessNode\t";
	StatLog << "GenNode\t";
	StatLog << "InforNode\t";
	StatLog << "MarketNode\t";
	StatLog << "MattNode\t";
	StatLog << "ResAdminNode\t";
	StatLog << "ResearchUNode\t";
	StatLog << "UOTTNode\t";

	StatLog << "FinSourceProc\t";
	StatLog << "GenProc\t";
	StatLog << "ProductProc\t";
	StatLog << "ReportProc\t";
	StatLog << "ResearchProc\t";
	StatLog << "SocialProc\t";
	StatLog << "TTransferProc\t";
	StatLog << "TTSearchProc\t";
		
	StatLog << "FinancInfo\t";
	StatLog << "GenInfo\t";
	StatLog << "OficialInfo\t";
	StatLog << "PackInfo\t";
	StatLog << "SocialInfo\t";

	StatLog << "WezlyMaxWaga\t";
	StatLog << "MaxWagaWezla\t";

	StatLog << "PolaczeniaMaxWaga\t";
	StatLog << "MaxWagaPolaczenia\t";

	for (int i = 0; i < STAT_HISTOGRAM_WAZNOSCI - 1; ++i) {
		StatLog << "WH <" << i << "," << (i + 1) << ")\t";
	}
	StatLog << "WH <" << (STAT_HISTOGRAM_WAZNOSCI - 1) << ",INFTY)\t";
	
	for (int i = 0; i < STAT_HISTOGRAM_WAZNOSCI - 1; ++i) {
		StatLog << "LH <" << ((float)i/(float)STAT_HISTOGRAM_WAZNOSCI) << "," << ((float)(i + 1)/(float)STAT_HISTOGRAM_WAZNOSCI) << ")\t";
	}
	StatLog << "LH <" << ((float)(STAT_HISTOGRAM_WAZNOSCI - 1)/(float)STAT_HISTOGRAM_WAZNOSCI) << ",INFTY)\t";

	StatLog << "MaxPolaczeniaWezla\t";
	StatLog << "WezlyZMaxPolaczen\t";
	
	StatLog << "MaxProcesyWezla\t";
	StatLog << "WezlyZMaxProcesow\t";

	StatLog << "MaxKomunikatyAdminLink\t";
	StatLog << "AdminLinkZMaxKomunikatow\t";
	StatLog << "MaxKomunikatyCoopLink\t";
	StatLog << "CoopLinkZMaxKomunikatow\t";
	StatLog << "MaxKomunikatyDirGenLink\t";
	StatLog << "DirGenLinkZMaxKomunikatow\t";
	StatLog << "MaxKomunikatyDirParaLink\t";
	StatLog << "DirParaLinkZMaxKomunikatow\t";
	StatLog << "MaxKomunikatyGenLink\t";
	StatLog << "GenLinkZMaxKomunikatow\t";
	StatLog << "MaxKomunikatyParaLink\t";
	StatLog << "ParaLinkZMaxKomunikatow\t";
	StatLog << "MaxKomunikatySocialLink\t";
	StatLog << "SocialLinkZMaxKomunikatow\t";

	StatLog << "\n";

	StatLog.close();

	StatLogExists = true;
	StatLogPoprawny = true;
}

string NazwaPowiazania(Powiazanie * ActPowiazanie) {
	string Res = Swiat::Wezel(ActPowiazanie->Poczatek())->Nazwa();
	Res += "<->";
	Res += Swiat::Wezel(ActPowiazanie->Koniec())->Nazwa();

	return Res;
}

int PozycjaWHistogramie(double Wartosc, double MaxWartosc, int LiczbaPrzedzialow) {
	if (MaxWartosc == 0) {
		return 0;
	}

	if (MaxWartosc <= Wartosc) {
		return (LiczbaPrzedzialow - 1);
	}

	return floor(((Wartosc/MaxWartosc) * (double)LiczbaPrzedzialow));
}

bool Swiat::Statystyka_po_kroku()
// Zainicjowanie StatLogu plikiem w katalogu ./WynikiOpiTT											X DONE
// który trzeba utworzyæ jak go nie ma.																X DONE
// Najlepiej jakby plik nazywa³ siê tak jak ¿ród³o danych tyle ¿e
// z unikalnym dopiskiem utworzonym z daty i czasu. Rozszerzenie: .sdat
//-----------------------------------------------------------------------------
//Plik TAB DELIMITED z nag³ówkiem,
//* W pierwszej kolumnie numer kroku.																X DONE
//NAJWA¯NIEJSZE:																					X DONE
//* Kolejne kolumny z liczb¹ obiektów ka¿dego typu znalezionego na liscie							X DONE
//    ElementModelu::WirtualnyKonstruktor::ListaNazwTypow()											X DONE
//Uporz¹dkowana kategoriami "*Node", "*Proc", "*Link", "*Info"										X DONE
//Pewnie trzeba bêdzie u¿yæ jakiejœ "mapy" (z STL), co niestety mo¿e byæ kosztowne					X DONE
//ale statystyki liczy siê nie zawsze w ka¿dym kroku, a zliczanie konstrukcji						X DONE
//i destrukcji musia³oby byæ zawsze.																X DONE

//JAK SIÊ UDA ZROBIÆ EFEKTYWNIE W OPARCIU O AKTUALNY INTERFAC										X DONE
//* Nazwa wêz³a o najwiêkszej liczbie po³¹czeñ (albo kilku oddzielonych œrednikami jak równe)		X DONE
//* i ta liczba																						X DONE
//* i nazwa wêz³a najwiêkszej liczbie procesów (j.w.)												X DONE
//* i ta liczba																						X DONE
//* link o najwiêkszej liczbie komunikatów (identyfikowany przez nazwê pocz¹tku i konca)			X DONE
//* i ta liczba																						X DONE
//(te¿ potrzebne "mapy" albo du¿e tablice "równoleg³e" do ukrytych tablic Swiata					X DONE

//TO SIÊ DA DOŒÆ £ATWO:
//* Nazwa wêz³a o najwiêkszej wadze																	X DONE
//* i ta waga																						X DONE
//* link o najwyszej wadze i ta waga																X DONE
//** A potem osobno dla wêz³ów i dla linków histogramy Waznosc()'i w zadanej liczbie przedzia³ow	X DONE
{
	map<string,int> ItemsCounts;
	
	int * PolaczeniaWezlow = (int*) malloc(IleWezlow() * sizeof(int));
	memset(PolaczeniaWezlow, 0, IleWezlow() * sizeof(int));
	int * ProcesyWezlow = (int*) malloc(IleWezlow() * sizeof(int));
	memset(ProcesyWezlow, 0, IleWezlow() * sizeof(int));
	int * KomunikatyPolaczen = (int*) malloc(IlePowiazan() * sizeof(int));
	memset(KomunikatyPolaczen, 0, IlePowiazan() * sizeof(int));
	list<string> WezlyZMaxPolaczen;
	list<string>::iterator WezlyZMaxPolaczenIt;
	int MaxPolaczeniaWezla = 0;
	list<string> WezlyZMaxProcesow;
	list<string>::iterator WezlyZMaxProcesowIt;
	int MaxProcesyWezla = 0;

	map<string,list<string> > PolaczeniaZMaxKomunikatow;
	list<string>::iterator PolaczeniaZMaxKomunikatowIt;

	map<string,int> MaxKomunikatyPolaczenia;
	
	WezelSieci* ActWezel;
	Proces* ActProces;
	Powiazanie* ActPowiazanie;
	Komunikat* ActKomunikat;

	list<string> MaxWezly;
	list<string>::iterator MaxWezlyIt;
	double MaxWagaWezla = 0;

	list<string> MaxPowiazania;
	list<string>::iterator MaxPowiazaniaIt;
	double MaxWagaPowiazania = 0;

	int* WezlyHist = (int*) malloc(STAT_HISTOGRAM_WAZNOSCI * sizeof(int));
	int* PowiazaniaHist = (int*) malloc(STAT_HISTOGRAM_WAZNOSCI * sizeof(int));

	if (!StatLogExists) {
		InitStatLog();
	}
	
	ItemsCounts.insert( pair<string,int>("AdminLink",0) );
	ItemsCounts.insert( pair<string,int>("CoopLink",0) );
	ItemsCounts.insert( pair<string,int>("DirGenLink",0) );
	ItemsCounts.insert( pair<string,int>("DirParaLink",0) );
	ItemsCounts.insert( pair<string,int>("GenLink",0) );
	ItemsCounts.insert( pair<string,int>("ParaLink",0) );
	ItemsCounts.insert( pair<string,int>("SocialLink",0) );

	ItemsCounts.insert( pair<string,int>("BOTTNode\t",0) );
	ItemsCounts.insert( pair<string,int>("BusinessNode\t",0) );
	ItemsCounts.insert( pair<string,int>("GenNode\t",0) );
	ItemsCounts.insert( pair<string,int>("InforNode\t",0) );
	ItemsCounts.insert( pair<string,int>("MarketNode\t",0) );
	ItemsCounts.insert( pair<string,int>("MattNode\t",0) );
	ItemsCounts.insert( pair<string,int>("ResAdminNode\t",0) );
	ItemsCounts.insert( pair<string,int>("ResearchUNode\t",0) );
	ItemsCounts.insert( pair<string,int>("UOTTNode\t",0) );
	
	ItemsCounts.insert( pair<string,int>("FinSourceProc\t",0) );
	ItemsCounts.insert( pair<string,int>("GenProc\t",0) );
	ItemsCounts.insert( pair<string,int>("ProductProc\t",0) );
	ItemsCounts.insert( pair<string,int>("ReportProc\t",0) );
	ItemsCounts.insert( pair<string,int>("ResearchProc\t",0) );
	ItemsCounts.insert( pair<string,int>("SocialProc\t",0) );
	ItemsCounts.insert( pair<string,int>("TTransferProc\t",0) );
	ItemsCounts.insert( pair<string,int>("TTSearchProc\t",0) );
		
	ItemsCounts.insert( pair<string,int>("FinancInfo",0) );
	ItemsCounts.insert( pair<string,int>("GenInfo",0) );
	ItemsCounts.insert( pair<string,int>("OficialInfo",0) );
	ItemsCounts.insert( pair<string,int>("PackInfo",0) );
	ItemsCounts.insert( pair<string,int>("SocialInfo",0) );

	for (int i = 0; i < IleWezlow(); ++i){
		ActWezel = Wezel(i);

		if (ActWezel != NULL) {
			ItemsCounts[ActWezel->VKonstruktor()->NazwaTypu()]++;

			if (ActWezel->Waznosc() > MaxWagaWezla) {
				MaxWezly.clear();
				MaxWezly.push_back(ActWezel->Nazwa());
				MaxWagaWezla = ActWezel->Waznosc();
			}
			else if (ActWezel->Waznosc() == MaxWagaWezla) {
				MaxWezly.push_back(ActWezel->Nazwa());
			}

			for (int j = 0; j < IleProcesow(i); ++j){
				ActProces = Proc(j,i);

				if (ActProces != NULL) {
					ItemsCounts[ActProces->VKonstruktor()->NazwaTypu()]++;
				}
			}

			ProcesyWezlow[i] = IleProcesow(i);
		}
	}
			
	for (int i = 0; i < IlePowiazan(); ++i){
		ActPowiazanie = Lacze(i);

		if (ActPowiazanie != NULL) {
			ItemsCounts[ActPowiazanie->VKonstruktor()->NazwaTypu()]++;
			
			if (ActPowiazanie->Waznosc() > MaxWagaPowiazania) {
				MaxPowiazania.clear();
				MaxPowiazania.push_back(NazwaPowiazania(ActPowiazanie));
				MaxWagaPowiazania = ActPowiazanie->Waznosc();
			}
			else if (ActPowiazanie->Waznosc() == MaxWagaPowiazania) {
				MaxPowiazania.push_back(NazwaPowiazania(ActPowiazanie));
			}

			PolaczeniaWezlow[ActPowiazanie->Poczatek()]++;
			PolaczeniaWezlow[ActPowiazanie->Koniec()]++;
		}
	}

	for (int i = 0; i < IleInformacji(); ++i){
		ActKomunikat = Info(i);

		if (ActKomunikat != NULL) {
			ItemsCounts[ActKomunikat->VKonstruktor()->NazwaTypu()]++;

			KomunikatyPolaczen[ActKomunikat->Kanal()]++;
		}
	}

	//Histogramy
	for (int i = 0; i < STAT_HISTOGRAM_WAZNOSCI; ++i) {
		WezlyHist[i] = 0;
		PowiazaniaHist[i] = 0;
	}

	for (int i = 0; i < IleWezlow(); ++i){
		ActWezel = Wezel(i);

		if (ActWezel != NULL) {
			WezlyHist[PozycjaWHistogramie(ActWezel->Waznosc(), MAX_WARTOSC_WEZLA, STAT_HISTOGRAM_WAZNOSCI)]++;
		}
	}
	
	for (int i = 0; i < IlePowiazan(); ++i){
		ActPowiazanie = Lacze(i);

		if (ActPowiazanie != NULL) {
			PowiazaniaHist[PozycjaWHistogramie(ActPowiazanie->Waznosc(), MAX_WARTOSC_POWIAZANIA, STAT_HISTOGRAM_WAZNOSCI)]++;
		}
	}

	//Dodatkowe statystyki
	for (int i = 0; i < IleWezlow(); ++i){
		if (Wezel(i) != NULL) {
			if (PolaczeniaWezlow[i] > MaxPolaczeniaWezla) {
				WezlyZMaxPolaczen.clear();
				WezlyZMaxPolaczen.push_back(Wezel(i)->Nazwa());
				MaxPolaczeniaWezla = PolaczeniaWezlow[i];
			}
			else if (PolaczeniaWezlow[i] == MaxPolaczeniaWezla) {
				WezlyZMaxPolaczen.push_back(Wezel(i)->Nazwa());
			}

			if (ProcesyWezlow[i] > MaxProcesyWezla) {
				WezlyZMaxProcesow.clear();
				WezlyZMaxProcesow.push_back(Wezel(i)->Nazwa());
				MaxProcesyWezla = ProcesyWezlow[i];
			}
			else if (ProcesyWezlow[i] == MaxProcesyWezla) {
				WezlyZMaxProcesow.push_back(Wezel(i)->Nazwa());
			}
		}
	}

	//Powoduje utworzenie w mapie odpowiednich elementów
	MaxKomunikatyPolaczenia["AdminLink"] = 0;
	MaxKomunikatyPolaczenia["CoopLink"] = 0;
	MaxKomunikatyPolaczenia["DirGenLink"] = 0;
	MaxKomunikatyPolaczenia["DirParaLink"] = 0;
	MaxKomunikatyPolaczenia["GenLink"] = 0;
	MaxKomunikatyPolaczenia["ParaLink"] = 0;
	MaxKomunikatyPolaczenia["SocialLink"] = 0;

	PolaczeniaZMaxKomunikatow["AdminLink"].clear();
	PolaczeniaZMaxKomunikatow["CoopLink"].clear();
	PolaczeniaZMaxKomunikatow["DirGenLink"].clear();
	PolaczeniaZMaxKomunikatow["DirParaLink"].clear();
	PolaczeniaZMaxKomunikatow["GenLink"].clear();
	PolaczeniaZMaxKomunikatow["ParaLink"].clear();
	PolaczeniaZMaxKomunikatow["SocialLink"].clear();
	
	for (int i = 0; i < IlePowiazan(); ++i){
		if (Lacze(i) != NULL) {
			if (KomunikatyPolaczen[i] > MaxKomunikatyPolaczenia[Lacze(i)->VKonstruktor()->NazwaTypu()]) {
				PolaczeniaZMaxKomunikatow[Lacze(i)->VKonstruktor()->NazwaTypu()].clear();
				PolaczeniaZMaxKomunikatow[Lacze(i)->VKonstruktor()->NazwaTypu()].push_back(NazwaPowiazania(Lacze(i)));
				MaxKomunikatyPolaczenia[Lacze(i)->VKonstruktor()->NazwaTypu()] = KomunikatyPolaczen[i];
			}
			else if (KomunikatyPolaczen[i] == MaxKomunikatyPolaczenia[Lacze(i)->VKonstruktor()->NazwaTypu()]) {
				PolaczeniaZMaxKomunikatow[Lacze(i)->VKonstruktor()->NazwaTypu()].push_back(NazwaPowiazania(Lacze(i)));
			}
		}
	}

	free(PolaczeniaWezlow);
	free(ProcesyWezlow);
	free(KomunikatyPolaczen);
	
	//if(COS NIE WYSZLO) return false;

	//... zapisy
	StatLog.open(StatLogPath.c_str(), ios::app);

	StatLog << NumerKroku() << "\t";

	StatLog << ItemsCounts["AdminLink"] << "\t";
	StatLog << ItemsCounts["CoopLink"] << "\t";
	StatLog << ItemsCounts["DirGenLink"] << "\t";
	StatLog << ItemsCounts["DirParaLink"] << "\t";
	StatLog << ItemsCounts["GenLink"] << "\t";
	StatLog << ItemsCounts["ParaLink"] << "\t";
	StatLog << ItemsCounts["SocialLink"] << "\t";

	StatLog << ItemsCounts["BOTTNode"] << "\t";
	StatLog << ItemsCounts["BusinessNode"] << "\t";
	StatLog << ItemsCounts["GenNode"] << "\t";
	StatLog << ItemsCounts["InforNode"] << "\t";
	StatLog << ItemsCounts["MarketNode"] << "\t";
	StatLog << ItemsCounts["MattNode"] << "\t";
	StatLog << ItemsCounts["ResAdminNode"] << "\t";
	StatLog << ItemsCounts["ResearchUNode"] << "\t";
	StatLog << ItemsCounts["UOTTNode"] << "\t";

	StatLog << ItemsCounts["FinSourceProc"] << "\t";
	StatLog << ItemsCounts["GenProc"] << "\t";
	StatLog << ItemsCounts["ProductProc"] << "\t";
	StatLog << ItemsCounts["ReportProc"] << "\t";
	StatLog << ItemsCounts["ResearchProc"] << "\t";
	StatLog << ItemsCounts["SocialProc"] << "\t";
	StatLog << ItemsCounts["TTransferProc"] << "\t";
	StatLog << ItemsCounts["TTSearchProc"] << "\t";
		
	StatLog << ItemsCounts["FinancInfo"] << "\t";
	StatLog << ItemsCounts["GenInfo"] << "\t";
	StatLog << ItemsCounts["OficialInfo"] << "\t";
	StatLog << ItemsCounts["PackInfo"] << "\t";
	StatLog << ItemsCounts["SocialInfo"] << "\t";

	for (MaxWezlyIt=MaxWezly.begin() ; MaxWezlyIt != MaxWezly.end(); MaxWezlyIt++ ) {
		StatLog << *MaxWezlyIt << ";";
	}
	StatLog << "\t";
	StatLog << MaxWagaWezla << "\t";

	for (MaxPowiazaniaIt=MaxPowiazania.begin() ; MaxPowiazaniaIt != MaxPowiazania.end(); MaxPowiazaniaIt++ ) {
		StatLog << *MaxPowiazaniaIt << ";";
	}
	StatLog << "\t";
	StatLog << MaxWagaPowiazania << "\t";

	for (int i = 0; i < STAT_HISTOGRAM_WAZNOSCI; ++i) {
		StatLog << WezlyHist[i] << "\t";
	}

	for (int i = 0; i < STAT_HISTOGRAM_WAZNOSCI; ++i) {
		StatLog << PowiazaniaHist[i] << "\t";
	}

	StatLog << MaxPolaczeniaWezla << "\t";
	for (WezlyZMaxPolaczenIt=WezlyZMaxPolaczen.begin() ; WezlyZMaxPolaczenIt != WezlyZMaxPolaczen.end(); WezlyZMaxPolaczenIt++ ) {
		StatLog << *WezlyZMaxPolaczenIt << ";";
	}
	StatLog << "\t";
	
	StatLog << MaxProcesyWezla << "\t";
	for (WezlyZMaxProcesowIt=WezlyZMaxProcesow.begin() ; WezlyZMaxProcesowIt != WezlyZMaxProcesow.end(); WezlyZMaxProcesowIt++ ) {
		StatLog << *WezlyZMaxProcesowIt << ";";
	}
	StatLog << "\t";

	StatLog << MaxKomunikatyPolaczenia["AdminLink"] << "\t";
	if (MaxKomunikatyPolaczenia["AdminLink"] > 0) {
		for (PolaczeniaZMaxKomunikatowIt=PolaczeniaZMaxKomunikatow["AdminLink"].begin() ; PolaczeniaZMaxKomunikatowIt != PolaczeniaZMaxKomunikatow["AdminLink"].end(); PolaczeniaZMaxKomunikatowIt++ ) {
			StatLog << *PolaczeniaZMaxKomunikatowIt << ";";
		}
		StatLog << "\t";
	}
	else {
		StatLog << "<MAX=0>\t";
	}

	StatLog << MaxKomunikatyPolaczenia["CoopLink"] << "\t";
	if (MaxKomunikatyPolaczenia["CoopLink"] > 0) {
		for (PolaczeniaZMaxKomunikatowIt=PolaczeniaZMaxKomunikatow["CoopLink"].begin() ; PolaczeniaZMaxKomunikatowIt != PolaczeniaZMaxKomunikatow["CoopLink"].end(); PolaczeniaZMaxKomunikatowIt++ ) {
			StatLog << *PolaczeniaZMaxKomunikatowIt << ";";
		}
		StatLog << "\t";
	}
	else {
		StatLog << "<MAX=0>\t";
	}

	StatLog << MaxKomunikatyPolaczenia["DirGenLink"] << "\t";
	if (MaxKomunikatyPolaczenia["DirGenLink"] > 0) {
		for (PolaczeniaZMaxKomunikatowIt=PolaczeniaZMaxKomunikatow["DirGenLink"].begin() ; PolaczeniaZMaxKomunikatowIt != PolaczeniaZMaxKomunikatow["DirGenLink"].end(); PolaczeniaZMaxKomunikatowIt++ ) {
			StatLog << *PolaczeniaZMaxKomunikatowIt << ";";
		}
		StatLog << "\t";
	}
	else {
		StatLog << "<MAX=0>\t";
	}
	
	StatLog << MaxKomunikatyPolaczenia["DirParaLink"] << "\t";
	if (MaxKomunikatyPolaczenia["DirParaLink"] > 0) {
		for (PolaczeniaZMaxKomunikatowIt=PolaczeniaZMaxKomunikatow["DirParaLink"].begin() ; PolaczeniaZMaxKomunikatowIt != PolaczeniaZMaxKomunikatow["DirParaLink"].end(); PolaczeniaZMaxKomunikatowIt++ ) {
			StatLog << *PolaczeniaZMaxKomunikatowIt << ";";
		}
		StatLog << "\t";
	}
	else {
		StatLog << "<MAX=0>\t";
	}

	StatLog << MaxKomunikatyPolaczenia["GenLink"] << "\t";
	if (MaxKomunikatyPolaczenia["GenLink"] > 0) {
		for (PolaczeniaZMaxKomunikatowIt=PolaczeniaZMaxKomunikatow["GenLink"].begin() ; PolaczeniaZMaxKomunikatowIt != PolaczeniaZMaxKomunikatow["GenLink"].end(); PolaczeniaZMaxKomunikatowIt++ ) {
			StatLog << *PolaczeniaZMaxKomunikatowIt << ";";
		}
		StatLog << "\t";
	}
	else {
		StatLog << "<MAX=0>\t";
	}

	StatLog << MaxKomunikatyPolaczenia["ParaLink"] << "\t";
	if (MaxKomunikatyPolaczenia["ParaLink"] > 0) {
		for (PolaczeniaZMaxKomunikatowIt=PolaczeniaZMaxKomunikatow["ParaLink"].begin() ; PolaczeniaZMaxKomunikatowIt != PolaczeniaZMaxKomunikatow["ParaLink"].end(); PolaczeniaZMaxKomunikatowIt++ ) {
			StatLog << *PolaczeniaZMaxKomunikatowIt << ";";
		}
		StatLog << "\t";
	}
	else {
		StatLog << "<MAX=0>\t";
	}

	StatLog << MaxKomunikatyPolaczenia["SocialLink"] << "\t";
	if (MaxKomunikatyPolaczenia["SocialLink"] > 0) {
		for (PolaczeniaZMaxKomunikatowIt=PolaczeniaZMaxKomunikatow["SocialLink"].begin() ; PolaczeniaZMaxKomunikatowIt != PolaczeniaZMaxKomunikatow["SocialLink"].end(); PolaczeniaZMaxKomunikatowIt++ ) {
			StatLog << *PolaczeniaZMaxKomunikatowIt << ";";
		}
		StatLog << "\t";
	}
	else {
		StatLog << "<MAX=0>\t";
	}
			
	StatLog << "\n";

	StatLog.flush(); //NA WYPADEK AWARII PROGRAMU

	StatLog.close();

	free(WezlyHist);
	free(PowiazaniaHist);

	return true;
}


/********************************************************************/
/*			          SPS/OPI  version 2011                         */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://wwww.iss.uw.edu.pl/borkowski/                */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/

