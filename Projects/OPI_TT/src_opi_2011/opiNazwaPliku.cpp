#include <windows.h>
#include <cstdio>
#include <cstdlib>
using namespace std;

bool podaj_nazwe_pliku(char* Bufor,unsigned size,const char* Monit)
// Funkcja wczytwywania nazwy pliku - mo¿e byæ trywialna, czyli konsolowa, ale
// mog³aby byæ te¿ bardziej zaawansowana i przez to zale¿na od systemu - np.
// jako dialog.
{
	OPENFILENAME ofn; // common dialog box structure
	HWND hwnd; // owner window
	HANDLE hf; // file handle

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = 0; //hwnd - nie ma bo to pewnie konsolowe
	ofn.lpstrFile = Bufor;
	//
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not
	// use the contents of szFile to initialize itself.
	//
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = size;

	ofn.lpstrFilter = "All\0*.*\0DAT\0*.dat\0TXT\0*.txt\0OPI\0*.opi\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle= Monit;

	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box.

	if (GetOpenFileName(&ofn) != TRUE)
	{
		printf("\n%s: ", Monit);
		char format2[32];
		sprintf(format2, "%c%u%c%c", '%', size, 's', '\0');
		unsigned ret = scanf(format2, Bufor);
		// unsigned ret=scanf_s("%s",size,Bufor); //???
		if(ret==1) //Jedno pole zeskanowane
			return true;
			else
			return false;
	}
	else return true;
}


/********************************************************************/
/*			          SPS/SPS  version 2011                         */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://wwww.iss.uw.edu.pl/borkowski/                */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/

