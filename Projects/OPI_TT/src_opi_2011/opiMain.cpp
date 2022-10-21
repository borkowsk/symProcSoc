////////////////////////////////////////////////////////////////////////////////
//			Symulacja transferu technologii dla OPI
//------------------------------------------------------------------------------
//Plik main() dostosowany do u¿ytej najbardziej prymitywnej biblioteki graficznej
//Bez wodotrysków, ¿eby nie inwestowaæ zanadto w kosmetykê, która ju¿ mog¹ zrobiæ
//zawodowi informatycy.
////////////////////////////////////////////////////////////////////////////////
//POD Dev-Cpp potrzebne s¹ dwie bibloteki:
//".../Dev-Cpp/lib/libgdi32.a"
//".../Dev-Cpp/lib/libcomdlg32.a"

#include <iostream>
#include <cassert>
#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef MS_WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
	#include <unistd.h>
    #define GetCurrentDir getcwd
 #endif
using namespace std;

#include "INCLUDE/platform.hpp"
#ifndef LOCAL_VERSION
	unsigned wbrtm::_lingo_selector=0;
#else
	unsigned wbrtm::_lingo_selector=1;
#endif


#include "INCLUDE/wb_ptr.hpp"
#include "SYMSHELL/symshell.h"
#include "SYMSHELL/sshmenuf.h"
#include "MISCCLASSES/Wieloboki.hpp"
#include "INCLUDE/wb_smartlog.h"

#include "spsModel.h" //Najwa¿niejsze deklaracje konieczne do wspó³pracy modu³ów
#include "spsGenLink.h" //Ze wzglêdu na statyczne parametry wizualizacji linków

#include "res/opiresource.h"

bool ViewHtml(string URL); //Otwieranie

//extern int		wbrtm::wb_smartlog::log_level;//=0;   POZIOM LOGOWANIA PLIKU LOG U¯YWANEGO SPS

extern char PlikWejsciowy[256];//="ModelDefaults.dat"; //Wejœciowy plik struktury (dat)
extern char DelimiterDanych;//='\t'; //Jaki znak s³u¿y do rozdzielania danych w pliku DAT (mo¿e byæ te¿ : ; , | \ / itp

//Lokalne funkcje strukturalizuj¹ce proto-interfejs programu
bool obsluz_mouse_click(); //Jak true to trafiony i trzeba zastopowaæ symulacje
bool symuluj=false;
bool czyscciagle=true;
unsigned czekaj_ms=50;
unsigned co_ile_krokow_wizualizacja=1;

void MarkujWszystkieTypy();

void ZmienWidocznoscObiektu(const char* NazwaKlasy, unsigned int MenuId, ssh_menu_handle MainMenu)
{
	ElementModelu::WirtualnyKonstruktor* WK=
					ElementModelu::WirtualnyKonstruktor::DajWirtualnyKonstruktor(NazwaKlasy);
	unsigned FL=WK->Flagi();
	unsigned VF=FL & ElementModelu::WirtualnyKonstruktor::VFLAG;
	if(VF==3) VF=1; else VF=3;
	if(VF==3) ssh_menu_mark_item(MainMenu,1,MenuId);
	else  ssh_menu_mark_item(MainMenu,0,MenuId);
	FL&=~ElementModelu::WirtualnyKonstruktor::VFLAG; //Wyzerowanie czêœci wizualnej
	FL|=VF;		 //Naniesienie nowej czêœci wizyjnej
	WK->Flagi()=FL; //Zapisanie
	ssh_realize_menu(MainMenu);

	Swiat::Ten.Wymazuj();
	Swiat::Wizualizacja_po_kroku();
	flush_plot();
}

