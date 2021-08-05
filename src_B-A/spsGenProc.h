////////////////////////////////////////////////////////////////////////////////
// Symulator Procesów Sieciowych/Spolecznych (c) Instytut Studiów Spo³ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
////////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
////////////////////////////////////////////////////////////////////////////////
//
//---------------------------------------------------------------------------
#ifndef spsGenProcH
#define spsGenProcH
//---------------------------------------------------------------------------
#include "spsModel.h" //Tu s¹ deklaracje potrzebnych typow

class GenerycznyProces:public Proces
{
  public:
	GenerycznyProces();//Konstruktor musi tylko zapewniæ sensowne powstanie
	GenerycznyProces(const char* Nazwa,float KoniecznaPraca,float DoDeadlinu);//I do tworzenia normalnie
	static KonstruktorElementowModelu<GenerycznyProces> WirtualnyKonstruktor;
	ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtualnyKonstruktor;}
	virtual ~GenerycznyProces();//Destruktor wirtualny, bo bêd¹ metody wirtualne
	virtual bool Poprawny(); //true jeœli jest dobrze zdefiniowany (wci¹¿ istnieje procesor)
	virtual double  Waznosc() { return Prior; }  //Po prostu priorytet innymi s³owy
  //Metoda pobiera wszystkie potrzebne dane z listy stringów. Jak blad to podaje ktora pozycja listy
	virtual bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);
  //REYSOWANIE ITP.
	virtual void Narysuj();
	virtual void Wymazuj();
	virtual void ChwilaDlaCiebie(); //Daje mu procesowi mo¿liwoœæ zmiany stanów - zrobienia czegoœ
  //Specyficzne dla Procesu-projektu
	virtual double   JakPracochlonny(){return  WymaganaIloscPracy;}
	virtual double   Priorytet(); //Im wy¿szy tym proces wiêcej dziala
	virtual double   JakZaawansowany();//0 po starcie, jak 1 to ju¿ zrobiony. PROCES_NAD_ZAAWANSOWANY - gotowy do usuniecia
	virtual void     UznajZaZakonczony() { TmpPorcjaPracy=-1; }
  protected:
	float WymaganaIloscPracy;//Ile "roboczogodzin" musi dzia³aæ ¿eby byæ gotowy
	float PracaDotychczasowa;//Ile ju¿ zrobiono
  public:
	float  Prior;//Jaki ma priorytet dzia³ania (mial  w poprzednim obliczeniu)
				//ale te¿ mo¿na "rêcznie" zmniejszaæ lub zwiêkszaæ jak potrzeba
	float  TmpPorcjaPracy;//O ile "roboczogodzin" posuwa siê w ka¿dym kroku czasu
						  //- zamiast parametru do ChwilaDlaCiebie()
						  //WSTAWIENIE TU WARTOSÆI UJEMNEJ (<0) OZNACZA KONIEC:
						  //JakZaawansowany() zwraca wtedy wartoœæ PROCES_NAD_ZAAWANSOWANY
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
