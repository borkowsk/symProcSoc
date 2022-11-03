// Transfer technologii 2011 - w?a?ciwa implementacja modelu     25-10-2011
//*//////////////////////////////////////////////////////////////////////////////
// Implementacja akcji klas Zespo?ow Naukowych, Administracji naukowej i
// proces?w raportowania i finansowania dzia?alno?ci "statutowej"
// *TODO!!!-> i tworzenia wynik?w bada? (i bez technikali?w obslugi interfaceu)
//*//////////////////////////////////////////////////////////////////////////////

#include "opiKlasyWersja.hpp"
#include "wb_smartlog.hpp"
#include "lingo.hpp"
using namespace wbrtm;

double ILE_RAZY_BADANIA_DROZSZE_NIZ_TOWAR=3; //Na razie nieznacznie bo nie ma finansowania zewnetrznego
extern float  WAGA_NA_DZIENDOBRY;//Wsp?czynnik zmniejszania wagi przy pierwszym kontakcie - nie mo?e by? za du?a
static const int LOCLOG=2; //Na jakim poziomie logowania domy?lnie zapisywa? problemy z tego pliku

//Klasa wewn?trzna skupiaj?ca wsp?lne w?a?ciwo?ci w?z??w z pracownikami (zespo??w)
//////////////////////////////////////////////////////////////////////////////////
/*
class _ZespolRoboczy:public GenerycznyWezelSieci
{
 protected:
 friend class  ProcesProdukcyjny;
 friend class  ProcesRaportowany;
 friend class  ProcesSpoleczny;
//Ile Etat?w	Efektywnosc	Aktywno??	Centralno??	Probiznesowosc	Prolegalnosc	Czasochlonnosc	Innowacyjno?c	Finans. swobodne	Finanse zaplan	D?ug do nadrz.	Udzial w zysku TT	Ekspercko?? w dziedzinie	Uwagi
 unsigned IleEtatow;//Ile etat?w. Co najmniej 1, 0 - "duch" zespo?u - plan utworzenia
 float    Efektywnosc;//0..2. Czy dzia?a na 50% wydajno?ci czy na 150%. Zale?y od motywacji
 float    Proaktywnosc;//Aktywno??? 0..1. Podtrzymywanie aktywno?ci zewn?trznej, tworzenie link?w
 float    Centralno??;//0..1. Jak bardzo aktywno?? (zewn?trzna?) zale?y od szefa/szef?w
 float    Doswiadczenie;//POZIOM EKSPERTYZY? Jak tego u?ywa?? Mo?e to samo co efektywno?????
 float    Czasochlonnosc;//Jaki jest wsp?czynnik czasoch?onno?ci typowej dzia?alno?ci. Wygenerowania pojedynczego wyniku bada??
 float    Prolegalnosc;//????Uczciwo????? Jak ?atwo podj?? dzia?ania "szarostrefowe"
};
*/

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

ProcesBadawczy::ProcesBadawczy(const char* Nazwa,EfektNaukowy RodzajProd,unsigned ZaplanowanyOdbiorca)
//Mo?e od razu zna? odbiorce i rodzaj produktu albo nie
{
	Dane.Realokuj(15);//Nie za du?o?
	Dane[1]=Nazwa;
	this->ZaplanowanyEfekt=RodzajProd;
	this->ZaplOdbiorca=ZaplanowanyOdbiorca;
}

float  ProcesBadawczy::_KalkulujKosztBadan()
{     //Badania s? dro?sze ni? wytworzenie parti danego towaru
	return _ZespolRoboczy::_KalkulujKosztProduktu(PodajDziedzine())*ILE_RAZY_BADANIA_DROZSZE_NIZ_TOWAR;//Dosy? trywialnie te koszty na razie
}

FormalnaKooperacja::FormalnaKooperacja(unsigned Inicjator,unsigned Kooperator,float Waga,unsigned Termin/*=365*/)
{
	this->parametr=(0.1+DRAND()*Waga)*(RANDOM(2)==1?1:-1);
	this->Waga=Waga;//Waga jest te? miar? udzia?u w zyskach z kooperacji, gdy w gr? wchodzi kasa
	this->Wykonany=0;
	this->Termin=Termin;
	this->_S=Inicjator;
	this->_T=Kooperator;
	this->_Z=RANDOM(10);
}

