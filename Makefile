COMP = g++-11
CFLAGS = -std=c++20 -Wall -Wextra -pedantic -O2
LFLAGS = -I include -L lib -l raylib -l fmt -framework iokit -framework Cocoa -framework OpenGL
SRC = src/*.cpp

all: clean build run

main: $(SRC)
	$(COMP) $(CFLAGS) $^ -o build/$@ $(LFLAGS)

build:
	@echo "Compiling..."

run: main
	mv build/main build/game
	./build/game

clean:
	clear
	rm -rf build/game