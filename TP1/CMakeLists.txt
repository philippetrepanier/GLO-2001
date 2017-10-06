cmake_minimum_required(VERSION 3.8)
project(TP1)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        RTC-8aout-1dec/__licence.txt
        RTC-8aout-1dec/__lisez-moi.txt
        RTC-8aout-1dec/agency.txt
        RTC-8aout-1dec/calendar_dates.txt
        RTC-8aout-1dec/feed_info.txt
        RTC-8aout-1dec/Horaire_Boucle_Partage.txt
        RTC-8aout-1dec/routes.txt
        RTC-8aout-1dec/shapes.txt
        RTC-8aout-1dec/stop_times.txt
        RTC-8aout-1dec/stops.txt
        RTC-8aout-1dec/transfers.txt
        RTC-8aout-1dec/trips.txt
        "Sources fournies/arret.cpp"
        "Sources fournies/arret.h"
        "Sources fournies/auxiliaires.cpp"
        "Sources fournies/auxiliaires.h"
        "Sources fournies/CMakeLists.txt"
        "Sources fournies/coordonnees.cpp"
        "Sources fournies/coordonnees.h"
        "Sources fournies/DonneesGTFS.cpp"
        "Sources fournies/DonneesGTFS.h"
        "Sources fournies/ligne.cpp"
        "Sources fournies/ligne.h"
        "Sources fournies/main.cpp"
        "Sources fournies/station.cpp"
        "Sources fournies/station.h"
        "Sources fournies/voyage.cpp"
        "Sources fournies/voyage.h")

add_executable(TP1 ${SOURCE_FILES})