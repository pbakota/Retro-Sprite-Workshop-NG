EXE=rswng
IMGUI=imgui_modified
SDL2_WIN32?=$(HOME)/SDK/SDL2-2.30.9/x86_64-w64-mingw32

all: build

build:
	$(CXX) -DEXCLUDE_IMGUI_DEMO -DUSE_CLIPBOARD_FOR_COPY_AND_PASTE -std=c++17 -Wall -I. -I$(IMGUI) -I$(IMGUI)/filedialog_modified -I$(IMGUI)/stb_image $(shell pkg-config sdl2 --cflags) \
		$(IMGUI)/imgui*.cpp $(IMGUI)/filedialog_modified/imgui*.cpp main.cpp $(shell pkg-config sdl2 --libs) -o $(EXE)

build-win32:
	$(CXX) -DEXCLUDE_IMGUI_DEMO -DUSE_CLIPBOARD_FOR_COPY_AND_PASTE -std=c++17 -Wall -I. -I$(IMGUI) -I$(IMGUI)/filedialog_modified -I$(IMGUI)/stb_image -Dmain=SDL_main -I$(SDL2_WIN32)/include -I$(SDL2_WIN32)/include/SDL2 \
		$(IMGUI)/imgui*.cpp $(IMGUI)/filedialog_modified/imgui*.cpp main.cpp -L$(SDL2_WIN32)/lib -lmingw32 -mwindows -lSDL2main -lSDL2 -static-libgcc -static-libstdc++ -o $(EXE).exe

win32-distro:
	zip -9 $(EXE).zip $(EXE).exe SDL2.dll