static wbrtm::wb_pchar SciezkaGlowna;
const char* SciezkaDoExe(const char* Par0)
{
  if(SciezkaGlowna)
			return  SciezkaGlowna.get();
  SciezkaGlowna=Par0;
  char* ptr=strrchr(SciezkaGlowna.get_ptr_val(),'/');
  if(ptr)
  {
	unsigned pos=ptr-SciezkaGlowna.get_ptr_val();
	SciezkaGlowna[pos+1]=0; //Odciecie za /
  }
  else
  if((ptr=strrchr(SciezkaGlowna.get_ptr_val(),'\\'))!=NULL)
  {
	unsigned pos=ptr-SciezkaGlowna.get_ptr_val();
	SciezkaGlowna[pos+1]=0; //Odciecie za backslashem
  }
  else
  {
	SciezkaGlowna.alloc(4);
	SciezkaGlowna[0]='.';  SciezkaGlowna[1]='/';
    SciezkaGlowna[2]=0;
  }
  return SciezkaGlowna.get();
}

/*     FUNKCJA MAIN     */
/************************/
int main(int argc,const char* argv[])
{
cout<<endl; //POCZ¥TEK "TABLICZKI ZNAMIONOWEJ" NA KONSOLI TEKSTOWEJ PROGRAMU
cout<<_LPL("      SYMULACJA TRANSFERU TECHNOLOGII       \n",
		   "       TECHNOLOGY TRANSFER SIMULATION       \n")
	<<_LPL("              wersja ISS 1.0b               \n",
		   "             version ISS 1.0b               \n");
cout<<     "============================================\n";
cout<<_LPL("    wykonana na zamowienie OPI przez        \n",
		   "           made for OPI by                  \n");
//cout<<_LPL("  Oœrodek Badania Uk³adów Z³o¿onych ISS UW  \n",
cout<<_LPL("  Osrodek Badania Ukladow Zlozonych ISS UW  \n",
		   "    Center for Complex Systems ISS UW       \n");
cout<<     "============================================\n";
//cout<<_LPL("    Programowa³ Wojciech Borkowski          \n",
cout<<_LPL("     Programowal Wojciech Borkowski          \n",
		   "     Programmed by Wojciech Borkowski        \n");
cout<<     "    www.iss.uw.edu.pl ,  www.opi.org.pl      \n"<<endl<<endl;

WB_error_enter_before_clean=0;
czytaj_parametry(argc,argv); //Odczytanie parametrów wywo³ania innych ni¿ zaczynaj¹ce siê od -

{//SPRAWDZANIE AKTUALNEGO KATALOGU
 char cCurrentPath[FILENAME_MAX];
 if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
 {
  perror("get current directory");
  goto FINAL;
  }
  else cerr<<_LPL("Aktualny katalog to","Current directory is")<<' '<<cCurrentPath<<endl;
}

cout<<_LPL("\nPreinstalowane typy elementow sieci:","\nPreregistered classes of network elements:")<<endl
			<<ElementModelu::WirtualnyKonstruktor::ListaNazwTypow()<<endl<<endl;

RANDOMIZE();  //Dla generatora liczb losowych

if(!Swiat::Inicjalizacja(PlikWejsciowy,DelimiterDanych))
			{ cerr<<endl<<_LPL("Nieudana inicjalizacja!","Initialisation failed!");goto FINAL;}

//Ustawienie parametrów okna wizualizacji i jego otwarcie
set_background(NumerKoloruTla);// Jaki kolor t³a
fix_size(0);       // Czy okno o sta³ym rozmiarze
buffering_setup(1);// Czy wlaczona animacja
mouse_activity(1);  // Czy potrzebna jest mysz

shell_setup(_LPL("TRANSFER TECHNOLOGII w SPS-Frame","TECHNOLOGY TRANSFER in SPS-Frame"),argc,argv); //Szuka parametrów symshella (tych zaczynaj¹cych siê od - )

if(!init_plot(hor,ver,0,0))
{
	cerr<<_LPL("Okno graficzne nie daje sie otworzyc",
			   "Can't initialize graphics")<<endl;
	goto FINAL;
}
else
{
	WB_error_enter_before_clean=1;  // Dla close_plot() - czy zamykac od razu czy ze "stopem"
	cout<<endl<<_LPL("Podreczne sterowanie z klawiatury:","")<<endl;
	cout<<_LPL("q,Q - zatrzymaj; a - start, p - pauza, ESC - zmiana\n",
			   "q,Q - for stop; a - start, p - pause, ESC - swith\n")<<endl;
    MarkujWszystkieTypy(); //Markery przy wszystkich typach w menu bo domyœlnie sa widoczne
}

//if(!testy_graficzne()) {cerr<<endl<<"Nieudane testy grafiki!";goto FINAL;}
//Tak naprawdê po inicjalizacji - czyli krok 0
if(!Swiat::Wizualizacja_po_kroku()){ cerr<<endl<<"Nieudana pierwsza wizualizacja!";goto FINAL;}
if(!Swiat::Statystyka_po_kroku())  { cerr<<endl<<"Nieudane pierwsze liczenie statystyk!";goto FINAL;}
flush_plot();

{
ssh_menu_handle MainMenu=ssh_main_menu(); // Daje uchwyt do g³ównego menu
int cont=1; //Ze wzglêdu na GOTO nie mo¿e byæ bez okreœlenia zasiêgu
while(cont)//Petla zdarzen na czas pracy algorytmu
{
	if(input_ready())
	{
		int  tab=get_char();
		switch(tab)
		{
		case  _WWW_OPI_ :             	//	40025
				ViewHtml("http://www.opi.org.pl/");
				break;
		case  _WWW_ISS_	:		  		//	40026
				ViewHtml("http://www.iss.uw.edu.pl/");
				break;
		case  _STRONA_	:		//MENUITEM "Strona programu",
				ViewHtml("http://borkowski.iss.uw.edu.pl/software/opiTransfer.html");
				break;
		case  _ABOUT_   :      	 		 //	40027       //MENUITEM "O programie"
				{
				string Sciezka=SciezkaDoExe(argv[0]);
				ViewHtml(Sciezka+"../doc/opiTransfer.mht");
				}
				break;
		case  _MANUAL_	:		//MENUITEM "Dokumentacja",
				{
				string Sciezka=SciezkaDoExe(argv[0]);
				ViewHtml(Sciezka+"../doc/Manual.mht");
				}
				break;
		case  _MANUAL_PDF_  :		//MENUITEM "Dokumentacja PDF",
				{
				string Sciezka=SciezkaDoExe(argv[0]);
				ViewHtml(Sciezka+"../doc/Manual.pdf");
				}
				break;
		case  _SPS_RUN_ :
		case 'a':symuluj=true;
			 ssh_menu_mark_item(MainMenu,symuluj,_SPS_RUN_);
			 ssh_realize_menu(MainMenu);
			break;
		PAUSE:
		case _SPS_PAUSE_ :
		case 'p':symuluj=false;
			 flush_plot();
			 ssh_menu_mark_item(MainMenu,symuluj,_SPS_RUN_);
			 ssh_realize_menu(MainMenu);
			 break;
		case _LOG_MORE_DET_:				//MENUITEM "Logowanie dok³adniejsze",
					//extern int		wbrtm::wb_smartlog::log_level;//=0;   POZIOM LOGOWANIA PLIKU LOG U¯YWANEGO SPS
					wbrtm::wb_smartlog::log_level++;
					cout<<endl<<_LPL("Domyslny poziom logowania zmieniony na","Default log level set to")<<' '<<dec<<wbrtm::wb_smartlog::log_level<<endl;
					TLOG( 0 , <<_LPL("!!! Domyslny poziom logowania zmieniony na","Default log level set to")<<' '<<dec<<wbrtm::wb_smartlog::log_level   )
					break;
		case _LOG_LESS_DET_:				//MENUITEM "Mniej dok³adne logowanie",
					wbrtm::wb_smartlog::log_level--;
					if(wbrtm::wb_smartlog::log_level<0)wbrtm::wb_smartlog::log_level=0;
					cout<<endl<<_LPL("Domyslny poziom logowania zmieniony na","Default log level set to")<<' '<<dec<<wbrtm::wb_smartlog::log_level<<endl;
					TLOG( 0 , <<_LPL("!!! Domyslny poziom logowania zmieniony na","Default log level set to")<<' '<<dec<<wbrtm::wb_smartlog::log_level   )
					break;
		case 'n':
		case _REFR_NODES_:      // 	40032       //MENUITEM "Refresh nodes"
					Swiat::WyswietlajWezly=!Swiat::WyswietlajWezly;
					ssh_menu_mark_item(MainMenu,Swiat::WyswietlajWezly,_REFR_NODES_);
					ssh_realize_menu(MainMenu);
					goto ODRYSOWANIE;break;
		case 'l':
		case _REFR_LINKS_:      // 	40033 		//MENUITEM "Refresh links"
					Swiat::WyswietlajLinki=!Swiat::WyswietlajLinki;
					ssh_menu_mark_item(MainMenu,Swiat::WyswietlajLinki,_REFR_LINKS_);
					ssh_realize_menu(MainMenu);
					goto ODRYSOWANIE;break;
		case 'm':
		case _REFR_MESSG_:      //	40034		//MENUITEM "Refresh messages"
					Swiat::WyswietlajInformacje=!Swiat::WyswietlajInformacje;
					ssh_menu_mark_item(MainMenu,Swiat::WyswietlajInformacje,_REFR_MESSG_);
					ssh_realize_menu(MainMenu);
					goto ODRYSOWANIE;break;
		case 'r':
		case _REFR_PROCS_:   	  //	40035		//MENUITEM "Refresh procesess"
					Swiat::WyswietlajProcesy=!Swiat::WyswietlajProcesy;
					ssh_menu_mark_item(MainMenu,Swiat::WyswietlajProcesy,_REFR_PROCS_);
					ssh_realize_menu(MainMenu);
					goto ODRYSOWANIE;break;
		case _THICK_LINKS_:     //  40037       //MENUITEM "Thicker links"
					GenerycznePowiazanie::MINIMALNA_GRUBOSC_LINKU*=5;
					GenerycznePowiazanie::MAKSYMALNA_GRUBOSC_LINKU*=1.05;
					goto ODRYSOWANIE;break;
		case _THIN_LINKS_:  	  //	40038		//MENUITEM "Thiner links"
					GenerycznePowiazanie::MINIMALNA_GRUBOSC_LINKU/=5;
					GenerycznePowiazanie::MAKSYMALNA_GRUBOSC_LINKU/=1.05;
					goto ODRYSOWANIE;break;
		case _SOCIAL_INFO_ONOFF:
					ZmienWidocznoscObiektu("SocialInfo", _SOCIAL_INFO_ONOFF, MainMenu);
					
					break;
		case _DIR_PARA_LINK_ONOFF:
					ZmienWidocznoscObiektu("DirParaLink", _DIR_PARA_LINK_ONOFF, MainMenu);

					break;
		case _PARA_LINK_ONOFF:
					ZmienWidocznoscObiektu("ParaLink", _PARA_LINK_ONOFF, MainMenu);

					break;
		case _MATT_NODE_ONOFF:
					ZmienWidocznoscObiektu("MattNode", _MATT_NODE_ONOFF, MainMenu);

					break;
		case _GEN_NODE_ONOFF:
					ZmienWidocznoscObiektu("GenNode", _GEN_NODE_ONOFF, MainMenu);

					break;
		case _DIR_GEN_LINK_ONOFF:
					ZmienWidocznoscObiektu("DirGenLink", _DIR_GEN_LINK_ONOFF, MainMenu);

					break;
		case _GEN_LINK_ONOFF:
					ZmienWidocznoscObiektu("GenLink", _GEN_LINK_ONOFF, MainMenu);

					break;
		case _GEN_INFO_ONOFF:
					ZmienWidocznoscObiektu("GenInfo", _GEN_INFO_ONOFF, MainMenu);

					break;
		case _UOTT_NODE_ONOFF:
					ZmienWidocznoscObiektu("UOTTNode", _UOTT_NODE_ONOFF, MainMenu);

					break;
		case _BOTT_NODE_ONOFF:
					ZmienWidocznoscObiektu("BOTTNode", _BOTT_NODE_ONOFF, MainMenu);

					break;
		case _BUSINESS_NODE_ONOFF:
					ZmienWidocznoscObiektu("BusinessNode", _BUSINESS_NODE_ONOFF, MainMenu);

					break;
		case _RES_ADMIN_NODE_ONOFF:
					ZmienWidocznoscObiektu("ResAdminNode", _RES_ADMIN_NODE_ONOFF, MainMenu);

					break;
		case _RESEARCH_U_NODE_ONOFF:
					ZmienWidocznoscObiektu("ResearchUNode", _RESEARCH_U_NODE_ONOFF, MainMenu);

					break;
		case _MARKET_NODE_ONOFF:
					ZmienWidocznoscObiektu("MarketNode", _MARKET_NODE_ONOFF, MainMenu);

					break;
		case _PATENT_NODE_ONOFF:    //  40407  //MENUITEM "Systemy informacji OFF/ON"
					ZmienWidocznoscObiektu("InforNode", _PATENT_NODE_ONOFF, MainMenu);

					break;
		case _FINANC_INFO_ONOFF:
					ZmienWidocznoscObiektu("FinancInfo", _FINANC_INFO_ONOFF, MainMenu);

					break;
		case _PACK_INFO_ONOFF:
					ZmienWidocznoscObiektu("PackInfo", _PACK_INFO_ONOFF, MainMenu);

					break;
		case _OFICIAL_INFO_ONOFF:
					ZmienWidocznoscObiektu("OficialInfo", _OFICIAL_INFO_ONOFF, MainMenu);

					break;
		case _SOCIAL_LINK_ONOFF:
					ZmienWidocznoscObiektu("SocialLink", _SOCIAL_LINK_ONOFF, MainMenu);

					break;
		case _COOP_LINK_ONOFF:
					ZmienWidocznoscObiektu("CoopLink", _COOP_LINK_ONOFF, MainMenu);

					break;
		case _ADMIN_LINK_ONOFF:
					ZmienWidocznoscObiektu("AdminLink", _ADMIN_LINK_ONOFF, MainMenu);

					break;
		case _SOCIAL_PROC_ONOFF:
					ZmienWidocznoscObiektu("SocialProc", _SOCIAL_PROC_ONOFF, MainMenu);

					break;
		case _REPORT_PROC_ONOFF:
					ZmienWidocznoscObiektu("ReportProc", _REPORT_PROC_ONOFF, MainMenu);

					break;
		case _PRODUCT_PROC_ONOFF:
					ZmienWidocznoscObiektu("ProductProc", _PRODUCT_PROC_ONOFF, MainMenu);

					break;
		case _RESEARCH_PROC_ONOFF: //   40305  //"Proc. badawcze     OFF/ON"
					ZmienWidocznoscObiektu("ResearchProc", _RESEARCH_PROC_ONOFF, MainMenu);

					break;
		case _TTINSPIR_PROC_ONOFF: //  	40306  //"Szukanie inspiracji OFF/ON"
					ZmienWidocznoscObiektu("TTSearchProc", _TTINSPIR_PROC_ONOFF, MainMenu);

					break;
		case _TTRANSFER_PROC_ONOFF://   40307  //"Proc. transferu    OFF/ON"
					ZmienWidocznoscObiektu("TTransferProc", _TTRANSFER_PROC_ONOFF, MainMenu);

					break;
		case _GEN_PROC_ONOFF:
					ZmienWidocznoscObiektu("GenProc", _GEN_PROC_ONOFF, MainMenu);

					break;
		case 'f':
		case _FASTER_:          //	40030		//MENUITEM "Faster   'f'"
			//unsigned czekaj_ms=0, co_ile_krokow_wizualizacja=1;
			if(czekaj_ms>0)
			{
				czekaj_ms/=2; //Czas przez pó³
			}
			else
			{
				 co_ile_krokow_wizualizacja*=2;
			}
			//cout. ioflags   //Z jakiegoœ powodu (ustawienia flag?) bez rzutu na double wyœwietla b³êdnie
			cout<<endl<<_LPL("Wizualizacja co ","Visualisation every ")<< double(co_ile_krokow_wizualizacja) <<_LPL(" krok(ow)"," step(s)")
				<<", "<<_LPL("z opoznieniem ","with delay ")<< double(czekaj_ms) <<"ms"<<endl;
			break;
		case 's':
		case _SLOWER_:      	  //	40031		//MENUITEM "Slower   's'"
			if(co_ile_krokow_wizualizacja>1)
			{
				co_ile_krokow_wizualizacja/=2;
				if(co_ile_krokow_wizualizacja<=0)
					  co_ile_krokow_wizualizacja=1;
			}
			else
			{
				 if(czekaj_ms<=0)
					czekaj_ms=2;
					else
					czekaj_ms*=2;
			}
			cout<<endl<<_LPL("Wizualizacja co ","Visualisation every ")<<double(co_ile_krokow_wizualizacja)<<_LPL(" krok(ow)"," step(s)")
				<<", "<<_LPL("z opoznieniem ","with delay ")<<double(czekaj_ms)<<"ms"<<endl;
			break;
		case 'Q':
		case 'q':
		case EOF:cont=0;break;
		case '\b':
				if(obsluz_mouse_click()) goto PAUSE;
				break;
		case 27:symuluj=!symuluj;
			 ssh_menu_mark_item(MainMenu,symuluj,_SPS_RUN_);
			 ssh_realize_menu(MainMenu);
			 break;
		case _SPS_CLEAN_ ://Czyœæ z menu jest dwuznaczne wiêc nie tylko prze³acza
				Swiat::Ten.Wymazuj();//Ale i czyœci
				Swiat::Wizualizacja_po_kroku();//oraz odrysowuje
				flush_plot();
		case 'c':czyscciagle=!czyscciagle;break;
		case '\n':
			cout<<_LPL("Wymuszone wyczyszczenie","Forced repaint")<<endl;
		ODRYSOWANIE:
		case '\r':
				Swiat::Ten.Wymazuj();
				Swiat::Wizualizacja_po_kroku();
				flush_plot();
				break;
		};
	}
	else //W³aœciwe robienie kroku
	if(symuluj)
	{
	   if(czyscciagle) Swiat::Ten.Wymazuj();

	   for(unsigned odlicz=0;odlicz<co_ile_krokow_wizualizacja;odlicz++)
	   {
		  if(!Swiat::Krok_modelu())
		  {
			flush_plot();
			cerr<<endl<<"Awaria modelu w kroku "<<Swiat::NumerKroku()<<endl;
			goto FINAL;
		  }
	   }

	   if(
		   Swiat::Statystyka_po_kroku()
		   && Swiat::Wizualizacja_po_kroku()
		   && Swiat::Status_po_kroku() )
		{
		  //OK - mo¿na potwierdziæ                                                      unsigned =0;
		  flush_plot();
		  cout<<'\r'<<Swiat::NumerKroku()<<_LPL(". krok","-th step")<<"         ";
		  TLOG(1, <<Swiat::NumerKroku()<<_LPL(". krok","-th step")        )
		  delay_ms(czekaj_ms);
		}
		else
		{
			//Licznik kroków jest w œrodku
			flush_plot();
			cerr<<endl<<"Awaria wyœwietlania po kroku "<<Swiat::NumerKroku()<<endl;
			goto FINAL;
		}
	}
}} //while(cont)

close_plot(); //Normalne zamkniêcie grafiki
cout<<endl<<_LPL("Koniec poprawny.","Finished correctly")<<endl;
TLOG(  0, <<_LPL("Koniec poprawny.","Finished correctly")  )
return 0;
FINAL:
clog<<endl<<_LPL("Przepraszamy, przerwane awaryjnie...","Sorry, we stopped because of an error...")<<endl;
TLOG( 0,  <<_LPL("Przepraszamy, przerwane awaryjnie...","Sorry, we stopped because of an error...")   )

if(!WB_error_enter_before_clean)
		system("PAUSE");
return 1;
}

