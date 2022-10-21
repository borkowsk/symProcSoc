// Transfer technologii 2011 - w³aœciwa implementacja modelu     25-10-2011
////////////////////////////////////////////////////////////////////////////////
// Implementacja akcji klas Zespo³ow Naukowych, Administracji naukowej i
// procesów raportowania i finansowania dzia³alnoœci "statutowej"
// *TODO!!!-> i tworzenia wyników badañ (i bez technikaliów obslugi interfaceu)
////////////////////////////////////////////////////////////////////////////////

#include "opiKlasyWersja.hpp"
#include "INCLUDE/wb_smartlog.h"
using namespace wbrtm;

static const int LOCLOG=2; //Na jakim poziomie logowania domyœlnie zapisywaæ problemy z tego pliku

//Klasa wewnêtrzna skupiaj¹ca wspólne w³aœciwoœci wêz³ów z pracownikami (zespo³ów)
//////////////////////////////////////////////////////////////////////////////////
/*
class _ZespolRoboczy:public GenerycznyWezelSieci
{
 protected:
 friend class  ProcesProdukcyjny;
 friend class  ProcesRaportowany;
 friend class  ProcesSpoleczny;
//Ile Etatów	Efektywnosc	Aktywnoœæ	Centralnoœæ Probiznesowosc	Prolegalnosc	Czasochlonnosc	Innowacyjnoœc	Finans. swobodne	Finanse zaplan	D³ug do nadrz.	Udzial w zysku TT	Eksperckoœæ w dziedzinie	Uwagi
 unsigned IleEtatow;//Ile etatów. Co najmniej 1, 0 - "duch" zespo³u - plan utworzenia
 float    Efektywnosc;//0..2. Czy dzia³a na 50% wydajnoœci czy na 150%. Zale¿y od motywacji
 float    Proaktywnosc;//Aktywnoœæ? 0..1. Podtrzymywanie aktywnoœci zewnêtrznej, tworzenie linków
 float    Centralnoœæ;//0..1. Jak bardzo aktywnoœæ (zewnêtrzna?) zale¿y od szefa/szefów
 float    Doswiadczenie;//POZIOM EKSPERTYZY? Jak tego u¿ywaæ? Mo¿e to samo co efektywnoœæ???
 float    Czasochlonnosc;//Jaki jest wspó³czynnik czasoch³onnoœci typowej dzia³alnoœci. Wygenerowania pojedynczego wyniku badañ?
 float    Prolegalnosc;//????Uczciwoœæ??? Jak ³atwo podj¹æ dzia³ania "szarostrefowe"
};
*/

 
// WLASCIWA IMPLEMENTACJA TEGO CO ROBI¥ KLASY
////////////////////////////////////////////////////////////////////////////////
KomunikacjaOficjalna::KomunikacjaOficjalna(const char* Kateg,unsigned JakiAutor)
//Konstruktor z ustalonym rodzajem komunikatu
{
   Dane.Realokuj(11); Dane[9]=Kateg;
   this->Autor=JakiAutor;
}

const string& KomunikacjaOficjalna::Rodzaj()
//Gdzieœ w danych jest rodzaj tego komunikatu
{
	return Dane[9];
}

bool  KomunikacjaOficjalna::UstawRodzaj(const char* Nowy)
//A jak typ nie udostêpnia zmiany to zwraca false
{
	Dane[9]=Nowy;
	return true;
}

bool PodlegloscOrganizacyjna::Akceptacja(Komunikat* Co)
//Selekcja komunikatów
{   //Oficjalne i finansowe w obie strony
	if(dynamic_cast<KomunikacjaOficjalna*>(Co)!=NULL
	|| dynamic_cast<KomunikacjaFinansowa*>(Co)!=NULL)
	return true;
	else //A inne???
	return GenerycznePowiazanie::Akceptacja(Co);
}

