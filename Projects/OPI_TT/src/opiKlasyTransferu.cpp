// Transfer technologii 2011 - w³aœciwa implementacja modelu     25-10-2011
// //////////////////////////////////////////////////////////////////////////////
// ***TODO!!!!-->
// Implementacja akcji klas w³aœciwego modelu bezpoœrednio zwi¹zanych z TT
// na poziomie rynkowym (BOTT) i naukowym (UOTT) i odpowiednie procesy oraz komunikaty
// (bez technikaliów obslugi interfaceu )
// //////////////////////////////////////////////////////////////////////////////

#include "opiKlasyWersja.hpp"
#include "INCLUDE/wb_smartlog.h"
using namespace wbrtm;

double OTT_FANTAZJA=0.05; //Jak czêsto OTT bez kontaktu z rynkiem "zapuszcza" sobie proces TT.
					//Oczywiscie przy zachowaniu innych uwarunkowañ
bool   PROMUJ_SKOMPLIKOWANE_ROZWIAZANIA=false;
double ILE_RAZY_PROTOTYP_DROZSZY_NIZ_TOWAR=2; //Na razie nieznacznie bo nie ma finansowania zewnetrznego
extern double ILE_RAZY_BADANIA_DROZSZE_NIZ_TOWAR; //Na razie nieznacznie bo nie ma finansowania zewnetrznego
extern const char* const  KONTO; //Marker komunikatu finansowego z przep³ywem

static const int LOCLOG=0; //Na jakim poziomie logowania domyœlnie zapisywaæ problemy z tego pliku

ProcesTransferuTech::ProcesTransferuTech()
{
	AutorPomyslu=-1;
	/*
	struct ElementListy {
	double Kiedy;unsigned Wykonawca;bool Check;
	ElementListy(){Kiedy=-1;Wykonawca=-1;Check=false;}
	};
	static const unsigned PRODUCENT=24;    //Gdzie zaznaczamy kto wyprodukuje
	static const unsigned FINANSOWANIE=25; //Gdzie zaznaczamy kto to finansuje
	ElementListy Fragmenty[32];//Sk¹d s¹ poszczególne fragmenty/elementy produktu
	*/
}

ProcesTransferuTech::ProcesTransferuTech(const char* Nazwa,DziedzinaWKolorze PomyslZweryfikowany,unsigned AutorPomyslu,float IleCzasu):
	   GenerycznyProces(Nazwa,IleCzasu*2,IleCzasu)
{
   this->AutorPomyslu=AutorPomyslu; //Autor ma pierwszenstwo we wspolpracy, ale jak siê wykpi to moze pomysl zrealizowaæ kto inny!
   ElementModelu::UstawDziedzine(PomyslZweryfikowany);
}

