#include<stdio.h>
#include<stdlib.h>


int main()
{

	u_int32_t i = 0x11223344;
	printf("%b\n",i&0xFFFFFFFF);
	printf("%x\n",(i>>16)+(i&(0xFFFFFFFF)>>16));
	exit(0);
}

