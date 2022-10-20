// //////////////////////////////////////////////////////////////////////////////
// Symulator Proces�w Sieciowych/Spolecznych (c) Instytut Studi�w Spo�ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
// //////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
// //////////////////////////////////////////////////////////////////////////////
//
// G��wna funkcja symulacji kroku i to co jej potrzebne (ale mo�e nie wszystko?)
// - wyodr�bnione �eby by�o �atwo znale�� i rozbudowywa�
// UWAGA! Cz�� modelu mo�e by� w innych plikach, jak ten zanadto uro�nie!
// //////////////////////////////////////////////////////////////////////////////

#include "spsModel.h"

#include <iostream>
#include <cassert>
using namespace std;

#include "wb_ptr.hpp"
#include "lingo.hpp"

using namespace wbrtm;


static double NumerKrokuSymulacji=0; //"static" bo schowane przed linkerem

double Swiat::NumerKroku()
// Mo�e kroki b�d� te� u�amkowe? A w og�le double ma wi�cej cyfr znacz�cych
{
   return  NumerKrokuSymulacji;
}

static wb_dynarray<unsigned> ListaKom;
static unsigned IleKomFakt=0;

bool Swiat::Krok_modelu()
// Wykonanie kroku modelu - mo�e by� wizualizacja w trakcie
{
	for(unsigned i=0;i<Swiat::IleWezlow();i++)
	{
		WezelSieci* Pom=Swiat::Wezel(i);
		//Teraz niech si� rz�dzi
		if(Pom)
		{
			if( ! Pom->Poprawny() )
				UsunWezel(i);
			else
			{
				Pom->ChwilaDlaCiebie();
				//A co z procesami?  Trzeba im uj�� dost�pnego czasu
				// i ewentualnie usun�� jak nie s� poprawne.
				//Wykonaniem steruj� ju� w�z�y samodzielnie
				unsigned ile_p=IleProcesow(i);
				for(unsigned p=0;p<ile_p;p++)
				{
					Proces* Pr=Swiat::Proc(p,i);
					if(Pr)
					{
						if( Pr->Procesor()!=i )
						{ //Powa�ny k�opot
							cerr<<_LPL("Na wezle ","On node ")<<Pom->Nazwa()<<_LPL(" wykryto niezwi�zany proces"," unlinked proces detected")<<endl;
						}
						else
						{
						if( ! Pr->Poprawny()    //Jak nie jest poprawny to usuwamy    (hiperzabezpieczenie)
						|| Pr->JakZaawansowany()>=PROCES_NAD_ZAAWANSOWANY)  //... albo jak bardzo po-zakonczony
											//W ten spos�b proces sam mo�e zdecydowa� o swoim zakonczeniu
											//Bo nie mo�e si� bezpiecznie sam usun��! (bo delete!)
						UsunProc(p,i);
						else
						Pr->_IleKrokowDoKonca--;
						}
					}
				}
			}
		}
	}

	//Ewentualne zmiany link�w
	for(unsigned i=0;i<Swiat::IlePowiazan();i++)
	{
		Powiazanie* Pom=Swiat::Lacze(i);
		if(Pom)
		{
			if( ! Pom->Poprawny() ) //Jak nie jest poprawny to usuwamy    (hiperzabezpieczenie)
				UsunLacze(i);
				else
				Pom->ChwilaDlaCiebie();
		}
	}

	//Komunikaty trzeba przetwarza� tak, �eby nowe zaczekaly - lista bardzo zmienna
	if(Swiat::IleInformacji()>0) //W og�le s� jakie�
	{
		ListaKom.alloc(Swiat::IleInformacji());
		IleKomFakt=0;
		for(unsigned i=0;i<Swiat::IleInformacji();i++)
		{
			Komunikat* Messg=Swiat::Info(i);
			if(Messg)
			{
				if(!Messg->Poprawny()) //Jak nie jest poprawny to usuwamy    (hiperzabezpieczenie)
					Swiat::UsunInfo(i);
				else
					ListaKom[IleKomFakt++]=i;
			}
		}

		//Stare komunikaty b�d� przetworzone, a nowe - powsta�e w trakcie przetwarzania ju� nie
		for(unsigned i=0;i<IleKomFakt;i++)
		{
			Komunikat* Messg=Swiat::Info(ListaKom[i]);
			if(Messg==NULL) continue; //Jakby jednak znikn�� w miedzyczasie (???)

			if(Messg->JakDostawa()>=1) //Je�li ju� dowieziony do celu
			{  //To trzeba zorganizowa� odbi�r
			   unsigned Cel=Messg->Odbiorca();
			   WezelSieci* Odbiorca=Swiat::Wezel(Cel); 		assert(Odbiorca!=NULL);
			   Odbiorca->InterpretujKomunikat(Messg);
			   Swiat::UsunInfo(ListaKom[i]);//... i potem usuniecie
			}
			else
			Messg->ChwilaDlaCiebie();
		}
	}
	//Poprawny koniec kroku
	//cout<<'\b';
	NumerKrokuSymulacji+=1; // Ca�y krok?
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
