# (c)ISS UW, @author Wojciech Borkowski
################################################################################

cmake_minimum_required(VERSION 3.6)
project(SymProcSoc)

#Te ścieżki trzeba ustawić ręcznie.
set( WBRTM     "/home/borkowsk/SCC/public/symShell2andRTM/RTM/" )
set( SYMSHELL  "/home/borkowsk/SCC/public/SymShellLight/" )

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
#    (c) ISS UW 2015-2016 Wojciech Borkowski, modyfikacja 2022.10
#*//////////////////////////////////////////////////////////////////////////////



