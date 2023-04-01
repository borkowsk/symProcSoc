// Transfer technologii 2011
////////////////////////////////////////////////////////////////////////////////
// Testowa dynamika dla obiekt�w generycznych - w�z��w i link�w. Komunikaty s�
// z definicji bierne - "p�yn�" nios�c informacje. Mo�na to co prawda zmieni�,
// ale dopiero w klasach potomnych.
////////////////////////////////////////////////////////////////////////////////
// Klasy generyczne takie jak GenericNode czy GenericLink powinny mie� swoj�
// domy�ln� dynamik� odpowiedni� do badanego modelu. W�a�ciwie �eby
// stworzy� najprostrzy model wystarczy po prostu reimplementowac ten plik
// W przypadku modelu OPI klasy s� tak pomy�lane, �eby wykonywa�y procesy
// i przesy�a�y komunikaty z namno�eniem.
////////////////////////////////////////////////////////////////////////////////
#include "spsModel.h"
#include "spsGenNode.h"
#include "spsGenProc.h"
#include "spsGenLink.h"
#include "spsGenInfo.h"
#include "spsMatrixNode.h"
#include "wb_ptr.hpp"
#include "wb_smartlog.hpp"
using namespace wbrtm;

float GENERYCZNY_POZIOM_PRZECIEKOW=0.01;//Jak czesto domyslny komunikat leci dalej ni� do odbiorcy

void GenerycznyWezelSieci::InterpretujKomunikat(Komunikat* Co)
//Najpierw proponuje do obr�bki swoim procesom, a jak si� nie uda to z rzadka
//"brodkastuje" komunikat losowo do dwu  lub wi�cej s�siad�w o ile ma
//co najmniej dwa polaczenia.
{
	if(_KomunikatDoProcesow(Co)) //Je�li kt�ry� z proces�w obrobi�
				return ; //To ju� nic nie robimy
	//Ta funkcja jest dosy� kosztowna i intensywnie namna�a komunikaty
	if(DRAND()<GENERYCZNY_POZIOM_PRZECIEKOW) //Wiec robimy to z rzadka
		_KomunikatPrzekazLosowo(Co,GENERYCZNY_POZIOM_PRZECIEKOW);//Przekazuje dalej, jeszcze rzadziej dwu odbiorcom
	//return; //DEBUG!!!!!!!!!!!!!!!!!!!!!
}

void WezelMacierzowy::InterpretujKomunikat(Komunikat* Co)
//Interpretuje komunikat  -  odpowiada dziedzin� losowego elementu
//albo mo�e sprawdza� czy pytanie o konkretny w�ze�
//albo mo�e by szuka� najbardziej podobnego do Col komunikatu?
{
	if(_KomunikatDoProcesow(Co)) //Je�li kt�ry� z proces�w obrobi�
				return ; //To ju� nic nie robimy - tak jak generic

	//Losowe zapami�tanie w maciezy
	unsigned Gdzie=RANDOM(this->Szerokosc*this->Wysokosc);
	Tablica[Gdzie]=Co->PodajDziedzine();

	//Na inne zazwyczaj odpowiadamy na podstawie losowo wybranego elementu
	if(DRAND()<(1-GENERYCZNY_POZIOM_PRZECIEKOW) //Czasem zamiast odwpowiada� rozsy�a dalej domyslnym algorytmem
		&& Tablica)
	{
		_OdpowiedzLosowymBitem(Co, Swiat::INV_INDEX, true);//ostatnie to AND_OR
	}
	//Rozsylanie ...
	else _KomunikatPrzekazLosowo(Co,GENERYCZNY_POZIOM_PRZECIEKOW);//Przekazuje dalej, z rzadka dwum odbiorcom
}

void GenerycznyWezelSieci::ChwilaDlaCiebie()
//Daje mu szanse na endogenne zmiany stan�w
{
	this->ZarzadcaProcesowPrymitywny();
}

void WezelMacierzowy::ChwilaDlaCiebie()
//Daje jakies szanse na endogenne zmiany stan�w
{
	//Mo�e nap. mie� w�asn� dynamik�
	//Ale te� powinien robi� co� jak klasa macie�ysta - np. popycha� procesy
	//GenerycznyWezelSieci::ChwilaDlaCiebie(); //Likwidacja chwilowo zb�dnego wywo�ania
	this->ZarzadcaProcesowPrymitywny();//Tu jest ta sama zawarto��!!!
}

void GenerycznePowiazanie::ChwilaDlaCiebie()
//Mo�e daj mu szanse na endogenne zmiany stan�w
{
	//Tu jednak nie robi nic...
	//bo niby co mia�by robi� co jest sensowne dla ka�dego potomnego typu!?!?
}


/********************************************************************/
/*			          SPS/OPI  version 2011                         */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://wwww.iss.uw.edu.pl/borkowski/                */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/

/*
		 //Proste mutowanie dziedziny odbiorcy na skutek komunikatu
		 //i losowa zmiana komunikatu
		 DziedzKol D=Klon->PodajDziedzine();

		 //Przej�cie informacji
		 unsigned Bit=RANDOM(24); //Kt�ry bit
		 Bit=1<<Bit;
		 if(Bit&D.ARGB)    //Bierze stan tego bitu
			Col.ARGB|=Bit; //i nak�ada go je�li 1
			else //Kasuje je�li 0
			Col.ARGB&=~Bit;

		 //Teraz mutacja komunikatu
		 Bit=RANDOM(64);//Ma spore szanse �e si� w og�le nie zmieni
		 if(Bit<32)
		 {
			Bit=1<<Bit;
			D.ARGB^=Bit;
		 }
		 Klon->UstawDziedzine(D);
*/