bool ProcesTransferuTech::InterpretujKomunikat(Komunikat* Co)
//Pobiera ró¿ne komunikaty i ³¹czy z nich pomys³y
//Tak ¿eby by³y zgodne z profilem firmy. Od czasu do czasu odpala
//zwyk³y ProcesTT
{                                                                assert(Co->Kanal()!=Swiat::INVINDEX);
	WezelSieci* Proc=Swiat::Wezel(Procesor());					assert(Proc!=NULL);

	if(Co->Rodzaj()=="WYPRODUKUJE!"
	&& Co->PodajDziedzine()==PodajDziedzine()
	&& (!Fragmenty[PRODUCENT].Check) )
	{
		//Znalaza³ producenta?
		KomunikacjaTowarzyska* KTCo=dynamic_cast<KomunikacjaTowarzyska*>(Co);
		if(KTCo)
		{ //W uproszczeniu bo tego ju¿ nie potwierdzamy
		 clog<<endl<<"ZNALEZIONO PRODUCENTA NA \""<<Proc->Nazwa()<<"\" PROJEKT TT: "<<hex<<PodajDziedzine().ARGB<<dec<<endl;
		 TLOG(LOCLOG,<<"ZNALEZIONO PRODUCENTA NA \""<<Proc->Nazwa()<<"\" PROJEKT TT: "<<hex<<PodajDziedzine().ARGB<<dec  )
		 Fragmenty[PRODUCENT].Kiedy=Swiat::NumerKroku();
		 Fragmenty[PRODUCENT].Wykonawca=KTCo->OKimTaGadka(); //Mo¿e przyjœæ poœrednio
		 Fragmenty[PRODUCENT].Check=true;//W uproszczeniu bo tego ju¿ nie potwierdzamy, ktoœ moglby przeslac fa³szywkê!
		 wb_ptr<FormalnaKooperacja>   FK(  new	FormalnaKooperacja(Procesor(),
																  Fragmenty[PRODUCENT].Wykonawca,
																  0.25,//Waga czyli udzia³ - umownie bo siê zmieni pod koniec projektu
																  this->_IleKrokowDoKonca*1.1 //Z zapasem 10%
																  )
										);//Konstruktowanie kooperacji wstepnej
		 FK->UstawDziedzine(PodajDziedzine()); //Link ma kolor produktu
		 Swiat::WstawLacze(FK.give());
		 return true;//Moje - i ju¿ nic wiecej nie potrzeba
		}
	}
	else
	if(Co->Rodzaj()=="MAM.BADANIA!" &&  Co->PodajDziedzine().A==PodajDziedzine().A) //Chyba chodzi o ten produkt
	{
		DziedzinaWKolorze D=Co->PodajDziedzine();
		D.A=0; //Zobaczmy o jakie bity chodzi?
		KomunikacjaTowarzyska* KTCo=dynamic_cast<KomunikacjaTowarzyska*>(Co);
		if(KTCo!=NULL && ZarejestrujCoSiePrzyda(D,KTCo->OKimTaGadka()))  //Czy to coœ faktycznie?
		{
			WezelSieci* Badacz=Swiat::Wezel(KTCo->OKimTaGadka());
			clog<<endl<<"ZNALEZIONO BADACZA \""<<Badacz->Nazwa()<<"\" DLA \""<<Proc->Nazwa()<<"\" D:"<<hex<<D.ARGB<<" DLA PROJEKTU TT: "<<PodajDziedzine().ARGB<<dec<<endl;
			TLOG(LOCLOG,<<"ZNALEZIONO BADACZA \""<<Badacz->Nazwa()<<"\" DLA \""<<Proc->Nazwa()<<"\" D:"<<hex<<D.ARGB<<" DLA PROJEKTU TT: "<<PodajDziedzine().ARGB<<dec )
			wb_ptr<FormalnaKooperacja>   FK(  new	FormalnaKooperacja(Procesor(),
																  KTCo->OKimTaGadka(),
																  0.25,//Waga czyli udzia³ - umownie bo siê zmieni pod koniec projektu
																  this->_IleKrokowDoKonca*1.5 //Z zapasem 10%
																  )
										);//Konstruktowanie kooperacji wstepnej
			FK->UstawWygiecie(DRAND()-0.5);
			FK->UstawDziedzine(PodajDziedzine()); //Link ma kolor produktu
			unsigned COlacze=Swiat::WstawLacze(FK.give());                        assert(COlacze!=Swiat::INVINDEX);

			//Po nawi¹zaniu kooperacji precyzujemy zamówieni na badania
			wb_ptr<KomunikacjaOficjalna> KO( new KomunikacjaOficjalna("ZAMOWIENIE!",Procesor()) );
			D.A=PodajDziedzine().A;
			KO->UstawDziedzine(D);//Nie koniecznie wszystkie bity z propozycji
			if(!KO->Zaadresuj(COlacze,true,0.05))
					TLOG(0,<<"NIEudane adresowanie na nowym linku kooperacyjnym!?" )
					else
					if(Swiat::WstawInfo(KO.give())==Swiat::INVINDEX)
						 TLOG(0,<<"NIEudane WYSYLANIE na nowym linku kooperacyjnym!?" )
			this->_IleKrokowDoKonca*=1.5; //Przedluzamy czas, skoro dopiero teraz znaleŸliœmy
			return true;//Moje - i ju¿ nikomu nic do tego
		}
	}
	else
	if(Co->Rodzaj()=="RAPORT.BADANIA")
	{
		DziedzinaWKolorze D=Co->PodajDziedzine();
		if( D.A==PodajDziedzine().A
		&& OdhaczWyniki(D,Co->Nadawca())
		)
		{
			WezelSieci* Badacz=Swiat::Wezel(Co->Nadawca());
			clog<<endl<<"RAPORT OD \""<<Badacz->Nazwa()<<"\" DLA \""<<Proc->Nazwa()<<"\" D:"<<hex<<D.ARGB<<" DLA PROJEKTU TT: "<<PodajDziedzine().ARGB<<dec<<endl;
			TLOG(LOCLOG,<<"RAPORT OD \""<<Badacz->Nazwa()<<"\" DLA \""<<Proc->Nazwa()<<"\" D:"<<hex<<D.ARGB<<" DLA PROJEKTU TT: "<<PodajDziedzine().ARGB<<dec )
			double KosztFaktury=_ZespolRoboczy::_KalkulujKosztProduktu(D)*ILE_RAZY_BADANIA_DROZSZE_NIZ_TOWAR*1.2;//I musieli coœ zarobiæ badacze
			_ZespolRoboczy* MyProcZR=dynamic_cast<_ZespolRoboczy*>(Proc);
			if(MyProcZR)
					MyProcZR->PrzyjmijKase( - KosztFaktury ); //W innych wypadkach nie mo¿e zabraæ kasy z wêz³a
			wb_ptr<KomunikacjaFinansowa> Zaplata( new KomunikacjaFinansowa(KONTO,KosztFaktury)  );
			if( !Zaplata->Zaadresuj(Co->Kanal(),Co->KierunekZgodny(),0.03)  )//Kasa idzie powoli, a w ogóle to sztuczka
					TLOG(0, <<"Nocna sztuczka nie wychodzi..." )
					else
					if(!Zaplata->Zwrotnie())//Je¿eli by³ kana³ spo³eczny to tu siê zmieni³ na odpowiedni w drug¹ stronê
						TLOG(0, <<"Nocna sztuczka z wyslaniem zaplaty za badania nie wychodzi..." )
						else
						if(Swiat::WstawInfo(Zaplata.give())==Swiat::INVINDEX)
						   TLOG(0, <<"Nie uda³o siê wys³¹æ zaplaty za badania nocn¹ sztuczk¹..." )
						   else
						   TLOG(LOCLOG, <<"Wys³ano zaplaty za badania dla\t"<<Badacz->Nazwa()<<"\t w projekcie TT:\t"<<hex<<PodajDziedzine().ARGB<<dec )
			return true;
		}
	}

	bool flaga=GenerycznyProces::InterpretujKomunikat(Co);
																 assert(Co->Kanal()!=Swiat::INVINDEX);
	return flaga;
}

void ProcesTransferuTech::SprawdzajZapotrzebowania()
{
	DziedzinaWKolorze D=WzorzecZapotrzebowania();
	//Mamy te których wci¹¿ brakuje
	D.A=this->PodajDziedzine().A; //To trzeba skopiowaæ
	 //JAKIS IF???
	{
	 wb_ptr<KomunikacjaTowarzyska> Wzor( new KomunikacjaTowarzyska("?SZUKAM.BADAN",Procesor()) );
	 Wzor->UstawDziedzine(D);
	 if(this->AutorPomyslu!=Procesor() && DRAND()>0.33)//Istnieje naukowy autor pomys³u
	 {  //Ma specjalny status w wysylaniu zapotrzebowan naukowych - jak sam nie zrobi to moze przekaze?
		unsigned ile_linkow=Swiat::IlePowiazan();
		Powiazanie* link=NULL;
		for(unsigned i=0;i<ile_linkow;i++)
		 if((link=Swiat::Lacze(i))!=NULL
		 && link->Poczatek()==Procesor()
		 && link->Koniec()==AutorPomyslu //Albo jakiekolwiek ³¹cze do autora?
		 && Wzor->Zaadresuj(i,true,0.1) )
		 {
			if(Swiat::WstawInfo(Wzor.give())==Swiat::INVINDEX)
				TLOG(0, <<"NIE UDA£O SIÊ WYS£AÆ" )
			break;//Wiecej nie próbujemy
		 }
	 }
	 else //Jak jest autor to i tak czasem do innych
	 _RozeslijPoLinkachSocjalnych(Wzor.get_ptr_val());
	}

	//Co z wykonawc¹
	if( !Fragmenty[PRODUCENT].Check )    //Gdzie zaznaczamy kto wyprodukuje
	{
		wb_ptr<KomunikacjaTowarzyska> Wzor( new KomunikacjaTowarzyska("?SZUKAM.PRODUCENTA",Procesor()) );
		Wzor->UstawDziedzine(PodajDziedzine());
		_RozeslijPoLinkachSocjalnych(Wzor.get_ptr_val());
	}
	//Co z finansami?
	if( !Fragmenty[FINANSOWANIE].Check ) //Gdzie zaznaczamy kto to finansuje
	{
		wb_ptr<KomunikacjaTowarzyska> Wzor( new KomunikacjaTowarzyska("?SZUKAM.FUNDUSZY",Procesor()) );
		Wzor->UstawDziedzine(PodajDziedzine());
		_RozeslijPoLinkachSocjalnych(Wzor.get_ptr_val());
	}
}

