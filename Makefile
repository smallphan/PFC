PKG_CONFIG := pkg-config
PKG_CFLAGS := $(shell $(PKG_CONFIG) --cflags cairo)
PKG_LIBS := $(shell $(PKG_CONFIG) --libs cairo)

all: pfc-draw pfc

bin: 
	mkdir -p bin

pfc-draw: draw.cpp bin
	g++ $(PKG_CFLAGS) $< -o pfc-draw $(PKG_LIBS)
	mv pfc-draw bin

pfc: lexical.cpp syntax.cpp format.cpp main.cpp bin
	g++ lexical.cpp syntax.cpp format.cpp main.cpp -o pfc
	mv pfc bin
	
clean:
	rm -rf bin

.PHONY: all clean
