#include <stdio.h>
#include <iostream>
#include "Parser.H"
#include "Absyn.H"
#include "ContextBuilder.h"
#include "TypeCheckVisitor.h"
#include "CodeGenVisitor.h"

void typecheck(Program *pgm) {
	ContextBuilder contextBuilder;
	pgm->accept(&contextBuilder);

	if (contextBuilder.anyErrors) {
		exit(1);
	}

	Context *globalContext = contextBuilder.getGlobalContext();

	TypeCheckVisitor typeChecker(globalContext);
	pgm->accept(&typeChecker);

	if (typeChecker.anyErrors) {
		exit(1);
	}

	CodeGenVisitor codeGen(globalContext);
	pgm->accept(&codeGen);

	codeGen.printIR();

	exit(0);
}

void process(FILE* input) {
	Program *parse_tree = pProgram(input);
	if (parse_tree) {
		typecheck(parse_tree);
	} else {
		printf("SYNTAX ERROR\n");
		exit(1);
	}
}

int main(int argc, char ** argv) {
	FILE *input;
	char *filename = NULL;

	if (argc > 1)
		filename = argv[1];
	else
		input = stdin;

	if (filename) {
		input = fopen(filename, "r");
		if (!input) {
			printf("Cannot open the input file");
			exit(1);
		}
	} else
		input = stdin;
	process(input);
}

