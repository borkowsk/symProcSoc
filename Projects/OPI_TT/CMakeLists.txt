# CMAKE configuration for OPI_TT simulation project
######################################################################
# @Author: Wojciech Borkowski
#
cmake_minimum_required(VERSION 3.0)

project(OPI_TT)

set( WBRTM     "/home/borkowsk/SCC/private/Simulations202x/WBRTM_Linux/" )
set( SYMSHELL  "/home/borkowsk/SCC/private/Simulations202x/WBRTM_Linux/SYMSHELL/" )
set( SPS       "/home/borkowsk/SCC/public/SymProcSoc/src/" )

# Also: -DUSE_X11 -DUSE_WINDOWS -DUSE_CONSOLE_ONLY -DMULTITR -DDEF_MAXTHREADS=16
add_definitions( -DHAVE_X11 -DUSE_ZENITY )


# FOR BUILDING LIBRARIES WHERE THEY ARE: #add_subdirectory( ${} ${} )
add_subdirectory( ${WBRTM}   ${WBRTM}  )
#add_subdirectory( ${SYMSHELL} ${SYMSHELL}  ) #aktualnie robione przez cmake z WBRTM
add_subdirectory( ${SPS} ${SPS} )

# Define WITHTOOLS if you want build helpers applications!
#if(WITHTOOLS)
#   add_subdirectory( HELPERS/ )
#endif()

include_directories( "${SYMSHELL}" "${WBRTM}" "${WBRTM}INCLUDE/" )

# MAIN APPLICATION 
add_executable( opi_tt
        "src/opiGlobals.cpp"
        "src/opiKlasyAdmNaukowej.cpp"
        "src/opiKlasyGeneryczne.cpp"
        "src/opiKlasyJednNaukowych.cpp"
        "src/opiKlasyRynkowe.cpp"
        "src/opiKlasySpoleczne.cpp"
        "src/opiKlasyTransferu.cpp"
        "src/opiKlasyWersja.cpp"
        "src/opiKlasyWersja.hpp"
        "src/opiKlasyZespolRoboczy.cpp"
        "src/opiMain.cpp"
        "src/opiNazwaPliku.cpp"
        "src/opiStats.cpp"
#        "src/res
        )

target_compile_options( opi_tt  PRIVATE "${CMAKE_CXX_FLAGS}" 
                        "-std=c++11" "-pthread" "-fpermissive" )
## "-fpermissive" do usunięcia z czasem

target_link_libraries( opi_tt
#        "-L ${MYLIBS}"
        wbsymshell wbsyshX11 wbrtm X11 Xpm
        ${CMAKE_THREAD_LIBS_INIT}
        pthread
        rt
     )

