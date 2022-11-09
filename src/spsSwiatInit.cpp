/// \file
/// \brief Funkcje inicjalizacja struktur danych świata  na podstawie pliku
///        ----------------------------------------------------------------
///
/// \details
///      Wyodrębnione żeby było łatwo znaleźć i rozbudowywać
////
///      UŻYWAMY LINIOWEJ ORGANIZACJI KONTENERÓW, ALE NIELINIOWO JEJ UŻYWAMY.
///      (TODO) MOŻNA BY TO ZASTĄPIĆ JAKĄŚ INNĄ IMPLEMENTACJĄ
/// \note Cześć modelu może być w innych plikach, jak ten zanadto urośnie!
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
/// \date
///     2022.11.09 (last updated)
//*////////////////////////////////////////////////////////////////////////////////

#include "spsModel.h"
//#include "spsGenNode.h"
//#include "spsGenLink.h"
//#include "spsGenInfo.h"
//#include "wieloboki.hpp"
#include "TabDelimited.hpp"
#include "wb_smartlog.hpp"
#include "compatyb.h"
#include "lingo.hpp"

#include <iostream>
#include <cassert>
#include <cstring>
using namespace std;

/// Dane struktury.
/// Istnieją cały czas po wczytaniu, choć bezpośrednio nie są dostępne.
static wbrtm::TabelaTabDelimited DaneStruktury;
bool Swiat::ForceTolerant=0; ///< Wymuszenie tolerancji dla błędów w opisie struktury.

