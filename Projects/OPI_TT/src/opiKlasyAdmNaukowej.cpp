/// \file
/// \property Transfer technologii 2011
/// \brief  w³aœciwa implementacja modelu     25-10-2011
//*/////////////////////////////////////////////////////////////////////////////
/// \details
///     Implementacja akcji klas Zespo³ow Naukowych, Administracji naukowej i
///     procesów raportowania i finansowania dzia?alno?ci "statutowej"
///     TODO!!!-> i tworzenia wynik?w bada? (i bez technikaliów obslugi interface'u)
//*/////////////////////////////////////////////////////////////////////////////////

#include "opiKlasyWersja.hpp"
#include "wb_smartlog.hpp"
#include "lingo.hpp"
using namespace wbrtm;

static const int LOCLOG=2; //Na jakim poziomie logowania domy?lnie zapisywa? problemy z tego pliku

//  Klasa wewn?trzna skupiaj?ca wsp?lne w?a?ciwo?ci w?z??w z pracownikami (zespo??w)
//*////////////////////////////////////////////////////////////////////////////////
/*
class _ZespolRoboczy:public GenerycznyWezelSieci
{
 protected:
 friend class  ProcesProdukcyjny;
 friend class  ProcesRaportowany;
 friend class  ProcesSpoleczny;
//Ile Etat?w	Efektywnosc	Aktywno??	Centralno?? Probiznesowosc	Prolegalnosc	Czasochlonnosc	Innowacyjno?c	Finans. swobodne	Finanse zaplan	D?ug do nadrz.	Udzial w zysku TT	Ekspercko?? w dziedzinie	Uwagi
 unsigned IleEtatow;//Ile etat?w. Co najmniej 1, 0 - "duch" zespo?u - plan utworzenia
 float    Efektywnosc;//0..2. Czy dzia?a na 50% wydajno?ci czy na 150%. Zale?y od motywacji
 float    Proaktywnosc;//Aktywno??? 0..1. Podtrzymywanie aktywno?ci zewn?trznej, tworzenie link?w
 float    Centralno??;//0..1. Jak bardzo aktywno?? (zewn?trzna?) zale?y od szefa/szef?w
 float    Doswiadczenie;//POZIOM EKSPERTYZY? Jak tego u?ywa?? Mo?e to samo co efektywno?????
 float    Czasochlonnosc;//Jaki jest wsp?czynnik czasoch?onno?ci typowej dzia?alno?ci. Wygenerowania pojedynczego wyniku bada??
 float    Prolegalnosc;//????Uczciwo????? Jak ?atwo podj?? dzia?ania "szarostrefowe"
};
*/

 
//  W£AŒCIWA IMPLEMENTACJA TEGO CO ROBI? KLASY
//*//////////////////////////////////////////////////////////////////////////////
KomunikacjaOficjalna::KomunikacjaOficjalna(const char* Kateg,unsigned JakiAutor)
//Konstruktor z ustalonym rodzajem komunikatu
{
   Dane.Realokuj(11); Dane[9]=Kateg;
   this->Autor=JakiAutor;
}

const string& KomunikacjaOficjalna::Rodzaj()
//Gdzie? w danych jest rodzaj tego komunikatu
{
	return Dane[9];
}

bool  KomunikacjaOficjalna::UstawRodzaj(const char* Nowy)
//A jak typ nie udost?pnia zmiany to zwraca false
{
	Dane[9]=Nowy;
	return true;
}