void ProcesTransferuTech::ChwilaDlaCiebie()
//Endogenne zmiany stanów - nigdy siê nie koñczy...
{
	if(this->JakZaawansowany()==0)
		 RemanentPoczatkowy(); //Tylko raz

	GenerycznyProces::ChwilaDlaCiebie();  //Teraz ju¿ mo¿na formalnie popchn¹æ
	//I zabrac siê za prawdziwe dzia³ania
	SprawdzajZapotrzebowania();

	//A jak ju¿ koniec to ustaliæ wynik i ewent. wdrozyæ rezultaty
	if(this->JakZaawansowany()>=1)
	{
		if(RemanentCzySukces())
			 ObsluzSukces();
		else
			ObsluzPorazke();
		this->UznajZaZakonczony();
	}
}


void ProcesTransferuTech::ObsluzPorazke()
//W razie pora¿ki zamienia linki koop. na s³abe powi¹zania socjalne (gdy ich brak) lub obni¿a wagi
{
	 WezelSieci* Proc=Swiat::Wezel(Procesor());							        assert(Proc!=NULL);
	 IFTL(LOCLOG) clog<<endl<<"Porazka na "<<Proc->Nazwa()<<" TT: "<<hex<<PodajDziedzine().ARGB<<dec<<" ... ";
	 TLOG(LOCLOG, <<"Porazka na "<<Proc->Nazwa()<<" TT: "<<hex<<PodajDziedzine().ARGB<<dec  )
	 _ZespolRoboczy* MyProcZR=dynamic_cast<_ZespolRoboczy*>(Proc);
	 if(MyProcZR)
		  MyProcZR->ZmienWage(0.99);//Nie za du¿o bo sukcesy s¹ bardzo rzadkie, aprób du¿o

	 unsigned ilu=0;
	 unsigned Badacze[24];//Nie ma ich wiêcej niz bitow
	 for(unsigned i=0;i<24;i++)
	 {
		unsigned Roboczy=Fragmenty[i].Wykonawca;
		if(Roboczy==Swiat::INVINDEX)
				goto CONTINUE;
		for(unsigned j=0;j<ilu;j++)
		   if(Badacze[j]==Roboczy) //Ju¿ byl
			   goto CONTINUE;
		 Badacze[ilu++]=Roboczy;
		 CONTINUE: ;
	 }
	 if(ilu>0)
	 {
	   clog<<endl<<"Oslabienie polaczen spolecznych \""<<Proc->Nazwa()<<"\" do zaangazowanych badaczy:";
	   TLOG(LOCLOG+1,<<"Oslabienie polaczen spolecznych \""<<Proc->Nazwa()<<"\" do zaangazowanych badaczy."  )
	 }
	 for(unsigned j=0;j<ilu;j++)
	 {
		 WezelSieci* Bada=Swiat::Wezel(Badacze[j]);								assert(Proc!=NULL);
		 clog<<endl<<"  * "<<Bada->Nazwa();
		 unsigned link1=ProcesSpoleczny::_JestPowiazanySocjalnie(Procesor(),Badacze[j]);
		 if(link1!=Swiat::INVINDEX)
		 {
			KontaktTowarzyski* KT=dynamic_cast<KontaktTowarzyski*>(Swiat::Lacze(link1));
																				assert(KT!=NULL);
			KT->ZmienWage(0.5);//Silne rozczarowanie
			clog<<" -->DO ";
		 }
		 unsigned link2=ProcesSpoleczny::_JestPowiazanySocjalnie(Badacze[j],Procesor());
		 if(link2!=Swiat::INVINDEX)
		 {
			KontaktTowarzyski* KT=dynamic_cast<KontaktTowarzyski*>(Swiat::Lacze(link2));
																				assert(KT!=NULL);
			KT->ZmienWage(0.5);//Silne rozczarowanie
			clog<<" OD--> ";
		 }
	 }
	 clog<<endl;
}

