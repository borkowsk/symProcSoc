////////////////////////////////////////////////////////////////////////////////
// Symulator Procesów Sieciowych/Spolecznych (c) Instytut Studiów Spo³ecznych
// Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
////////////////////////////////////////////////////////////////////////////////
// Wersja okrojona dla OPI - Projekt "Transfer technologii 2011"
////////////////////////////////////////////////////////////////////////////////
//
// Definicje danych i funkcji Œwiata
// zwiazane z
///////////////////
// U¯YWAMY LINIOWEJ ORGANIZACJI KONTENERÓW, ALE NIELINIOWO JEJ U¯YWAMY.
// MO¯NA TO ZAST¥PIÆ JAK¥Œ INN¥ IMPLEMENTACJ¥
////////////////////////////////////////////////////////////////////////////////////////////
#include "spsModel.h"

#include "INCLUDE/platform.hpp"
#include "INCLUDE/wb_ptr.hpp"
#include <cassert>
#include <iostream>
#include <float.h>
using namespace std;
using namespace wbrtm;

unsigned DefLiczbaProcesowNaWezel=2;

//Lokalnie tylko powinno byc
struct _PodtablicaProcesow
{
  wb_dynarray<Proces*> Tab;
  unsigned wolnych;
  _PodtablicaProcesow():wolnych(0){}
};

wb_dynarray< _PodtablicaProcesow >   Procesy;

static unsigned PoIleProcesowAlokowac=1;

bool Swiat::_UstalPoczLiczbeProc(unsigned IleMaxDlaWezla)
{
  unsigned len=Swiat::IleMoznaWezlow();//Tyle co najmniej musi byæ list procesów
  if(len==0) return false; //Coœ jest nie tak z pewnoœci¹, ale mo¿e bêdzie kiedyœ lepiej
  Procesy.alloc(len);
  PoIleProcesowAlokowac=IleMaxDlaWezla;
  return Procesy.get_size()==Swiat::IleWezlow();//I to siê musi udaæ raczej, ale ...
}

unsigned Swiat::WstawProc(Proces* Jaki,unsigned KtoryWezel)
//Musi byæ jasne który wêze³
{
								assert(Procesy.get_size()>=Swiat::IleWezlow());
	Jaki->_MojWezel=KtoryWezel; //Jakby kod zarz¹dzaj¹cy nie zadba³?
	if(!Jaki->Poprawny()) //Teraz powinien byæ ju¿ poprawny, czyli gotowy do dzia³ania
	{
		cerr<<'\b'<<_LPL("Niepoprawny proces nie zostanie uruchomiony","Invalid process can't be started")<<endl;
		delete Jaki;
		return -1;
	}

	if(Procesy[KtoryWezel].wolnych==0)//Trzeba realokowaæ jak brak miejsca
	{
	   wb_dynarray<Proces*> Pom=Procesy[KtoryWezel].Tab; //TRANSFER!!!
	   unsigned len=Pom.get_size()+PoIleProcesowAlokowac;
	   Procesy[KtoryWezel].Tab.alloc(len);
	   Procesy[KtoryWezel].Tab.fill(NULL);//Zapewniamy pustostan
	   unsigned bytes=Pom.get_size()*sizeof(Proces*);
	   if(bytes>0)
	   {
		Proces** Tar=Procesy[KtoryWezel].Tab.get_ptr_val(0);
		Proces** Sou=Pom.get_ptr_val(0);
		memcpy(Tar,Sou,bytes);
		Pom.dispose();//wskaŸniki nie maj¹ destruktorów wiêc to bezpieczne
	   }
	   Procesy[KtoryWezel].wolnych=PoIleProcesowAlokowac;
	}
	//Teraz szukamy wolnego - od koñca
	int i;//Pozycja
	for(i=Procesy[KtoryWezel].Tab.get_size()-1;i>=0;i--)
		if(Procesy[KtoryWezel].Tab[i]==NULL)
			break;
										assert(i>=0);//!!!
										assert(Procesy[KtoryWezel].wolnych>0);
	Procesy[KtoryWezel].wolnych--;
	Procesy[KtoryWezel].Tab[i]=Jaki;
	Jaki->_MojIndeks=i;
	return i;
}

Proces*		Swiat::Proc(unsigned Ktory,unsigned KtoryWezel/*=-1*/)
//Zwi¹zane z wez³ami   - jak nie ma wêz³a to k³opot
{
	if(KtoryWezel==INVINDEX)
	{
		assert("Wariant nie zaimplemetowany"=="Swiat::Proc()");
		return NULL;//Na razie nic z tym nie umie zrobic
	}
	else
	{
		if(Ktory<Procesy[KtoryWezel].Tab.get_size())
			return Procesy[KtoryWezel].Tab[Ktory];
			else
			return NULL;
	}
}

