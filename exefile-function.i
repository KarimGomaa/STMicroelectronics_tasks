# 0 "function.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "function.c"
# 1 "function.h" 1
void fun_ChangeToGlobalValues(int *x, int *y,int *z);
# 2 "function.c" 2

int first_global=5;
int second_global=10;

void fun_ChangeToGlobalValues(int *x, int *y,int *z){

 int local_variable=20;

 *x = first_global;
 *y = second_global;
 *z = local_variable;
}
