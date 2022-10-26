////////////////////////////////////////////////////////////////////////////////
// Symulator Procesów Sieciowych/Spolecznych (c) Instytut Studiów Spo³ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
////////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
////////////////////////////////////////////////////////////////////////////////
//
//---------------------------------------------------------------------------
#ifndef spsGenInfoH
#define spsGenInfoH
//---------------------------------------------------------------------------
#include "spsModel.h" //Tu s¹ deklaracje potrzebnych typow

class GeneryczneInfo:public Komunikat
{
  public:
	GeneryczneInfo(); //Generyczny konstruktor tworz¹cy pusty komunikat
	static KonstruktorElementowModelu<GeneryczneInfo> WirtualnyKonstruktor;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtualnyKonstruktor;}
	virtual const std::string& Rodzaj();//"???" - klasy potomne mog¹ mieæ rózne rodzaje interpretacji DziedzinyWKolorze komunikatu
	virtual bool  UstawRodzaj(const char* );
	virtual bool Poprawny(); //true jeœli kanal jest dobrze zdefiniowany (wci¹¿ istnieje link, nadawca i odbiorca etc...
	virtual double  Waznosc() { return Predkosc; }  //Po prostu jak szybko ma dotrzeæ. Trochê jak priorytet.
	//Metoda pobiera wszystkie potrzebne dane z listy stringów. Jak blad to podaje ktora pozycja
	virtual bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);
	virtual ~GeneryczneInfo();//Destruktor wirtualny, bo s¹ metody wirtualne
	virtual void Narysuj();
	virtual void Wymazuj();
	virtual void ChwilaDlaCiebie(); //Daje jakies szanse na endogenne zmiany stanów - np. zbli¿a do dostawy
  //Specyficzne dla Komunikatu (Message)
	virtual unsigned  Kanal() { return Link;} //Po ktorym po³¹czeniu czyli linku
	virtual bool 	  KierunekZgodny(){return Kierunek;}//Czy idzie zgodnie z nominalnym kierunkiem linku czy pod pr¹d
	virtual unsigned  Nadawca(); //Od kogo ...
	virtual unsigned Odbiorca(); //... do kogo.
	virtual float  JakDostawa(); //0 po starcie, jak 1 to ju¿ dotar³
	virtual float  JakiTermin();//W ile kroków powinien zostaæ dostarczony?
	//Specyficzne dla generycznego komunikatu
	virtual Komunikat* Klonuj(); //Robi na stercie kopie komunikatu do przechowania lub wstawienia
	bool 	Zwrotnie(float _Szybkosc=0);//Mo¿na odwróciæ komunikat - zw³aszcza jak jest to klon
	bool  	Zaadresuj(unsigned _Nadawca,unsigned _Odbiorca,float _Szybkosc=0); //Zwracaj¹ true jak jest
	bool  	Zaadresuj(unsigned _Powiazanie,bool KierunekZwykly,float _Szybkosc=0);//...poprawny kana³ komunikacji
  protected:
	//Rysowanie kszta³tu zale¿nego od typu potomnego, ale z uwzglêdnieniem wielkoœci i koloru)
	virtual void RysujKsztalt(float X,float Y,float Rad,unsigned R,unsigned G,unsigned B);
	GeneryczneInfo(const GeneryczneInfo* Wzor);  //Kopiuj¹cy jest potrzebny tylko Klonuj(), ale tez w klasach potomnych
		//Pola
		//DziedzKol  		Col;   //Dziedziczony z ElementModelu
		//DaneLokalne		Dane;
		unsigned 	    Link;      //U¿ywany kana³ komunikacji
		bool 			Kierunek;  //Czy zgodnie z kana³em czy "pod pr¹d"
		unsigned 		LicznikZycia;
		float			Predkosc; //Jak szybko ma dotrzeæ
};


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
#endif