/// Wczytanie struktury modelu Z PLIKU.
/// TODO: Alternatywnie wylosowanie?
/// \note Wykonanie tej funkcji drugi raz w programie może zrobić kaszanę!!!
bool Swiat::Inicjalizacja(const char* PlikWejsciowy,const char DelimiterDanych)
{
	DaneStruktury.Opisowo=VerboseInput; //Testowanie danych struktury przy wczytywaniu i wypis na ekran

	TheApplicationLog.SetName("_LastExpDetailedLog.log");
    
    //char PlikWejsciowy[] - Wejściowy plik struktury (dat)
    //char DelimiterDanych - Jaki znak sxuxy do rozdzielania danych w pliku DAT (może byx \t : ; , | \ / itp
	bool Udane=DaneStruktury.WczytajZPliku(PlikWejsciowy,DelimiterDanych); 
    
    TLOG(0, <<"START "<<PlikWejsciowy )
	TLOG(0, <<"============================================================================================="   )
	if(!Udane) 
    {
        cerr<<endl<<_LPL("Nieudane wczytywanie z pliku","Failed to read the file")<<" "<<PlikWejsciowy<<endl;
        TLOG(0, <<_LPL("Nieudane wczytywanie z pliku","Failed to read the file")<<" "<<PlikWejsciowy )
        return false;
    }

	unsigned Wierszy=DaneStruktury.IleWierszy();  //Liczba wierszy będzie jeszcze wiele razy potrzebna
	unsigned Kolumn=DaneStruktury.IleKolumn();	  //Liczba kolumn też pewnie kilka razy...

	//SPRAWDZENIE ILE JEST WĘZŁÓW, A ILE LINKÓW
	unsigned IleWezlow=0;
	unsigned IleLinkow=0;
	unsigned IleInform=0;

	for(unsigned i=0;i<Wierszy;i++)
	{
		const char* Nazwa=DaneStruktury(i,0).c_str();
		while(isspace(*Nazwa)) Nazwa++; //SKIP BLANKS!!!
        
		if(Nazwa!=DaneStruktury(i,0).c_str()) //Były spacje
			   DaneStruktury(i,0)=Nazwa; //Podmieniamy, żeby dalej nie było tego kłopotu
		if(Nazwa[0]=='#')  //SKIP ALL COMMENTS HERE!
				continue;

		if( strstr(Nazwa,"node") || strstr(Nazwa,"NODE") || strstr(Nazwa,"Node"))
			IleWezlow++;
        
		if( strstr(Nazwa,"link") || strstr(Nazwa,"LINK") || strstr(Nazwa,"Link"))
			IleLinkow++;
        
		if( strstr(Nazwa,"info") || strstr(Nazwa,"INFO") || strstr(Nazwa,"Info"))
			IleInform++;
	}

	//USTALANIE ROZMIARÓW STRUKTURY
	//unsigned DefMaxLiczbaWezlow=2;  //Liczby od 1 do 10 oznaczajx wielokrotnoxx liczby początkowej...
	//unsigned DefMaxLiczbaLinkow=-1;  //większe sx brane literalnie. -1 dla linkxw oznacza graf pełny
	//unsigned MaxLiczbaKomunikatow=10*1024; //Ile moxe jednoczesnie istniec komunikatów w systemie?
	if(0 < DefMaxLiczbaWezlow && DefMaxLiczbaWezlow<=10)
		Swiat::_UstalLiczbeWezlow(DefMaxLiczbaWezlow*IleWezlow);
		else
		Swiat::_UstalLiczbeWezlow(DefMaxLiczbaWezlow);
        
	if(0 < DefMaxLiczbaLinkow && DefMaxLiczbaLinkow<=10)
		Swiat::_UstalLiczbeLaczy(DefMaxLiczbaLinkow*IleLinkow);
		else
		{
			if(DefMaxLiczbaLinkow==unsigned(-1))
			  DefMaxLiczbaLinkow=Swiat::IleMoznaWezlow()*Swiat::IleMoznaWezlow(); //Mogx byx różne rodzaje, lepiej z zapasem
			Swiat::_UstalLiczbeLaczy(DefMaxLiczbaLinkow);
		}
        
	Swiat::_UstalLiczbeInfo(DefMaxLiczbaKomunikatow);
	Swiat::_UstalPoczLiczbeProc(DefLiczbaProcesowNaWezel);

	if(Swiat::IleMoznaWezlow()<IleWezlow)
    {
         cerr<<endl<<"Tablica wezlow za krotka. Za malo slotow o "<<IleWezlow-Swiat::IleMoznaWezlow()<<endl;
		 TLOG(0, <<"Tablica wezlow za krotka. Za malo slotow o "<<IleWezlow-Swiat::IleMoznaWezlow()  )
         return false;
    }

	if(Swiat::IleMoznaPowiazan()<IleLinkow)
    {
         cerr<<endl<<"Tablica powiązań za krotka. Za malo slotow o "<<IleLinkow-Swiat::IleMoznaPowiazan()<<endl;
		 TLOG(0, << "Tablica powiązań za krotka. Za malo slotow o "<<IleLinkow-Swiat::IleMoznaPowiazan()   )
         return false;
    }

    if(Swiat::IleMoznaInformacji()<IleInform)
    {
         cerr<<endl<<"Tablica komunikatow za krotka. Za malo slotow o "<<IleInform-Swiat::IleMoznaInformacji()<<endl;
		 TLOG(0, <<"Tablica komunikatow za krotka. Za malo slotow o "<<IleInform-Swiat::IleMoznaInformacji()   )
         return false;
    }

	// WŁAŚCIWE TWORZENIE STRUKTURY
	//...Na razie uproszczone, bo sx tylko elementy typu "generic"
	const char* Lang=_LPL("pl","en");
	for(unsigned i=0;i<Wierszy;i++)
	{
		string* NazwaTypu=&DaneStruktury(i,0);
		if(strlen(NazwaTypu->c_str())<1) //Pusta komórka typu!!!
		{
			cerr<<_LPL("Pusta komórka typu. Kolumna A, wiersz ","Empty type definition. Column A, row ")<<i+1<<endl;
			TLOG(0, << _LPL("Pusta komórka typu. Kolumna A, wiersz ","Empty type definition. Column A, row ")<<i+1     )
			continue;
		}
		RECOVERY:
		if(*(NazwaTypu->c_str())=='#')  //Komentarz, #config lub definicja nazw pxl
		{
			//Parametr konfiguracyjny
			if(strcasecmp("#CONFIG",NazwaTypu->c_str())==0)
			{
				konfiguruj( DaneStruktury(i,1).c_str(),DaneStruktury(i,2).c_str());
				continue;
			}

			//Definicja nazw pól dla jakiegoś typu
			const char* pom;
			if((pom=strchr(NazwaTypu->c_str(),'*'))!=NULL)
			 if(*(pom-1)=='#' || strncmp(pom-2,Lang,2)==0 //*(pom-2)==*Lang
			 )
			 { //JEST DEFINICJA
                  pom++; //Jaki typ?
                  ElementModelu::WirtualnyKonstruktor* Kon=ElementModelu::WirtualnyKonstruktor::DajWirtualnyKonstruktor(pom);
                  if(Kon)
                  {
                    if(Kon->IleNazwPol()>0 && strcmp(Kon->NazwaPola(1),"<DEF>")!=0) //Jux cox byxo
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
				if(Pom==NULL) //Jakix bxxd
				{
					cerr<<_LPL("Niepoprawna komórka danych. Wiersz:","Invalid cell. Row:")<<i+1
                        <<_LPL(" kolumna:"," column")<<char('A'+GdzieBlad)<<_LPL(" Typ:"," Type:")
                        <<"\""<<NazwaTypu->c_str()<<'"'<<endl;
					TLOG(0, <<_LPL("Niepoprawna komórka danych. Wiersz:","Invalid cell. Row:")<<i+1
                            <<_LPL(" kolumna:"," column")<<char('A'+GdzieBlad)<<_LPL(" Typ:"," Type:")
                            <<"\""<<NazwaTypu->c_str()<<'"'  )
					cerr<<'"'<<DaneStruktury(i,GdzieBlad)<<'"'<<endl;
					if(!ForceTolerant)
						return false;
						else
						continue;
				}
				else
				{
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
							 {   //Problem: Nie dało six zrzutować!!!
								cerr<<_LPL(" Typ:"," Type:")<<NazwaTypu->c_str()
                                    <<" nie jest ani wezlem (node) ani powiązaniem (link)\n ani procesem(proc) ani komunikatem (info)"<<endl;
								TLOG( 0 , <<_LPL(" Typ:"," Type:")<<NazwaTypu->c_str()
                                    <<" nie jest ani wezlem (node) ani powiązaniem (link)\n ani procesem(proc) ani komunikatem (info)"  )
								cerr<<_LPL("Zastanie zignorowany","It will be ignored")<<endl;
								delete Pom;
							 }
				   if(!Pom->Poprawny())  //Ostateczne sprawdzenie
						{
							cerr<<i+1<<". wiersz. Obiekt \""<<NazwaTypu->c_str()
                                <<"\" jednak nie jest poprawny, sprawdź dozwolone zakresy wartości"<<endl;
							TLOG( 0 , <<i+1<<". wiersz. Obiekt \""<<NazwaTypu->c_str()<<"\" jednak nie jest poprawny" )
							if(!ForceTolerant)
								return false;
								else
								continue;
                        }
				}
			}
			else
			{
				const char* Nazwa=NazwaTypu->c_str();
				if(strcmp(Nazwa,"<UNKNOWN>")==0)
				{
					cerr<<_LPL("Prawdopodobnie plik w nieobslugiwanym formacie","Probably unsuported file format")<<endl;
					TLOG(0, <<_LPL("Prawdopodobnie plik w nieobslugiwanym formacie","Probably unsuported file format")   )
								return false;
				}

				cerr<<endl<<i+1<<_LPL(". wiersz.","-th row.")<<": "
                    <<_LPL("Podmieniono nieznany typ","Replace unknown type")<<" \""<<Nazwa<<"\"";
				TLOG(0, <<i+1<<_LPL(". wiersz.","-th row.")<<": "
                    <<_LPL("Podmieniono nieznany typ","Replace unknown type")<<" \""<<Nazwa<<"\""  )

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
    TLOG(0, <<" FILE +-OK "<<endl ) //KONIEC WCZYTYWANIA
	Swiat::UwagaZmienionoStrukture(); //I tak wykonanie tej funkcji drugi raz w programie może zrobić kaszanę!
	return true;
}

	/*   // JAKIŚ FRAGMENT STAREJ WERSJI
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
