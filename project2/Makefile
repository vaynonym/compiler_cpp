CXXFLAGS := -g -D_GLIBCXX_DEBUG $(shell llvm-config-9 --cxxflags | sed 's/-fno-exceptions//')
LDFLAGS := $(shell llvm-config-9 --ldflags --libs)

SRC := $(wildcard src/*.cpp)
OBJS := $(SRC:%.cpp=bin/%.o)

GEN_SRC := gen/lex.yy.C gen/Printer.C gen/y.tab.C
GEN_OBJ := $(GEN_SRC:%.C=bin/%.o)

all: dirs gen bin/compiler

dirs:
	mkdir -p bin

bin/src/%.o: src/%.cpp
	@mkdir -p $(@D)
	clang++-9 $(CXXFLAGS) -c -Igen/ $< -o $@

bin/gen/%.o: gen/%.C
	@mkdir -p $(@D)
	clang++-9 $(CXXFLAGS) -c -Isrc/ -Igen/ $< -o $@

bin/compiler: $(GEN_OBJ) $(OBJS)
	clang++-9 $(CXXFLAGS) $(LDFLAGS) -o bin/compiler $^

gen: src/CPP.cf
	bnfc -cpp -o gen/ src/CPP.cf
	rm -f gen/Test.C
	cd gen/; flex CPP.l
	cd gen/; yacc CPP.y
	mv gen/lex.yy.c gen/lex.yy.tmp
	mv gen/y.tab.c gen/y.tab.tmp
	mv gen/lex.yy.tmp gen/lex.yy.C
	mv gen/y.tab.tmp gen/y.tab.C
	rm gen/Absyn.C gen/Absyn.H

domjudge: bin/compiler
	rm -rf domjudge/
	mkdir -p domjudge
	cp src/* domjudge/
	cp gen/* domjudge/
	rm domjudge/CPP.cf
	rm domjudge/*.l
	rm domjudge/*.y

clean:
	rm -rf bin/
	rm -rf gen/
	rm -rf domjudge/