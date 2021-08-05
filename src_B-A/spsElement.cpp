////////////////////////////////////////////////////////////////////////////////
// Symulator Procesów Sieciowych/Spolecznych (c) Instytut Studiów Spo³ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
////////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
////////////////////////////////////////////////////////////////////////////////
//
// Te metody klasy bazowej ElementModelu
////////////////////////////////////////////////////////////////////////////////
#include "spsElement.h"


#include "INCLUDE/platform.hpp"
#include <cassert>
#include <iostream>
using namespace std;

#include "INCLUDE/wb_bits.h"
using namespace wbrtm;

unsigned DziedzinaWKolorze::IleBitow()
//Zwraca liczbê bitów 1 w tych danych
{
    return bits(A)+bits(R)+bits(G)+bits(B);
}

ElementModelu::ElementModelu()
//Kolor czyli "dziedzinê" musi miec kazdy obiekt w modelu
{Col.ARGB=0;_MojIndeks=-1;}

//virtual
ElementModelu::~ElementModelu()
//Destruktor wirtualny, bo bêd¹ metody wirtualne
{
//	assert(_MojIndeks!=-1);    //Tak naprawdê to siê zdarza
}

unsigned ElementModelu::MojID()
//Zwraca Indeks ustawiony przez Œwiat albo jakiœ innych identyfikator wobec swiata
{
	assert(_MojIndeks!=-1);    //Tak naprawdê to siê zdarza jak obiekt nie wstawiony do œwiata
	return _MojIndeks;
}

DziedzKol ElementModelu::PodajDziedzine()
{
	return Col;
}

void ElementModelu::UstawDziedzine(const DziedzinaWKolorze& D)
{ Col=D; }

void ElementModelu::UstawDziedzine(unsigned R,unsigned G, unsigned B, unsigned A)//A=0 domyslnie
{
   Col.A=A;
   Col.R=R;
   Col.G=G;
   Col.B=B;
}



ElementModelu::WirtualnyKonstruktor* ElementModelu::VKonstruktor()
{
	//Klasa bazowa nie ma bo jest pure-virtual
	return NULL;
}

unsigned  ElementModelu::VWidocznosc()
//Bazuj¹ca na FLAGS informacja o widocznosci
{
   WirtualnyKonstruktor* Kons=this->VKonstruktor();
   if(Kons)
   {
	  // cerr<<Kons->NazwaTypu()<<endl;
	   return  Kons->Flagi() & 0x3; //Dwa najmlodsze bity
   }
   else return 4; //Na wszelki wypadek pe³na wizualizacja
}

unsigned ElementModelu::WirtualnyKonstruktor::IleNazwPol()
{  //Dwa pola s¹ obslugiwane nawet jak nie ma nazw!
	if(Nazwy.Ile()>2)
		return Nazwy.Ile();
	else
		return 2;
}

const char* ElementModelu::WirtualnyKonstruktor::NazwaPola(unsigned i)
{
	if(Nazwy.Ile()>i)
		return Nazwy[i].c_str();
	else
		return "<DEF>";
}

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

//Klasa pomocnicza obslugujaca zarz¹daznie danymi tekstowymi obiektów modelu
//a mog¹ca te¿ przej¹c zarz¹dzanie ich konwersjami i przekszta³cenie w warianty
//
// class ElementModelu::DaneLokalne
//	string* Dane;
//	unsigned 	IleDanych;

ElementModelu::DaneLokalne::~DaneLokalne()
{
	if(IleDanych!=0) delete [] Dane;
}

string& ElementModelu::DaneLokalne::operator [] (unsigned index) const
{
	if(index<IleDanych) return Dane[index];

	cerr<<_LPL("Wadliwy indeks danej","Index out of range")<<" = "<<index<<endl;
	exit(-1);
}

void ElementModelu::DaneLokalne::InicjujWgListy(const std::string* Lista,unsigned Ile)
{
	if(IleDanych!=0)
			delete [] Dane;
	Dane=new string[Ile];             		assert(Dane!=NULL);
	for(unsigned i=0;i<Ile;i++)
	{
		Dane[i]=Lista[i];
	}
	IleDanych=Ile;
}

