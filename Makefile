all: grammar

grammar: src/grammars/CPP.cf
	bnfc -m -o bin/parser/ src/grammars/CPP.cf
	$(MAKE) -C bin/parser/

