#include<stdio.h>



int main()
{
	
	int i = 12;
        int j = 209;
	int *const p = &i;  // 指针常量  指向的地址不可改变
	const int *q = &j;  // 常量指针  指向的地址值不可改变
	const int *const h;  // 都不可变
//F	p = &j;
//T	*p = j;


//F	*q = i;
	q = &i;

	printf("*p----->%d--->p----->%p\n",*p,p);
	printf("*q----->%d--->q----->%p\n",*q,q);





	return 0;
}