void ProcesTransferuTech::ObsluzSukces()
//Przekazuje produkt do produkcji i przeksztalca sieæ linków
{
	 WezelSieci* Proc=Swiat::Wezel(Procesor());									assert(Proc!=NULL);
	 WezelSieci* Prod=Swiat::Wezel(Fragmenty[PRODUCENT].Wykonawca);				assert(Prod!=NULL);
	 IFTL(LOCLOG) clog<<endl
				  <<"\a SUKCES na "<<Proc->Nazwa()<<" TT: "<<hex<<PodajDziedzine().ARGB<<dec<<" producent: "<<Prod->Nazwa();
	 TLOG(LOCLOG,    <<"SUKCES na "<<Proc->Nazwa()<<" TT: "<<hex<<PodajDziedzine().ARGB<<dec<<" producent: "<<Prod->Nazwa() )
	 //Zmiany parametrów po sukcesie
	 _ZespolRoboczy* MyProcZR=dynamic_cast<_ZespolRoboczy*>(Proc);
	 if(MyProcZR)
	 {
		  MyProcZR->Sukcesy++; //Udany transfer
		  MyProcZR->Doswiadczenie*=1.05; if(MyProcZR->Doswiadczenie>1) MyProcZR->Doswiadczenie=1;
		  MyProcZR->ZmienWage(1.05);//Sukcesy s¹ i tak rzadkie, ale przynosz¹ kasê która i tak zwiêksza wagê
	 }
	 //3 ify poni¿ej to niestetu wynik zmian designu modelu w trakcie -  do przemyslenie i poprawienia kiedyœ TODO!
	 Firma* MyProcF=dynamic_cast<Firma*>(Proc);
	 AdministracjaNaukowa* MyProcA=dynamic_cast<AdministracjaNaukowa*>(Proc);
	 JednostkaBadawcza* MyProcB=dynamic_cast<JednostkaBadawcza*>(Proc);
	 if(MyProcF)
	 {
		MyProcF->ProAkademickosc*=1.1; if(MyProcF->ProAkademickosc>1) MyProcF->ProAkademickosc=1;
	 }
	 else
	 if(MyProcA)
	 {
		MyProcA->Probiznesowosc*=1.1;  if(MyProcA->Probiznesowosc>1) MyProcA->Probiznesowosc=1;
	 }
	 else
	 if(MyProcB)
	 {
		MyProcB->Probiznesowosc*=1.1;  if(MyProcA->Probiznesowosc>1) MyProcA->Probiznesowosc=1;
	 }

	 //Szukanie zaangazowanych badaczy
	 unsigned ilu=0;
	 unsigned Badacze[24];//Nie ma ich wiêcej niz bitow
	 for(unsigned i=0;i<24;i++)
	 {
		unsigned Roboczy=Fragmenty[i].Wykonawca;
		if(Roboczy==Swiat::INVINDEX)
				goto CONTINUE;
		for(unsigned j=0;j<ilu;j++)
		   if(Badacze[j]==Roboczy) //Ju¿ byl
			   goto CONTINUE;
		 Badacze[ilu++]=Roboczy;
		 CONTINUE: ;
	 }

	 //Wzmocnienia spoleczne
	 if(ilu>0)
	   clog<<endl<<"Wzmocnienie polaczen spolecznych \""<<Proc->Nazwa()<<"\" do zaangazowanych badaczy:";
	 for(unsigned j=0;j<ilu;j++)
	 {
		 WezelSieci* Bada=Swiat::Wezel(Badacze[j]);					 			assert(Bada!=NULL);

		 JednostkaBadawcza* BadaZR=dynamic_cast<JednostkaBadawcza*>(Bada);
		 if(BadaZR) //Zwiekszenie parametrów po sukcesie JN. Innym nie, ale to i tak nie bardzo mieœci³oby siê w zalo¿eniach modelu
		 {
			BadaZR->Sukcesy+=2; //Udany transfer liczy siê bardziej ni¿ publikacja czy patent
			BadaZR->ZmienWage(1.01);//Sukcesy s¹ raczej rzadkie
			BadaZR->Doswiadczenie*=1.1; if(BadaZR->Doswiadczenie>1) BadaZR->Doswiadczenie=1;
			BadaZR->Probiznesowosc*=1.1;  if(BadaZR->Probiznesowosc>1) BadaZR->Probiznesowosc=1;
		 }

		 clog<<endl<<"  * "<<Bada->Nazwa();
		 unsigned link1=ProcesSpoleczny::_JestPowiazanySocjalnie(Procesor(),Badacze[j]);
		 if(link1!=Swiat::INVINDEX)
		 {
			KontaktTowarzyski* KT=dynamic_cast<KontaktTowarzyski*>(Swiat::Lacze(link1));
																				assert(KT!=NULL);
			KT->ZmienWage(2.5);//Silna euforia
			clog<<" -->DO ";
		 }
		 unsigned link2=ProcesSpoleczny::_JestPowiazanySocjalnie(Badacze[j],Procesor());
		 if(link2!=Swiat::INVINDEX)
		 {
			KontaktTowarzyski* KT=dynamic_cast<KontaktTowarzyski*>(Swiat::Lacze(link2));
																				assert(KT!=NULL);
			KT->ZmienWage(2.5);//Silna euforia
			clog<<" OD--> ";
		 }
	 }

	 double RndPar=DRAND(); //Do zagiecia linków
	 clog<<endl<<"Uruchomienie produkcji i stworzenie sieci dystrybucji wyplat na "<<Prod->Nazwa();
	 TLOG(LOCLOG,<<"Uruchomienie produkcji i stworzenie sieci dystrybucji wyplat na "<<Prod->Nazwa() )
	 string NazwaProduktu=Nazwa()+('A'+PodajDziedzine().IleBitow());
	 if(PodajDziedzine().A==0)
	 {
		DziedzinaWKolorze D=PodajDziedzine();D.A=1+RANDOM(99);
		   UstawDziedzine(D);
	 }
	 double Cena=_ZespolRoboczy::_KalkulujKosztProduktu(PodajDziedzine())*1.4;//Na poczatek wiêksza
	 //ProcesProdukcyjny(const char* Nazwa, unsigned IleSztuk,float JakaCena,unsigned IleWPaczce=-1);//Dzieli na 10 pak jak -1
	 wb_ptr<ProcesProdukcyjny> Proces( new ProcesProdukcyjny(NazwaProduktu.c_str(),1000,Cena,10));
	 Proces->UstawDziedzine(PodajDziedzine());
	 if(Swiat::WstawProc(Proces.give(),Fragmenty[PRODUCENT].Wykonawca)==Swiat::INVINDEX)
			TLOG(0, <<"NIE UDA£O SIÊ URUCHOMIÆ PROCESU! (???)" )
	 //A teraz linki kooperacyjne
	 //FormalnaKooperacja(unsigned Inicjator,unsigned Kooperator,float Waga/*=Udzial=1*/,unsigned Termin=(10*365));//Konstruktor domyœlny
	 if(Fragmenty[PRODUCENT].Wykonawca!=Procesor())
	 {
		wb_ptr<FormalnaKooperacja>  LinkCooP( new FormalnaKooperacja(Fragmenty[PRODUCENT].Wykonawca,Procesor(),
												0.1+DRAND()*0.1, 365*20)  ); //10%-20% dla organizatora
		LinkCooP->UstawDziedzine(PodajDziedzine());
		LinkCooP->UstawWykonano(true);
		if(Swiat::WstawLacze(LinkCooP.give())==Swiat::INVINDEX)
			TLOG(0, <<"NIE UDA£O SIÊ WSTAWIÆ £¥CZA CooP! (???)"<<"  * "<<Proc->Nazwa() )
			else
			clog<<endl<<"  * "<<Proc->Nazwa();
	 }
	 //A teraz pozostale linki kooperacyjne
	 for(unsigned j=0;j<ilu;j++)
	 {
		WezelSieci* Bada=Swiat::Wezel(Badacze[j]);								assert(Bada!=NULL);
		wb_ptr<FormalnaKooperacja>  LinkCooP( new FormalnaKooperacja(Fragmenty[PRODUCENT].Wykonawca,Badacze[j],
												0.2/ilu)  ); //do 20% dla badacza
		LinkCooP->UstawDziedzine(PodajDziedzine());
		LinkCooP->UstawWykonano(true);//¯eby siê kiedyœ jednak konczyl!
		if(Swiat::WstawLacze(LinkCooP.give())==Swiat::INVINDEX)
			TLOG(0, <<"NIE UDA£O SIÊ WSTAWIÆ £¥CZA CooP! (???)"<<"  * "<<Bada->Nazwa() )
			else
			clog<<endl<<"  * "<<Bada->Nazwa();
	 }
	 clog<<endl;
}