bool obsluz_mouse_click()
{
  //int  get_mouse_event(int* xpos,int* ypos,int* click);/* Funkcja odczytujaca ostatnie zdazenie myszy */
  int xpos,ypos,click;
  get_mouse_event(&xpos,&ypos,&click);
  if(click==1 //Prawy klawisz
	&& Swiat::Inspekcja(xpos,ypos,true))//I jak udana inspekcja to PAUSE!!!
	{
		return true;
	}

	return false;
}

void MarkujWszystkieTypy()
{
	ssh_menu_handle MainMenu=ssh_main_menu(); // Daje uchwyt do g³ównego menu

 //	ssh_menu_mark_item(MainMenu,WIDOCZNE,MenuId);   //MENUITEM " * OFF"
	ssh_menu_mark_item(MainMenu,1,_SOCIAL_INFO_ONOFF);// 		40100
	ssh_menu_mark_item(MainMenu,1,_OFICIAL_INFO_ONOFF);//  	40101
	ssh_menu_mark_item(MainMenu,1,_FINANC_INFO_ONOFF);//  	40102
	ssh_menu_mark_item(MainMenu,1,_GEN_INFO_ONOFF);//  		40103
	ssh_menu_mark_item(MainMenu,1,_PACK_INFO_ONOFF);//  		40104
	ssh_menu_mark_item(MainMenu,1,_SOCIAL_LINK_ONOFF);//  	40201
	ssh_menu_mark_item(MainMenu,1,_ADMIN_LINK_ONOFF);//  		40209
	ssh_menu_mark_item(MainMenu,1,_COOP_LINK_ONOFF);//  		40208

	ssh_menu_mark_item(MainMenu,1,_DIR_PARA_LINK_ONOFF);//  	40210
	ssh_menu_mark_item(MainMenu,1,_PARA_LINK_ONOFF);//  		40211
	ssh_menu_mark_item(MainMenu,1,_DIR_GEN_LINK_ONOFF);//  	40212
	ssh_menu_mark_item(MainMenu,1,_GEN_LINK_ONOFF);//  		40213

	ssh_menu_mark_item(MainMenu,1,_GEN_PROC_ONOFF);//  		40301
	ssh_menu_mark_item(MainMenu,1,_SOCIAL_PROC_ONOFF);//  	40302
	ssh_menu_mark_item(MainMenu,1,_REPORT_PROC_ONOFF);//  	40303
	ssh_menu_mark_item(MainMenu,1,_PRODUCT_PROC_ONOFF);//  	40304

	ssh_menu_mark_item(MainMenu,1,_UOTT_NODE_ONOFF);//  		40401
	ssh_menu_mark_item(MainMenu,1,_BOTT_NODE_ONOFF);//  		40402
	ssh_menu_mark_item(MainMenu,1,_BUSINESS_NODE_ONOFF);//  	40403
	ssh_menu_mark_item(MainMenu,1,_RES_ADMIN_NODE_ONOFF);//  	40404
	ssh_menu_mark_item(MainMenu,1,_RESEARCH_U_NODE_ONOFF);//  40405
	ssh_menu_mark_item(MainMenu,1,_MARKET_NODE_ONOFF);//  	40406

	ssh_menu_mark_item(MainMenu,1,_MATT_NODE_ONOFF);//  		40410
	ssh_menu_mark_item(MainMenu,1,_GEN_NODE_ONOFF);//  		40411

	ssh_menu_mark_item(MainMenu,1,_RESEARCH_PROC_ONOFF);   // 40305  //"Proc. badawcze     OFF/ON"
	ssh_menu_mark_item(MainMenu,1,_TTINSPIR_PROC_ONOFF);   //	40306  //"Szukanie inspiracji OFF/ON"
	ssh_menu_mark_item(MainMenu,1,_TTRANSFER_PROC_ONOFF);  // 40307  //"Proc. transferu    OFF/ON"
	ssh_menu_mark_item(MainMenu,1,_MARKET_NODE_ONOFF);    //	40406  //_MARKET_NODE_ONOFF          MENUITEM "Rynki            OFF/ON",
	ssh_menu_mark_item(MainMenu,1,_PATENT_NODE_ONOFF);  //    40407  //MENUITEM "Systemy informacji OFF/ON"

	ssh_realize_menu(MainMenu);
}


//Ró¿ne  testy graficzne przed w³aœciwym modelem
//bool testy_graficzne();



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
