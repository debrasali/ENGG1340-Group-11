# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Project files
# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Project files
SRCS = memory_main.cpp memory_utils.cpp
HEADERS = memory_utils.h memory_structs.h
OBJS = $(SRCS:.cpp=.o)
TARGET = memory_game

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Compilation with header dependencies
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)

# Run the game
run: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean run
# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Compilation with header dependencies
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)

# Run the game
run: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean run 