void ProcesTransferuTech::_RozeslijPoLinkachSocjalnych(Komunikat* Wzor)
//Mno¿y komunikat po linkach socjalnych
{
	unsigned ile_linkow=Swiat::IlePowiazan();
	Powiazanie* link=NULL;
	for(unsigned i=0;i<ile_linkow;i++)
	 if(
		(link=Swiat::Lacze(i))!=NULL
	 && link->Poczatek()==Procesor()
	 && 0.2>DRAND()	//Skladnik czysto losowy? A moze zalezny od stopnia zaawansowania procesu?
	 && pow(link->Waznosc(),2)>DRAND()//Skladnik zale¿ny od wagi linku
	 && (dynamic_cast<KontaktTowarzyski*>(link)!=NULL)
	 )
	 {
		Komunikat* Klon=Wzor->Klonuj();
		Klon->Zaadresuj(i,true,0.02+DRAND()*0.1);
		if(Swiat::WstawInfo(Klon)==Swiat::INVINDEX)
			TLOG(1, <<"NIEUDANE WYSYLANIE W PO KONTAKTACH TOWARZYSKICH::" );
	 }
}

double ProcesTransferuTech::SkalkulujKosztyProjektu(DziedzinaWKolorze D)
{
	double Koszty=_ZespolRoboczy::_KalkulujKosztProduktu(D);
	Koszty=Koszty*ILE_RAZY_PROTOTYP_DROZSZY_NIZ_TOWAR*1.5 // Dodaje 50% rezerwê
		+  Koszty*ILE_RAZY_BADANIA_DROZSZE_NIZ_TOWAR*1.5; //
	return Koszty;
}

DziedzinaWKolorze ProcesTransferuTech::WzorzecZapotrzebowania()
{
	DziedzinaWKolorze D;
	D.ARGB=0;
	for(unsigned i=0;i<24;i++)
	{
	   unsigned Maska=0x1<<i;
	   if( Fragmenty[i].Wykonawca==-1 && (!Fragmenty[i].Check) ) //Niepotrzebne jak Check jest
	   {  //Jeœli nie mamy wykonawcy a potrzebujemy
		   D.ARGB|=Maska;//tu rejestrujemy wci¹¿ potrzebn¹ wiedze...
	   }
	}
	return D;
}

bool ProcesTransferuTech::ZarejestrujCoSiePrzyda(DziedzinaWKolorze& D,unsigned Badacz)
{
	for(unsigned i=0;i<24;i++)//a tu patrzymy na wci¹¿ potrzebn¹ wiedze...
	{
	   unsigned Maska=0x1<<i;
	   if( (D.ARGB & Maska)!=0   //Jest ten bit w D
	   &&  (!Fragmenty[i].Check) //i ci¹gle go nie mamy
	   &&  (Fragmenty[i].Wykonawca==Swiat::INVINDEX) //i nikt nam go nie obieca³
	   )
	   {
			Fragmenty[i].Wykonawca=Badacz;
	   }
	   else //Nie chcemy tego bitu bo niepotrzeby albo ju¿ jest
	   {
		   D.ARGB&=~Maska; //Zostaj¹ tylko inne ni¿ Maska - zerowanie bitu z maski
	   }
	}
	return D.ARGB!=0;//Czy coœ zosta³o?
}

bool ProcesTransferuTech::OdhaczWyniki(DziedzinaWKolorze& D,unsigned Badacz)
{
	for(unsigned i=0;i<24;i++)//odhaczamy zdobyt¹ wiedze...
	{
	   unsigned Maska=0x1<<i;

	   if( (D.ARGB & Maska)!=0        //Jest ten bit w D
	   &&  (!Fragmenty[i].Check)      //i ci¹gle go nie mamy
	  // &&  (Fragmenty[i].Wykonawca==Badacz //Jakby bylo superlegalnie, ale nie przy tak ma³ej liczbie agentów w sieci!
	   )
	   {
			Fragmenty[i].Wykonawca=Badacz;
			Fragmenty[i].Check=true;
			Fragmenty[i].Kiedy=Swiat::NumerKroku();
	   }
	   else //Nie chcemy tego bitu bo niepotrzeby albo ju¿ jest
	   {
		   D.ARGB&=~Maska; //Zostaj¹ tylko inne ni¿ Maska - zerowanie bitu z maski
	   }
	}
	bool flaga=((D.ARGB & 0x00FFFFFF) !=0);
	return flaga;//Czy coœ zosta³o nie licz¹c A?
}

void ProcesTransferuTech::RemanentPoczatkowy()
{
	DziedzinaWKolorze D=PodajDziedzine();
	WezelSieci* Proc=Swiat::Wezel(Procesor());									assert(Proc!=NULL);
	clog<<endl<<"TT OTWARCIE NA \""<<Proc->Nazwa()<<"\" PROJEKT: ";
	clog<<hex<<D.ARGB<<" A:"<<D.A<<" R:"<<D.R<<" G:"<<D.G<<" B:"<<D.B<<"-->";
	TLOG(LOCLOG,<<"TT OTWARCIE NA \""<<Proc->Nazwa()<<"\" PROJEKT: "<<D.ARGB<<" A:"<<D.A<<" R:"<<D.R<<" G:"<<D.G<<" B:"<<D.B )
	D.A=0; //"A" siê nie liczy w odhaczaniu - jest markerem konkretnego zastosowania wiedzy
	clog<<hex<<D.ARGB<<" ";
	for(unsigned i=0;i<24;i++)//a tu odhaczamy wiedze...
	{
	   unsigned Maska=0x1<<i;
	   unsigned Res=D.ARGB & Maska;
	   if( Res == 0 ) //nie ma tego bitu z projekcie
	   {
		 Fragmenty[i].Kiedy=0;
		 Fragmenty[i].Wykonawca=-1;//Zaden wykonawca a jest potrzebny
		 Fragmenty[i].Check=true;//oznacza ¿e bit niepotrzebny!
		 clog<<"x";
	   }
	   else
		 clog<<"1"; //Jest bit - trzeba zdobyæ
	}
	//Jak ma w³asne podobne procesy produkcyjne
	unsigned ile_proc=Swiat::IleProcesow(Procesor());
	unsigned licznik=0; Proces* p;
	for(unsigned i=0;i<ile_proc;i++)
		if((p=Swiat::Proc(i,Procesor()))!=NULL && dynamic_cast<ProcesProdukcyjny*>(p)!=NULL)
		{
			float Podob=_ZespolRoboczy::_PodobienstwoDziedzin(p->PodajDziedzine(),this->PodajDziedzine());
			if(Podob>0.66) //Sa podobne
				licznik++;
		}
	if(licznik>0)//Moze jakoœ ulosowiæ?
	{  //Przyj¹³ ¿e SAM mo¿e produkowaæ
	   Fragmenty[PRODUCENT].Check=true;
	   Fragmenty[PRODUCENT].Wykonawca=Procesor();
	   clog<<"P!";
	}
	//Czy staæ go ¿eby przygotowaæ to samemu
	double Koszty=SkalkulujKosztyProjektu(PodajDziedzine());
	_ZespolRoboczy*  MojProc=dynamic_cast<_ZespolRoboczy*>(Swiat::Wezel(Procesor()));
	if(
	(MojProc!=NULL && MojProc->StacGoNaWydatek(Koszty))
	||
	(MojProc==NULL && OTT_FANTAZJA>DRAND())
	)
	{ //W ten czy inny sposób uznal ¿e tak
	   Fragmenty[FINANSOWANIE].Check=true;
	   Fragmenty[FINANSOWANIE].Wykonawca=Procesor();
	   clog<<"F!";
	}

	clog<<dec<<endl;
}

