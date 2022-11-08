# (c)ISS UW, @author Wojciech Borkowski
################################################################################

cmake_minimum_required(VERSION 3.6)
project(SymProcSoc)

#Te ścieżki trzeba ustawić ręcznie.
set( WBRTM     "/home/borkowsk/SCC/public/symShell2andRTM/RTM/" )
set( SYMSHELL  "/home/borkowsk/SCC/public/SymShellLight/" )

IF (WIN32)
  # set stuff for windows
  message( "MS Windows platform assumed.\n\tInside C/C++ code automatic _MSC_VER variable will be used!" )
  #set( WINDOWS_ONLY_SRC  "${WBRTM}/BASE/wb_assert.c" )
ELSE()
  # set stuff for other systems
  message( "Linux/unix platform assumed.")
  #set( X_ONLY_SRC  "" )
ENDIF()

include_directories( "${WBRTM}/"
		      "${WBRTM}INCLUDE/" 
		      "${SYMSHELL}"
		    )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )

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
   
#*//////////////////////////////////////////////////////////////////////////////
# SymProcSoc:
# PL: Symulator Procesów Społecznych/Sieciowych  
# EN: Social Processes Simulator Library
#    (c) ISS UW 2015-2016 Wojciech Borkowski, modyfikacja 2022.11
#*//////////////////////////////////////////////////////////////////////////////



