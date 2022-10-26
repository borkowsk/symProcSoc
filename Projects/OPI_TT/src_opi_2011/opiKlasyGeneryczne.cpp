// Transfer technologii 2011
////////////////////////////////////////////////////////////////////////////////
// Testowa dynamika dla obiektów generycznych - wêz³ów i linków. Komunikaty s¹
// z definicji bierne - "p³yn¹" nios¹c informacje. Mo¿na to co prawda zmieniæ,
// ale dopiero w klasach potomnych.
////////////////////////////////////////////////////////////////////////////////
// Klasy generyczne takie jak GenericNode czy GenericLink powinny mieæ swoj¹
// domyœln¹ dynamikê odpowiedni¹ do badanego modelu. W³aœciwie ¿eby
// stworzyæ najprostrzy model wystarczy po prostu reimplementowac ten plik
// W przypadku modelu OPI klasy s¹ tak pomyœlane, ¿eby wykonywa³y procesy
// i przesy³a³y komunikaty z namno¿eniem.
////////////////////////////////////////////////////////////////////////////////
#include "spsModel.h"
#include "spsGenNode.h"
#include "spsGenProc.h"
#include "spsGenLink.h"
#include "spsGenInfo.h"
#include "spsMatrixNode.h"
#include "INCLUDE/wb_ptr.hpp"
#include "INCLUDE/wb_smartlog.h"
using namespace wbrtm;

float GENERYCZNY_POZIOM_PRZECIEKOW=0.01;//Jak czesto domyslny komunikat leci dalej ni¿ do odbiorcy

void GenerycznyWezelSieci::InterpretujKomunikat(Komunikat* Co)
//Najpierw proponuje do obróbki swoim procesom, a jak siê nie uda to z rzadka
//"brodkastuje" komunikat losowo do dwu  lub wiêcej s¹siadów o ile ma
//co najmniej dwa polaczenia.
{
	if(_KomunikatDoProcesow(Co)) //Jeœli któryœ z procesów obrobi³
				return ; //To ju¿ nic nie robimy
	//Ta funkcja jest dosyæ kosztowna i intensywnie namna¿a komunikaty
	if(DRAND()<GENERYCZNY_POZIOM_PRZECIEKOW) //Wiec robimy to z rzadka
		_KomunikatPrzekazLosowo(Co,GENERYCZNY_POZIOM_PRZECIEKOW);//Przekazuje dalej, jeszcze rzadziej dwu odbiorcom
	//return; //DEBUG!!!!!!!!!!!!!!!!!!!!!
}

void WezelMacierzowy::InterpretujKomunikat(Komunikat* Co)
//Interpretuje komunikat  -  odpowiada dziedzin¹ losowego elementu
//albo mo¿e sprawdzaæ czy pytanie o konkretny wêze³
//albo mo¿e by szuka³ najbardziej podobnego do Col komunikatu?
{
	if(_KomunikatDoProcesow(Co)) //Jeœli któryœ z procesów obrobi³
				return ; //To ju¿ nic nie robimy - tak jak generic

	//Losowe zapamiêtanie w maciezy
	unsigned Gdzie=RANDOM(this->Szerokosc*this->Wysokosc);
	Tablica[Gdzie]=Co->PodajDziedzine();

	//Na inne zazwyczaj odpowiadamy na podstawie losowo wybranego elementu
	if(DRAND()<(1-GENERYCZNY_POZIOM_PRZECIEKOW) //Czasem zamiast odwpowiadaæ rozsy³a dalej domyslnym algorytmem
		&& Tablica)
	{
		_OdpowiedzLosowymBitem(Co,Swiat::INVINDEX,true);//ostatnie to AND_OR
	}
	//Rozsylanie ...
	else _KomunikatPrzekazLosowo(Co,GENERYCZNY_POZIOM_PRZECIEKOW);//Przekazuje dalej, z rzadka dwum odbiorcom
}

void GenerycznyWezelSieci::ChwilaDlaCiebie()
//Daje mu szanse na endogenne zmiany stanów
{
	this->ZarzadcaProcesowPrymitywny();
}

void WezelMacierzowy::ChwilaDlaCiebie()
//Daje jakies szanse na endogenne zmiany stanów
{
	//Mo¿e nap. mieæ w³asn¹ dynamikê
	//Ale te¿ powinien robiæ coœ jak klasa macie¿ysta - np. popychaæ procesy
	//GenerycznyWezelSieci::ChwilaDlaCiebie(); //Likwidacja chwilowo zbêdnego wywo³ania
	this->ZarzadcaProcesowPrymitywny();//Tu jest ta sama zawartoœæ!!!
}

void GenerycznePowiazanie::ChwilaDlaCiebie()
//Mo¿e daj mu szanse na endogenne zmiany stanów
{
	//Tu jednak nie robi nic...
	//bo niby co mia³by robiæ co jest sensowne dla ka¿dego potomnego typu!?!?
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

		 //Przejêcie informacji
		 unsigned Bit=RANDOM(24); //Który bit
		 Bit=1<<Bit;
		 if(Bit&D.ARGB)    //Bierze stan tego bitu
			Col.ARGB|=Bit; //i nak³ada go jeœli 1
			else //Kasuje jeœli 0
			Col.ARGB&=~Bit;

		 //Teraz mutacja komunikatu
		 Bit=RANDOM(64);//Ma spore szanse ¿e siê w ogóle nie zmieni
		 if(Bit<32)
		 {
			Bit=1<<Bit;
			D.ARGB^=Bit;
		 }
		 Klon->UstawDziedzine(D);
*/
