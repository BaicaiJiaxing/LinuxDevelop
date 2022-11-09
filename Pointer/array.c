#include<stdio.h>
#include<stdlib.h>


int main()
{

	int i = 10;
	int *p = &i;
	int **q = &p;
	int ***h = &q;
	printf("%p---------->%d\n",p,*p);
	printf("%p---------->%p-------->%d\n",q,*q,**q);
	printf("%p---------->%p-------->%p-------->%d\n",h,*h,**h,***h);
	exit(0);
}