void ProcesRaportowany::ChwilaDlaCiebie()
{
	GenerycznyProces::ChwilaDlaCiebie(); //Popycha tylko liczniki

	if(JakZaawansowany()>=1) //Jeœli w³aœnie zakonczony
	{
		WezelSieci* MojProc=Swiat::Wezel(this->Procesor());

		if(DoKonca()<-7)//SpóŸni³ siê wiêcej ni¿ tydzieñ
		{
			IFTL(LOCLOG+1) clog<<endl<<"\""<<MojProc->Nazwa()<<"\" "<<_LPL("z poslizgiem zaraportowal proces","with dalay reported proces")<<' '<<Dane[1]<<':'<<hex<<PodajDziedzine().ARGB<<' '<<" ("<<-DoKonca()<<_LPL(" krokow po terminie"," steps after deadline")<<")"<<endl;
			TLOG(LOCLOG , <<"\""<<MojProc->Nazwa()<<"\" "<<_LPL("z poslizgiem zaraportowal proces","with dalay reported proces")<<' '<<Dane[1]<<':'<<hex<<PodajDziedzine().ARGB<<' '<<"\t("<<-DoKonca()<<_LPL(" krokow po terminie"," steps after deadline")<<")"   )
		}

		//Udajemy, ¿e go usuwamy
		Wymazuj();

		if( _WyslijRaportOdbiorcy() ) //Jak uda siê wys³aæ raport?
		{
			//to go restartujemy po prostu!!!
			//double Zaangazowanie=(1+DRAND())*IleEtatow*7;//Jak pracoch³onne sprawozdanie to je ~1.5*7*IleEtatow...
			this->PracaDotychczasowa=0;
			this->_IleKrokowDoKonca=30+DRAND()*60;//Domyslnie nie mniej ni¿ miesi¹c nie wiêcej ni¿ trzy miesi¹ce
			this->Prior=1;

			//Tu by trzeba ustaliæ sensowniejszy czas do konca na podstawie cech wêz³a
			_ZespolRoboczy* ProcesorZR=dynamic_cast<_ZespolRoboczy*>(MojProc);
			if(ProcesorZR) //Uda³o siê sprowadziæ do tego "mianownika"
			{   //Mo¿na lepiej zaplanowaæ czas wykonania
				_IleKrokowDoKonca=ProcesorZR->Czasochlonnosc*(1+4*DRAND());
			}

			IFTL(LOCLOG+1) clog<<"\""<<MojProc->Nazwa()<<"\" "<<_LPL("reaktywowal proces","reactivated proc.")<<' '<<Dane[1]<<':'<<hex<<PodajDziedzine().ARGB<<' '<<endl;
			TLOG(LOCLOG , <<"\""<<MojProc->Nazwa()<<"\" "<<_LPL("reaktywowal proces","reactivated proc.")<<' '<<Dane[1]<<':'<<hex<<PodajDziedzine().ARGB<<dec )
		}
		else
		{   //Pewnie nie ma komu albo jak wysylac!!!
			IFTL(LOCLOG+1) clog<<"\""<<MojProc->Nazwa()<<"\" "<<_LPL("ZLIKWIDOWAL proces","reactivated proc.")<<' '<<Dane[1]<<':'<<hex<<PodajDziedzine().ARGB<<dec<<' '<<endl;
			TLOG(LOCLOG , <<"\""<<MojProc->Nazwa()<<"\" "<<_LPL("ZLIKWIDOWAL proces","reactivated proc.")<<' '<<Dane[1]<<':'<<hex<<PodajDziedzine().ARGB<<dec  )
			UznajZaZakonczony(); //NAPRAWDÊ kasujemy...
		}
	}
}