void ElementModelu::AktualizujListeDanych()
// Funkcja musi zadbaæ ¿eby wszystko by³o aktualne
// ale klasy potomne mog¹ gdzie indziej chcieæ zapisywaæ
// dziedzinê, a to jedyne pole poza nazw¹ typu któr etu by mo¿na zaktualizowaæ
{
	if(Dane.Ile()==0)
	{
		Dane.Realokuj(1);
		WirtualnyKonstruktor* Kons=this->VKonstruktor();
		if(Kons)  	//Chyba nie warto robiæ nic wiêcej?
			Dane[0]=Kons->NazwaTypu();
	}
}

void ElementModelu::DaneLokalne::InicjujZDanych(const DaneLokalne& Wzor)
//Przekopiowywanie danych z innej listy danych
{
	 if(IleDanych!=0) delete [] Dane;
	 Dane=new string[Wzor.Ile()];   		assert(Dane!=NULL);
	 IleDanych=Wzor.Ile();
	 for(unsigned i=0;i<IleDanych;i++)
	 {
		Dane[i]=Wzor[i];
	 }
}
//const DaneLokalne& operator = (const DaneLokalne& Wzor);  - powinno byæ zabronione na wszelki wypadek


void ElementModelu::DaneLokalne::Realokuj(unsigned Ile)//Rozbudowuje lub skraca listê
{
	if(Ile==IleDanych) //Nic siê nie zmieni³o!
		return;
	string* StaraLista=Dane;
	Dane=new string[Ile];                    assert(Dane!=NULL);
	unsigned limit=min(Ile,IleDanych);
	if(limit>0)
	  for(unsigned i=0;i<limit;i++)//Nie jest to najszybszy sposób
		Dane[i]=StaraLista[i];	//ale nie wiem czy mo¿na bezpiecznie zrobiæ cos lepszego
	if(StaraLista!=NULL)
		delete [] StaraLista;
	IleDanych=Ile;
}


//Przypisania i konwersje na elementy listy
//Zwracaj¹ true jak siê uda³o
bool ElementModelu::DaneLokalne::PrzypiszZ(unsigned gdzie,double co)
{
	char bufor[512];//Du¿o za du¿o?
	if(sprintf(bufor,"%g",co)!=EOF)
	{
		Dane[gdzie]=bufor;
		return true;
	}
	else return false;
}

bool ElementModelu::DaneLokalne::PrzypiszZ(unsigned gdzie,long   co)
{
	char bufor[512];//Du¿o za du¿o?
	if(sprintf(bufor,"%lu",co)!=EOF)
	{
		Dane[gdzie]=bufor;
		return true;
	}
	else return false;
}

//Zwracaj¹ indeks znaku który nie pasowa³ lub -1 jako sukces
int ElementModelu::DaneLokalne::KonwertujDo(unsigned zkad,float& naco)
{
	double pom;
	int ret=KonwertujDo(zkad,pom);
	if(ret!=-1) return ret;
	naco=pom; //TU SIÊ MO¯E SKOPAÆ !!!
	return -1;
}

int ElementModelu::DaneLokalne::KonwertujDo(unsigned zkad,double& naco)
{
	bool procent=false;
	char  temp[512];//Du¿a tablica na stosie
	char* pom=temp;
	strncpy(temp,Dane[zkad].c_str(),511);

	if(strlen(temp)==0) //PUSTY!!!
		   { naco=0;  return 0; }

	if( strchr(temp,'.')==NULL //Gdy nie ma kropki
	  && (pom=strrchr(temp,','))!=NULL ) //To ostatni przecinek zmienia na kropkê
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

int ElementModelu::DaneLokalne::KonwertujDo(unsigned zkad,int& naco)
{
	char* endptr=NULL;
	naco=strtol(Dane[zkad].c_str(),&endptr,0);
	if(endptr!=NULL && *endptr!='\0')
		return endptr-Dane[zkad].c_str();
	else
		return -1;  //Wbrew pozorom OK
}

int ElementModelu::DaneLokalne::KonwertujDo(unsigned zkad,unsigned& naco)
{
	char* endptr=NULL;
	naco=strtoul(Dane[zkad].c_str(),&endptr,0);
	if(endptr!=NULL && *endptr!='\0')
		return endptr-Dane[zkad].c_str();
	else
		return -1;  //Wbrew pozorom OK
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