bool ProcesTransferuTech::RemanentCzySukces()
//Sprawdzenie czy siê uda³o
{
	WezelSieci* Proc=Swiat::Wezel(Procesor());									assert(Proc!=NULL);
	bool sukces=true;//Zakladamy ¿e siê uda³o i szukamy bledu z tym zalozoeniu
	DziedzinaWKolorze D=PodajDziedzine();
	clog<<endl<<"TT ZAMKNIECIE \""<<Proc->Nazwa()<<"\" PROJEKT: ";
	clog<<hex<<D.ARGB<<" A:"<<D.A<<" R:"<<D.R<<" G:"<<D.G<<" B:"<<D.B<<"-->";
	TLOG(LOCLOG,<<"TT ZAMKNIECIE \""<<Proc->Nazwa()<<"\" PROJEKT: "<<hex<<D.ARGB<<" A:"<<D.A<<" R:"<<D.R<<" G:"<<D.G<<" B:"<<D.B  )
	D.A=0; //"A" siê nie liczy w odhaczaniu - jest markerem konkretnego zastosowania wiedzy
	clog<<hex<<D.ARGB<<" ";

	for(unsigned i=0;i<24;i++)//a tu odhaczamy zdobyt¹ (lub nie) wiedze...
	{
		if(Fragmenty[i].Check==true)
		{
		   if(Fragmenty[i].Wykonawca==-1) //Niewa¿ny bit
				clog<<"x";
				else
				clog<<"V";        //Wa¿ny i zdobyty
		}
		else
		{
		   sukces=false;     //Brak kompletu niestety
		   clog<<"?";        //Nie ma go...
		}
	}
	if( Fragmenty[PRODUCENT].Check )    //Gdzie zaznaczamy kto wyprodukuje
		clog<<"P!";
		else
		{ clog<<"P?";sukces=false;} 	//Brak producenta
	if( Fragmenty[FINANSOWANIE].Check ) //Gdzie zaznaczamy kto to finansuje
		clog<<"F!";
		else
		{ clog<<"F?";sukces=false;}     //Brak finansowania
	clog<<dec<<endl;
	return sukces;
}

ProcesPoszukiwanTT::ProcesPoszukiwanTT():
		 GenerycznyProces("POSZUKIWANIE",365,365)
//Konstruktor domyslny wykonuje robotê za pomoc¹ domyœnego GenerycznyProces()
{
     ;
}