bool ProcesRaportowany::_WyslijRaportOdbiorcy()
{
	//Generuje raport
	KomunikacjaOficjalna* KO=new KomunikacjaOficjalna("RAPORT",Procesor());
	KO->UstawDziedzine(PodajDziedzine());
	WezelSieci* MojProcesor=Swiat::Wezel(this->Procesor());

	unsigned Odbiorca=-1;//Preferowany odbiorca
	if(IlePolDanych()>=9 || strlen(WartoscPola(8))!=0)
	{
		Odbiorca=Swiat::ZnajdzIndeksWezla(WartoscPola(8));
	}

	if(Odbiorca!=Swiat::INVINDEX)
	{
	   if(! KO->Zaadresuj(Procesor(),Odbiorca,0.05))
	   {
		  WezelSieci* Adresat=Swiat::Wezel(Odbiorca);
		  clog<<endl<<"\""<<MojProcesor->Nazwa()<<"\"-->\""<<(Adresat?Adresat->Nazwa():"???")<<"\" NIEUDANE ADRESOWANIE NIE POWINNO SIE ZDARZAC!!!???"<<endl;
		  TLOG(0, <<"\""<<MojProcesor->Nazwa()<<"\"-->\""<<(Adresat?Adresat->Nazwa():"???")<<"\" NIEUDANE ADRESOWANIE NIE POWINNO SIÊ ZDARZAC!!!???"   )
		  goto ERROR; //Przeskakujemy wysylanie choæ to raczej siê nie powinno zdarzaæ
	   }
	}
	else
	{  	//Niezdefiniowany odbiorca - dosyæ skomplikowane szukanie adresata raportu.
		unsigned Kanal=-1;
		Powiazanie* LN=NULL;
		PodlegloscOrganizacyjna* P=NULL;
		for(unsigned i=0;i<Swiat::IlePowiazan();i++)
			if((LN=Swiat::Lacze(i))!=NULL
			&&(LN->Poczatek()==Procesor())
			&&(P=dynamic_cast<PodlegloscOrganizacyjna*>(LN))!=NULL
			)
		{
				Kanal=i;
				break;
		}

		if(Kanal!=Swiat::INVINDEX) //Kana³ znaleziony, mo¿na wyslaæ
		  if(! KO->Zaadresuj(Kanal,true,0.05) )
		  {
			  WezelSieci* Adresat=Swiat::Wezel(LN->Koniec());   	assert(Adresat!=NULL);
			  clog<<endl<<"\""<<MojProcesor->Nazwa()<<"\"-->\""<<(Adresat?Adresat->Nazwa():"???")<<"\" NIEUDANE ADRESOWANIE NIE POWINNO SIE ZDARZAC!!!???"<<endl;
			  TLOG(0, <<"\""<<MojProcesor->Nazwa()<<"\"-->\""<<(Adresat?Adresat->Nazwa():"???")<<"\" NIEUDANE ADRESOWANIE NIE POWINNO SIÊ ZDARZAC!!!???"   )
			  goto ERROR; //Przeskakujemy wysylanie
		  }
	}

	if(Swiat::WstawInfo(KO)!=Swiat::INVINDEX)
	{
	   IFTL(LOCLOG+1) clog<<endl<<_LPL("Wyslano raport o ","Sending report about ")<<Dane[1]<<_LPL(" z "," from ")<<MojProcesor->Nazwa()<<_LPL(" do "," to ")<<'"'<<WartoscPola(8)<<'"'<<endl;
	   TLOG(LOCLOG ,      <<_LPL("Wyslano raport o ","Sending report about ")<<Dane[1]<<_LPL(" z "," from ")<<MojProcesor->Nazwa()<<_LPL(" do "," to ")<<'"'<<WartoscPola(8)<<'"'  )
	   return true;
	}
	else
	{  ERROR: //Ni uda³o siê adresowanie to i wstawienie siê nie uda!!!
	   IFTL(LOCLOG-1) clog<<endl<<_LPL("NIE wyslano raportu o ","Failed to send report about ")<<Dane[1]<<_LPL(" z "," from ")<<MojProcesor->Nazwa()<<endl;
	   TLOG(LOCLOG-1,      <<_LPL("NIE wyslano raportu o ","Failed to send report about ")<<Dane[1]<<_LPL(" z "," from ")<<MojProcesor->Nazwa() )
	   return false;
	}
}

bool ProcesRaportowany::InterpretujKomunikat(Komunikat* Co)
//Przesyla dalej raporty, wysy³a kase, inkasuje kasê
{
   	KomunikacjaFinansowa* KFI=dynamic_cast<KomunikacjaFinansowa*>(Co);
	if(KFI!=NULL && KFI->Rodzaj()==KONTO)  //Kwestie finansowe, np. finansowe od jednostki nadrzêdnej
	{
		//Tu mo¿e zwiêkszyæ swój fundusz lub zmniejszyæ
		float wplyw=KFI->JakaKwota();
		_ZespolRoboczy* ZR=dynamic_cast<_ZespolRoboczy*>(Swiat::Wezel(Procesor()));
		if(ZR) //Tylko wtedy gdy ZespólRoboczy to proces wie jak przekazaæ mu kasê
			  ZR->PrzyjmijKase(wplyw);
		return true; //Przetworzony!
	}
	return false; //Niech siê ktoœ inny martwi
}

