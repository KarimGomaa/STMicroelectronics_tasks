#include <stdio.h>
#include "function.h"

int global_main1=1;
int global_main2=2;

int main() {
	int local_main1 = 3;
	printf("Before Function \n ");
	printf("global_main1=%d , global_main2=%d , local_main1= %d \n",global_main1,global_main2,local_main1);
	fun_ChangeToGlobalValues(&global_main1,&global_main2,&local_main1);
	printf("After Function \n ");
	printf("global_main1=%d , global_main2=%d , local_main1= %d \n",global_main1,global_main2,local_main1);	
	return 0;
}