int sort_function_procesy(const void *a, const void *b)
{
   Proces* Ap=*(Proces**)a;
   Proces* Bp=*(Proces**)b;
   double Zb=Ap?Ap->Priorytet():-1000000;//W kolejnosci odwrotnej!
   double Za=Bp?Bp->Priorytet():-1000000;//A nieistniejace na koniec!!!
   if(Za>Zb) return 1;
   else if(Za==Zb) return 0;
		else return -1;
}

unsigned     Swiat::NajpilniejszyProc(unsigned KtoryWezel,unsigned* IleRealnie,bool Posortuj)
//Daje indeks procesu o najwy¿szym priorytecie
{
	if(!Posortuj)
	{
		double PriorMax=-DBL_MAX;
		unsigned Ktory=Swiat::INVINDEX;
		unsigned Licznik=0;
		for(unsigned i=0;i<Procesy[KtoryWezel].Tab.get_size();i++)
		{
			Proces* Pr=Procesy[KtoryWezel].Tab[i];
			if(Pr==NULL) continue;
			Licznik++;
			double Priorytet=Pr->Priorytet();
			if(Priorytet>PriorMax)
			{
				PriorMax=Priorytet;
				Ktory=i;
			}
		}
		if(IleRealnie!=NULL)  //Gdy odbiorca jest zainteresowany?
			*IleRealnie=Licznik; //to mo¿e dostaæ te¿ liczbê realnie dzia³ajacych procesów
		return Ktory; //Mo¿e zwrócic -1 jak nie ma procesów!!!
	}
	else //if(Posortuj) //Jak sortowanie to najpilniejszy znajdzie siê na pocz¹tku
	{

		qsort(Procesy[KtoryWezel].Tab.get_ptr_val(),Procesy[KtoryWezel].Tab.get_size(),sizeof(Proces*),sort_function_procesy);

		unsigned Licznik=0;
		for(unsigned i=0;i<Procesy[KtoryWezel].Tab.get_size();i++)
		  if(Procesy[KtoryWezel].Tab[i]!=NULL)
					Licznik++;
		if(IleRealnie!=NULL)  //Gdy odbiorca jest zainteresowany?
			*IleRealnie=Licznik; //to mo¿e dostaæ te¿ liczbê realnie dzia³ajacych procesów
		if(Licznik>0)
		{
			Proces* Pr=Procesy[KtoryWezel].Tab[0];      assert(Pr!=NULL);
			double Prior=Pr->Priorytet(); //Debug
			return 0; //Jak sortowanie to najpilniejszy znajdzie siê na pocz¹tku
		}
		else return Swiat::INVINDEX; //Nie ma nic
	}
}

void 	 Swiat::UsunProc(unsigned Ktory,unsigned KtoryWezel/*=-1*/)
{
	if(KtoryWezel==INVINDEX)
	{
		assert("Wariant nie zaimplemetowany"=="Swiat::UsunProc()");
	}
	else
	{
		delete Procesy[KtoryWezel].Tab[Ktory];
		Procesy[KtoryWezel].Tab[Ktory]=NULL;
		Procesy[KtoryWezel].wolnych++;
    }
}

unsigned Swiat::UsunProc(Proces* Jaki,unsigned KtoryWezel/*=-1*/)    //Dlaczego unsigned zwraca?
{
	assert("Nie zaimplemetowane"=="Swiat::UsunProc(Proces* Jaki)");
   //???
   return -1;
}

unsigned Swiat::IleProcesow(unsigned KtoryWezel/*=-1*/)
//jak -1 to ogolnie, jak numer wêz³a to lokalnie
{
	if(KtoryWezel==INVINDEX)
	{
		assert("Wariant nie zaimplemetowany"=="Swiat::IleProcesow()");
		return -1;//Na razie nic z tym nie umie zrobic
	}
	else
	{
		return Procesy[KtoryWezel].Tab.get_size();
	}
}

unsigned Swiat::IleMoznaProcesow(unsigned KtoryWezel/*=-1*/)
//jak -1 to ogolnie, jak numer wêz³a to lokalnie
{
	if(KtoryWezel==INVINDEX)
	{
		assert("Wariant nie zaimplemetowany"=="Swiat::IleMoznaProcesow()");
		return -1;//Na razie nic z tym nie umie zrobic
	}
	else
	{
		return Procesy[KtoryWezel].wolnych; //Zwraca liczbê wolnych slotow, ale brak oznacza tylko ¿e bêdzie realokacja
	}
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







