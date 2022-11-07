/// \file
/// \brief Metody klasy bazowa definiującej najbardziej podstawowy interfejs do elementów modelu
///        --------------------------------------------------------------------------------------
///
/// \details
///              Głównie chodzi o wizualizacje i inicjalizację z pliku
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
/// \date
///     2022.11.04 (last updated)
// ///////////////////////////////////////////////////////////////////////////////////////
#include "spsElement.h"

#include <cassert>
#include <cstring>
#include <iostream>
using namespace std;

#include "wb_bits.h"
#include "lingo.hpp"

/// \details Zwraca liczbę bitów 1 w tych danych
unsigned DziedzinaWKolorze::IleBitow()
{
    return bits(A)+bits(R)+bits(G)+bits(B);
}

/// \details Kolor czyli "dziedzina" musi miec kazdy obiekt w modelu
ElementModelu::ElementModelu()
{Col.ARGB=0;_MojIndeks=-1;}

//virtual
/// \details Destruktor wirtualny, bo ma metody wirtualne
ElementModelu::~ElementModelu()
{
//	assert(_MojIndeks!=-1);    //Tak naprawdę to siś zdarza?
}

/// Zwraca Indeks ustawiony przez świat albo jakiś innych identyfikator wobec swiata
unsigned ElementModelu::MojID()
{                                                                                               assert(_MojIndeks!=-1);
                                                        //Tak naprawdę to się zdarza, jak obiekt nie wstawiony do świata
	return _MojIndeks;
}

DziedzKol ElementModelu::PodajDziedzine()
{
	return Col;
}

void ElementModelu::UstawDziedzine(const DziedzinaWKolorze& D)
{
    Col=D;
}

void ElementModelu::UstawDziedzine(unsigned R,unsigned G, unsigned B, unsigned A) //A=0 domyślnie
{
   Col.A=A;
   Col.R=R;
   Col.G=G;
   Col.B=B;
}

/// Klasa bazowa nie ma wirtualnego konstruktora, bo jest pure-virtual \return NULL
ElementModelu::WirtualnyKonstruktor* ElementModelu::VKonstruktor()
{
	return NULL;
}

/// Bazująca na FLAGS informacja o widocznosci
unsigned  ElementModelu::VWidocznosc()
{
   WirtualnyKonstruktor* Kons=this->VKonstruktor();
   if(Kons)
   {
	  // cerr<<Kons->NazwaTypu()<<endl;
	   return  Kons->Flagi() & 0x3; //Dwa najmłodsze bity
   }
   else return 4; //Na wszelki wypadek pełna wizualizacja, gdy nie ma "wirtualnego konstruktora"
}

/// Dwa pola są obsługiwane nawet jak nie ma nazw!
unsigned ElementModelu::WirtualnyKonstruktor::IleNazwPol()
{
	if(Nazwy.Ile()>2)
		return Nazwy.Ile();
	else
		return 2;
}

/// \return zwraca nazwę pola
const char* ElementModelu::WirtualnyKonstruktor::NazwaPola(unsigned i)
{
	if(Nazwy.Ile()>i)
		return Nazwy[i].c_str();
	else
		return "<DEF>";
}

/// \return liczbę pól danych lokalnych
unsigned 	ElementModelu::IlePolDanych()
{
   return Dane.Ile();
}

const char* ElementModelu::NazwaPola(unsigned Ktore)
{
   if(Ktore==0) return _LPL("NazwaTypu","TypeName");
   WirtualnyKonstruktor* Konstr=this->VKonstruktor();
   if(Konstr!=NULL && Konstr->Nazwy.Ile()>Ktore)
   {
		return Konstr->Nazwy[Ktore].c_str();
   }
   else
   {
		if(Ktore==1) return _LPL("Nazwa","Name");
		else return _LPL("Pole","Field");
   }
}

const char* ElementModelu::WartoscPola(unsigned Ktore)
{
	if(Ktore<Dane.Ile())
			return Dane[Ktore].c_str();
			else
			return NULL;
}

bool   ElementModelu::UstawPole(unsigned Ktore,const char* NowaWartosc)
{
	if(Ktore<Dane.Ile())
	{
		  Dane[Ktore]=NowaWartosc;return true;
	}
	else
			return false;
}

// Klasa pomocnicza obsługująca zarządzanie danymi tekstowymi obiektów modelu
// a mogąca też przejąć zarządzanie ich konwersjami i przekształcenie w warianty
//
// class ElementModelu::DaneLokalne
//	  string* Dane;
//	  unsigned 	IleDanych;

/// Jeśli ma zaalokowane `Dane` to robi im `delete`
ElementModelu::DaneLokalne::~DaneLokalne()
{
	if(IleDanych!=0) delete [] Dane;
}

/// Dostęp do danych ze sprawdzeniem indeksu. Jak wadliwy to `exit(-1)`.
string& ElementModelu::DaneLokalne::operator [] (unsigned index) const
{
	if(index<IleDanych) return Dane[index];

	cerr<<_LPL("Wadliwy indeks danej","Index out of range")<<" = "<<index<<endl;
	exit(-1);
}

