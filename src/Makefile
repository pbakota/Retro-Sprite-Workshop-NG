EXE=rswng
IMGUI=imgui_modified

all: build

build:
	$(CXX) -DEXCLUDE_IMGUI_DEMO -std=c++17 -Wall -I. -I$(IMGUI) -I$(IMGUI)/filedialog_modified -I$(IMGUI)/stb_image $(shell pkg-config sdl2 --cflags) \
		$(IMGUI)/imgui*.cpp $(IMGUI)/filedialog_modified/imgui*.cpp main.cpp $(shell pkg-config sdl2 --libs) -o $(EXE)

