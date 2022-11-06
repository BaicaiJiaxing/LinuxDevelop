#include<stdio.h>
#include<stdlib.h>

int main()
{
	
	int *p = (int *)malloc(sizeof(int));
	int *q = (int *)calloc(10,sizeof(int));
	printf("p--->%ld\nq--->%ld\n",sizeof(*p),sizeof(*q));

	return 0;
}
