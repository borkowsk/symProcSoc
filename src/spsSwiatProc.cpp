/// \file
/// \brief Definicje danych i funkcji świata
///        ---------------------------------
///
/// \details
///      UŻYWAMY LINIOWEJ ORGANIZACJI KONTENERÓW, ALE NIELINIOWO JEJ UŻYWAMY.
///      (TODO) MOŻNA BY TO ZASTĄPIĆ JAKĄŚ INNĄ IMPLEMENTACJĄ
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
/// \date
///     2022.11.09 (last updated)
//*////////////////////////////////////////////////////////////////////////////////

#include "spsModel.h"

#include "INCLUDE/wb_ptr.hpp"
#include <cassert>
#include <iostream>
#include <float.h>

using namespace std;

#include "lingo.hpp"

using namespace wbrtm;

unsigned DefLiczbaProcesowNaWezel=2;

/// Lokalnie tylko powinno być.
struct _PodtablicaProcesow
{
  wb_dynarray<Proces*> Tab;
  unsigned wolnych;
  _PodtablicaProcesow():wolnych(0){}
};

/// Lokalna tablica procesów
static wb_dynarray< _PodtablicaProcesow >   Procesy;

static unsigned PoIleProcesowAlokowac=1;

/// ...
bool Swiat::_UstalPoczLiczbeProc(unsigned IleMaxDlaWezla)
{
  unsigned len=Swiat::IleMoznaWezlow(); //Tyle, co najmniej musi być list procesów

  if(len==0)
      return false; //Coś jest nie tak z pewnością, ale może będzie kiedyś lepiej

  Procesy.alloc(len);
  PoIleProcesowAlokowac=IleMaxDlaWezla;
  return Procesy.get_size()==Swiat::IleWezlow(); //I to się musi udać raczej, ale ...
}

/// \param Jaki - proces do wstawienia.
/// \param KtoryWezel - Musi byc jasne który węzeł ma przyjąć proces.
unsigned Swiat::WstawProc(Proces* Jaki,unsigned KtoryWezel)
{
								                                         assert(Procesy.get_size()>=Swiat::IleWezlow());
	Jaki->_MojWezel=KtoryWezel; //Jakby kod zarządzający nie zadbał?

	if(!Jaki->Poprawny()) //Teraz powinien byc już poprawny, czyli gotowy do działania
	{
		cerr<<'\b'<<_LPL("Niepoprawny proces nie zostanie uruchomiony",
                         "Invalid process can't be started" )<<endl;
		delete Jaki;
		return -1;
	}

	if(Procesy[KtoryWezel].wolnych==0) //Trzeba realokować jak brak miejsca
	{
	   wb_dynarray<Proces*> Pom=Procesy[KtoryWezel].Tab; //TRANSFER!!!
	   unsigned len=Pom.get_size()+PoIleProcesowAlokowac;
	   Procesy[KtoryWezel].Tab.alloc(len);
	   Procesy[KtoryWezel].Tab.fill(NULL); //Zapewniamy pustostan
	   unsigned bytes=Pom.get_size()*sizeof(Proces*);

	   if(bytes>0)
	   {
		Proces** Tar=Procesy[KtoryWezel].Tab.get_ptr_val(0);
		Proces** Sou=Pom.get_ptr_val(0);
		memcpy(Tar,Sou,bytes);
		Pom.dispose(); //wskaźniki nie mają destruktorów więc to bezpieczne (TODO CHECK IT)
	   }

	   Procesy[KtoryWezel].wolnych=PoIleProcesowAlokowac;
	}

	//Teraz szukamy wolnego miejsca. Od końca
	int i; //Pozycja
	for(i=Procesy[KtoryWezel].Tab.get_size()-1;i>=0;i--)
		if(Procesy[KtoryWezel].Tab[i]==NULL)
			break;
										                                                             assert(i>=0); //!!!
										                                          assert(Procesy[KtoryWezel].wolnych>0);
	Procesy[KtoryWezel].wolnych--;
	Procesy[KtoryWezel].Tab[i]=Jaki;
	Jaki->_MojIndeks=i;

	return i;
}

/// Procesy są zawsze związane z węzłami. \return NULL jak nie ma węzła, i to kłopot.
Proces*		Swiat::Proc(unsigned Ktory,unsigned KtoryWezel/*=-1*/)
{
	if(KtoryWezel == INV_INDEX)
	{
		                                                         assert("Wariant nie zaimplemetowany"=="Swiat::Proc()");
		return NULL; //Na razie nic z tym nie umie zrobić
	}
	else
	{
		if(Ktory<Procesy[KtoryWezel].Tab.get_size())
			return Procesy[KtoryWezel].Tab[Ktory];
			else
			return NULL;
	}
}

