TARGET := test

SOURCES := $(wildcard *.cpp)
OBJ := $(SOURCES:.cpp=.o)
HEADERS = $(wildcard *.hpp)

%.o: %.cpp $(HEADERS)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -std=gnu++0x -O3 -g -Wall -pedantic

$(TARGET): $(OBJ)
	$(CXX) -o $@ $(OBJ) -lglfw

clean:
	rm -f $(TARGET)
	rm -f $(OBJ)

.PHONY: clean

