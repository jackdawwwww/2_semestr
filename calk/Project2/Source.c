#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include<math.h>
#include<string.h>
typedef struct CHARSTK {
	char stack[1000];
	int count;
} charstack;
char top_op(const charstack*c) {
	return c->stack[c->count - 1];
}
void push_op(charstack*c, char el) {
	c->stack[c->count] = el;
	c->count++;
}
char pop_op(charstack*c) {
	c->count--;
	return c->stack[c->count];
}
typedef struct INTSTK {
	int stack[1000];
	int count;
} intstack;
int top_val(const intstack*c) {
	return c->stack[c->count - 1];
}
void push_val(intstack*c, int el) {
	c->stack[c->count] = el;
	c->count++;
}
int pop_val(intstack*c) {
	c->count--;
	return c->stack[c->count];
}
int ch_to_digit(char k) {
	char * alph = "0123456789";
	for (int i = 0; i < 11; i++) {
		if (alph[i] == k) return i;
	}
	return -1;
}
int ch_is_oper(char k) {
	char * alph = "()*/+-^$";
	for (int i = 0; i < 8; i++) {
		if (alph[i] == k)return 1;
	}
	return 0;
}
char priority(char op) {
	switch (op) {
	case '(':
	case ')':
		return 1;
	case '+':
	case '-':
		return 2;
	case '*':
	case '/':
		return 3;
	case '^':
		return 4;
	default: return 0;
	}
}
int calc_oper(intstack* values, char oper) {
	if (values->count < 2) {
		printf("syntax error");
		return 0;
	}
	if (oper == '/'&&top_val(values) == 0) {
		printf("division by zero");
		return 0;
	}
	int a = pop_val(values);
	int b = pop_val(values);
	switch (oper) {
	case '+':
		push_val(values, a + b);
		break;
	case '-':
		push_val(values, b - a);
		break;
	case '*':
		push_val(values, a*b);
		break;
	case '/':
		push_val(values, b / a);
		break;
	case '^':
		push_val(values, pow(b, a));
		break;
	}
	return 1;
}
int calc(int *result, int *fix) {
	int index = 0;
	intstack values;
	values.count = 0;
	charstack operations;
	operations.count = 0;
	int k;
	while ((k = getchar()) != EOF) {
		if ((char)k == '\n' || (char)k == '\r')	break;
		if ((char)k == '$') push_val(&values, *fix);
		int digit = ch_to_digit(k);
		if (digit >= 0 || (char)k == '$') {
			if (index == 1 && (char)k != '$') {
				push_val(&values, pop_val(&values) * 10 + digit);
			}
			else {
				if ((char)k != '$') {
					push_val(&values, digit);
				}
			}
			index = 1;
		}
		else if (ch_is_oper(k) == 1) {
			if (index == 1 && (char)k == '(') {
				printf("syntax error");
				return 0;
			}
			if (index == 2 && (char)k == ')' && top_op(&operations) == '(') {
				printf("syntax error");
				return 0;
			}
			index = 0;
			if ((char)k == '(') {
				index = 2;
				push_op(&operations, k);
			}
			else if ((char)k == ')') {
				while (operations.count > 0 && top_op(&operations) != '(') {
					if (calc_oper(&values, pop_op(&operations)) == 0) {
						return 0;
					}
				}
				if (operations.count > 0 && top_op(&operations) == '(') {
					pop_op(&operations);
				}
				else {
					printf("syntax error");
					return 0;
				}
			}
			else {
				if (priority(k) < 4) {
					while (operations.count > 0 && priority(top_op(&operations)) >= priority(k)) {
						if (calc_oper(&values, pop_op(&operations)) == 0) {
							return 0;
						}

					}
					push_op(&operations, k);
				}
				else {
					while (operations.count > 0 && priority(top_op(&operations)) > priority(k)) {
						if (calc_oper(&values, pop_op(&operations)) == 0) {
							return 0;
						}

					}
					push_op(&operations, k);
				}
			}
		}
		else {
			if (k == 'e') if (getchar() == 'x') 	if (getchar() == 'i') if (getchar() == 't') 		return 2;
			printf("syntax error");
			return 0;
		}
	}
	while (operations.count > 0) {
		if (calc_oper(&values, pop_op(&operations)) == 0) return 0;
	}
	if (values.count == 1) *result = pop_val(&values);
	else {
		printf("syntax error");
		return 0;
	}
	return 1;
}

int main(int argc, char *argv[]) {
	int fix = 0;
	int command;
	if (argc == 2) {
		if (strcmp(argv[1], "-i") == 0) command = 1;
		else command = 0;
	}
	else command = 0;
	int result;
	if (command == 1) {
		printf("\n in>");
		while (calc(&result, &fix) == 1) {
			printf("out< %d \n in>", result);
			fix = result;
		}
	}
	else if (calc(&result, &fix) == 1) printf("%d", result);
	return 0;

}
