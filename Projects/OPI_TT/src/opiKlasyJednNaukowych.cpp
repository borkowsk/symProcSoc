// Transfer technologii 2011 - w³aœciwa implementacja modelu     25-10-2011
////////////////////////////////////////////////////////////////////////////////
// Implementacja akcji klas Zespo³ow Naukowych, Administracji naukowej i
// procesów raportowania i finansowania dzia³alnoœci "statutowej"
// *TODO!!!-> i tworzenia wyników badañ (i bez technikaliów obslugi interfaceu)
////////////////////////////////////////////////////////////////////////////////

#include "opiKlasyWersja.hpp"
#include "INCLUDE/wb_smartlog.h"
using namespace wbrtm;

double ILE_RAZY_BADANIA_DROZSZE_NIZ_TOWAR=3; //Na razie nieznacznie bo nie ma finansowania zewnetrznego
extern float  WAGA_NA_DZIENDOBRY;//Wspó³czynnik zmniejszania wagi przy pierwszym kontakcie - nie mo¿e byæ za du¿a
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
//Ile Etatów	Efektywnosc	Aktywnoœæ	Centralnoœæ	Probiznesowosc	Prolegalnosc	Czasochlonnosc	Innowacyjnoœc	Finans. swobodne	Finanse zaplan	D³ug do nadrz.	Udzial w zysku TT	Eksperckoœæ w dziedzinie	Uwagi
 unsigned IleEtatow;//Ile etatów. Co najmniej 1, 0 - "duch" zespo³u - plan utworzenia
 float    Efektywnosc;//0..2. Czy dzia³a na 50% wydajnoœci czy na 150%. Zale¿y od motywacji
 float    Proaktywnosc;//Aktywnoœæ? 0..1. Podtrzymywanie aktywnoœci zewnêtrznej, tworzenie linków
 float    Centralnoœæ;//0..1. Jak bardzo aktywnoœæ (zewnêtrzna?) zale¿y od szefa/szefów
 float    Doswiadczenie;//POZIOM EKSPERTYZY? Jak tego u¿ywaæ? Mo¿e to samo co efektywnoœæ???
 float    Czasochlonnosc;//Jaki jest wspó³czynnik czasoch³onnoœci typowej dzia³alnoœci. Wygenerowania pojedynczego wyniku badañ?
 float    Prolegalnosc;//????Uczciwoœæ??? Jak ³atwo podj¹æ dzia³ania "szarostrefowe"
};
*/

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

ProcesBadawczy::ProcesBadawczy(const char* Nazwa,EfektNaukowy RodzajProd,unsigned ZaplanowanyOdbiorca)
//Mo¿e od razu znaæ odbiorce i rodzaj produktu albo nie
{
	Dane.Realokuj(15);//Nie za du¿o?
	Dane[1]=Nazwa;
	this->ZaplanowanyEfekt=RodzajProd;
	this->ZaplOdbiorca=ZaplanowanyOdbiorca;
}

float  ProcesBadawczy::_KalkulujKosztBadan()
{     //Badania s¹ dro¿sze ni¿ wytworzenie parti danego towaru
	return _ZespolRoboczy::_KalkulujKosztProduktu(PodajDziedzine())*ILE_RAZY_BADANIA_DROZSZE_NIZ_TOWAR;//Dosyæ trywialnie te koszty na razie
}

FormalnaKooperacja::FormalnaKooperacja(unsigned Inicjator,unsigned Kooperator,float Waga,unsigned Termin/*=365*/)
{
	this->parametr=(0.1+DRAND()*Waga)*(RANDOM(2)==1?1:-1);
	this->Waga=Waga;//Waga jest te¿ miar¹ udzia³u w zyskach z kooperacji, gdy w grê wchodzi kasa
	this->Wykonany=0;
	this->Termin=Termin;
	this->_S=Inicjator;
	this->_T=Kooperator;
	this->_Z=RANDOM(10);
}

