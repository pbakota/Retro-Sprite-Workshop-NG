EXE=rswng
IMGUI=imgui_modified

all: build

build:
	$(CXX) -DEXCLUDE_IMGUI_DEMO -std=c++17 -Wall -I. -I$(IMGUI) -I$(IMGUI)/filedialog_modified -I$(IMGUI)/stb_image $(shell pkg-config sdl2 --cflags) \
		$(IMGUI)/imgui*.cpp $(IMGUI)/filedialog_modified/imgui*.cpp main.cpp $(shell pkg-config sdl2 --libs) -o $(EXE)

build-win32:
	$(CXX) -DEXCLUDE_IMGUI_DEMO -std=c++17 -Wall -I. -I$(IMGUI) -I$(IMGUI)/filedialog_modified -I$(IMGUI)/stb_image -Dmain=SDL_main -I$(HOME)/SDK/x86_64-w64-mingw32/include -I$(HOME)/SDK/x86_64-w64-mingw32/include/SDL2 \
		$(IMGUI)/imgui*.cpp $(IMGUI)/filedialog_modified/imgui*.cpp main.cpp -L/home/sorel/SDK/x86_64-w64-mingw32/lib -lmingw32 -mwindows -lSDL2main -lSDL2 -static-libgcc -static-libstdc++ -o $(EXE).exe

win32-distro:
	zip -9 $(EXE).zip $(EXE).exe SDL2.dll

