# Variables
CC = g++
CFLAGS = -std=c++17 -Wall -g
LDFLAGS =
SOURCES = main.cpp drone.cpp flotte.cpp SerialManager.cpp
HEADERS = drone.hpp flotte.hpp SerialManager.hpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = main

# Règle principale
all: $(TARGET)

# Générer l'exécutable
$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJECTS)

# Générer les fichiers .o
%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyer les fichiers générés
clean:
	rm -f $(OBJECTS) $(TARGET)

# Réinitialiser tout
rebuild: clean all

# Test automatique
run: $(TARGET)
	./$(TARGET)
