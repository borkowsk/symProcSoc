/// \file
/// \brief Metody klasy bazowej ElementModelu  służące rejestracji wirtualnych konstruktorów
///        ---------------------------------------------------------------------------------
///
/// \details
///		W sytuacji gdy nie można użyć <map>.  Szuka liniowo po klasycznej liście,
///		więc przy większej liczbie typów będzie nieefektywne, ale przy małej jest
///		całkiem skuteczne, szybko się kompiluje i nie wywala nawet w "Borlandzie"
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
/// \date
///     2022.11.15 (last updated)
//*////////////////////////////////////////////////////////////////////////////////


#include <cstring>
#include <string>

using namespace std;

#include "spsElement.h"
#include "compatyb.h"

// #define SAFE_LIST_CONSTRUCTION

struct WezelListy
{
	string Nazwa;
	ElementModelu::WirtualnyKonstruktor* Konst;
	WezelListy* Nastepny;
};

WezelListy* Head=NULL;


#ifdef SAFE_LIST_CONSTRUCTION
/// GENEROWANIE LISTY ZA POMOCĄ MOJE KLASY wb_pchar - działa zawsze ...
#include"INCLUDE/wb_ptr.hpp" 

using namespace wbrtm;

static wb_pchar ListaNazwTypow; ///< Ukryta przed linkerem tekstowa lista typów.
static bool	ListaNazwTypowAktualna = false;

/// TESTOWA I TEKSTOWA(!) LISTA TYPÓW?
const char*  dummy() 
{
	if(ListaNazwTypowAktualna)
			return ListaNazwTypow.get();

	WezelListy* Top=Head; //Początek
	size_t Rozmiar=0;
	while(Top!=NULL)
	{
	   if(Top->Konst!=NULL)
	   {
		  Rozmiar+=strlen(Top->Nazwa.c_str())+1; //Zliczanie rozmiarów
	   }
	   Top=Top->Nastepny; //Szuka dalej
	}

	ListaNazwTypow.alloc(Rozmiar+1);
	Top=Head; //Znowu początek
	while(Top!=NULL)
	{
	   if(Top->Konst!=NULL)
	   {
		   ListaNazwTypow.add("%s;",Top->Nazwa.c_str()); //Rozmiar musi być OK!!!
	   }
	   Top=Top->Nastepny; //Szuka dalej
	}

	ListaNazwTypowAktualna=true;
	const char*  fordebu=ListaNazwTypow.get_ptr_val();

	return fordebu;
}

/// \note Wewnątrz tej funkcji nie można użyć klas z nagłówków ??? Zwłaszcza STL (to chyba w Borlandzie?)
const char*   ElementModelu::WirtualnyKonstruktor::ListaNazwTypow()
{
	return dummy();
}
#else
/// GENEROWANIE LISTY ZA POMOCĄ KLASY stringstream - nie w każdym kompilatorze działa poprawnie ! ! !
#include <sstream>

static stringstream ListaNazwTypow; ///< Ukryta przed linkerem tekstowa lista typów.
static bool	ListaNazwTypowAktualna = false;

static const char*  dummy()
{
	if(ListaNazwTypowAktualna)
			return ListaNazwTypow.str().c_str();

	WezelListy* Top=Head; //Początek
	while(Top!=NULL)
	{
	   if(Top->Konst!=NULL)
	   {
		  ListaNazwTypow<<Top->Nazwa<<";";
	   }
	   Top=Top->Nastepny; //Szuka dalej
	}

	ListaNazwTypowAktualna=true;
	const char*  fordebu=ListaNazwTypow.str().c_str();
	return fordebu;
}

/// \note Wewnątrz tej funkcji nie można użyć klas z nagłówków ??? Zwłaszcza STL (to chyba w Borlandzie?)
///       Bezpośrednio nie rozpoznaje poprawnie strstream np.!!!
const char*   ElementModelu::WirtualnyKonstruktor::ListaNazwTypow()
{
	return dummy(); 
}

#endif

/// \return obiekt informacji o typie wg. zarejestrowanej nazwy klasy.
WezelListy* Znajdz(const char* NazwaKlasy)
{
	WezelListy* Top=Head; //Początek
	while(Top!=NULL)
	{
	   if(strcasecmp(NazwaKlasy,Top->Nazwa.c_str())==0)
			return Top; //Znalazł!
			else
			Top=Top->Nastepny; //Szuka dalej
	}
	return NULL; //Nie ma
}

/// \note Obiekt musi istnieć puki jest potrzebny 
/// `Mapa[NazwaKlasy]=Ten;`
void ElementModelu::WirtualnyKonstruktor::Zarejestruj(const char* NazwaKlasy,WirtualnyKonstruktor* Ten)
{
	 WezelListy* Pom=new WezelListy;
	 Pom->Nastepny=Head;
	 Pom->Nazwa=NazwaKlasy;
	 Pom->Konst=Ten;
	 Head=Pom;
	 ListaNazwTypowAktualna=false;
}

/// \note Gdy obiekt konstriktora juz niepotrzebny mozna wyrejestrować
/// `if(Mapa[NazwaKlasy]==Ten) Mapa[NazwaKlasy]=NULL;`
void ElementModelu::WirtualnyKonstruktor::Wyrejestruj(const char* NazwaKlasy,WirtualnyKonstruktor* Ten)
{
	WezelListy* Pom=Znajdz(NazwaKlasy);
	if(Pom)
		Pom->Konst=NULL; //Skasowane...
	//TODO: Można by dealokować?
	ListaNazwTypowAktualna=false;
}

/// \return Metoda daje zarejestrowany obiekt gdy potrzebny
/** ```
	map<string, ElementModelu::WirtualnyKonstruktor* , less<string> >::const_iterator i;
	i = Mapa.find(NazwaKlasy);
	if (i == Mapa.end ())
		return NULL; //Nie ma
		else
		return i->second;  
    ```
*/
ElementModelu::WirtualnyKonstruktor* ElementModelu::WirtualnyKonstruktor::DajWirtualnyKonstruktor(const char* NazwaKlasy)
{ 
	WezelListy* Pom=Znajdz(NazwaKlasy);
	if(Pom)
		return Pom->Konst;
		else
		return NULL;
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

