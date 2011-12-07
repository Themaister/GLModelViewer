ifeq ($(platform),)
platform = unix
ifeq ($(shell uname -a),)
   platform = win
else ifneq ($(findstring MINGW,$(shell uname -a)),)
   platform = win
else ifneq ($(findstring win,$(shell uname -a)),)
   platform = win
else ifneq ($(findstring Darwin,$(shell uname -a)),)
   platform = osx
endif
endif

ifeq ($(platform), unix)
   TARGET := modelviewer
   LIBS := $(shell pkg-config libglfw --libs)
   CXXFLAGS += $(shell pkg-config libglfw --cflags)
else ifeq ($(platform), osx)
   TARGET := modelviewer
   LIBS := $(shell pkg-config libglfw --libs) -framework OpenGL
   CXXFLAGS += $(shell pkg-config libglfw --cflags)
else
   TARGET := modelviewer.exe
   CXX = g++
   LDFLAGS += -L. -static-libgcc -static-libstdc++
   CXXFLAGS += -I.
   LIBS := -lglfw
endif

SOURCES := $(wildcard *.cpp)
OBJ := $(SOURCES:.cpp=.o)
HEADERS = $(wildcard *.hpp)

%.o: %.cpp $(HEADERS)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -std=gnu++0x -O3 -g -Wall -pedantic

$(TARGET): $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LIBS) $(LDFLAGS)

clean:
	rm -f $(TARGET)
	rm -f $(OBJ)

.PHONY: clean