void AdministracjaNaukowa::InterpretujKomunikat(Komunikat* Co)
//Przyjmowanie komunikatów
{
	if(_KomunikatDoProcesow(Co)) //Jeœli któryœ z procesów obrobi³
				return ; //To ju¿ nic nie robimy

	//Jeœli pozostaly nieprzetworzone raporty
	KomunikacjaOficjalna* OKO=NULL;
	if((OKO=dynamic_cast<KomunikacjaOficjalna*>(Co))!=NULL && OKO->Rodzaj()=="RAPORT" )
	{
		//Testowanie kana³u - tylko raporty z do³u wymagaj¹ przes³ania kasy -
		//jak góra przesy³a raport to sobie poczytasz i tyle, albo jak dó³ ale
		//nieoficjalnym kana³em
		unsigned iKan=Co->Kanal();
		Powiazanie* pKan=Swiat::Lacze(iKan);
		if( pKan->Koniec()!=this->MojID() //Nie w t¹ stronê
		|| dynamic_cast<PodlegloscOrganizacyjna*>(pKan)==NULL //£acze niew³aœciwego typu do przekazów finansowych
		)
		return;  //NIE TYM RAZEM

		//Finansowanie w odpowiedzi na raport z dol
		KomunikacjaFinansowa* KFI=new KomunikacjaFinansowa(KONTO,JEDNOSTKOWA_WYPLATA);
		this->FinanseSwobodne-=JEDNOSTKOWA_WYPLATA;
		if(KFI->Zaadresuj(iKan,!Co->KierunekZgodny(),0.07)
		   &&  (Swiat::WstawInfo(KFI)!=Swiat::INVINDEX)  )
				; //OK
				else
				{
					WezelSieci* W=Swiat::Wezel(OKO->Nadawca()); //Np. linkiem administracyjnym "pod pr¹d"
					IFTL(LOCLOG) clog<<endl <<"\""<<Nazwa()<<"\" nie mo¿e przes³aæ finansowania w zamian za raport: "<<(W?W->Nazwa():"???")<<endl;
					TLOG(LOCLOG, <<"\""<<Nazwa()<<"\" nie mo¿e przes³aæ finansowania w zamian za raport: "<<(W?W->Nazwa():"???")  )
				}
	   //Przesy³aæ raporty dalej, jak jest do kogo
	   Komunikat* NKO=OKO->Klonuj();
	   //Dosyæ skomplikowane szukanie adresata. Trzeba by zrobiæ lepsz¹ obslugê linków w³asnych wêz³a, ale nie mam pomys³u na interface
	   unsigned Kanal=-1;
	   for(unsigned i=0;i<Swiat::IlePowiazan();i++)
	   {
			PodlegloscOrganizacyjna* P=dynamic_cast<PodlegloscOrganizacyjna*>(Swiat::Lacze(i));
			if(P!=NULL && P->Poczatek()==MojID())
					{Kanal=i;break;}
	   }
	   if(Kanal!=Swiat::INVINDEX && NKO->Zaadresuj(Kanal,true,0.07)
		  && (Swiat::WstawInfo(NKO)!=Swiat::INVINDEX) )
				 ;//OK
				 else
				 TLOG(LOCLOG, <<"\""<<Nazwa()<<"\" nie mo¿e przes³aæ raportu dalej" )
	}
}

void AdministracjaNaukowa::ChwilaDlaCiebie()
{
	//Np. na posuniêcie do przodu procesów
	//_ZarzadcaProcesowLeniwy();  - o wiele za ma³o efektywny, du¿e opoŸnienia w zwyk³ych raportach
	if(this->Doswiadczenie<0.75)
		this->_ZarzadcaProcesowFantazyjny();
		else
		this->_ZarzadcaProcesowFinezyjny();
}


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

