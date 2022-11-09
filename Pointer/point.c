#include<stdio.h>


int main()
{

	int i = 10;
	char ch = 'a';
	int *p = &i;
	char *q = &ch;
	char ch2 = 'b';
	char *h = &ch2;
	int b = 2;
	int *j = &b;
	printf("p ---->int ---->%p\n",p);
	printf("j ---->int ---->%p\n",j);
	printf("h ---->char ---->%p\n",h);
	printf("*q ---->char ---->%p\n",q);

	return 0;
}