///     - Usuwa stare dane jeśli są.
///     - Potem alokuje `Ile` stringów.
///     - Potem kopiuje stringi z `Lista` na `Dane`
void ElementModelu::DaneLokalne::InicjujWgListy(const std::string* Lista,unsigned Ile)
{
	if(IleDanych!=0)
			delete [] Dane;

	Dane=new string[Ile];             		                                                         assert(Dane!=NULL);
	for(unsigned i=0;i<Ile;i++)
	{
		Dane[i]=Lista[i];
	}

	IleDanych=Ile;
}

/// Funkcja jest wymagana, ale dla klasy praktycznie bazowej nie ma wiele do roboty.
/// Musi zadbać żeby wszystko było aktualne, jednak klasy potomne mogą gdzie indziej chcieć zapisywać
/// dziedzinę, a to jedyne pole poza nazwą typu, które tu by można zaktualizować
void ElementModelu::AktualizujListeDanych()
{
	if(Dane.Ile()==0)
	{
		Dane.Realokuj(1);
		WirtualnyKonstruktor* Kons=this->VKonstruktor();

		if(Kons)  	//Chyba nie warto robić nic więcej?
			Dane[0]=Kons->NazwaTypu();
	}
}

/// Przekopiowywanie danych z innej listy danych.
///     - Usuwa stare dane jeśli są.
/// TODO? `const DaneLokalne& operator = (const DaneLokalne& Wzor);`  - powinno być zabronione na wszelki wypadek!
void ElementModelu::DaneLokalne::InicjujZDanych(const DaneLokalne& Wzor)
{
	 if(IleDanych!=0) delete [] Dane;
	 Dane=new string[Wzor.Ile()];   		                                                         assert(Dane!=NULL);
	 IleDanych=Wzor.Ile();
	 for(unsigned i=0;i<IleDanych;i++)
	 {
		Dane[i]=Wzor[i];
	 }
}

/// Rozszerza lub skraca listę danych
void ElementModelu::DaneLokalne::Realokuj(unsigned Ile)
{
	if(Ile==IleDanych) //Nic się nie zmieniło!
		return;

	string* StaraLista=Dane;
	Dane=new string[Ile];                                                                            assert(Dane!=NULL);
	unsigned limit=min(Ile,IleDanych);

    if(limit>0)
	  for(unsigned i=0;i<limit;i++) // Nie jest to najszybszy sposób,
		Dane[i]=StaraLista[i];	    // ale czy można bezpiecznie zrobić cos lepszego?
                                    // TODO Chyba że w C++17?

	if(StaraLista!=NULL)
		delete [] StaraLista;

    IleDanych=Ile;
}


/// Przypisania i konwersje na elementy listy
/// Zwraca `true` jak się udało
bool ElementModelu::DaneLokalne::PrzypiszZ(unsigned gdzie,double co)
{
	char bufor[512];
	if(sprintf(bufor,"%g",co)!=EOF)
	{
		Dane[gdzie]=bufor;
		return true;
	}
	else return false;
}

/// Zwraca `true` jak się udało
bool ElementModelu::DaneLokalne::PrzypiszZ(unsigned gdzie,long   co)
{
	char bufor[512];
	if(sprintf(bufor,"%lu",co)!=EOF)
	{
		Dane[gdzie]=bufor;
		return true;
	}
	else return false;
}

/// Zwraca indeks znaku który nie pasował lub -1 jako sukces
int ElementModelu::DaneLokalne::KonwertujDo(unsigned zkad,float& naco)
{
	double pom;
	int ret=KonwertujDo(zkad,pom);
	if(ret!=-1) return ret;
	naco=pom; //TU SIĘ MOŻE SKOPAĆ !!!???
	return -1;
}

/// Zwraca indeks znaku który nie pasował lub -1 jako sukces
int ElementModelu::DaneLokalne::KonwertujDo(unsigned zkad,double& naco)
{
	bool procent=false;
	char  temp[1024]; //Duża tablica na stosie
	char* pom=temp;
	strncpy(temp,Dane[zkad].c_str(),511);

	if(strlen(temp)==0) //PUSTY!!!
		   { naco=0;  return 0; }

	if( strchr(temp,'.')==NULL //Gdy nie ma kropki
	  && (pom=strrchr(temp,','))!=NULL ) //To ostatni przecinek zmienia na kropkę
			*pom='.';

	if( (pom=strrchr(temp,'%'))!=NULL )
	{
			procent=true;
			*pom='\0';
	}

	char* endptr=NULL;
	naco=strtod(temp,&endptr);
	if(endptr!=NULL && *endptr!='\0')
		return endptr-temp;

	if(procent)
		naco/=100.0;

	return -1;  //Wbrew pozorom OK
}

/// Zwraca indeks znaku który nie pasował lub -1 jako sukces
int ElementModelu::DaneLokalne::KonwertujDo(unsigned zkad,int& naco)
{
	char* endptr=NULL;
	naco=strtol(Dane[zkad].c_str(),&endptr,0);
	if(endptr!=NULL && *endptr!='\0')
		return endptr-Dane[zkad].c_str();
	else
		return -1;  //Wbrew pozorom OK
}

/// Zwraca indeks znaku który nie pasował lub -1 jako sukces
int ElementModelu::DaneLokalne::KonwertujDo(unsigned zkad,unsigned& naco)
{
	char* endptr=NULL;
	naco=strtoul(Dane[zkad].c_str(),&endptr,0);
	if(endptr!=NULL && *endptr!='\0')
		return endptr-Dane[zkad].c_str();
	else
		return -1;  //Wbrew pozorom OK
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