// WLASCIWA IMPLEMENTACJA TEGO CO ROBI? KLASY
////////////////////////////////////////////////////////////////////////////////
extern float GENERYCZNY_POZIOM_PRZECIEKOW;//=0.01;//Jak czesto domyslny komunikat leci dalej ni? do odbiorcy
void ProcesBadawczy::ChwilaDlaCiebie()
//Endogenne zmiany stan?w, popychanie pracy do przodu itp.
{
   GenerycznyProces::ChwilaDlaCiebie();  //Troch? popycha
   WezelSieci* Proc=Swiat::Wezel(Procesor());                                   assert(Proc!=NULL);
   _ZespolRoboczy* ZR=dynamic_cast<_ZespolRoboczy*>(Proc);
   double Zaawansowanie=this->JakZaawansowany();

   if(DRAND()<GENERYCZNY_POZIOM_PRZECIEKOW*Zaawansowanie*Zaawansowanie) //Prezentuje dopiero bli?ej ko?ca
		this->_WyslijInformacje();//Co jaki? czas opowiada czym si? zajmuje. Nie s? to pe?ne wyniki, ale...

   if( ZaplOdbiorca==Swiat::INV_INDEX
   && ZaplanowanyEfekt==NIEWIEM
   && Zaawansowanie>0.75
   && DRAND()<0.333) //Od po po?owie bada? pr?bujemy ustali? jak je spo?ytkowa? cho? nie od razu
   {
		unsigned Skomplikowanie=PodajDziedzine().IleBitow();//Im bardziej skomplikowany, tym bardziej Probiznesowy a nie podstawowy projekt
		JednostkaBadawcza* JB=dynamic_cast<JednostkaBadawcza*>(Proc);
		if(JB) //Tylko w jednostce naukowej jest jaki? prawdziwy wyb?r
		{
			double Selektor=Skomplikowanie*DRAND()*JB->Probiznesowosc;//Im wi?ksza Probiznesowosc tym wi?ksza szansa na du?? liczb?
			if(Selektor<0.10)//Na razie zupelnie nieudane
			ZaplanowanyEfekt==NIEWIEM;//NAdal
			else if(Selektor<0.25)//Niezbyt wyszly te badania
			{ZaplanowanyEfekt=RAPORT;ZaplOdbiorca=-2;}//Umownie, ?e tylko do jednostki Adm
			else if(Selektor<0.75)//W sam raz na publikacje
			ZaplanowanyEfekt=PUBLIKACJA;
			else if(Selektor<0.95)//Wydaj? si? miec zastosowanie praktyczne
			ZaplanowanyEfekt=PATENT;
			else //Za dobre ?eby tak po prostu publikowa?
			ZaplanowanyEfekt=RAPORT;//B?dziemy czeka?/szuka??
		}
		else
		{
			ZaplanowanyEfekt=RAPORT;
			ZaplOdbiorca=Procesor(); //W firmie odbiorc? jest jaki? inny proces tej firmy!!!
		}
   }

   if(Zaawansowanie>=1)
   {
	   //Co zrobi? z wynikami?
	   switch(ZaplanowanyEfekt){
	   case PATENT:
			if(this->_WyslijPatent())
					break;//Jak si? uda?o
				else goto ZAMROZ;//... a jak nie
	   case PUBLIKACJA:
			if(this->_WyslijPublikacje())
				   break;//Jak si? uda?o
				else goto ZAMROZ;//... a jak nie
	   break;
	   case RAPORT:
			if(ZaplOdbiorca!=Swiat::INV_INDEX)
				if(this->_WyslijRaport())
					break;//Jak si? uda?o
			goto ZAMROZ;//... a jak nie
	   ZAMROZ:
	   case NIEWIEM:
	   default: //Gdy nie jest jasne co pocz??, mrozimy proces w oczekiwaniu
			if(ZaplanowanyEfekt==RAPORT && DRAND()<0.2) //Czasem dajemy szanse na zmiane decyzji
					{  ZaplanowanyEfekt=NIEWIEM;ZaplOdbiorca=Swiat::INV_INDEX;}
			//A zawsze przed?u?amy proces w stanie ma?ych wymaga?
			IFTL(LOCLOG) clog<<endl<<"Wezel \""<<Proc->Nazwa()<<"\" ODWLEKL proces badawczy \""<<this->Nazwa()<<"\" D:"<<hex<<PodajDziedzine().ARGB<<" W/Po terminie ("<<dec<<this->DoKonca()<<")"<<endl;
			TLOG(LOCLOG, <<"Wezel \""<<Proc->Nazwa()<<"\" ODWLEK? proces badawczy\t\""<<this->Nazwa()<<"\"\tD:\t"<<hex<<PodajDziedzine().ARGB<<"\tW/Po terminie\t"<<dec<<this->DoKonca()   )
			_IleKrokowDoKonca=365; //Odkladamy na rok
			WymaganaIloscPracy=PracaDotychczasowa*1.1;//Jeszcze 10%
			return ; //WYSKAKUJEMY Z FUNCJI ?EBY NIE ZAKOPNCZY? PROCESU
	   }

	   if(ZR) //Tylko gdy proces  na solidnym w?le  kt?ry ma atrybut "Doswiadczenie"
	   {
		   ZR->Doswiadczenie*=1.01;
		   if(ZR->Doswiadczenie>1) ZR->Doswiadczenie=1;
       }
	   IFTL(LOCLOG) clog<<endl<<"Wezel \""<<Proc->Nazwa()<<"\" zakonczyl proces badawczy \""<<this->Nazwa()<<"\" D:"<<hex<<PodajDziedzine().ARGB<<" W/Po terminie ("<<dec<<this->DoKonca()<<")"<<endl;
	   TLOG(LOCLOG, <<"Wezel \""<<Proc->Nazwa()<<"\" zakonczyl proces badawczy\t\""<<this->Nazwa()<<"\"\tD:\t"<<hex<<PodajDziedzine().ARGB<<"\tW/Po terminie\t"<<dec<<this->DoKonca()   )
	   this->UznajZaZakonczony();
   }
}

