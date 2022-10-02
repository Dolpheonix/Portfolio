#include "MathUtil.h"

int MathUtil::CircularMinus(int now, int length)
{
	int temp = now - 1;
	if (temp < 0)
	{
		temp = length - 1;
	}
	return temp;
}

int MathUtil::CircularPlus(int now, int length)
{
	int temp = now + 1;
	if (temp >= length)
	{
		temp = 0;
	}
	return temp;
}