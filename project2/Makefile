all:
	bnfc -cpp CPP.cf
	rm -f Test.C
	flex CPP.l
	yacc CPP.y
	clang++-9 -g -static *.cpp *.C *.c -O3 -o compiler

clean:
	rm -f compiler

distclean: clean 
	rm -f Absyn.* CPP.l CPP.y lex.yy.c y.tab.c Parser.H Printer.* Skeleton.*
