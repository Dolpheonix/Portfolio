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

float MathUtil::AngleBetweenTwoVectors_2D(FVector& A, FVector& B)
{
	float cosine = FVector::DotProduct(A, B);
	FVector cross = FVector::CrossProduct(A, B);
	float sine = cross.Z;

	float tangent = sine / cosine;

	float angle = FMath::Atan2(sine, cosine);

	return angle;
}