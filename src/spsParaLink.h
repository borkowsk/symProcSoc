/// \file
/// \brief Deklaracja/Definicja linku ogólnego, ale rysowanego jako łuk paraboliczny
///        -------------------------------------------------------------------------
///
/// \details
///              ...
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
/// \date
///     2022.11.07 (last updated)
//*////////////////////////////////////////////////////////////////////////////////

#ifndef spsParaLinkH
#define spsParaLinkH

#include "spsGenLink.h"

/// \brief Klasa linku ogólnego, ale rysowanego jako łuk paraboliczny
class PowiazanieParaboliczne: public GenerycznePowiazanie
{
    static KonstruktorElementowModelu<PowiazanieParaboliczne> WirtualnyKonstruktor;
  public:
	PowiazanieParaboliczne();             /// \brief Domyślny konstruktor ustawiający pusty link
	PowiazanieParaboliczne(double par);   /// \brief Konstruktor z ustawieniem parametru
    virtual ~PowiazanieParaboliczne();    /// \brief Destruktor  wirtualny

	/// \brief Metoda pobiera wszystkie potrzebne dane z listy stringów. Jak blad to podaje która pozycja
	virtual bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);

    /// \brief Test poprawności
    virtual bool Poprawny();

    // Do wizualizacji
    // ///////////////
	virtual void Narysuj();    /// \brief Tak naprawdę to różni się tylko sposobem rysowania
	virtual void Wymazuj();    /// \brief Wymazywanie kolorem tła

    /// \brief Pozycja na powiązaniu
    /// \note Przelicza położenie wzdłuż linku ewentualnie uwzględniając też specyfikę komunikatu
	virtual void  PodajPozycje(double D,                ///< Jakie zaawansowanie dostarczenia (0..1)
                               bool KierunekZwykly,     ///< Jaki kierunek ruchu (z prądem czy pod prąd)
							   double& X,               ///< [out] obliczona współrzędna X
                               double& Y,               ///< [out] obliczona współrzędna Y
                               Komunikat* K=NULL        ///< wskaźnik do komunikatu, jeśli chcemy uwzględnić jego specyfikę
                                       );

    // Specyficzne dla takiego połączenia
    // //////////////////////////////////
	void  UstawWygiecie(double par);       ///< \brief Zmiana parametru łuku \note więc i pomocnicze zmienne się zmieniają
	float DajWygiecie(){ return parametr;} ///< \brief Dostęp do wartości parametru łuku

  protected:
	void _PoliczParametryLuku(); /// \brief Liczy zmienne pomocnicze łuku dla danej wartości parametru

	float parametr;  ///< Wartość decydująca o sposobie zagięcia

  private:
    unsigned krokow; ///< Ile odcinków łuku przy rysowaniu ~ odległość PK/10 pix

	//Pomocnicze pola zawierające niezbędne parametry łuku
	double Xa,Ya,Xb,Yb,Promien,alfa,cos_alfa,sin_alfa;
};

class PowiazanieParaboliczneSkierowane: public PowiazanieParaboliczne
{
    static KonstruktorElementowModelu<PowiazanieParaboliczneSkierowane> WirtualnyKonstruktor;
  public:
		virtual bool  Kierunkowy() { return true;}
};

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
#endif
