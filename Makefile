all: parser

parser: src/grammars/CPP.cf clean-debug
	bnfc -m -o bin/parser/ src/grammars/CPP.cf
	$(MAKE) -C bin/parser/ && cp bin/parser/TestCPP bin/

debug: src/grammars/CPP.cf
	bnfc -m -o debug/parser/ src/grammars/CPP.cf
	sed -i -e 's/happy --ghc/happy -da --ghc/g' debug/parser/Makefile
	$(MAKE) -C debug/parser/ && cp debug/parser/TestCPP debug/

test: parser
	./run-test

clean: clean-bin clean-debug

clean-bin:
	rm -rf bin/

clean-debug:
	rm -rf debug/