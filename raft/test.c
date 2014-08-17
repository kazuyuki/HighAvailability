#include <stdio.h>

int	a = 2;

int func()
{
	printf("[%d]\n", a);
	return 0;
}

int main(int argc, char** argv)
{
	func();

	return 0;
}
