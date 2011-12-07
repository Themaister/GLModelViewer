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
   LIBS := -lGL -lX11 -lXxf86vm
else ifeq ($(platform), osx)
   TARGET := modelviewer
   LIBS := -framework OpenGL
else
   TARGET := modelviewer.exe
   CC = gcc
   CXX = g++
   LDFLAGS += -L. -static-libgcc -static-libstdc++
   LIBS := -lopengl32
endif

CXXSOURCES := $(wildcard *.cpp)
CSOURCES := sgl/sgl.o
OBJ := $(CXXSOURCES:.cpp=.o) $(CSOURCES:.c=.o)
HEADERS = $(wildcard *.hpp)

%.o: %.cpp $(HEADERS)
	$(CXX) -c -o $@ $< $(CXXFLAGS) -std=gnu++0x -O3 -g -Wall -pedantic

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CFLAGS) -std=gnu99 -O3 -g -Wall -pedantic

$(TARGET): $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LIBS) $(LDFLAGS)

clean:
	rm -f $(TARGET)
	rm -f $(OBJ)

.PHONY: clean

