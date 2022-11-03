/// \file
/// \brief Deklaracja/Definicja linku ogólnego, ale rysowanego jako łuk
///        -------------------------------------------------------------
///
/// \details
///              ...
///     ## (c)
///     Symulator Procesów Sieciowych/Społecznych (c) Instytut Studiów Społecznych
///     Uniwersytetu Warszawskiego, ul. Stawki 5/7., 2011 , wborkowski@uw.edu.pl
/// \date
///     2022.11.03 (last updated)
//*////////////////////////////////////////////////////////////////////////////////

#ifndef spsArcLinkH
#define spsArcLinkH

#include "spsGenLink.h" //Tu są deklaracje potrzebnych typów

/// \brief link ogólny, ale rysowanego jako łuk
class PowiazanieZagiete:public GenerycznePowiazanie
{
  public:
	PowiazanieZagiete();            ///< Domyślny konstruktor ustawiający pusty link
	PowiazanieZagiete(double par);  ///< Konstruktor z ustawieniem parametru
	/// \brief Powiązany obiekt wirtualnego konstruktora
	static KonstruktorElementowModelu<PowiazanieZagiete> WirtualnyKonstruktor;
    //ElementModelu::WirtualnyKonstruktor* VKonstruktor() { return &WirtualnyKonstruktor;} //TODO CZY TO POTRZBNE?

    /// \brief   Czytanie danych obiektu
	/// \details Metoda pobiera wszystkie potrzebne dane z listy stringów. Jak błąd to podaje która pozycja
	virtual bool ZrobWgListy(const std::string* Lista,unsigned Ile,unsigned& Blad);

    /// Destruktor
	virtual ~PowiazanieZagiete();

	/// \brief rysowanie \details Od zwykłego powiązania różni się tylko sposobem rysowania
	virtual void Narysuj();

    /// \brief czyszczenie z ekranu
	virtual void Wymazuj();

    /// \brief   Pozycja na połączeniu
    /// \details Przelicza położenie wzdłuż linku
    ///          Może uwzględniać też specyfikę komunikatu
	virtual void  PodajPozycje(double D,                        ///< Pozycja między źródłem a celem
                               bool KierunekZwykly,             ///< Czy kierunek zwykły czy odwrotny
							   double& X,                       ///< [out] X wynikowe
                               double& Y,                       ///< [out] Y wynikowe
                               Komunikat* K=NULL                ///< opcjonalnie link do komunikatu, jeśli jakaś specyfika.
                              );

    /// \brief   Zmienia parametr
    /// \details Parametr to wartość decydująca o sposobie zagięcia...
    ///          więc i pomocnicze zmienne się zmieniają.
	void UstawParametr(double par);

	protected: //Pola i inne atrybuty rodzinne
	void _PoliczParametryLuku();    // Liczy parametry łuku dla danej wartości parametru
	double parametr;                ///< Wartość decydująca o sposobie zagięcia

private:
	//Pomocnicze pola zawierające niezbędne parametry łuku
	double Xa,Ya,Xb,Yb,Xo,Yo,Promien,alfa,beta;
	unsigned krokow; //Ile odcinków łuku przy rysowaniu ~ odległość PK/10 pix
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
