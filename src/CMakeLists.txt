# (c)ISS UW, @author Wojciech Borkowski
# @date 2022-11-16 (last update)
################################################################################

project(SymProcSoc)
cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 11) # See: https://matgomes.com/setting-cpp-standard-cmake/
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)  

#! \note Ścieżki do katalogów niezbędnych bibliotek trzeba ustawić ręcznie PONIŻEJ!

IF (WIN32)
  # set stuff for windows
  message( ${PROJECT_NAME} ": MS Windows platform assumed.\n\tInside C/C++ code automatic _MSC_VER variable will be used!" )
  # Typowo dla Windows
  set( WBRTM     "c:/users/borkowsk/SCC/public/symShell2andRTM/RTM/" )
  set( SYMSHELL  "c:/users/borkowsk/SCC/public/SymShellLight/" )
  #set( WINDOWS_ONLY_SRC  "${WBRTM}/BASE/wb_assert.c" )
ELSE()
  # set stuff for other systems
  message( ${PROJECT_NAME} ": Linux/unix platform assumed.")
  # Typowo dla linuxa
  set( WBRTM     "/home/borkowsk/SCC/public/symShell2andRTM/RTM/" )
  set( SYMSHELL  "/home/borkowsk/SCC/public/SymShellLight/" )
  #set( X_ONLY_SRC  "${WBRTM}/...." )
  #add_compile_options( -Wall )
  #set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )
ENDIF()

include_directories( "${WBRTM}/"
		      "${WBRTM}INCLUDE/" 
		      "${SYMSHELL}/"
		    )

set( SOURCE_FILES
	spsArcLink.cpp
	spsArcLink.h
	spsElement.cpp
	spsElement.h
	spsGenInfo.cpp
	spsGenInfo.h
	spsGenLink.cpp
	spsGenLink.h
	spsGenNode.cpp
	spsGenNode.h
	spsGenProc.cpp
	spsGenProc.h
	spsKonstrLst.cpp
	spsMatNodeFiles.cpp
	spsMatrixNode.cpp
	spsMatrixNode.h
	spsModel.h
	spsParaLink.cpp
	spsParaLink.h
	spsSwiatBaza.cpp
	spsSwiatInit.cpp
	spsSwiatKrokSymul.cpp
	spsSwiatProc.cpp
	spsSwiatWizual.cpp
	)

add_library(SymProcSoc STATIC ${SOURCE_FILES})
   
#********************************************************************/
# 	SymProcSoc library:					              */			
# 	PL: Symulator Procesów Społecznych/Sieciowych	              */		  
# 	EN: Social Processes Simulator Library	                    */
#         (c) ISS UW 2015-2016, modyfikacja 2022.11	              */		
#********************************************************************/
#*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
#*            W O J C I E C H   B O R K O W S K I                   */
#*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
#*    GITHUB: https://github.com/borkowsk                           */
#*                                                                  */
#*    Instytut Studiów Społecznych im. Profesora Roberta Zajonca    */
#*    Uniwersytetu Warszawskiego                                    */
#*    PL.WWW: http://iss.uw.edu.pl/                                 */
#*                                                                  */
#*    Robert Zajonc Institute for Social Studies                    */
#*    University of Warsaw                                          */
#*    EN.WWW: http://iss.uw.edu.pl/en/                              */
#*                                                                  */
#*                               (Don't change or remove this note) */
#********************************************************************/
