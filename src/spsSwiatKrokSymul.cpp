/// \file
/// \brief Główna funkcja symulacji kroku i to co jej potrzebne (ale może nie wszystko?)
//         -----------------------------------------------------------------------------
///
/// \details
///      Wyodrębnione, żeby było łatwo znaleźć i rozbudowywać
///
///      UŻYWAMY LINIOWEJ ORGANIZACJI KONTENERÓW, ALE NIELINIOWO JEJ UŻYWAMY.
///      (TODO) MOŻNA BY TO ZASTĄPIĆ JAKĄŚ INNĄ IMPLEMENTACJĄ
/// \note Cześć modelu może być w innych plikach, jak ten zanadto urośnie!
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011, wborkowski@uw.edu.pl
/// \date
///     2022.11.09 (last updated)
//*////////////////////////////////////////////////////////////////////////////////

#include "spsModel.h"

#include <iostream>
#include <cassert>
using namespace std;

#include "wb_ptr.hpp"
#include "lingo.hpp"

using namespace wbrtm;


static double NumerKrokuSymulacji=0; ///< "static" bo schowane przed linkerem i MODYFIKACJĄ

/// Dostęp _read only_ do licznika kroków.
/// Zwraca `double` bo może kroki bedą też ułamkowe? A w ogóle `double` ma więcej cyfr znaczących
double Swiat::NumerKroku()
{
   return  NumerKrokuSymulacji;
}

static wb_dynarray<unsigned> ListaKom; ///< ???
static unsigned IleKomFakt=0;

/// Wykonanie kroku modelu może zawierać wizualizacje w trakcie!
/// Każdy węzeł dostaje swoją chwilę CPU. TODO: Być może WIELOWĄTKOWO!
/// Wykonaniem procesów węzły sterują samodzielnie, ale świat zajmuje się kalkulacją czasu pracy.
/// Musi  im ująć z dostępnego czasu i ewentualnie usunąć jak ZOMBI i jak nie są poprawne.
bool Swiat::Krok_modelu()
{
    // OBSŁUGA WĘZŁÓW I ICH PROCESÓW
    // /////////////////////////////////
	for(unsigned i=0;i<Swiat::IleWezlow();i++)
	{
		WezelSieci* Pom=Swiat::Wezel(i);

		if(Pom) //Teraz niech się rządzi
		{
			if( ! Pom->Poprawny() )
				UsunWezel(i);
			else
			{
				Pom->ChwilaDlaCiebie(); // CZAS DLA WĘZŁA

				// A co z procesami?  Trzeba im ująć dostępnego czasu
				// i ewentualnie usunąć.
				unsigned ile_p=IleProcesow(i);
				for(unsigned p=0;p<ile_p;p++)
				{
					Proces* Pr=Swiat::Proc(p,i);
					if(Pr)
					{
						if( Pr->Procesor()!=i )
						{   //Poważny kłopot
							cerr<<_LPL("Na wezle ","On node ")<<Pom->Nazwa()
                                <<_LPL(" wykryto nieprzypisany proces"," unlinked proces detected")<<endl;
						}
						else
						{
						if( ! Pr->Poprawny()    //Jak nie jest poprawny, to usuwamy (hiper-zabezpieczenie)
						|| Pr->JakZaawansowany()>=PROCES_NAD_ZAAWANSOWANY
                        )   //... albo jak bardzo po-zakonczony
                            // W ten sposób proces sam może zdecydować o swoim zakończeniu.
                            // Bo nie może się bezpiecznie sam usunąć ze względu na konieczność delete!
						    UsunProc(p,i);
						else
						    Pr->_IleKrokowDoKonca--;
						}
					}
				}
			}
		}
	}

	// Ewentualne zmiany linków
    // //////////////////////////
	for(unsigned i=0;i<Swiat::IlePowiazan();i++)
	{
		Powiazanie* Pom=Swiat::Lacze(i);
		if(Pom)
		{
			if( ! Pom->Poprawny() ) //Jak nie jest poprawny, to usuwamy (hiper-zabezpieczenie)
				UsunLacze(i);
            else
				Pom->ChwilaDlaCiebie();
		}
	}

	// Komunikaty trzeba przetwarzać tak, żeby nowe zaczekały. Ta lista bardzo zmienna
    // /////////////////////////////////////////////////////////////////////////////////
	if(Swiat::IleInformacji()>0) // Czy w ogóle są jakieś?
	{
		ListaKom.alloc(Swiat::IleInformacji());
		IleKomFakt=0;
		for(unsigned i=0;i<Swiat::IleInformacji();i++)
		{
			Komunikat* Messg=Swiat::Info(i);
			if(Messg)
			{
				if(!Messg->Poprawny()) //Jak nie jest poprawny, to usuwamy (hiper-zabezpieczenie)
					Swiat::UsunInfo(i);
				else
					ListaKom[IleKomFakt++]=i;
			}
		}

		// Stare komunikaty będą przetworzone, a nowe, powstałe w trakcie przetwarzania już muszą poczekać na kolejny krok
		for(unsigned i=0;i<IleKomFakt;i++)
		{
			Komunikat* Messg=Swiat::Info(ListaKom[i]);
			if(Messg==NULL) continue; //Jakby jednak zniknął w miedzyczasie (???)

			if(Messg->JakDostawa()>=1) //Jeśli już dowieziony do celu
			{
			   unsigned Cel=Messg->Odbiorca(); // To trzeba zorganizować odbiór
			   WezelSieci* Odbiorca=Swiat::Wezel(Cel); 		                             assert(Odbiorca!=NULL);

               Odbiorca->InterpretujKomunikat(Messg);

               Swiat::UsunInfo(ListaKom[i]); // A po przetworzeniu usuniecie
			}
			else
			Messg->ChwilaDlaCiebie();
		}
	}

	// Poprawny koniec kroku
    // /////////////////////
	//cout<<'\b';
	NumerKrokuSymulacji+=1; // Cały krok zakończony

    return true; // JAK ZWYKLE SIĘ UDAŁO
}

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
