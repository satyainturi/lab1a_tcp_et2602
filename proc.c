#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "proc.h"

#define RANDOM_MAX_VALUE 100000

char* arith[] = {"add", "div", "mul", "fsub", "fadd", "fdiv", "fmul", "fsub"};


int random_int(void) {
  return rand() % RANDOM_MAX_VALUE;
};

double random_double(void) {
	return (double)(random_int() / 3.0);
};

void create_command(struct command* cmd) {
	int count = sizeof(arith) / (sizeof(char*)); 
	int randId = rand() % count;
	strcpy(cmd->str, arith[randId]);
	cmd->val1 = random_int();
	cmd->val2 = random_int();
	cmd->fval1 = random_double();
	cmd->fval2 = random_double();
}

void get_command_string(struct command* cmd, char* text) {
	if (strlen(cmd->str) == 3) {
		sprintf(text, "%s %d %d\n", cmd->str, cmd->val1, cmd->val2);
	}
	else {
		sprintf(text, "%s %.8g %.8g\n", cmd->str, cmd->fval1, cmd->fval2);
	}
}

double calculate(struct command* cmd) {
	if (strcmp(cmd->str, "add") == 0) {
		return (double)(cmd->val1 + cmd->val2);
	}
	if (strcmp(cmd->str, "sub") == 0) {
		return (double)(cmd->val1 - cmd->val2);
	}
	if (strcmp(cmd->str, "mul") == 0) {
		return (double)(cmd->val1 * cmd->val2);
	}
	if (strcmp(cmd->str, "div") == 0) {
		return (double)cmd->val1 / (double)cmd->val2;
	}
	if (strcmp(cmd->str, "fadd") == 0) {
		return cmd->fval1 + cmd->fval2;
	}
	if (strcmp(cmd->str, "fsub") == 0) {
		return cmd->fval1 - cmd->fval2;
	}
	if (strcmp(cmd->str, "fmul") == 0) {
		return cmd->fval1 * cmd->fval2;
	}
	if (strcmp(cmd->str, "fdiv") == 0) {
		return cmd->fval1 / cmd->fval2;
	}
	return 0;
}