bool ProcesBadawczy::InterpretujKomunikat(Komunikat* Co)
//Przyjmowanie komunikat?w - zwi?zane g??wnie z procesem TT
//bo tu mo?e zosta? ustalony jaki? odbiorca wyprodukowanej informacji
{                                            	assert(Co->Kanal()!=Swiat::INV_INDEX);
	if(Co->Rodzaj()=="?SZUKAM.BADAN")
	{
		DziedzinaWKolorze D=this->PodajDziedzine();
		D=D.ARGB & Co->PodajDziedzine().ARGB;
		D.A=0;//To nas nie interesuje. Chwilowo?

		if(D.IleBitow()>0            //Jakies wspolne bity
		&& _ZespolRoboczy::_PodobienstwoDziedzin(D,PodajDziedzine())>DRAND())    //I ewentualnie prawdopodobienstwo
		{
			Komunikat* Klon=Co->Klonuj();     				assert(Klon->Kanal()!=Swiat::INV_INDEX);
			Klon->UstawRodzaj("MAM.BADANIA!");
			D.A=Co->PodajDziedzine().A;//Numer produktu b?dzie ten sam
			Klon->UstawDziedzine(D);   //A w reszcie tylko obiecane bity

			if(  ! Klon->Zwrotnie()  )//Jak towarzyski to musialby istnie? zwrotny link
			{   //A jak nie to tworzymy towarzyski do jednostki wskazanej w Co
				unsigned IndInny=Co->Nadawca();
				KomunikacjaTowarzyska* KT=dynamic_cast<KomunikacjaTowarzyska*>(Co);
				if(KT) IndInny=KT->OKimTaGadka();

				wb_ptr<KontaktTowarzyski>  Nowy( new KontaktTowarzyski(Procesor(),IndInny,WAGA_NA_DZIENDOBRY,DRAND())  );
				Nowy->UstawDziedzine(D); //Lacze pokazuje jakie bity byly
				unsigned Kanal=Swiat::WstawLacze(Nowy.give());       		assert(Kanal!=Swiat::INV_INDEX);
				Klon->Zaadresuj(Kanal,true,0.1);		//Tu si? b??du juz nie spodziewam...
			}

			KomunikacjaTowarzyska* KlonT=dynamic_cast<KomunikacjaTowarzyska*>(Klon);
			if(KlonT) KlonT->OKimTaGadka()=Procesor(); //Informuje ?e gada o sobie :-)
			if(Swiat::WstawInfo(Klon)==Swiat::INV_INDEX)
					TLOG(0,<<"ODPOWIEDZ NIE WYSLANA" )
					else
					return true;  //Czyli obsluzyl?
		}
	}
	else
	if( Co->Rodzaj()=="ZAMOWIENIE!"
		&& //Jak jeszcze nie ma zaplanowanego konkretnego odbiorcy
	  (  this->ZaplOdbiorca==Swiat::INV_INDEX
	  || this->ZaplOdbiorca==unsigned(-2)
	  || this->ZaplOdbiorca==_ZespolRoboczy::DajUrzadPatentowy()
	  || this->ZaplOdbiorca==_ZespolRoboczy::DajPublikator()
	  )
	  )
	  {
		DziedzinaWKolorze C=Co->PodajDziedzine();
		DziedzinaWKolorze D=this->PodajDziedzine();
		D=D.ARGB & C.ARGB;
		C.A=D.A=0;//To nas nie interesuje. Chwilowo?
		if( D.IleBitow()>=C.IleBitow() )//Jak wspolne bity w calosci si? mieszcza w badaniu
		{
			KomunikacjaOficjalna* KO=dynamic_cast<KomunikacjaOficjalna*>(Co);
			if(KO && KO->KtoJestAutorem()==Co->Nadawca())//TYLKO DROGA OFICJALNA!!!
			{
				D.A=Co->PodajDziedzine().A;  //Oznakowanie do w?asciwego produktu
				this->UstawDziedzine(D);
				this->ZaplanowanyEfekt=RAPORT; //Nowy spos?b "upublicznienia"
				this->ZaplOdbiorca=Co->Nadawca();//I nowy odbiorca
				WezelSieci* Odbiorca=Swiat::Wezel(this->ZaplOdbiorca);
				this->Dane[1]=Odbiorca->Nazwa();
				return true; //Obrobiony
			}
		}
	}
															assert(Co->Kanal()!=Swiat::INV_INDEX);
	return false;
}

