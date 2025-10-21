# --- Compiler setup ---
CXX = g++                                 # C++ compiler
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude  # C++17 + all warnings + include path

# --- Source and object files ---
# Automatically find all .cpp files in src/
SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)

# --- Output binary name ---
BIN = cards

# --- Default rule (build everything) ---
all: $(BIN)

# --- Link all object files into one executable ---
$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

# --- Compile each .cpp into .o ---
# $< means "the current .cpp file"
# $@ means "the target .o file"
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Remove build artifacts ---
clean:
	rm -f $(OBJ) $(BIN)

# --- Declare non-file targets ---
.PHONY: all clean
