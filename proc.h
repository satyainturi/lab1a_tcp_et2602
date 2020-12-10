#ifndef _PROP_H_
#define _PROP_H_

struct command {
	char str[10];
	int val1, val2;
	double fval1, fval2;
};

void create_command(struct command cmd);
void get_command_string(struct command cmd, char msg);
double calculate(struct command cmd);
char compare_value(struct command cmd, char msg);

#endif