void JednostkaBadawcza::InterpretujKomunikat(Komunikat* Co)
//Przyjmowanie komunikat?w
{  //Obr?bka domy?lna za pomoc? proces?w
   if(_KomunikatDoProcesow(Co)) //Je?li kt?ry? z proces?w obrobi?
				return ; //To ju? nic nie robimy

	//Obr?bka wyspecjalizowana
	KomunikacjaOficjalna* KO=dynamic_cast<KomunikacjaOficjalna*>(Co);

	//Gdy wraca potwierdzenie publikacji lub patentu
	if(KO!=NULL
	&& KO->KtoJestAutorem()==this->MojID()
	&& (KO->Rodzaj()=="PUBLIKACJA" || KO->Rodzaj()=="PATENT")
	)
	{
		this->Sukcesy++; //Udana publikacja lub patent
		Waga*=1.01;//Wzrost wagi wez?a przy sukcesach
		//Na razie wp?ywa tylko na szeroko?? proces?w ale i tak
		//powinien by? jako? logarytmicznie bo tak za bardzo rosn? w koncu
		unsigned KoopLinkInd=Co->Kanal();  //Gdy ??cze kooperacyjne to uznajemy za wykonane
		FormalnaKooperacja*   LK=dynamic_cast<FormalnaKooperacja*>(Swiat::Lacze(KoopLinkInd));
		if(LK) LK->UstawWykonano();
	}
	else
	if(KO!=NULL
	&& (KO->PodajDziedzine().ARGB & PodajDziedzine().ARGB !=0)
	&& ((KO->Rodzaj()=="ZAMOWIENIE!"  && DRAND()<Probiznesowosc)
		||
		 (KO->Rodzaj()=="?SZUKAM.BADAN" && DRAND()<0.01)
		)
	) //Jakie? zam?wienie, kt?re nie znalaz?o odpowiedzi w aktualnych procesach
	//Jaka? obietnica finansowanie, ale czego? innego ni? to czym zwyk?e? si? zajmowa?
	//Mo?e spowodowa? ?e nieco zmieniamy dziedzin? zainteresowa?
	{
		DziedzinaWKolorze D=KO->PodajDziedzine();
		unsigned Maska=0x1<<RANDOM(24);
		D.ARGB&=Maska;
		D.ARGB=D.ARGB | PodajDziedzine().ARGB;
		UstawDziedzine(D); //Lekka zmiana dziedziny
	}
	//...???????????
	//KIEDYS<-GenerycznyWezelSieci::InterpretujKomunikat(Co);
}

void JednostkaBadawcza::ChwilaDlaCiebie()
// Tu spontanicznie tworzy badania na podstawie swojej dziedziny
// ale oczywi?cie mo?e te? wykonywac procesy badawcze powsta?e w inny spos?b
{
	//Zale?ny od czasu spadek znaczenia w?z?a
	Waga*=0.99999; //Raczej powolny
	//Np. na posuni?cie do przodu proces?w
	if(this->Doswiadczenie<0.89)
		this->_ZarzadcaProcesowFantazyjny();
		else
		this->_ZarzadcaProcesowFinezyjny();


	if(unsigned(Swiat::NumerKroku())%10==0)//Czasem chcemy posortowa? priorytetowo i zobaczy? ile jest  realnie
	{
		unsigned IleRealnie;
		Swiat::NajpilniejszyProc(this->MojID(),&IleRealnie,true);//Ale to nie zupelnie dziala
	}

	//Co jaki? czas sprawdza czy mo?e utworzy? jaki? proces badawczy
	//(zw?aszcza jak brak to go sobie tworzy)
	if(RANDOM(30)==0)
	{
		unsigned ile=Swiat::IleProcesow(this->MojID());
		unsigned ile_bad=0;
		ProcesBadawczy* Pr=NULL;
		for(unsigned i=0;i<ile;i++)   //raczej "ile" dla pewno?ci bo to sortowanie co? dziwnie dzia?a
		 if((Pr=dynamic_cast<ProcesBadawczy*>(Swiat::Proc(i,this->MojID())))!=NULL)
		 {
			ile_bad++; //Moglby te? liczy? obci?zenie i dawa? sobie wi?cej proces?w jak niedoci??ony
		 }

		if(ile_bad==0
		|| DRAND()< (0.1*IleEtatow*Innowacyjnosc*Proaktywnosc*Prolegalnosc*Doswiadczenie*(IleEtatow>2?1-Centralnosc:1)))//Na razie do?? rzadko spontanicznie, chyba ?e nie ma nic do roboty
		{
			//TERAZ DODAJEMY NOWY
			wb_ptr<ProcesBadawczy> Nowy( new  ProcesBadawczy("BST") ); //Nie wiadomo jaki efekt i dla kogo
			unsigned char Maska;
			DziedzinaWKolorze D;
			D.A=0;
			do{
			D=PodajDziedzine(); D.A=0;
			Maska=0x1<<RANDOM(8);D.R&=Maska;
			Maska=0x1<<RANDOM(8);D.G&=Maska;
			Maska=0x1<<RANDOM(8);D.R&=Maska;
			}while(D.ARGB==0); //Powtarza a? wylosuje niezerowy
			D.A=RANDOM(256)*Probiznesowosc; //Teraz "numer" konkretnego badania, ma?o probiznesowi maj? niskie numery
			Nowy->UstawDziedzine(D);

			double KosztyBadan=Nowy->_KalkulujKosztBadan();
			if(this->StacGoNaWydatek( KosztyBadan )  )
			{
				Nowy->PracaDotychczasowa=0; Nowy->TmpPorcjaPracy=1; //Na poczatek
				double IleDni=2*Czasochlonnosc*DRAND_LOOP(3);//Typowa czasochlonnosc w dniach b?dzie sredni?
				Nowy->_IleKrokowDoKonca=IleDni;
				double NaIleOsob=0.5*(1.0+IleEtatow*DRAND());//Jeden p?-etat lub wi?cej, czasem sporo wi?cej  *DRAND_LOOP(-3)
				Nowy->WymaganaIloscPracy=IleDni*NaIleOsob;

				if(Swiat::WstawProc(Nowy.give(),this->MojID())!=Swiat::INV_INDEX)
				{
					this->PrzyjmijKase( - KosztyBadan); //Na pocz?tku kasuje, bo potem mo?e nie mie?
					IFTL(LOCLOG+1) clog<<endl<<"Wezel \""<<Nazwa()<<"\" uruchomil nowy proces badawczy. Dni "<<IleDni<<". Etatow "<<double(NaIleOsob)<<endl;
					TLOG(LOCLOG, <<"W?ze? \""<<Nazwa()<<"\" uruchomi? nowy proces badawczy. Dni\t"<<IleDni<<"\t. Etat?w\t"<<double(NaIleOsob) )
				}
				else
				TLOG( 0, <<"W?z?owi \""<<Nazwa()<<"\" NIE uda?o si? uruchomi? nowego procesu badawczego" )
			}
		}
	}
}


