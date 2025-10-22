CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -g

SRC = src/Abilities.cpp src/Card.cpp src/ActionCard.cpp src/Champion.cpp src/main.cpp src/Market.cpp src/Deck.cpp src/Player.cpp
OBJ = $(SRC:.cpp=.o)
BIN = hero_realms

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all clean