bool ProcesPoszukiwanTT::InterpretujKomunikat(Komunikat* Co)
//Pobiera ró¿ne komunikaty i ³¹czy z nich pomys³y
//Tak ¿eby by³y zgodne z profilem firmy, albo bierze PREZENTACJE badaczy.
//Probi z tego PROPOZYCJA'e produktów o które pyta RYNEK.
//Jak rynek odpowie ¿e OK i firma ma zasoby na kolejny proces
//to z pewnym prawdopodbienstwem odpala zwyk³y ProcesTT
{
	DziedzinaWKolorze D=Co->PodajDziedzine();
	unsigned        Rynek=_ZespolRoboczy::DajRynek();
	WezelSieci*		MyProc=Swiat::Wezel(Procesor());
	_ZespolRoboczy* MyProcZR=dynamic_cast<_ZespolRoboczy*>(MyProc);

	if(Co->Rodzaj()=="TEST.OK")//Ju¿ jest odpowiedz z RYNKU. Dowolna klasa komunikatu (?)
	{
		this->UstawDziedzine(D); //Bêdzie teraz myslal o uzyskanych wynikach
		if(D.A==0) 	D.A=1+RANDOM(255);
		unsigned Autor=Procesor(); //Prowizorycznie
		//Mo¿e tu jeszcze sprawdziæ liczbê ju¿ odpalonych Procesów TT albo Aktywnoœæ wêz³a?
		unsigned ile_proc=Swiat::IleProcesow(Procesor());
		unsigned licznik=0; Proces* p;
		for(unsigned i=0;i<ile_proc;i++)
			if((p=Swiat::Proc(i,Procesor()))!=NULL && dynamic_cast<ProcesTransferuTech*>(p)!=NULL)
				  licznik++;
		//... Bo co z tego ¿e obiecuj¹ce, skoro nie ma si³, œrodków lub chêci?
		double prob=1/(1.0+licznik*licznik); //Im ma ich wiêcej ju¿ ma tym mniej chetnie
		if(MyProcZR)
		{ //A dla klas typu zespó³ roboczy dokonujemy manipulacji z uwzglednieniem ich charakterystyki
		  prob*=MyProcZR->Efektywnosc*MyProcZR->Proaktywnosc*MyProcZR->Doswiadczenie;
		}

		if(DRAND()<prob)
		{
			//...Czy jest znany autor badanego raportu (powinien byæ zapisany jak to KomunikacjaOficjalna)
			KomunikacjaOficjalna* KO=dynamic_cast<KomunikacjaOficjalna*>(Co);
			if(KO) Autor=KO->KtoJestAutorem();

			_SprobujOdpalicProcesTT(D,Autor);
		}
		return true;//To jest ju¿ obrobione
	}
	//Dalej to ju¿ dobrze mieæ odnaleziony kontakt z rynkiem


	unsigned    iLMRK=Swiat::INVINDEX; //Na nuker powiazania z rynkiem
	Powiazanie* LMRK=NULL;//Do szukania powiazania

	if(Rynek!=Swiat::INVINDEX) //Jak w ogóle jest rynek na scenie!!!
	{
		unsigned ile_powiazan=Swiat::IlePowiazan();
		for(unsigned i=0;i<ile_powiazan;i++)
		if( (LMRK=Swiat::Lacze(i))!=NULL
		&& ((LMRK->Poczatek()==Procesor() && LMRK->Koniec()==Rynek && (!LMRK->Kierunkowy()))
				||
			(LMRK->Poczatek()==Rynek && LMRK->Koniec()==Procesor() && (!LMRK->Kierunkowy())))
		  )  //Jakiekolwiek po³¹czenie, byle symetryczne (najlepiej zwykle generic?)
		  {
			 iLMRK=i;//Odnaleziono po³¹czenie z rynkiem
			 break;
		  }
	}

	//Zbieranie informacji, wysylanie zapytan do rynku
	////////////////////////////////////////////////////////////////////////////
	unsigned Autor=Swiat::INVINDEX; //Kto jest autorem zapytania? Mo¿e byæ badacz!

	//Teraz sprawdzamy czy nie jest to niezamówiona prezentacja pomyslu
	if(Co->Rodzaj()=="PREZENTACJA" && DRAND()<0.1)//Tylko czasem zainteresuje siê naprawdê, inaczej bierze tylko bity jak zwykle
	{
		DziedzinaWKolorze Wspolna;
		Wspolna=D.ARGB & MyProc->PodajDziedzine().ARGB;
		if(Wspolna.IleBitow()>1)//??? Czy dziedzina pomys³u  ma coœ wspólnego dzia³alnosci¹ mojego wêz³a
		{                     //Albo od autora, albo za czyimœ poœrednictwem
		 KomunikacjaOficjalna* KO=dynamic_cast<KomunikacjaOficjalna*>(Co);
		 Autor=Co->Nadawca(); //Prowizorycznie bo
		 if(KO)
			Autor=KO->KtoJestAutorem(); //dla KomunikacjiOficjalnej mo¿emy znac autora!
		 if(D.A==0) //Tak byc nie mo¿e. Tylko produkty z definicji sceny mog¹ mieæ A==0
			D.A=1+RANDOM(255);
		}
		else
		return false;   //Zbyt inna ni¿ dziedzina zainteresowan
	}
	else //Inny komunikat, albo tylko inspiacja z prezentacji - po prosu zbieranie informacji,
	//robienie za ich pomoca mainpulacji na dziedzinie i wysylanie pytan czasem
	{
		WezelSieci* W=Swiat::Wezel(Procesor());

		D.ARGB&=0x1<<RANDOM(32); //Zostanie najwyzej jeden bit
		if(DRAND()<0.01)
		   D=this->PodajDziedzine().ARGB & D.ARGB;
		   else
		   D=this->PodajDziedzine().ARGB | D.ARGB; //Czêsciej pobiera ni¿ kasuje
		D.ARGB&=W->PodajDziedzine().ARGB; //Ale nie moze wyjœc poza dziedzinê wêz³a
		if(D.A==0) //Tak byc nie mo¿e. Tylko produkty ze sceny mog¹ mieæ A==0
			D.A=1+RANDOM(255);                                                  assert(D.A!=0);
		Autor=Procesor(); //Tu autorem pomys³u jest sam wêze³, czyli wszystko trzeba by robiæ od pocz¹tku
		if(DRAND()>0.1) //Tylko czasem pytamy, czeœciej tylko rejestrujemy
		{
			this->UstawDziedzine(D);
			return false;//Generalnie poza oficjalnymi odpowiedziami reszty komunikatów nie blokuje
		}
		Autor=Procesor(); //Tu autorem pomys³u jest sam wêze³, czyli wszystko trzeba by robiæ od pocz¹tku
	}

	if(iLMRK!=Swiat::INVINDEX) //Mamy link do rynku
	{                                                                           assert(Autor!=Swiat::INVINDEX);
																				assert(D.A!=0);
	 wb_ptr<KomunikacjaOficjalna> KO( new KomunikacjaOficjalna("TEST",Autor)  );
	 KO->UstawDziedzine(D); //Dziedzina pytania
	 bool Kierunek=(LMRK->Poczatek()==Procesor());
	 if(KO->Zaadresuj(iLMRK,Kierunek,0.05))
		if(Swiat::WstawInfo(KO.give())!=Swiat::INVINDEX)
				{//Udalo sie                 <<Nazwa()<<"na \""<<MyProc->Nazwa()<<"\" spytal rynek w sprawie D:"<<hex<<D.ARGB<<dec
				IFTL(LOCLOG+2) clog<<endl<<Nazwa()<<" na \""<<MyProc->Nazwa()<<"\" SPYTAL rynek w sprawie D:"<<hex<<D.ARGB<<" Au:"<<dec<<Autor<<endl;
				TLOG(LOCLOG, <<Nazwa()<<" na \""<<MyProc->Nazwa()<<"\" spytal rynek w sprawie D:"<<hex<<D.ARGB<<" Au:"<<dec<<Autor )
				}
				else
				TLOG(0, <<Nazwa()<<" na \""<<MyProc->Nazwa()<<"\" - WYSLANIE POWINNO SIÊ UDAÆ" )
	}
	else   //JAK NIE MA RYNKU TO CZASEM ODPALAMY MIMO BRAKU POTWIERDZENIA
	if(DRAND()<OTT_FANTAZJA*(MyProcZR?MyProcZR->Proaktywnosc:1)) //Probujemy start bez pytania rynku, jak go nie ma
	{
		//Mo¿e tu jeszcze sprawdziæ Aktywnoœæ wêz³a? Albo liczbê ju¿ odpalonych Procesów TT
		//... Bo co z tego ¿e obiecuj¹ce, skoro nie ma si³, œrodków lub chêci?
																				assert(Autor!=Swiat::INVINDEX);
		_SprobujOdpalicProcesTT(D,Autor);
	}

	return false;  //Generalnie poza oficjalnymi odpowiedziami reszty komunikatów nie blokuje
	//return GenerycznyProces::InterpretujKomunikat(Co);
}

void ProcesPoszukiwanTT::ChwilaDlaCiebie()
//Endogenne zmiany stanów - nigdy siê nie koñczy...
//Resztê robi w reakcji na komunikaty
{
	unsigned    MPRNID=this->Procesor();     					assert(MPRNID!=Swiat::INVINDEX);
	WezelSieci* MojProcesor=Swiat::Wezel(MPRNID);
	if(PROMUJ_SKOMPLIKOWANE_ROZWIAZANIA)
	   if(this->PodajDziedzine().ARGB==0) //Skopiowanie dziedziny gdy jej nie ma
				this->UstawDziedzine(MojProcesor->PodajDziedzine());

	GenerycznyProces::ChwilaDlaCiebie();
	if(this->JakZaawansowany()>=1)//Koniec "cyklu"
	{   //W prosty sposób zaczynamy od nowa
		this->PracaDotychczasowa=0;
		if(this->WymaganaIloscPracy<100)//1/3 etatu na poszukiwania to minimum
			this->WymaganaIloscPracy=100;
		this->_IleKrokowDoKonca+=365;//Cykl z definicji roczny (? NA RAZIE? - chyba bez znaczenia)
				//Jak skonczy przed terminem to ma trochê wiêcej czasu, a jak po to mniej (_IleKrokowDoKonca<0 !)
	}
}