bool ProcesBadawczy::_WyslijPublikacje()
//Pomocnicza procedura publikowania - domyslnie szuka w?z?a PUBLIKATOR
{
  //if(DRAND()<0.1) return false;//DEBUG
  unsigned Publikator;
  if(ZaplOdbiorca!=-2 && ZaplOdbiorca!=Swiat::INV_INDEX)
	  Publikator=ZaplOdbiorca;
	  else
	  Publikator=_ZespolRoboczy::DajPublikator();
  if(Publikator==Swiat::INV_INDEX) //Nie ma gdzie publikowa?
  {
	 ZaplOdbiorca=Swiat::INV_INDEX;
	 if(DRAND()>0.8) ZaplanowanyEfekt=PATENT;//Zmieniamy plan na patentowanie
		else  ZaplanowanyEfekt=RAPORT;//Albo jaki? raport
	 return false;
  }
  //Jest publikator
  unsigned KoopLinkInd=Swiat::INV_INDEX;
  Powiazanie* Link=NULL;
  unsigned IleLinkow=Swiat::IlePowiazan();
  for(unsigned i=0;i<IleLinkow;i++)
	if( (Link=Swiat::Lacze(i))!=NULL
	&& ((Link->Poczatek()==Procesor() && Link->Koniec()==Publikator)
		||
	   (Link->Poczatek()==Publikator && Link->Koniec()==Procesor()))
	&& (dynamic_cast<FormalnaKooperacja*>(Link))
	)
	{   KoopLinkInd=i; break; }

  if(KoopLinkInd==Swiat::INV_INDEX)
  {
	 wb_ptr<FormalnaKooperacja>  NewKoop(  new FormalnaKooperacja(Procesor(),Publikator,0.25)  );
	 if((KoopLinkInd=Swiat::WstawLacze(NewKoop.give()))!=Swiat::INV_INDEX)
			;//OK
			else
			return false; //Nie uda?o si? utworzy?
  }
  //KWESTIE ESTETYCZNE
  FormalnaKooperacja*   LK=dynamic_cast<FormalnaKooperacja*>(Swiat::Lacze(KoopLinkInd));
  LK->UstawDziedzine(PodajDziedzine());   LK->UstawTermin(365);
  LK->UstawWykonano(false);    LK->UstawWygiecie(LK->DajWygiecie()*0.66);
  //WLASCIWE WYSYLANIE
  wb_ptr<KomunikacjaOficjalna> KO( new KomunikacjaOficjalna("PUBLIKACJA",Procesor()) );//WNiosek patentowy
  KO->UstawDziedzine(PodajDziedzine());
  bool Kierunek;
  if(Swiat::Lacze(KoopLinkInd)->Poczatek()==Procesor())
	Kierunek=true;
	else
	Kierunek=false;
  if(!KO->Zaadresuj(KoopLinkInd,Kierunek,0.01+DRAND()*0.03)) //Zwykle true jest OK ale...
								return false; //Nie uda?o si? zaadresowa????
  if(Swiat::WstawInfo(KO.give())==Swiat::INV_INDEX)
								return false; //Nie uda?o si?  wys?a?
  return true;
}


