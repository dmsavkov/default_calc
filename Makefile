CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Idefault_calc/include

TARGET := default_calc.exe
TEST_TARGET := test.exe
SOURCES := default_calc/src/main.cpp default_calc/src/calc.cpp
OBJECTS := $(SOURCES:.cpp=.o)

.PHONY: all clean run test

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

default_calc/src/%.o: default_calc/src/%.cpp default_calc/include/calc.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

$(TEST_TARGET): default_calc/testing.cpp default_calc/src/calc.cpp default_calc/include/calc.hpp
	$(CXX) $(CXXFLAGS) default_calc/testing.cpp default_calc/src/calc.cpp -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f default_calc/src/*.o $(TARGET) $(TEST_TARGET)
