CXX = g++
CXXFLAGS = -std=c++17 -Iinclude
LIBS = -lglfw -lGL -ldl

SOURCES = src/main.cpp src/glad.c src/player.cpp src/field.cpp src/stadium.cpp
TARGET = RonaldinhoSoccer

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) $(LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)