bool ProcesBadawczy::_WyslijPatent()
//Pomocnicza procedura publikowania - domyslnie szuka w?z?a UPATENTOWY, publikacja wyklucza patent
{
  //if(DRAND()<0.1) return false;//DEBUG
  unsigned Patentowy;
  if(ZaplOdbiorca!=-2 && ZaplOdbiorca!=Swiat::INV_INDEX)
	  Patentowy=ZaplOdbiorca;
	  else
	  Patentowy=_ZespolRoboczy::DajUrzadPatentowy();
  if(Patentowy==Swiat::INV_INDEX) //Nie ma gdzie publikowa?
  {
	 ZaplOdbiorca=Swiat::INV_INDEX;
	 if(DRAND()>0.8) ZaplanowanyEfekt=PATENT;//Zmieniamy plan na patentowanie
		else  ZaplanowanyEfekt=RAPORT;//Albo jaki? raport
	 return false;
  }
  //Jest publikator
  unsigned KoopLinkInd=Swiat::INV_INDEX;
  Powiazanie* Link=NULL;
  unsigned IleLinkow=Swiat::IlePowiazan();
  for(unsigned i=0;i<IleLinkow;i++)
	if( (Link=Swiat::Lacze(i))!=NULL
	&& ((Link->Poczatek()==Procesor() && Link->Koniec()==Patentowy)
		||
	   (Link->Poczatek()==Patentowy && Link->Koniec()==Procesor()))
	&& (dynamic_cast<FormalnaKooperacja*>(Link))
	)
	{   KoopLinkInd=i; break; }

  if(KoopLinkInd==Swiat::INV_INDEX)
  {
	 wb_ptr<FormalnaKooperacja>  NewKoop(  new FormalnaKooperacja(Procesor(),Patentowy,0.25)  );
	 if((KoopLinkInd=Swiat::WstawLacze(NewKoop.give()))!=Swiat::INV_INDEX)
			;//OK
			else
			return false; //Nie uda?o si? utworzy?
  }
  //KWESTIE ESTETYCZNE
  FormalnaKooperacja*   LK=dynamic_cast<FormalnaKooperacja*>(Swiat::Lacze(KoopLinkInd));
  DziedzinaWKolorze D=PodajDziedzine();
  LK->UstawDziedzine(D);     LK->UstawTermin(2*365);
  LK->UstawWykonano(false);  LK->UstawWygiecie(LK->DajWygiecie()*0.66);
  //WLACIWE WYSYLANIE
  wb_ptr<KomunikacjaOficjalna> KO( new KomunikacjaOficjalna("PATENT",Procesor()) );//WNiosek patentowy
  KO->UstawDziedzine(D);
  bool Kierunek;
  if(Swiat::Lacze(KoopLinkInd)->Poczatek()==Procesor())
	Kierunek=true;
	else
	Kierunek=false;
  if(!KO->Zaadresuj(KoopLinkInd,Kierunek,0.01+DRAND()*0.03)) //Zwykle true jest OK ale...
								return false; //Nie uda?o si? zaadresowa????
  if(Swiat::WstawInfo(KO.give())==Swiat::INV_INDEX)
								return false; //Nie uda?o si?  wys?a?
  return true;
}

bool ProcesBadawczy::_WyslijRaport()
//Pomocnicza procedura raportowania - do odbiorcy albo administracji jak brak
{
	if(this->ZaplOdbiorca==Swiat::INV_INDEX)
					return false; //Nie jest jasne komu ten raport wysla?

	KomunikacjaOficjalna* KO=new KomunikacjaOficjalna("RAPORT.BADANIA",Procesor());
	KO->UstawDziedzine(PodajDziedzine());
	WezelSieci* MojProcesor=Swiat::Wezel(this->Procesor());


	//unsigned Odbiorca=Swiat::INVINDEX;//Preferowany odbiorca
	if(this->ZaplOdbiorca==-2)//Domyslny odbiorca
	{
		//Domyslny odbiorca - dosy? skomplikowane szukanie adresata raportu.
		unsigned Kanal=Swiat::INV_INDEX;
		for(unsigned i=0;i<Swiat::IlePowiazan();i++)
		{
			PodlegloscOrganizacyjna* P=dynamic_cast<PodlegloscOrganizacyjna*>(Swiat::Lacze(i));
			if(P!=NULL && P->Poczatek()==Procesor())
			{
				Kanal=i;
				break;
			}
		}

		if(  Kanal==Swiat::INV_INDEX  )
		{
		   IFTL(LOCLOG+1) clog<<endl<<"Proces "<<Nazwa()<<" na \""<<MojProcesor->Nazwa()<<"\"nie ma kanalu do jednostki nadrzednej administracyjnie"<<endl;
		   TLOG(LOCLOG, <<"Proces "<<Nazwa()<<" na \""<<MojProcesor->Nazwa()<<"\"nie ma kanalu do jednostki nadrzednej administracyjnie")
		   return true; //Nie ma instytucjonalnego odbiorcy raportu - juz z tym nic nie da zrobic
		}

		if(! KO->Zaadresuj(Kanal,true,0.05)  )
		  {
			  TLOG(0, <<"NIEUDANE ADRESOWANIE w ProcesBadawczy NIE POWINNO SI? ZDARZAC!!!???")
			  goto ERROR;
		  }
	}
	else
	{
	   if(! KO->Zaadresuj(Procesor(),ZaplOdbiorca,0.05))  //Dowolnym kanalem... ?
	   {
		  IFTL(LOCLOG) clog<<endl<<"NIEUDANE RAPORTOWANIE  \""<<Nazwa()<<"\" na "<<MojProcesor->Nazwa()<<"\" ZERWANA KOOPERACJA!!!???"<<endl;
		  TLOG(0, <<"NIEUDANE RAPORTOWANIE \""<<Nazwa()<<"\" na "<<MojProcesor->Nazwa()<<"\" ZERWANA KOOPERACJA!!!???")
		  goto ERROR;//	return false; //Raczej si? nie powinno zdarza?
	   }
	}

	//Wysylanie
	if(Swiat::WstawInfo(KO)!=Swiat::INV_INDEX)
	{
	   IFTL(LOCLOG+1) clog<<_LPL("Wyslano raport o ","Sending report about ")<<Nazwa()<<" D:"<<hex<<PodajDziedzine().ARGB<<dec<<_LPL(" z "," from ")<<MojProcesor->Nazwa()<<_LPL(" do nr "," to n# ")<<dec<<ZaplOdbiorca<<endl;
	   TLOG(LOCLOG ,      <<_LPL("Wyslano raport o ","Sending report about ")<<Nazwa()<<" D:"<<hex<<PodajDziedzine().ARGB<<dec<<_LPL(" z "," from ")<<MojProcesor->Nazwa()<<_LPL(" do nr "," to n# ")<<dec<<ZaplOdbiorca<<'"'  )
	   return true;
	}
	else
	{
	   ERROR:
	   IFTL(LOCLOG-1) clog<<_LPL("NIE wyslano raportu o ","Failed to send report about ")<<Nazwa()<<" D:"<<hex<<PodajDziedzine().ARGB<<dec<<_LPL(" z "," from ")<<MojProcesor->Nazwa()<<_LPL(" do "," to ")<<dec<<ZaplOdbiorca<<endl;
	   TLOG(LOCLOG-1,      <<_LPL("NIE wyslano raportu o ","Failed to send report about ")<<Nazwa()<<" D:"<<hex<<PodajDziedzine().ARGB<<dec<<_LPL(" z "," from ")<<MojProcesor->Nazwa()<<_LPL(" do "," to ")<<dec<<ZaplOdbiorca )
	   return false;
	}
}

