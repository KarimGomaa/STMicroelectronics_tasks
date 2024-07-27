#include "function.h"

int first_global=5;
int second_global=10;

void fun_ChangeToGlobalValues(int *x, int *y,int *z){
	
	int local_variable=20;

	*x = first_global;
	*y = second_global;
	*z = local_variable;
}