/// Funkcja sortowania procesów wg. priorytetu.
int sort_function_procesy(const void *a, const void *b)
{
   Proces* Ap=*(Proces**)a;
   Proces* Bp=*(Proces**)b;
   double Zb=Ap?Ap->Priorytet():-1000000; //W kolejności odwrotnej!
   double Za=Bp?Bp->Priorytet():-1000000; //A nieistniejące na koniec!!!
   if(Za>Zb) return 1;
   else if(Za==Zb) return 0;
		else return -1;
}

/// Daje indeks procesu o najwyższym priorytecie
unsigned     Swiat::NajpilniejszyProc(unsigned KtoryWezel,unsigned* IleRealnie,bool Posortuj)
{
	if(!Posortuj)
	{
		double PriorMax=-DBL_MAX;
		unsigned Ktory=Swiat::INV_INDEX;
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
		if(IleRealnie!=NULL)     //Gdy odbiorca jest zainteresowany?
			*IleRealnie=Licznik; //to mo�e dosta� te� liczb� realnie dzia�ajacych proces�w
		return Ktory; //Mo�e zwr�cic -1 jak nie ma proces�w!!!
	}
	else //if(Posortuj) //Jak sortowanie to najpilniejszy znajdzie si� na pocz�tku
	{
        // SORTOWANIE!!!
        // /////////////
		qsort(Procesy[KtoryWezel].Tab.get_ptr_val(),Procesy[KtoryWezel].Tab.get_size(),
              sizeof(Proces*),sort_function_procesy );

		unsigned Licznik=0;
		for(unsigned i=0;i<Procesy[KtoryWezel].Tab.get_size();i++)
		  if(Procesy[KtoryWezel].Tab[i]!=NULL)
					Licznik++;

        if(IleRealnie!=NULL)  //Gdy odbiorca jest zainteresowany?
			*IleRealnie=Licznik; //to mo�e dosta� te� liczb� realnie dzia�ajacych proces�w

		if(Licznik>0)
		{
			Proces* Pr=Procesy[KtoryWezel].Tab[0];                                                     assert(Pr!=NULL);
			double Prior=Pr->Priorytet(); //Debug
			return 0; //Jak sortowanie to najpilniejszy znajdzie si� na pocz�tku
		}
		else return Swiat::INV_INDEX; //Jednak nie ma nic
	}
}

/// Usuwanie procesu z węzła.
void 	 Swiat::UsunProc(unsigned Ktory,unsigned KtoryWezel/*=-1*/)
{
	if(KtoryWezel == INV_INDEX)
	{
		                                                     assert("Wariant nie zaimplementowany"=="Swiat::UsunProc()");
	}
	else
	{
		delete Procesy[KtoryWezel].Tab[Ktory];
		Procesy[KtoryWezel].Tab[Ktory]=NULL;
		Procesy[KtoryWezel].wolnych++;
    }
}

/// Usuwanie procesu ze świata na podstawie wskaźnika.
/// TODO: NIE ZAIMPLEMENTOWANE. I dlaczego unsigned zwraca?
unsigned Swiat::UsunProc(Proces* Jaki,unsigned KtoryWezel/*=-1*/)
{
	                                                     assert("Nie zaimplementowane"=="Swiat::UsunProc(Proces* Jaki)");
   //???
   return -1;
}

/// Jak podany numer węzła to działa lokalnie.
/// TODO: Jak `KtoryWezel==-1` to ogólnie w całym świecie.
unsigned Swiat::IleProcesow(unsigned KtoryWezel/*=-1*/)
{
	if(KtoryWezel == INV_INDEX)
	{
		                                                  assert("Wariant nie zaimplementowany"=="Swiat::IleProcesow()");
		return -1; //Na razie nic z tym nie umie zrobić
	}
	else
	{
		return Procesy[KtoryWezel].Tab.get_size();
	}
}

/// Jak podany numer węzła to działa lokalnie.
/// TODO: Jak `KtoryWezel==-1` to ogólnie w całym świecie.
/// \return liczba wolnych slotów, ale brak oznacza tylko że będzie realokacja!
unsigned Swiat::IleMoznaProcesow(unsigned KtoryWezel/*=-1*/)
{
	if(KtoryWezel == INV_INDEX)
	{
		                                            assert("Wariant nie zaimplementowany"=="Swiat::IleMoznaProcesow()");
		return -1; //Na razie nic z tym nie umie zrobic
	}
	else
	{
		return Procesy[KtoryWezel].wolnych; //Zwraca liczbę wolnych slotów, ale brak oznacza, tylko że będzie realokacja
	}
}

/* *******************************************************************/
/*			            SPS  version 2022                            */
/* *******************************************************************/
/*             THIS CODE IS DESIGNED & COPYRIGHT  BY:                */
/*              W O J C I E C H   B O R K O W S K I                  */
/*     Instytut Studiów Społecznych Uniwersytetu Warszawskiego       */
/*        RG:https://www.researchgate.net/profile/Wojciech-Borkowski */
/*        GitHub: https://github.com/borkowsk                        */
/*                                                                   */
/*                               (Don't change or remove this note)  */
/* *******************************************************************/