bool ProcesBadawczy::_WyslijInformacje(unsigned link/*=-1*/)
//Do rozsylania informacji ?e si? czyms takim zajmujemy - socjalna lub oficjalna
{
  Powiazanie* LNK;
  if(link!=Swiat::INV_INDEX && (LNK=Swiat::Lacze(link)) != NULL )
  {
	  if(LNK->Poczatek()!=Procesor())//Trzeba szuka? symetrycznego ??cza towarzyskiego
	  {
		 link=ProcesSpoleczny::_JestPowiazanySocjalnie(Procesor(),LNK->Poczatek());
		 if(link==Swiat::INV_INDEX) //Nie ma?
				return false;  //Mog?by tworzy?
	  }
	  //A w przeciwnym wypadku jest chyba OK, cho? mo?e by? to ??cze jako? "upierdliwe"? (nie przepuszczalne)
  }
  else //Sam musi ustali? komu to wys?a? po ?aczach towarzyskich
  {
	 unsigned ile_laczy=Swiat::IlePowiazan();
	 for(unsigned i=0;i<ile_laczy;i++)
	   if( (LNK=Swiat::Lacze(i))!=NULL
	   && (LNK->Poczatek()==Procesor())
	   && (dynamic_cast<KontaktTowarzyski*>(LNK)!=NULL)
	   && (DRAND()<LNK->Waznosc())
	   )
	 {  //Zosta? wybrany
		link=i;
		break; //A mo?e bez break??? Albo pocz?tkowe (+-starsze?) s? preferowane albo koncowe
	 }
	 //A mo?e robi? tu te? nowe? Ale czy to by nie by?o dublowanie ProcesuSpolecznego?
	 //...
  }
  //GOTOWY:
  if(link!=Swiat::INV_INDEX)//Nie znalaz? takiego!
  {
	wb_ptr<KomunikacjaOficjalna> KT( new  KomunikacjaOficjalna("PREZENTACJA",Procesor())  );
	KT->UstawDziedzine(this->PodajDziedzine());
	if(KT->Zaadresuj(link,true,0.05)) //Jak si? uda zaadresowa?
	{
	 if(Swiat::WstawInfo(KT.give())!=Swiat::INV_INDEX)
				{ return true;}
				else
				{
					TLOG(0, <<"BLAD WYSYLANIA KTORY NIE POWINIEN SIE ZDARZA?" )
					return false; // A TO DZIWNE, chyba ?e zabrak?o miejsca na komunikaty
				}
	} //else Blad adresowania
  } //else Brak linku
  return false; //Cha?ka, ale w koncu kogo to obchodzi... Byle czasem si? udawa?o  :-)
}