void ProcesPoszukiwanTT::_SprobujOdpalicProcesTT(DziedzinaWKolorze Wzorzec,unsigned Wynalazca)
{                                                                               assert(Wynalazca!=Swiat::INVINDEX);
	WezelSieci*		MyProc=Swiat::Wezel( Procesor() );
	WezelSieci*		KtoWyn=Swiat::Wezel( Wynalazca  );
	if( (Wzorzec.ARGB & 0x00FFFFFF)==0) //Pusty projekt
		return; //Nie ma sensu
	IFTL(LOCLOG)
		clog<<endl<<"\""<<MyProc->Nazwa()<<"\" uruchamia proces TT dla D:"<<hex<<Wzorzec.ARGB<<dec<<" z pomyslu \""<<KtoWyn->Nazwa()<<"\""<<endl;
	TLOG(LOCLOG,  <<"\""<<MyProc->Nazwa()<<"\" uruchamia proces TT dla D:"<<hex<<Wzorzec.ARGB<<dec<<" z pomyslu \""<<KtoWyn->Nazwa()<<"\""  )
	string Nazwa="TRANSFER"; //Rok do trzech na skompletowanie  i pewnie ze dwa etaty menadzera
	wb_ptr<ProcesTransferuTech> NowyProces( new ProcesTransferuTech(Nazwa.c_str(),Wzorzec,Wynalazca,365+356*RANDOM(3) ) );
	Swiat::WstawProc(NowyProces.give(),Procesor());
}


void BOTT::ChwilaDlaCiebie()
{   //Ile ma procesow szukania inspiracj
	unsigned ile_proc=Swiat::IleProcesow(MojID());
	unsigned licznik=0; Proces* p;
	for(unsigned i=0;i<ile_proc;i++)
	if((p=Swiat::Proc(i,MojID()))!=NULL && dynamic_cast<ProcesPoszukiwanTT*>(p)!=NULL)
				  licznik++;

	if(licznik==0) //Brak najwazniejszego procesu!!!
	{
	   wb_ptr<ProcesPoszukiwanTT> Poszukiwanie( new ProcesPoszukiwanTT() );

	   Swiat::WstawProc(Poszukiwanie.give(),MojID());
    }

  if(Doswiadczenie<0.5)
	this->_ZarzadcaProcesowFantazyjny();
	else
	this->_ZarzadcaProcesowFinezyjny();
	//this->_ZarzadcaProcesowSprawiedliwy(); //Nie dzia³a
}

void UOTT::ChwilaDlaCiebie()
{
	if(this->Doswiadczenie<0.75)
		this->_ZarzadcaProcesowFantazyjny();
	else
		this->_ZarzadcaProcesowFinezyjny();
	//this->_ZarzadcaProcesowSprawiedliwy(); //Nie dzia³a
}

void BOTT::InterpretujKomunikat(Komunikat* Co)
//Przyjmowanie komunikatów
{
	Firma::InterpretujKomunikat(Co);
}

void UOTT::InterpretujKomunikat(Komunikat* Co)
//Przyjmowanie komunikatów
{
	AdministracjaNaukowa::InterpretujKomunikat(Co);
}

bool FormalnaKooperacja::Akceptacja(Komunikat* Co)
//Selekcja komunikatów
{    bool flaga=PowiazaniePaboliczne::Akceptacja(Co);
	 if(flaga )
	 {
		if(!this->CzyWykonany())
		{
			float DodatkowyCzas=Co->JakiTermin();
			this->UstawTermin(DajTermin()+DodatkowyCzas*1.1); //¯eby poczeka³ na odpowiedŸ
		}
	 }
	 else
	 {
         cerr<<"Komunikat "<<Co->Rodzaj()<<" nie na link KOOPeracyjny"<<endl;
     }
	 return flaga;
}

bool  FormalnaKooperacja::Poprawny()
//Jak po terminie to znika
{
	if(Termin<=0)
			return false;
	return PowiazaniePaboliczne::Poprawny();
}

void FormalnaKooperacja::ChwilaDlaCiebie()
//Jak siê zakoñczy to z czasem siê coraz bardziej wygina
{
	this->Termin--;
	if(this->Wykonany && Termin>0)
	{
		double param=this->DajWygiecie();
		param*=( 1.0 + 1.0/pow(Termin+5.0,1.4) ); //Im bli¿ej terminu tym szybciej siê wyginaj¹
		this->UstawWygiecie(param);
	}
}

// Proces zajmuj¹cy siê dawaniem grantów, doatacji lub po¿yczek na procent
/////////////////////////////////////////////////////////////////////////////////////
/*
class ProcesGrantowoPozyczkowy:public GenerycznyProces
{
	public:
		 ProcesGrantowoPozyczkowy();//Konstruktor domyslny wykonuje robotê za pomoc¹ domyœnego GenerycznyProces()
		 ProcesGrantowoPozyczkowy();//Konstruktor domyslny wykonuje robotê za pomoc¹ domyœnego GenerycznyProces()

	void ChwilaDlaCiebie(); //Endogenne zmiany stanów, popychanie pracy do przodu itp.
	bool InterpretujKomunikat(Komunikat* Co);//Odbiera propozycje, a przydziela kasê (i inkasuje zwroty)
	private:
	//Wewnetrzne w³aœciwoœci. Mo¿e u¿ywaæ te¿ cech wêz³a, np. jego sk³onnoœci do ryzyka
	float Fundusz; //Fundusze do podzia³u
	float OczekiwanyZwrot;//0 - dotacja, do 100% po¿yczki czêœcowo zwrotne, pow 100% pozyczki komercyjne
};
*/
ProcesGrantowoPozyczkowy::ProcesGrantowoPozyczkowy()
//Konstruktor domyslny wykonuje robotê za pomoc¹ domyœnego GenerycznyProces()
{
   Fundusz=0;OczekiwanyZwrot=0;//Fundusz mo¿e wzi¹æ z pól od swojego wêz³a
}

bool ProcesGrantowoPozyczkowy::InterpretujKomunikat(Komunikat* Co)
//Pobiera ró¿ne komunikaty i ³¹czy z nich pomys³y
//Tak ¿eby by³y zgodne z profilem firmy. Od czasu do czasu odpala
//zwyk³y ProcesTT
{
	return GenerycznyProces::InterpretujKomunikat(Co);
}

void ProcesGrantowoPozyczkowy::ChwilaDlaCiebie()
//Endogenne zmiany stanów - nigdy siê nie koñczy...
{
	GenerycznyProces::ChwilaDlaCiebie();
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


