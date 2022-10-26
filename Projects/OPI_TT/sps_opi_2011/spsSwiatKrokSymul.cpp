////////////////////////////////////////////////////////////////////////////////
// Symulator Procesów Sieciowych/Spolecznych (c) Instytut Studiów Spo³ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
////////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
////////////////////////////////////////////////////////////////////////////////
//
// G³ówna funkcja symulacji kroku i to co jej potrzebne (ale mo¿e nie wszystko?)
// - wyodrêbnione ¿eby by³o ³atwo znaleŸæ i rozbudowywaæ
// UWAGA! Czêœæ modelu mo¿e byæ w innych plikach, jak ten zanadto uroœnie!
////////////////////////////////////////////////////////////////////////////////

#include "spsModel.h"

#include <iostream>
#include <cassert>
using namespace std;

#include "INCLUDE/platform.hpp"
#include "INCLUDE/wb_ptr.hpp"
using namespace wbrtm;


static double NumerKrokuSymulacji=0; //"static" bo schowane przed linkerem

double Swiat::NumerKroku()
// Mo¿e kroki bêd¹ te¿ u³amkowe? A w ogóle double ma wiêcej cyfr znacz¹cych
{
   return  NumerKrokuSymulacji;
}

static wb_dynarray<unsigned> ListaKom;
static unsigned IleKomFakt=0;

bool Swiat::Krok_modelu()
// Wykonanie kroku modelu - mo¿e byæ wizualizacja w trakcie
{
	for(unsigned i=0;i<Swiat::IleWezlow();i++)
	{
		WezelSieci* Pom=Swiat::Wezel(i);
		//Teraz niech siê rz¹dzi
		if(Pom)
		{
			if( ! Pom->Poprawny() )
				UsunWezel(i);
			else
			{
				Pom->ChwilaDlaCiebie();
				//A co z procesami?  Trzeba im uj¹æ dostêpnego czasu
				// i ewentualnie usun¹æ jak nie s¹ poprawne.
				//Wykonaniem steruj¹ ju¿ wêz³y samodzielnie
				unsigned ile_p=IleProcesow(i);
				for(unsigned p=0;p<ile_p;p++)
				{
					Proces* Pr=Swiat::Proc(p,i);
					if(Pr)
					{
						if( Pr->Procesor()!=i )
						{ //Powa¿ny k³opot
							cerr<<_LPL("Na wezle ","On node ")<<Pom->Nazwa()<<_LPL(" wykryto niezwi¹zany proces"," unlinked proces detected")<<endl;
						}
						else
						{
						if( ! Pr->Poprawny()    //Jak nie jest poprawny to usuwamy    (hiperzabezpieczenie)
						|| Pr->JakZaawansowany()>=PROCES_NAD_ZAAWANSOWANY)  //... albo jak bardzo po-zakonczony
											//W ten sposób proces sam mo¿e zdecydowaæ o swoim zakonczeniu
											//Bo nie mo¿e siê bezpiecznie sam usun¹æ! (bo delete!)
						UsunProc(p,i);
						else
						Pr->_IleKrokowDoKonca--;
						}
					}
				}
			}
		}
	}

	//Ewentualne zmiany linków
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

	//Komunikaty trzeba przetwarzaæ tak, ¿eby nowe zaczekaly - lista bardzo zmienna
	if(Swiat::IleInformacji()>0) //W ogóle s¹ jakieœ
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

		//Stare komunikaty bêd¹ przetworzone, a nowe - powsta³e w trakcie przetwarzania ju¿ nie
		for(unsigned i=0;i<IleKomFakt;i++)
		{
			Komunikat* Messg=Swiat::Info(ListaKom[i]);
			if(Messg==NULL) continue; //Jakby jednak znikn¹³ w miedzyczasie (???)

			if(Messg->JakDostawa()>=1) //Jeœli ju¿ dowieziony do celu
			{  //To trzeba zorganizowaæ odbiór
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
	NumerKrokuSymulacji+=1; // Ca³y krok?
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