void SystemInformacyjny::InterpretujKomunikat(Komunikat* Co)
//Przyjmowanie TYLKO OFICJALNYCH komunikat?w
{
	if(dynamic_cast<KomunikacjaOficjalna*>(Co)!=NULL)
			WezelMacierzowy::InterpretujKomunikat(Co);
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

void AdministracjaNaukowa::ChwilaDlaCiebie()
{
	//Np. na posuni?cie do przodu proces?w
	//_ZarzadcaProcesowLeniwy();  - o wiele za ma?o efektywny, du?e opo?nienia w zwyk?ych raportach
	if(this->Doswiadczenie<0.75)
		this->_ZarzadcaProcesowFantazyjny();
		else
		this->_ZarzadcaProcesowFinezyjny();
}

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


void ProcesRaportowany::ChwilaDlaCiebie()
{
	GenerycznyProces::ChwilaDlaCiebie(); //Popycha tylko liczniki

	if(JakZaawansowany()>=1) //Je?li w?a?nie zakonczony
	{
		WezelSieci* MojProc=Swiat::Wezel(this->Procesor());

		if(DoKonca()<-7)//Sp?ni? si? wi?cej ni? tydzie?
		{
			IFTL(LOCLOG+1) clog<<endl<<"\""<<MojProc->Nazwa()<<"\" "<<_LPL("z poslizgiem zaraportowal proces","with dalay reported proces")<<' '<<Dane[1]<<':'<<hex<<PodajDziedzine().ARGB<<' '<<" ("<<-DoKonca()<<_LPL(" krokow po terminie"," steps after deadline")<<")"<<endl;
			TLOG(LOCLOG , <<"\""<<MojProc->Nazwa()<<"\" "<<_LPL("z poslizgiem zaraportowal proces","with dalay reported proces")<<' '<<Dane[1]<<':'<<hex<<PodajDziedzine().ARGB<<' '<<"\t("<<-DoKonca()<<_LPL(" krokow po terminie"," steps after deadline")<<")"   )
		}

		//Udajemy, ?e go usuwamy
		Wymazuj();

		if( _WyslijRaportOdbiorcy() ) //Jak uda si? wys?a? raport?
		{
			//to go restartujemy po prostu!!!
			//double Zaangazowanie=(1+DRAND())*IleEtatow*7;//Jak pracoch?onne sprawozdanie to je ~1.5*7*IleEtatow...
			this->PracaDotychczasowa=0;
			this->_IleKrokowDoKonca=30+DRAND()*60;//Domyslnie nie mniej ni? miesi?c nie wi?cej ni? trzy miesi?ce
			this->Prior=1;

			//Tu by trzeba ustali? sensowniejszy czas do konca na podstawie cech w?z?a
			_ZespolRoboczy* ProcesorZR=dynamic_cast<_ZespolRoboczy*>(MojProc);
			if(ProcesorZR) //Uda?o si? sprowadzi? do tego "mianownika"
			{   //Mo?na lepiej zaplanowa? czas wykonania
				_IleKrokowDoKonca=ProcesorZR->Czasochlonnosc*(1+4*DRAND());
			}

			IFTL(LOCLOG+1) clog<<"\""<<MojProc->Nazwa()<<"\" "<<_LPL("reaktywowal proces","reactivated proc.")<<' '<<Dane[1]<<':'<<hex<<PodajDziedzine().ARGB<<' '<<endl;
			TLOG(LOCLOG , <<"\""<<MojProc->Nazwa()<<"\" "<<_LPL("reaktywowal proces","reactivated proc.")<<' '<<Dane[1]<<':'<<hex<<PodajDziedzine().ARGB<<dec )
		}
		else
		{   //Pewnie nie ma komu albo jak wysylac!!!
			IFTL(LOCLOG+1) clog<<"\""<<MojProc->Nazwa()<<"\" "<<_LPL("ZLIKWIDOWAL proces","reactivated proc.")<<' '<<Dane[1]<<':'<<hex<<PodajDziedzine().ARGB<<dec<<' '<<endl;
			TLOG(LOCLOG , <<"\""<<MojProc->Nazwa()<<"\" "<<_LPL("ZLIKWIDOWAL proces","reactivated proc.")<<' '<<Dane[1]<<':'<<hex<<PodajDziedzine().ARGB<<dec  )
			UznajZaZakonczony(); //NAPRAWD? kasujemy...
		}
	}
}

bool ProcesRaportowany::_WyslijRaportOdbiorcy()
{
	//Generuje raport
	KomunikacjaOficjalna* KO=new KomunikacjaOficjalna("RAPORT");
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
		  TLOG(0, <<"NIEUDANE ADRESOWANIE NIE POWINNO SI? ZDARZAC!!!???")
		 //	return false; //Raczej si? nie powinno zdarza?
	}
	else
	{  	//Niezdefiniowany odbiorca - dosy? skomplikowane szukanie adresata raportu.
		unsigned Kanal=-1;
		for(unsigned i=0;i<Swiat::IlePowiazan();i++)
		{
			PodlegloscOrganizacyjna* P=dynamic_cast<PodlegloscOrganizacyjna*>(Swiat::Lacze(i));
			if(P!=NULL && P->Poczatek()==Procesor())
			{
				Kanal=i;
				break;
			}
		}
		if(Kanal!=Swiat::INVINDEX)
		  if(! KO->Zaadresuj(Kanal,true,0.05) )
			  TLOG(0, <<"NIEUDANE ADRESOWANIE NIE POWINNO SI? ZDARZAC!!!???")
			  //return false;
	}

	if(Swiat::WstawInfo(KO)!=Swiat::INVINDEX)
	{
	   IFTL(LOCLOG+1) clog<<_LPL("Wyslano raport o ","Sending report about ")<<Dane[1]<<_LPL(" z "," from ")<<MojProcesor->Nazwa()<<_LPL(" do "," to ")<<'"'<<WartoscPola(8)<<'"'<<endl;
	   TLOG(LOCLOG ,      <<_LPL("Wyslano raport o ","Sending report about ")<<Dane[1]<<_LPL(" z "," from ")<<MojProcesor->Nazwa()<<_LPL(" do "," to ")<<'"'<<WartoscPola(8)<<'"'  )
	   return true;
	}
	else
	{
	   IFTL(LOCLOG-1) clog<<_LPL("NIE wyslano raportu o ","Failed to send report about ")<<Dane[1]<<_LPL(" z "," from ")<<MojProcesor->Nazwa()<<endl;
	   TLOG(LOCLOG-1,      <<_LPL("NIE wyslano raportu o ","Failed to send report about ")<<Dane[1]<<_LPL(" z "," from ")<<MojProcesor->Nazwa() )
	   return false;
	}
}

bool ProcesRaportowany::InterpretujKomunikat(Komunikat* Co)
//Przesyla dalej raporty, wysy?a kase, inkasuje kas?
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

