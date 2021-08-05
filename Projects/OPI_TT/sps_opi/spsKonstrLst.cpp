////////////////////////////////////////////////////////////////////////////////
// Symulator Procesów Sieciowych/Spolecznych (c) Instytut Studiów Spo³ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
////////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
////////////////////////////////////////////////////////////////////////////////
//
// Metody klasy bazowej ElementModelu  slu¿ace rejestracji wirtualnych konstruktorów
// w sytuacji gdy nie mo¿na u¿yæ <map>  Szuka liniowo po klasycznej liœcie,
// wiêc przy wiêkszej liczbie typów bêdzie nieefektywne, ale przy malej jest
// ca³kiem skuteczne, szybko siê kompiluje i nie wywala nawet w "Borlandzie"
////////////////////////////////////////////////////////////////////////////////////
#include<string>
using namespace std;

#include "spsElement.h"

struct WezelListy
{
	string Nazwa;
	ElementModelu::WirtualnyKonstruktor* Konst;
	WezelListy* Nastepny;
};

WezelListy* Head=NULL;
bool	ListaNazwTypowAktualna=false;

//#define SAFE_LIST_CONSTRUCTION
#ifdef SAFE_LIST_CONSTRUCTION
//GENEROWANIE LISTY ZA POMOC¥ MOJE KLASY wb_pchar - dziala zawsze ...
#include"INCLUDE/wb_ptr.hpp" 
using namespace wbrtm;

static wb_pchar ListaNazwTypow;

const char*  dummy() 
{
	if(ListaNazwTypowAktualna)
			return ListaNazwTypow.get();

	WezelListy* Top=Head; //Pocz¹tek
	size_t Rozmiar=0;
	while(Top!=NULL)
	{
	   if(Top->Konst!=NULL)
	   {
		  Rozmiar+=strlen(Top->Nazwa.c_str())+1;//Zliczanie rozmiarów
	   }
	   Top=Top->Nastepny;//Szuka dalej
	}
	ListaNazwTypow.alloc(Rozmiar+1);
	Top=Head; //Znowu pocz¹tek
	while(Top!=NULL)
	{
	   if(Top->Konst!=NULL)
	   {
		   ListaNazwTypow.add("%s;",Top->Nazwa.c_str());//Rozmiar musi byæ OK!!!
	   }
	   Top=Top->Nastepny;//Szuka dalej
	}
	ListaNazwTypowAktualna=true;
	const char*  fordebu=ListaNazwTypow.get_ptr_val();
	return fordebu;
}

const char*   ElementModelu::WirtualnyKonstruktor::ListaNazwTypow()
{
	return dummy();//Wewn¹trz tej funkcji nie mo¿na u¿yæ klas z nag³ówków ??? Zw³aszcza STL
}
#else
//GENEROWANIE LISTY ZA POMOC¥ KLASY stringstream - nie w ka¿dym kompilatorze dzia³a poprawnie ! ! !
#include <sstream>

stringstream ListaNazwTypow;

static const char*  dummy()
{
	if(ListaNazwTypowAktualna)
			return ListaNazwTypow.str().c_str();

	WezelListy* Top=Head; //Pocz¹tek
	while(Top!=NULL)
	{
	   if(Top->Konst!=NULL)
	   {
		  ListaNazwTypow<<Top->Nazwa<<";";
	   }
	   Top=Top->Nastepny;//Szuka dalej
	}
	ListaNazwTypowAktualna=true;
	const char*  fordebu=ListaNazwTypow.str().c_str();
	return fordebu;
}

const char*   ElementModelu::WirtualnyKonstruktor::ListaNazwTypow()
{
	return dummy();//Wewn¹trz tej funkcji nie mo¿na u¿yæ klas z nag³ówków ??? Zw³aszcza STL
				   //Bezpoœrednio nie rozpoznaje poprawnie strstream np.!!!
}

#endif


WezelListy* Znajdz(const char* NazwaKlasy)
{
	WezelListy* Top=Head; //Pocz¹tek
	while(Top!=NULL)
	{
	   if(stricmp(NazwaKlasy,Top->Nazwa.c_str())==0)
			return Top; //Znalaz³
			else
			Top=Top->Nastepny;//Szuka dalej
	}
	return NULL;//Nie ma
}

void ElementModelu::WirtualnyKonstruktor::Zarejestruj(const char* NazwaKlasy,WirtualnyKonstruktor* Ten)
//Obiekt musi istnieæ puki jest potrzebny
{//Mapa[NazwaKlasy]=Ten;
	 WezelListy* Pom=new WezelListy;
	 Pom->Nastepny=Head;
	 Pom->Nazwa=NazwaKlasy;
	 Pom->Konst=Ten;
	 Head=Pom;
	 ListaNazwTypowAktualna=false;
}

void ElementModelu::WirtualnyKonstruktor::Wyrejestruj(const char* NazwaKlasy,WirtualnyKonstruktor* Ten)
//A potem mo¿na wyrejestrowaæ
{//if(Mapa[NazwaKlasy]==Ten) Mapa[NazwaKlasy]=NULL;
	WezelListy* Pom=Znajdz(NazwaKlasy);
	if(Pom)
		Pom->Konst=NULL; //Skasowane...
	//... Mo¿na by dealokowaæ, ale to jest prosta implementacja :-)
	ListaNazwTypowAktualna=false;
}

ElementModelu::WirtualnyKonstruktor* ElementModelu::WirtualnyKonstruktor::DajWirtualnyKonstruktor(const char* NazwaKlasy)
//A to daje zarejestrowany obiekt gdy potrzebny
{ /* map<string, ElementModelu::WirtualnyKonstruktor* , less<string> >::const_iterator i;
	i = Mapa.find(NazwaKlasy);    // WirtualnyKonstruktor* Pom=
	if (i == Mapa.end ())
		return NULL; //Nie ma
		else
		return i->second;    */
	WezelListy* Pom=Znajdz(NazwaKlasy);
	if(Pom)
		return Pom->Konst;
		else
		return NULL;
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
