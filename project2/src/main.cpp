#include <stdio.h>
#include <iostream>
#include "Parser.H"
#include "Absyn.H"
#include "TypeCheckVisitor.h"

void typecheck(Program *pgm) {
	TypeCheckVisitor visitor;
	pgm->accept(&visitor);

	if (!visitor.anyErrors) {
		std::cout << "OK" << std::endl;
	}
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
	return 0;
}

