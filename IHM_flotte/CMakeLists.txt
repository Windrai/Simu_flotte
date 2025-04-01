cmake_minimum_required(VERSION 3.10)

# Définir explicitement le répertoire Qt6 si nécessaire
set(Qt6_DIR "/home/domer/Qt/6.5.3/gcc_64/lib/cmake/Qt6")
# Déclarer le projet
project(ProjetDrones VERSION 0.1 LANGUAGES CXX)

# Trouver les packages nécessaires
find_package(Qt6 REQUIRED COMPONENTS Core Quick QuickControls2)
find_package(Threads REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS SerialPort)

# Activer les options automatiques de gestion des ressources et de QML
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug) # pour permettre gdb

# Ajouter l'exécutable
set(SOURCES
    src/main.cpp
    src/drone.cpp
    src/flotte.cpp
    src/SerialManager.cpp
)

# Ajout des fichiers d'en-tête
set(HEADERS
    include/constants.hpp
    include/drone.hpp
    include/fleet.hpp
    include/SerialManager.hpp
)


# Ajouter les fichiers QML au projet
qt_add_resources(RESOURCES qml/qml.qrc)

# Force la génération moc
qt6_wrap_cpp(MOC_SOURCES include/drone.hpp include/flotte.hpp include/SerialManager.hpp)

# Incluez les fichiers MOC dans l'exécutable
add_executable(ProjetDrones ${SOURCES} ${MOC_SOURCES} ${RESOURCES})

# Inclure les fichiers d'en-tête
target_include_directories(ProjetDrones PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Lier les bibliothèques nécessaires
target_link_libraries(ProjetDrones PRIVATE Qt6::Core Qt6::Quick Qt6::QuickControls2 Threads::Threads Qt6::SerialPort)

# Message pour confirmer les chemins
message("Headers are included from: ${CMAKE_CURRENT_SOURCE_DIR}/include")