bool PodlegloscOrganizacyjna::Akceptacja(Komunikat* Co)
//Selekcja komunikat?w
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

	if(JakZaawansowany()>=1) //Je?li w?a?nie zakoñczony
	{
		WezelSieci* MojProc=Swiat::Wezel(this->Procesor());

		if(DoKonca()<-7)//Sp?ni? si? wi?cej ni? tydzie?
		{
			IFTL(LOCLOG+1) clog<<endl<<"\""<<MojProc->Nazwa()<<"\" "<<_LPL("z poœlizgiem zaraportowa³ proces","with delay reported proces")<<' '<<Dane[1]<<':'<<hex<<PodajDziedzine().ARGB<<' '<<" ("<<-DoKonca()<<_LPL(" krokow po terminie"," steps after deadline")<<")"<<endl;
			TLOG(LOCLOG , <<"\""<<MojProc->Nazwa()<<"\" "<<_LPL("z poœlizgiem zaraportowa³ proces","with delay reported proces")<<' '<<Dane[1]<<':'<<hex<<PodajDziedzine().ARGB<<' '<<"\t("<<-DoKonca()<<_LPL(" krokow po terminie"," steps after deadline")<<")"   )
		}

		//Udajemy, ?e go usuwamy
		Wymazuj();

		if( _WyslijRaportOdbiorcy() ) //Jak uda si? wys?a? raport?
		{
			//to go restartujemy po prostu!!!
			//double Zaangazowanie=(1+DRAND())*IleEtatow*7; //Jak pracoch³onne sprawozdanie to je ~1.5*7*IleEtatow...
			this->PracaDotychczasowa=0;
			this->_IleKrokowDoKonca=30+DRAND()*60; //Domyœlnie nie mniej ni? miesi?c nie wi?cej ni? trzy miesi?ce
			this->Prior=1;

			//Tu by trzeba ustali? sensowniejszy czas do konca na podstawie cech w?z?a
			_ZespolRoboczy* ProcesorZR=dynamic_cast<_ZespolRoboczy*>(MojProc);
			if(ProcesorZR) //Uda?o si? sprowadzi? do tego "mianownika"
			{   //Mo?na lepiej zaplanowa? czas wykonania
				_IleKrokowDoKonca=ProcesorZR->Czasochlonnosc*(1+4*DRAND());
			}

			IFTL(LOCLOG+1) clog<<"\""<<MojProc->Nazwa()<<"\" "<<_LPL("reaktywowany proces","reactivated proc.")<<' '<<Dane[1]<<':'<<hex<<PodajDziedzine().ARGB<<' '<<endl;
			TLOG(LOCLOG , <<"\""<<MojProc->Nazwa()<<"\" "<<_LPL("reaktywowany proces","reactivated proc.")<<' '<<Dane[1]<<':'<<hex<<PodajDziedzine().ARGB<<dec )
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

	if(Odbiorca!=Swiat::INV_INDEX)
	{
	   if(! KO->Zaadresuj(Procesor(),Odbiorca,0.05))
	   {
		  WezelSieci* Adresat=Swiat::Wezel(Odbiorca);
		  clog<<endl<<"\""<<MojProcesor->Nazwa()<<"\"-->\""<<(Adresat?Adresat->Nazwa():"???")<<"\" NIEUDANE ADRESOWANIE NIE POWINNO SIE ZDARZAC!!!???"<<endl;
		  TLOG(0, <<"\""<<MojProcesor->Nazwa()<<"\"-->\""<<(Adresat?Adresat->Nazwa():"???")<<"\" NIEUDANE ADRESOWANIE NIE POWINNO SI? ZDARZAC!!!???"   )
		  goto ERROR; //Przeskakujemy wysy³anie cho? to raczej si? nie powinno zdarza?
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

		if(Kanal!=Swiat::INV_INDEX) //Kana? znaleziony, mo?na wysla?
		  if(! KO->Zaadresuj(Kanal,true,0.05) )
		  {
			  WezelSieci* Adresat=Swiat::Wezel(LN->Koniec());   	assert(Adresat!=NULL);
			  clog<<endl<<"\""<<MojProcesor->Nazwa()<<"\"-->\""<<(Adresat?Adresat->Nazwa():"???")<<"\" NIEUDANE ADRESOWANIE NIE POWINNO SIE ZDARZAC!!!???"<<endl;
			  TLOG(0, <<"\""<<MojProcesor->Nazwa()<<"\"-->\""<<(Adresat?Adresat->Nazwa():"???")<<"\" NIEUDANE ADRESOWANIE NIE POWINNO SI? ZDARZAC!!!???"   )
			  goto ERROR; //Przeskakujemy wysylanie
		  }
	}

	if(Swiat::WstawInfo(KO)!=Swiat::INV_INDEX)
	{
	   IFTL(LOCLOG+1) clog<<endl<<_LPL("Wyslano raport o ","Sending report about ")<<Dane[1]<<_LPL(" z "," from ")<<MojProcesor->Nazwa()<<_LPL(" do "," to ")<<'"'<<WartoscPola(8)<<'"'<<endl;
	   TLOG(LOCLOG ,      <<_LPL("Wyslano raport o ","Sending report about ")<<Dane[1]<<_LPL(" z "," from ")<<MojProcesor->Nazwa()<<_LPL(" do "," to ")<<'"'<<WartoscPola(8)<<'"'  )
	   return true;
	}
	else
	{  ERROR: //Ni uda?o si? adresowanie to i wstawienie si? nie uda!!!
	   IFTL(LOCLOG-1) clog<<endl<<_LPL("NIE wyslano raportu o ","Failed to send report about ")<<Dane[1]<<_LPL(" z "," from ")<<MojProcesor->Nazwa()<<endl;
	   TLOG(LOCLOG-1,      <<_LPL("NIE wyslano raportu o ","Failed to send report about ")<<Dane[1]<<_LPL(" z "," from ")<<MojProcesor->Nazwa() )
	   return false;
	}
}

bool ProcesRaportowany::InterpretujKomunikat(Komunikat* Co)
//Przesy³a dalej raporty, wysy³a kasê, inkasuje kasê
{
   	KomunikacjaFinansowa* KFI=dynamic_cast<KomunikacjaFinansowa*>(Co);
	if(KFI!=NULL && KFI->Rodzaj()==KONTO)  //Kwestie finansowe, np. finansowe od jednostki nadrz?dnej
	{
		//Tu mo?e zwi?kszy? sw?j fundusz lub zmniejszy?
		float wplyw=KFI->JakaKwota();
		_ZespolRoboczy* ZR=dynamic_cast<_ZespolRoboczy*>(Swiat::Wezel(Procesor()));
		if(ZR) //Tylko wtedy gdy Zesp?lRoboczy to proces wie jak przekaza? mu kas?
			  ZR->PrzyjmijKase(wplyw);
		return true; //Przetworzony!
	}
	return false; //Niech si? kto? inny martwi
}

void AdministracjaNaukowa::InterpretujKomunikat(Komunikat* Co)
//Przyjmowanie komunikat?w
{
	if(_KomunikatDoProcesow(Co)) //Je?li kt?ry? z proces?w obrobi?
				return ; //To ju? nic nie robimy

	//Je?li pozostaly nieprzetworzone raporty
	KomunikacjaOficjalna* OKO=NULL;
	if((OKO=dynamic_cast<KomunikacjaOficjalna*>(Co))!=NULL && OKO->Rodzaj()=="RAPORT" )
	{
		//Testowanie kana?u - tylko raporty z do?u wymagaj? przes?ania kasy -
		//jak g?ra przesy?a raport to sobie poczytasz i tyle, albo jak d? ale
		//nieoficjalnym kana?em
		unsigned iKan=Co->Kanal();
		Powiazanie* pKan=Swiat::Lacze(iKan);
		if( pKan->Koniec()!=this->MojID() //Nie w t? stron?
		|| dynamic_cast<PodlegloscOrganizacyjna*>(pKan)==NULL //?acze niew?a?ciwego typu do przekaz?w finansowych
		)
		return;  //NIE TYM RAZEM

		//Finansowanie w odpowiedzi na raport z dol
		KomunikacjaFinansowa* KFI=new KomunikacjaFinansowa(KONTO,JEDNOSTKOWA_WYPLATA);
		this->FinanseSwobodne-=JEDNOSTKOWA_WYPLATA;
		if(KFI->Zaadresuj(iKan,!Co->KierunekZgodny(),0.07)
		   &&  (Swiat::WstawInfo(KFI)!=Swiat::INV_INDEX)  )
				; //OK
				else
				{
					WezelSieci* W=Swiat::Wezel(OKO->Nadawca()); //Np. linkiem administracyjnym "pod pr?d"
					IFTL(LOCLOG) clog<<endl <<"\""<<Nazwa()<<"\" nie mo?e przes?a? finansowania w zamian za raport: "<<(W?W->Nazwa():"???")<<endl;
					TLOG(LOCLOG, <<"\""<<Nazwa()<<"\" nie mo?e przes?a? finansowania w zamian za raport: "<<(W?W->Nazwa():"???")  )
				}
	   //Przesy?a? raporty dalej, jak jest do kogo
	   Komunikat* NKO=OKO->Klonuj();
	   //Dosy? skomplikowane szukanie adresata. Trzeba by zrobi? lepsz? obslug? link?w w?asnych w?z?a, ale nie mam pomys?u na interface
	   unsigned Kanal=-1;
	   for(unsigned i=0;i<Swiat::IlePowiazan();i++)
	   {
			PodlegloscOrganizacyjna* P=dynamic_cast<PodlegloscOrganizacyjna*>(Swiat::Lacze(i));
			if(P!=NULL && P->Poczatek()==MojID())
					{Kanal=i;break;}
	   }
	   if(Kanal!=Swiat::INV_INDEX && NKO->Zaadresuj(Kanal, true, 0.07)
          && (Swiat::WstawInfo(NKO)!=Swiat::INV_INDEX) )
				 ;//OK
				 else
				 TLOG(LOCLOG, <<"\""<<Nazwa()<<"\" nie mo?e przes?a? raportu dalej" )
	}
}

void AdministracjaNaukowa::ChwilaDlaCiebie()
{
	//Np. na posuni?cie do przodu proces?w
	//_ZarzadcaProcesowLeniwy();  - o wiele za ma?o efektywny, du?e opo?nienia w zwyk?ych raportach
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
   //Implementacja tego co musi by? wg. interfejsu typu Proces
	friend class KonstruktorElementowModelu<ProcesBadawczy>;
	static KonstruktorElementowModelu<ProcesBadawczy> WirtKonstr;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtKonstr;}
	public:
	//Konstruktor z parametrami do tworzenia procesu w kodzie
	enum EfektNaukowy { NIEWIEM=-1, PUBLIKACJA, PATENT, RAPORT };
	ProcesBadawczy(EfektNaukowy RodzajProd=-1,unsigned ZaplanowanyOdbiorca=-1);//Mo?e od razu zna? odbiorce i rodzaj produktu albo nie

	void ChwilaDlaCiebie(); //Endogenne zmiany stan?w, popychanie pracy do przodu itp.
							//Na koniec tworzy swoj? kopie i sibie przeznacza do skasowania
	bool InterpretujKomunikat(Komunikat* Co);//Przyjmowanie komunikat?w
	protected: //Ukryty przed innymi konstruktor domyslny wykonuje robot? za pomoc? domy?nego GenerycznyProces()
	ProcesBadawczy(){ ZaplanowanyEfekt=NIEWIEM; ZaplOdbiorca=-1;}
	virtual float _KalkulujKosztBadan(); //Furtka do "realistycznego" kalkulowania koszt?w bada?
	bool _WyslijInformacje(unsigned link=-1); //Do rozsylania informacji ?e si? czyms takim zajmujemy - socjalna lub oficjalna
	bool _WyslijPublikacje(); //Pomocnicza procedura publikowania - domyslnie szuka w?z?a PUBLIKATOR
	bool _WyslijPatent();     //Pomocnicza procedura publikowania - domyslnie szuka w?z?a UPATENTOWY, publikacja wyklucza patent
	bool _WyslijRaport();     //Pomocnicza procedura raportowania - do odbiorcy albo administracji jak brak
	//G??wna specyficzna zmienna
	private:
	EfektNaukowy ZaplanowanyEfekt;//Jakiego rodzaju efekt ma tu by?
	unsigned     ZaplOdbiorca;//Czy znany jest odbiorca i kto to
};
*/

	/*
void AdministracjaNaukowa::ChwilaDlaCiebie()
{
	//Np. na posuni?cie do przodu proces?w

	unsigned ile=Swiat::IleProcesow(MojID());
	unsigned ktory=Swiat::NajpilniejszyProc(MojID());

	if(ktory==Swiat::INVINDEX) //Nie ma nic do roboty
			return; //To wychodzimy z pracy :-)

	GenerycznyProces* Roboczy=dynamic_cast<GenerycznyProces*>(Swiat::Proc(ktory,MojID()));
	if(Roboczy)
	{
		if(Roboczy->Prior==0)
					return; //Duuuu?o czasu do terminu...
		//Trzeba popchn?? - nie wi?cej ni? o maksymalny dzie? pracy wszystkich
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
	//Np. na posuni?cie do przodu proces?w
	// do zmiany na: _ZarzadcaProcesowFantazyjny();

	unsigned ile_proc_real; //Ile faktycznie ma proces?w do obslu?enia
	unsigned ktory=Swiat::NajpilniejszyProc(MojID(),&ile_proc_real);
	if(ktory==Swiat::INVINDEX) //Nie ma w ogole nic do roboty
			return; //To wychodzimy z pracy :-)
	GenerycznyProces* Roboczy=dynamic_cast<GenerycznyProces*>(Swiat::Proc(ktory,MojID()));
															 assert(Roboczy!=NULL);

	if(ile_proc_real>1 &&  (???Roboczy->Prior==0??? || DRAND()>0.666) ) //Czasem z fantazji zmieniamy na inny,
								//ale te? czasem popychamy,nawet jak deleko do deadlinu
	{
		unsigned ile_proc=Swiat::IleProcesow(MojID());
		do{
		ktory=RANDOM(ile_proc);
		Roboczy=dynamic_cast<GenerycznyProces*>(Swiat::Proc(ktory,MojID()));
		}while(Roboczy==NULL);
	}

	if(Roboczy)  //M?g? si? zmieni? na pusty?
	{
		if(Roboczy->JakZaawansowany()<1) //Nie zakonczony
		{
			//Trzeba popchn?? - nie wi?cej ni? o maksymalny dzie? pracy wszystkich
			//ale pewnie zazwyczaj mniej???
			double OIle=IleEtatow*Efektywnosc;
			if(Roboczy->Prior==0) //Jak bardzo daleko do deadlineu
				OIle/=2; //Tak co? tam podlubie, ale bez egzaltacji
			Roboczy->TmpPorcjaPracy=OIle;
			Roboczy->ChwilaDlaCiebie(); //Popchnij
		}
	}
}*/

