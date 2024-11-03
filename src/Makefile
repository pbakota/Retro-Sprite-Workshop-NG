IMGUI=imgui_modified

all: build

build:
	$(CXX) -g -std=c++17 -Wall -I. -I$(IMGUI) -I$(IMGUI)/filedialog_modified $(shell pkg-config sdl2 --cflags) $(IMGUI)/imgui*.cpp $(IMGUI)/filedialog_modified/imgui*.cpp main.cpp $(shell pkg-config sdl2 --libs) -o p4tools