/*
class ProcesBadawczy:public GenerycznyProces
{
   //Implementacja tego co musi byæ wg. interfejsu typu Proces
	friend class KonstruktorElementowModelu<ProcesBadawczy>;
	static KonstruktorElementowModelu<ProcesBadawczy> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
	public:
	//Konstruktor z parametrami do tworzenia procesu w kodzie
	enum EfektNaukowy { NIEWIEM=-1, PUBLIKACJA, PATENT, RAPORT };
	ProcesBadawczy(EfektNaukowy RodzajProd=-1,unsigned ZaplanowanyOdbiorca=-1);//Mo¿e od razu znaæ odbiorce i rodzaj produktu albo nie

	void ChwilaDlaCiebie(); //Endogenne zmiany stanów, popychanie pracy do przodu itp.
							//Na koniec tworzy swoj¹ kopie i sibie przeznacza do skasowania
	bool InterpretujKomunikat(Komunikat* Co);//Przyjmowanie komunikatów
	protected: //Ukryty przed innymi konstruktor domyslny wykonuje robotê za pomoc¹ domyœnego GenerycznyProces()
	ProcesBadawczy(){ ZaplanowanyEfekt=NIEWIEM; ZaplOdbiorca=-1;}
	virtual float _KalkulujKosztBadan(); //Furtka do "realistycznego" kalkulowania kosztów badañ
	bool _WyslijInformacje(unsigned link=-1); //Do rozsylania informacji ¿e siê czyms takim zajmujemy - socjalna lub oficjalna
	bool _WyslijPublikacje(); //Pomocnicza procedura publikowania - domyslnie szuka wêz³a PUBLIKATOR
	bool _WyslijPatent();     //Pomocnicza procedura publikowania - domyslnie szuka wêz³a UPATENTOWY, publikacja wyklucza patent
	bool _WyslijRaport();     //Pomocnicza procedura raportowania - do odbiorcy albo administracji jak brak
	//G³ówna specyficzna zmienna
	private:
	EfektNaukowy ZaplanowanyEfekt;//Jakiego rodzaju efekt ma tu byæ
	unsigned     ZaplOdbiorca;//Czy znany jest odbiorca i kto to
};
*/

	/*
void AdministracjaNaukowa::ChwilaDlaCiebie()
{
	//Np. na posuniêcie do przodu procesów

	unsigned ile=Swiat::IleProcesow(MojID());
	unsigned ktory=Swiat::NajpilniejszyProc(MojID());

	if(ktory==Swiat::INVINDEX) //Nie ma nic do roboty
			return; //To wychodzimy z pracy :-)

	GenerycznyProces* Roboczy=dynamic_cast<GenerycznyProces*>(Swiat::Proc(ktory,MojID()));
	if(Roboczy)
	{
		if(Roboczy->Prior==0)
					return; //Duuuu¿o czasu do terminu...
		//Trzeba popchn¹æ - nie wiêcej ni¿ o maksymalny dzieñ pracy wszystkich
		//ale zazwyczaj mniej.
		double OIle=std::min(this->IleEtatow*this->Efektywnosc,Roboczy->Prior);
		Roboczy->TmpPorcjaPracy=OIle;
		Roboczy->ChwilaDlaCiebie(); //Popchnij
	}

}
*/
	/*   STARA WERSJA
void JednostkaBadawcza::ChwilaDlaCiebie()
{
	//Np. na posuniêcie do przodu procesów
	// do zmiany na: _ZarzadcaProcesowFantazyjny();

	unsigned ile_proc_real; //Ile faktycznie ma procesów do obslu¿enia
	unsigned ktory=Swiat::NajpilniejszyProc(MojID(),&ile_proc_real);
	if(ktory==Swiat::INVINDEX) //Nie ma w ogole nic do roboty
			return; //To wychodzimy z pracy :-)
	GenerycznyProces* Roboczy=dynamic_cast<GenerycznyProces*>(Swiat::Proc(ktory,MojID()));
															 assert(Roboczy!=NULL);

	if(ile_proc_real>1 &&  (???Roboczy->Prior==0??? || DRAND()>0.666) ) //Czasem z fantazji zmieniamy na inny,
								//ale te¿ czasem popychamy,nawet jak deleko do deadlinu
	{
		unsigned ile_proc=Swiat::IleProcesow(MojID());
		do{
		ktory=RANDOM(ile_proc);
		Roboczy=dynamic_cast<GenerycznyProces*>(Swiat::Proc(ktory,MojID()));
		}while(Roboczy==NULL);
	}

	if(Roboczy)  //Móg³ siê zmieniæ na pusty?
	{
		if(Roboczy->JakZaawansowany()<1) //Nie zakonczony
		{
			//Trzeba popchn¹æ - nie wiêcej ni¿ o maksymalny dzieñ pracy wszystkich
			//ale pewnie zazwyczaj mniej???
			double OIle=IleEtatow*Efektywnosc;
			if(Roboczy->Prior==0) //Jak bardzo daleko do deadlineu
				OIle/=2; //Tak coœ tam podlubie, ale bez egzaltacji
			Roboczy->TmpPorcjaPracy=OIle;
			Roboczy->ChwilaDlaCiebie(); //Popchnij
		}
	}
}*/

