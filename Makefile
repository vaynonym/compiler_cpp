all: grammar

grammar: src/grammars/CPP.cf
	bnfc -m -o bin/parser/ src/grammars/CPP.cf
	$(MAKE) -C bin/parser/

test: grammar
	bin/parser/TestCPP tests/1-hello.cc && bin/parser/TestCPP tests/2-greet.cc && bin/parser/TestCPP tests/3-med.cc && bin/parser/TestCPP tests/4-grade.cc && bin/parser/TestCPP tests/5-palin.cc && bin/parser/TestCPP tests/6-grammar.cc

test-literals: grammar
	bin/parser/TestCPP tests/literals-int.txt && bin/parser/TestCPP tests/literals-float.txt && bin/parser/TestCPP tests/literals-char.txt && bin/parser/TestCPP tests/literals-string.txt && bin/parser/TestCPP tests/literals-bool.txt

clean:
	rm -rf bin/
