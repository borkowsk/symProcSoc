////////////////////////////////////////////////////////////////////////////////
// Symulator Procesów Sieciowych/Spolecznych (c) Instytut Studiów Spo³ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
////////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
////////////////////////////////////////////////////////////////////////////////
//
// Funkcje inicjalizacja struktur danych Œwiata  na podstawie pliku
// - wyodrêbnione ¿eby by³o ³atwo znaleŸæ i rozbudowywaæ
////////////////////////////////////////////////////////////////////////////////

#include "spsModel.h"
#include "spsGenNode.h"
#include "spsGenLink.h"
#include "spsGenInfo.h"

#include "INCLUDE/platform.hpp"
#include "MISCCLASSES/Wieloboki.hpp"
#include "MISCCLASSES/TabelaTabDeli.h"

#include <iostream>
#include <cassert>
#include <cstring>
using namespace std;

static TabelaTabDelimited DaneStruktury;//Dane struktury
										//istniej¹ ca³y czas, choæ bezpoœrednio
										//nie s¹ dostêpne


bool Swiat::Inicjalizacja(const char* PlikWejsciowy,const char DelimiterDanych)
// Wczytanie (i/lub wylosowanie ???) struktury modelu
{   //WCZYTANIE  Z PLIKU
	DaneStruktury.Opisowo=VerboseInput; //Testownie danych struktury przy wczytywaniu i wypis na ekran
	bool Udane=DaneStruktury.WczytajZPliku(PlikWejsciowy,DelimiterDanych);//char PlikWejsciowy[] - Wejœciowy plik struktury (dat)
																//char DelimiterDanych - Jaki znak s³u¿y do rozdzielania danych w pliku DAT (mo¿e byæ \t : ; , | \ / itp
	if(!Udane) {cerr<<endl<<_LPL("Nieudane wczytywanie z pliku","Failed to read the file")<<" "<<PlikWejsciowy<<endl; return false;}
	unsigned Wierszy=DaneStruktury.IleWierszy();  //Liczba wierszy bêdzie jeszcze wiele razy potrzebna
	unsigned Kolumn=DaneStruktury.IleKolumn();	  //Liczba kolumn te¿ pewnie kilka razy...
	//SPRAWDZENIE ILE JEST WEZ£ÓW  A ILE LINKÓW
	unsigned IleWezlow=0;
	unsigned IleLinkow=0;
	unsigned IleInform=0;

	for(unsigned i=0;i<Wierszy;i++)
	{
		const char* Nazwa=DaneStruktury(i,0).c_str();
		while(isspace(*Nazwa)) Nazwa++; //SKIP BLANKS!!!
		if(Nazwa!=DaneStruktury(i,0).c_str()) //Byly spacje
			   DaneStruktury(i,0)=Nazwa; //Podmieniamy, ¿eby dalej nie by³o tego k³opotu
		if(Nazwa[0]=='#')  //SKIP ALL COMMENTS HERE!
				continue;

		if(strstr(Nazwa,"node") || strstr(Nazwa,"NODE") || strstr(Nazwa,"Node"))
			IleWezlow++;
		if(strstr(Nazwa,"link") || strstr(Nazwa,"LINK") || strstr(Nazwa,"Link"))
			IleLinkow++;
		if(strstr(Nazwa,"info") || strstr(Nazwa,"INFO") || strstr(Nazwa,"Info"))
			IleInform++;
	}

	//USTALANIE ROZMIAROW STRUKTURY
	//unsigned DefMaxLiczbaWezlow=2;  //Liczby od 1 do 10 oznaczaj¹ wielokrotnoœæ liczby pocz¹tkowej...
	//unsigned DefMaxLiczbaLinkow=-1;  //wiêksze s¹ brane literalnie. -1 dla linków oznacza graf pe³ny
	//unsigned MaxLiczbaKomunikatow=10*1024; //Ile mo¿e jednoczesnie istniec komunikatów w systemie
	if(0 < DefMaxLiczbaWezlow && DefMaxLiczbaWezlow<=10)
		Swiat::_UstalLiczbeWezlow(DefMaxLiczbaWezlow*IleWezlow);
		else
		Swiat::_UstalLiczbeWezlow(DefMaxLiczbaWezlow);
	if(0 < DefMaxLiczbaLinkow && DefMaxLiczbaLinkow<=10)
		Swiat::_UstalLiczbeLaczy(DefMaxLiczbaLinkow*IleLinkow);
		else
		{
			if(DefMaxLiczbaLinkow==unsigned(-1))
			  DefMaxLiczbaLinkow=(Swiat::IleMoznaWezlow()*(Swiat::IleMoznaWezlow()-1))/2;
			Swiat::_UstalLiczbeLaczy(DefMaxLiczbaLinkow);
		}
	Swiat::_UstalLiczbeInfo(DefMaxLiczbaKomunikatow);
	Swiat::_UstalPoczLiczbeProc(DefLiczbaProcesowNaWezel);
	if(Swiat::IleMoznaWezlow()<IleWezlow)
		{cerr<<endl<<"Tablica wezlow za krotka. Za malo slotow o "<<IleWezlow-Swiat::IleMoznaWezlow()<<endl;return false;}
	if(Swiat::IleMoznaPowiazan()<IleLinkow)
		{cerr<<endl<<"Tablica powiazan za krotka. Za malo slotow o "<<IleLinkow-Swiat::IleMoznaPowiazan()<<endl;return false;}
	if(Swiat::IleMoznaInformacji()<IleInform)
		{cerr<<endl<<"Tablica komunikatow za krotka. Za malo slotow o "<<IleInform-Swiat::IleMoznaInformacji()<<endl;return false;}

	//WLASCIWE TWORZENIE STRUKTURY
	//...Na razie uproszczone bo s¹ tylko elementy typu "generic"
	const char* Lang=_LPL("pl","en");
	for(unsigned i=0;i<Wierszy;i++)
	{
		string* NazwaTypu=&DaneStruktury(i,0);
		if(strlen(NazwaTypu->c_str())<1) //Pusta komórka typu!!!
		{
			cerr<<_LPL("Pusta komórka typu. Kolumna A, wiersz ","Empty type definition. Column A, row ")<<i+1<<endl;
			continue;
		}
		RECOVERY:
		if(*(NazwaTypu->c_str())=='#')  //Komentarz, #config lub definicja nazw pól
		{
			//Parametr konfiguracyjny
			if(stricmp("#CONFIG",NazwaTypu->c_str())==0)
			{
				konfiguruj( DaneStruktury(i,1).c_str(),DaneStruktury(i,2).c_str());
				continue;
			}

			//Definicja nazw pól dla jakiegoœ typu
			const char* pom;
			if((pom=strchr(NazwaTypu->c_str(),'*'))!=NULL)
			 if(*(pom-1)=='#' || strncmp(pom-2,Lang,2)==0 //*(pom-2)==*Lang
			 )
			 {//JEST DEFINICJA
			  pom++;//Jaki typ?
			  ElementModelu::WirtualnyKonstruktor* Kon=ElementModelu::WirtualnyKonstruktor::DajWirtualnyKonstruktor(pom);
			  if(Kon)
			  {
				if(Kon->IleNazwPol()>0 && strcmp(Kon->NazwaPola(1),"<DEF>")!=0) //Ju¿ coœ by³o
					cerr<<_LPL("Powtórzona definicja nazw kolumn. Wiersz:",
							   "Field names already defined. Row:")<<i+1
							   <<_LPL(" Typ:"," Type:")<<"\""<<NazwaTypu->c_str()<<'"'<<endl;
				Kon->Nazwy.InicjujWgListy(NazwaTypu,Kolumn);
			  }
			  else
			  {
			  		cerr<<endl<<i+1<<_LPL(". wiersz","-th row")<<": "
					<<_LPL("Zignorowano nieznany typ w definicji pol dla ",
					   "Ignored unknown type in the head row for ")<<" "<<NazwaTypu->c_str()<<endl;
              }
			 }
		}
		else
		{
			ElementModelu::WirtualnyKonstruktor* Kon=ElementModelu::WirtualnyKonstruktor::DajWirtualnyKonstruktor(NazwaTypu->c_str());
			if(Kon!=NULL) //Jest taki typ
			{
				unsigned GdzieBlad=0; WezelSieci* PomW; Proces* PomPr; Powiazanie* PomL; Komunikat* PomI;
				ElementModelu* Pom=Kon->KonstruktorWgListy(NazwaTypu,Kolumn,GdzieBlad);
				if(Pom==NULL) //Jakiœ b³¹d
				{
					cerr<<_LPL("Niepoprawna komorka danych. Wiersz:",
							   "Invalid cell. Row:")
						<<i+1<<_LPL(" kolumna:"," column")<<char('A'+GdzieBlad)
						<<_LPL(" Typ:"," Type:")<<"\""<<NazwaTypu->c_str()<<'"'<<endl;
					cerr<<'"'<<DaneStruktury(i,GdzieBlad)<<'"'<<endl;
					return false;
				}
				else
				if((PomW=dynamic_cast<WezelSieci*>(Pom))!=NULL)
				   WstawWezel(PomW);
				   else
				   if((PomL=dynamic_cast<Powiazanie*>(Pom))!=NULL)
						WstawLacze(PomL);
						else
						if((PomPr=dynamic_cast<Proces*>(Pom))!=NULL)
							Swiat::WstawProc(PomPr,PomPr->Procesor());
						else
						if((PomI=dynamic_cast<Komunikat*>(Pom))!=NULL)
							 WstawInfo(PomI);
							 else
							 {   //Problem - nie da³o siê zrzutowaæ!!!
								cerr<<_LPL(" Typ:"," Type:")<<NazwaTypu->c_str()
									<<" nie jest ani wezlem (node) ani powiazaniem (link)\n ani procesem(proc) ani komunikatem (info)"<<endl;
								cerr<<_LPL("Zastanie zignorowany","It will be ignored")<<endl;
								delete Pom;
							 }
			}
			else
			{
				const char* Nazwa=NazwaTypu->c_str();
				if(strcmp(Nazwa,"<UNKNOWN>")==0)
				{
						cerr<<_LPL("Prawdopodobnie plik w nieobslugiwanym formacie",
									"Probably unsuported file format")<<endl;
								return false;
				}
				cerr<<endl<<i+1<<_LPL(". wiersz.","-th row.")<<": "
				<<_LPL("Podmieniono nieznany typ",
					   "Replace unknown type")<<" \""<<Nazwa<<"\"";

				if(strstr(Nazwa,"node") || strstr(Nazwa,"NODE") || strstr(Nazwa,"Node"))
						*NazwaTypu="gennode";
						else
				if(strstr(Nazwa,"link") || strstr(Nazwa,"LINK") || strstr(Nazwa,"Link"))
						*NazwaTypu="genlink";
						else
				if(strstr(Nazwa,"info") || strstr(Nazwa,"INFO") || strstr(Nazwa,"Info"))
						*NazwaTypu="geninfo";
						else
				if(strstr(Nazwa,"proc") || strstr(Nazwa,"PROC") || strstr(Nazwa,"Proc"))
						*NazwaTypu="genproc";
						else
						*NazwaTypu="<UNKNOWN>";
				cerr<<" ==> "<<NazwaTypu->c_str()<<endl;
				goto RECOVERY;
			}
		}
	}
   //	cerr<<" OK"<<endl;
	Swiat::UwagaZmienionoStrukture(); //I tak wykonanie tej funkcji drugi raz w programie mo¿e zrobiæ kaszanê, ale moze kiedyœ ...
	return true;
}


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

	/*
		if(stricmp("GenNode",NazwaTypu->c_str())==0)
		{
		   GenerycznyWezelSieci* Pom=new GenerycznyWezelSieci;
		   unsigned GdzieBlad=0;
		   if(Pom->ZrobWgListy(NazwaTypu,Kolumn,GdzieBlad))
				Swiat::WstawWezel(Pom);
				else
				{
					cerr<<"Niepoprawna komorka danych: wiersz:"<<i+1<<" kolumna:"<<char('A'+GdzieBlad)<<endl;
					cerr<<'"'<<DaneStruktury(i,GdzieBlad)<<'"'<<endl;
					return false;
				}
		}
		else if(stricmp("GenLink",NazwaTypu->c_str())==0)
		{
		   GenerycznePowiazanie* Pom=new GenerycznePowiazanie;
		   unsigned GdzieBlad=0;
		   if(Pom->ZrobWgListy(NazwaTypu,Kolumn,GdzieBlad))
				Swiat::WstawLacze(Pom);
				else
				{
					cerr<<"Niepoprawna komorka danych: wiersz:"<<i+1<<" kolumna:"<<char('A'+GdzieBlad)<<endl;
					cerr<<'"'<<DaneStruktury(i,GdzieBlad)<<'"'<<endl;
					return false;
				}
		}
		else if(stricmp("GenInfo",NazwaTypu->c_str())==0)
		{
		   GeneryczneInfo*  Pom=new GeneryczneInfo;
		   unsigned GdzieBlad=0;
		   if(Pom->ZrobWgListy(NazwaTypu,Kolumn,GdzieBlad))
				Swiat::WstawInfo(Pom);
				else
				{
					cerr<<"Niepoprawna komorka danych: wiersz:"<<i+1<<" kolumna:"<<char('A'+GdzieBlad)<<endl;
					cerr<<'"'<<DaneStruktury(i,GdzieBlad)<<'"'<<endl;
					return false;
				}
		}
		else
		{
			cerr<<endl<<i<<_LPL(". wiersz","-th row")<<": "
				<<_LPL("Zignorowano nieznany typ","Ignored unknown type")<<" "<<NazwaTypu->c_str()<<endl;
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
