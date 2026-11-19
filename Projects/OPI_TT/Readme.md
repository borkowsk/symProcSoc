# "Narzędzie informatycznego modelowania procesów społeczno-ekonomicznych towarzyszących transferowi technologii oraz roli Ośrodków Transferu Technologii w tym procesie"

Badanie zrealizowane na zlecenie Ośrodka Przetwarzania Informacji jako komponent projektu "Wsparcie systemu zarządzania badaniami naukowymi oraz ich wynikami", realizowanego w ramach Programu Operacyjnego Innowacyjna Gospodarka 2007-2013, Działanie 1.1, Poddziałanie 1.1.3., Sub-projektu „Analiza działalności ośrodków transferu technologii (zarządzanie oraz komercjalizacja B+R)” Projekt współfinansowany przez Unię Europejską ze środków Europejskiego Funduszu Rozwoju Regionalnego”

* 2011 - [RAPORT CAŁOŚCIOWY Z BADANIA:](https://www.researchgate.net/publication/298069033_Narzedzie_informatycznego_modelowania_procesow_spoleczno-ekonomicznych_towarzyszacych_transferowi_technologii_oraz_roli_Osrodkow_Transferu_Technologii_w_tym_procesie_-_RAPORT_CALOSCIOWY_Z_BADANIA)
* 2012 - [Prezentacja konferencja OPI 26-09-12:](https://www.researchgate.net/publication/306079099_Modelowanie_roli_instrumentow_polityki_innowacyjnej_panstwa_na_przykladzie_Osrodkow_Transferu_Technologii_-_Prezentacja_konferencja_OPI_26-09-12)


## ZAWARTOŚĆ REPOZYTORIUM
Katalog ten zawiera wszystko, co jest niezbędne do kompilacji aplikacji modelującej:

	OPITRANSFER - transfer technologii dla OPI.

W katalogu _SRC znajdują się kody źródłowe modelu, w katalogu SPS znajdują się kody źródłowe 
biblioteki wizualizacji dynamiki sieci, a w katalogu WBRTM kody źródłowe biblioteki zapieniającej 
ogólne usługi dla programu symulacyjnego oraz obsługę okna graficznego w środowisku 
WINDOWS. 
Biblioteki  te stanowią własność  intelektualną autora i zostały udostępnione w tej postaci, w celu 
umożliwienia wygodnego rozwijania modelu OPITRANSFER przez odbiorcę i nie mogą być używane 
do żadnego innego celu.

Pierwotnie w katalogach  `__DevCpp` i `__MSVC2008` oraz `__RADSTUDIO`  znajdowały się predefiniowane projekty 
dla kompilatorów __gcc__ w wersji __DevCpp__, __Microsoft Visual C++__ oraz __EMBARCADERO RAD STUDIO__ . 
Ścieżki tych projektów były ustawione tak, jakby cała zawartość katalogu źródeł była dyskiem  o 
literze Z. 

Żeby uzyskać taką konfiguracje na __Windows__ można było posłużyć się plikiem

	_subst_z.bat

Który używa polecenia SUBST. Może to jednak być w konflikcie
z przypisaniami sieciowymi, jeśli używają one także litery Z.

Obecnie projekt jest przygotowany do kompilacji za pomocą __cmake__.

04.11.2011-19.11.2026
[W.Borkowski](https://www.researchgate.net/profile/Wojciech-Borkowski?ev=hdr_xprf)