// WLASCIWA IMPLEMENTACJA TEGO CO ROBI¥ KLASY
////////////////////////////////////////////////////////////////////////////////
extern float GENERYCZNY_POZIOM_PRZECIEKOW;//=0.01;//Jak czesto domyslny komunikat leci dalej ni¿ do odbiorcy
void ProcesBadawczy::ChwilaDlaCiebie()
//Endogenne zmiany stanów, popychanie pracy do przodu itp.
{
   GenerycznyProces::ChwilaDlaCiebie();  //Trochê popycha
   WezelSieci* Proc=Swiat::Wezel(Procesor());                                   assert(Proc!=NULL);
   _ZespolRoboczy* ZR=dynamic_cast<_ZespolRoboczy*>(Proc);
   double Zaawansowanie=this->JakZaawansowany();

   if(DRAND()<GENERYCZNY_POZIOM_PRZECIEKOW*Zaawansowanie*Zaawansowanie) //Prezentuje dopiero bli¿ej koñca
		this->_WyslijInformacje();//Co jakiœ czas opowiada czym siê zajmuje. Nie s¹ to pe³ne wyniki, ale...

   if( ZaplOdbiorca==Swiat::INVINDEX
   && ZaplanowanyEfekt==NIEWIEM
   && Zaawansowanie>0.75
   && DRAND()<0.333) //Od po po³owie badañ próbujemy ustaliæ jak je spo¿ytkowaæ choæ nie od razu
   {
		unsigned Skomplikowanie=PodajDziedzine().IleBitow();//Im bardziej skomplikowany, tym bardziej Probiznesowy a nie podstawowy projekt
		JednostkaBadawcza* JB=dynamic_cast<JednostkaBadawcza*>(Proc);
		if(JB) //Tylko w jednostce naukowej jest jakiœ prawdziwy wybór
		{
			double Selektor=Skomplikowanie*DRAND()*JB->Probiznesowosc;//Im wiêksza Probiznesowosc tym wiêksza szansa na du¿¹ liczbê
			if(Selektor<0.10)//Na razie zupelnie nieudane
			ZaplanowanyEfekt==NIEWIEM;//NAdal
			else if(Selektor<0.25)//Niezbyt wyszly te badania
			{ZaplanowanyEfekt=RAPORT;ZaplOdbiorca=-2;}//Umownie, ¿e tylko do jednostki Adm
			else if(Selektor<0.75)//W sam raz na publikacje
			ZaplanowanyEfekt=PUBLIKACJA;
			else if(Selektor<0.95)//Wydaj¹ siê miec zastosowanie praktyczne
			ZaplanowanyEfekt=PATENT;
			else //Za dobre ¿eby tak po prostu publikowaæ
			ZaplanowanyEfekt=RAPORT;//Bêdziemy czekaæ/szukaæ?
		}
		else
		{
			ZaplanowanyEfekt=RAPORT;
			ZaplOdbiorca=Procesor(); //W firmie odbiorc¹ jest jakiœ inny proces tej firmy!!!
		}
   }

   if(Zaawansowanie>=1)
   {
	   //Co zrobiæ z wynikami?
	   switch(ZaplanowanyEfekt){
	   case PATENT:
			if(this->_WyslijPatent())
					break;//Jak siê uda³o
				else goto ZAMROZ;//... a jak nie
	   case PUBLIKACJA:
			if(this->_WyslijPublikacje())
				   break;//Jak siê uda³o
				else goto ZAMROZ;//... a jak nie
	   break;
	   case RAPORT:
			if(ZaplOdbiorca!=Swiat::INVINDEX)
				if(this->_WyslijRaport())
					break;//Jak siê uda³o
			goto ZAMROZ;//... a jak nie
	   ZAMROZ:
	   case NIEWIEM:
	   default: //Gdy nie jest jasne co pocz¹æ, mrozimy proces w oczekiwaniu
			if(ZaplanowanyEfekt==RAPORT && DRAND()<0.2) //Czasem dajemy szanse na zmiane decyzji
					{  ZaplanowanyEfekt=NIEWIEM;ZaplOdbiorca=Swiat::INVINDEX;}
			//A zawsze przed³u¿amy proces w stanie ma³ych wymagañ
			IFTL(LOCLOG) clog<<endl<<"Wezel \""<<Proc->Nazwa()<<"\" ODWLEKL proces badawczy \""<<this->Nazwa()<<"\" D:"<<hex<<PodajDziedzine().ARGB<<" W/Po terminie ("<<dec<<this->DoKonca()<<")"<<endl;
			TLOG(LOCLOG, <<"Wezel \""<<Proc->Nazwa()<<"\" ODWLEK£ proces badawczy\t\""<<this->Nazwa()<<"\"\tD:\t"<<hex<<PodajDziedzine().ARGB<<"\tW/Po terminie\t"<<dec<<this->DoKonca()   )
			_IleKrokowDoKonca=365; //Odkladamy na rok
			WymaganaIloscPracy=PracaDotychczasowa*1.1;//Jeszcze 10%
			return ; //WYSKAKUJEMY Z FUNCJI ¯EBY NIE ZAKOPNCZYÆ PROCESU
	   }

	   if(ZR) //Tylko gdy proces  na solidnym wêŸle  który ma atrybut "Doswiadczenie"
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
//Przyjmowanie komunikatów - zwi¹zane g³ównie z procesem TT
//bo tu mo¿e zostaæ ustalony jakiœ odbiorca wyprodukowanej informacji
{                                            	assert(Co->Kanal()!=Swiat::INVINDEX);
	if(Co->Rodzaj()=="?SZUKAM.BADAN")
	{
		DziedzinaWKolorze D=this->PodajDziedzine();
		D=D.ARGB & Co->PodajDziedzine().ARGB;
		D.A=0;//To nas nie interesuje. Chwilowo?

		if(D.IleBitow()>0            //Jakies wspolne bity
		&& _ZespolRoboczy::_PodobienstwoDziedzin(D,PodajDziedzine())>DRAND())    //I ewentualnie prawdopodobienstwo
		{
			Komunikat* Klon=Co->Klonuj();     				assert(Klon->Kanal()!=Swiat::INVINDEX);
			Klon->UstawRodzaj("MAM.BADANIA!");
			D.A=Co->PodajDziedzine().A;//Numer produktu bêdzie ten sam
			Klon->UstawDziedzine(D);   //A w reszcie tylko obiecane bity

			if(  ! Klon->Zwrotnie()  )//Jak towarzyski to musialby istnieæ zwrotny link
			{   //A jak nie to tworzymy towarzyski do jednostki wskazanej w Co
				unsigned IndInny=Co->Nadawca();
				KomunikacjaTowarzyska* KT=dynamic_cast<KomunikacjaTowarzyska*>(Co);
				if(KT) IndInny=KT->OKimTaGadka();

				wb_ptr<KontaktTowarzyski>  Nowy( new KontaktTowarzyski(Procesor(),IndInny,WAGA_NA_DZIENDOBRY,DRAND())  );
				Nowy->UstawDziedzine(D); //Lacze pokazuje jakie bity byly
				unsigned Kanal=Swiat::WstawLacze(Nowy.give());       		assert(Kanal!=Swiat::INVINDEX);
				Klon->Zaadresuj(Kanal,true,0.1);		//Tu siê b³êdu juz nie spodziewam...
			}

			KomunikacjaTowarzyska* KlonT=dynamic_cast<KomunikacjaTowarzyska*>(Klon);
			if(KlonT) KlonT->OKimTaGadka()=Procesor(); //Informuje ¿e gada o sobie :-)
			if(Swiat::WstawInfo(Klon)==Swiat::INVINDEX)
					TLOG(0,<<"ODPOWIEDZ NIE WYSLANA" )
					else
					return true;  //Czyli obsluzyl?
		}
	}
	else
	if( Co->Rodzaj()=="ZAMOWIENIE!"
		&& //Jak jeszcze nie ma zaplanowanego konkretnego odbiorcy
	  (  this->ZaplOdbiorca==Swiat::INVINDEX
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
		if( D.IleBitow()>=C.IleBitow() )//Jak wspolne bity w calosci siê mieszcza w badaniu
		{
			KomunikacjaOficjalna* KO=dynamic_cast<KomunikacjaOficjalna*>(Co);
			if(KO && KO->KtoJestAutorem()==Co->Nadawca())//TYLKO DROGA OFICJALNA!!!
			{
				D.A=Co->PodajDziedzine().A;  //Oznakowanie do w³asciwego produktu
				this->UstawDziedzine(D);
				this->ZaplanowanyEfekt=RAPORT; //Nowy sposób "upublicznienia"
				this->ZaplOdbiorca=Co->Nadawca();//I nowy odbiorca
				WezelSieci* Odbiorca=Swiat::Wezel(this->ZaplOdbiorca);
				this->Dane[1]=Odbiorca->Nazwa();
				return true; //Obrobiony
			}
		}
	}
															assert(Co->Kanal()!=Swiat::INVINDEX);
	return false;
}

void JednostkaBadawcza::InterpretujKomunikat(Komunikat* Co)
//Przyjmowanie komunikatów
{  //Obróbka domyœlna za pomoc¹ procesów
   if(_KomunikatDoProcesow(Co)) //Jeœli któryœ z procesów obrobi³
				return ; //To ju¿ nic nie robimy

	//Obróbka wyspecjalizowana
	KomunikacjaOficjalna* KO=dynamic_cast<KomunikacjaOficjalna*>(Co);

	//Gdy wraca potwierdzenie publikacji lub patentu
	if(KO!=NULL
	&& KO->KtoJestAutorem()==this->MojID()
	&& (KO->Rodzaj()=="PUBLIKACJA" || KO->Rodzaj()=="PATENT")
	)
	{
		this->Sukcesy++; //Udana publikacja lub patent
		Waga*=1.01;//Wzrost wagi wez³a przy sukcesach
		//Na razie wp³ywa tylko na szerokoœæ procesów ale i tak
		//powinien byæ jakoœ logarytmicznie bo tak za bardzo rosn¹ w koncu
		unsigned KoopLinkInd=Co->Kanal();  //Gdy ³¹cze kooperacyjne to uznajemy za wykonane
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
	) //Jakieœ zamówienie, które nie znalaz³o odpowiedzi w aktualnych procesach
	//Jakaœ obietnica finansowanie, ale czegoœ innego ni¿ to czym zwyk³eœ siê zajmowaæ
	//Mo¿e spowodowaæ ¿e nieco zmieniamy dziedzinê zainteresowañ
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
// ale oczywiœcie mo¿e te¿ wykonywac procesy badawcze powsta³e w inny sposób
{
	//Zale¿ny od czasu spadek znaczenia wêz³a
	Waga*=0.99999; //Raczej powolny
	//Np. na posuniêcie do przodu procesów
	if(this->Doswiadczenie<0.89)
		this->_ZarzadcaProcesowFantazyjny();
		else
		this->_ZarzadcaProcesowFinezyjny();


	if(unsigned(Swiat::NumerKroku())%10==0)//Czasem chcemy posortowaæ priorytetowo i zobaczyæ ile jest  realnie
	{
		unsigned IleRealnie;
		Swiat::NajpilniejszyProc(this->MojID(),&IleRealnie,true);//Ale to nie zupelnie dziala
	}

	//Co jakiœ czas sprawdza czy mo¿e utworzyæ jakiœ proces badawczy
	//(zw³aszcza jak brak to go sobie tworzy)
	if(RANDOM(30)==0)
	{
		unsigned ile=Swiat::IleProcesow(this->MojID());
		unsigned ile_bad=0;
		ProcesBadawczy* Pr=NULL;
		for(unsigned i=0;i<ile;i++)   //raczej "ile" dla pewnoœci bo to sortowanie coœ dziwnie dzia³a
		 if((Pr=dynamic_cast<ProcesBadawczy*>(Swiat::Proc(i,this->MojID())))!=NULL)
		 {
			ile_bad++; //Moglby te¿ liczyæ obci¹zenie i dawaæ sobie wiêcej procesów jak niedoci¹¿ony
		 }

		if(ile_bad==0
		|| DRAND()< (0.1*IleEtatow*Innowacyjnosc*Proaktywnosc*Prolegalnosc*Doswiadczenie*(IleEtatow>2?1-Centralnosc:1)))//Na razie doœæ rzadko spontanicznie, chyba ¿e nie ma nic do roboty
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
			}while(D.ARGB==0); //Powtarza a¿ wylosuje niezerowy
			D.A=RANDOM(256)*Probiznesowosc; //Teraz "numer" konkretnego badania, ma³o probiznesowi maj¹ niskie numery
			Nowy->UstawDziedzine(D);

			double KosztyBadan=Nowy->_KalkulujKosztBadan();
			if(this->StacGoNaWydatek( KosztyBadan )  )
			{
				Nowy->PracaDotychczasowa=0; Nowy->TmpPorcjaPracy=1; //Na poczatek
				double IleDni=2*Czasochlonnosc*DRAND_LOOP(3);//Typowa czasochlonnosc w dniach bêdzie sredni¹
				Nowy->_IleKrokowDoKonca=IleDni;
				double NaIleOsob=0.5*(1.0+IleEtatow*DRAND());//Jeden pó³-etat lub wiêcej, czasem sporo wiêcej  *DRAND_LOOP(-3)
				Nowy->WymaganaIloscPracy=IleDni*NaIleOsob;

				if(Swiat::WstawProc(Nowy.give(),this->MojID())!=Swiat::INVINDEX)
				{
					this->PrzyjmijKase( - KosztyBadan); //Na pocz¹tku kasuje, bo potem mo¿e nie mieæ
					IFTL(LOCLOG+1) clog<<endl<<"Wezel \""<<Nazwa()<<"\" uruchomil nowy proces badawczy. Dni "<<IleDni<<". Etatow "<<double(NaIleOsob)<<endl;
					TLOG(LOCLOG, <<"Wêze³ \""<<Nazwa()<<"\" uruchomi³ nowy proces badawczy. Dni\t"<<IleDni<<"\t. Etatów\t"<<double(NaIleOsob) )
				}
				else
				TLOG( 0, <<"Wêz³owi \""<<Nazwa()<<"\" NIE uda³o siê uruchomiæ nowego procesu badawczego" )
			}
		}
	}
}


bool ProcesBadawczy::_WyslijPublikacje()
//Pomocnicza procedura publikowania - domyslnie szuka wêz³a PUBLIKATOR
{
  //if(DRAND()<0.1) return false;//DEBUG
  unsigned Publikator;
  if(ZaplOdbiorca!=-2 && ZaplOdbiorca!=Swiat::INVINDEX)
	  Publikator=ZaplOdbiorca;
	  else
	  Publikator=_ZespolRoboczy::DajPublikator();
  if(Publikator==Swiat::INVINDEX) //Nie ma gdzie publikowaæ
  {
	 ZaplOdbiorca=Swiat::INVINDEX;
	 if(DRAND()>0.8) ZaplanowanyEfekt=PATENT;//Zmieniamy plan na patentowanie
		else  ZaplanowanyEfekt=RAPORT;//Albo jakiœ raport
	 return false;
  }
  //Jest publikator
  unsigned KoopLinkInd=Swiat::INVINDEX;
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

  if(KoopLinkInd==Swiat::INVINDEX)
  {
	 wb_ptr<FormalnaKooperacja>  NewKoop(  new FormalnaKooperacja(Procesor(),Publikator,0.25)  );
	 if((KoopLinkInd=Swiat::WstawLacze(NewKoop.give()))!=Swiat::INVINDEX)
			;//OK
			else
			return false; //Nie uda³o siê utworzyæ
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
								return false; //Nie uda³o siê zaadresowaæ???
  if(Swiat::WstawInfo(KO.give())==Swiat::INVINDEX)
								return false; //Nie uda³o siê  wys³aæ
  return true;
}


bool ProcesBadawczy::_WyslijPatent()
//Pomocnicza procedura publikowania - domyslnie szuka wêz³a UPATENTOWY, publikacja wyklucza patent
{
  //if(DRAND()<0.1) return false;//DEBUG
  unsigned Patentowy;
  if(ZaplOdbiorca!=-2 && ZaplOdbiorca!=Swiat::INVINDEX)
	  Patentowy=ZaplOdbiorca;
	  else
	  Patentowy=_ZespolRoboczy::DajUrzadPatentowy();
  if(Patentowy==Swiat::INVINDEX) //Nie ma gdzie publikowaæ
  {
	 ZaplOdbiorca=Swiat::INVINDEX;
	 if(DRAND()>0.8) ZaplanowanyEfekt=PATENT;//Zmieniamy plan na patentowanie
		else  ZaplanowanyEfekt=RAPORT;//Albo jakiœ raport
	 return false;
  }
  //Jest publikator
  unsigned KoopLinkInd=Swiat::INVINDEX;
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

  if(KoopLinkInd==Swiat::INVINDEX)
  {
	 wb_ptr<FormalnaKooperacja>  NewKoop(  new FormalnaKooperacja(Procesor(),Patentowy,0.25)  );
	 if((KoopLinkInd=Swiat::WstawLacze(NewKoop.give()))!=Swiat::INVINDEX)
			;//OK
			else
			return false; //Nie uda³o siê utworzyæ
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
								return false; //Nie uda³o siê zaadresowaæ???
  if(Swiat::WstawInfo(KO.give())==Swiat::INVINDEX)
								return false; //Nie uda³o siê  wys³aæ
  return true;
}

bool ProcesBadawczy::_WyslijRaport()
//Pomocnicza procedura raportowania - do odbiorcy albo administracji jak brak
{
	if(this->ZaplOdbiorca==Swiat::INVINDEX)
					return false; //Nie jest jasne komu ten raport wyslaæ

	KomunikacjaOficjalna* KO=new KomunikacjaOficjalna("RAPORT.BADANIA",Procesor());
	KO->UstawDziedzine(PodajDziedzine());
	WezelSieci* MojProcesor=Swiat::Wezel(this->Procesor());


	//unsigned Odbiorca=Swiat::INVINDEX;//Preferowany odbiorca
	if(this->ZaplOdbiorca==-2)//Domyslny odbiorca
	{
		//Domyslny odbiorca - dosyæ skomplikowane szukanie adresata raportu.
		unsigned Kanal=Swiat::INVINDEX;
		for(unsigned i=0;i<Swiat::IlePowiazan();i++)
		{
			PodlegloscOrganizacyjna* P=dynamic_cast<PodlegloscOrganizacyjna*>(Swiat::Lacze(i));
			if(P!=NULL && P->Poczatek()==Procesor())
			{
				Kanal=i;
				break;
			}
		}

		if(  Kanal==Swiat::INVINDEX  )
		{
		   IFTL(LOCLOG+1) clog<<endl<<"Proces "<<Nazwa()<<" na \""<<MojProcesor->Nazwa()<<"\"nie ma kanalu do jednostki nadrzednej administracyjnie"<<endl;
		   TLOG(LOCLOG, <<"Proces "<<Nazwa()<<" na \""<<MojProcesor->Nazwa()<<"\"nie ma kanalu do jednostki nadrzednej administracyjnie")
		   return true; //Nie ma instytucjonalnego odbiorcy raportu - juz z tym nic nie da zrobic
		}

		if(! KO->Zaadresuj(Kanal,true,0.05)  )
		  {
			  TLOG(0, <<"NIEUDANE ADRESOWANIE w ProcesBadawczy NIE POWINNO SIÊ ZDARZAC!!!???")
			  goto ERROR;
		  }
	}
	else
	{
	   if(! KO->Zaadresuj(Procesor(),ZaplOdbiorca,0.05))  //Dowolnym kanalem... ?
	   {
		  IFTL(LOCLOG) clog<<endl<<"NIEUDANE RAPORTOWANIE  \""<<Nazwa()<<"\" na "<<MojProcesor->Nazwa()<<"\" ZERWANA KOOPERACJA!!!???"<<endl;
		  TLOG(0, <<"NIEUDANE RAPORTOWANIE \""<<Nazwa()<<"\" na "<<MojProcesor->Nazwa()<<"\" ZERWANA KOOPERACJA!!!???")
		  goto ERROR;//	return false; //Raczej siê nie powinno zdarzaæ
	   }
	}

	//Wysylanie
	if(Swiat::WstawInfo(KO)!=Swiat::INVINDEX)
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
//Do rozsylania informacji ¿e siê czyms takim zajmujemy - socjalna lub oficjalna
{
  Powiazanie* LNK;
  if(link!=Swiat::INVINDEX && (LNK=Swiat::Lacze(link))!=NULL )
  {
	  if(LNK->Poczatek()!=Procesor())//Trzeba szukaæ symetrycznego ³¹cza towarzyskiego
	  {
		 link=ProcesSpoleczny::_JestPowiazanySocjalnie(Procesor(),LNK->Poczatek());
		 if(link==Swiat::INVINDEX) //Nie ma?
				return false;  //Mog³by tworzyæ
	  }
	  //A w przeciwnym wypadku jest chyba OK, choæ mo¿e byæ to ³¹cze jakoœ "upierdliwe"? (nie przepuszczalne)
  }
  else //Sam musi ustaliæ komu to wys³aæ po ³aczach towarzyskich
  {
	 unsigned ile_laczy=Swiat::IlePowiazan();
	 for(unsigned i=0;i<ile_laczy;i++)
	   if( (LNK=Swiat::Lacze(i))!=NULL
	   && (LNK->Poczatek()==Procesor())
	   && (dynamic_cast<KontaktTowarzyski*>(LNK)!=NULL)
	   && (DRAND()<LNK->Waznosc())
	   )
	 {  //Zosta³ wybrany
		link=i;
		break; //A mo¿e bez break??? Albo pocz¹tkowe (+-starsze?) s¹ preferowane albo koncowe
	 }
	 //A mo¿e robiæ tu te¿ nowe? Ale czy to by nie by³o dublowanie ProcesuSpolecznego?
	 //...
  }
  //GOTOWY:
  if(link!=Swiat::INVINDEX)//Nie znalaz³ takiego!
  {
	wb_ptr<KomunikacjaOficjalna> KT( new  KomunikacjaOficjalna("PREZENTACJA",Procesor())  );
	KT->UstawDziedzine(this->PodajDziedzine());
	if(KT->Zaadresuj(link,true,0.05)) //Jak siê uda zaadresowaæ
	{
	 if(Swiat::WstawInfo(KT.give())!=Swiat::INVINDEX)
				{ return true;}
				else
				{
					TLOG(0, <<"BLAD WYSYLANIA KTORY NIE POWINIEN SIE ZDARZAÆ" )
					return false; // A TO DZIWNE, chyba ¿e zabrak³o miejsca na komunikaty
				}
	} //else Blad adresowania
  } //else Brak linku
  return false; //Cha³ka, ale w koncu kogo to obchodzi... Byle czasem siê udawa³o  :-)
}

void SystemInformacyjny::InterpretujKomunikat(Komunikat* Co)
//Przyjmowanie TYLKO OFICJALNYCH komunikatów
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
	//Np. na posuniêcie do przodu procesów
	//_ZarzadcaProcesowLeniwy();  - o wiele za ma³o efektywny, du¿e opoŸnienia w zwyk³ych raportach
	if(this->Doswiadczenie<0.75)
		this->_ZarzadcaProcesowFantazyjny();
		else
		this->_ZarzadcaProcesowFinezyjny();
}

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
		  TLOG(0, <<"NIEUDANE ADRESOWANIE NIE POWINNO SIÊ ZDARZAC!!!???")
		 //	return false; //Raczej siê nie powinno zdarzaæ
	}
	else
	{  	//Niezdefiniowany odbiorca - dosyæ skomplikowane szukanie adresata raportu.
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
			  TLOG(0, <<"NIEUDANE ADRESOWANIE NIE POWINNO SIÊ ZDARZAC!!!???")
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

