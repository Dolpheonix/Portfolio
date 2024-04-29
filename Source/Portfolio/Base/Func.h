#pragma once
#include "CoreMinimal.h"

// Json ���� ������ ���� �����ͳ�, sav ���Ϸ� ����Ǵ� enum class�� ���, �ش� ���ø��� ����ؼ� string <-> enum ��ȯ.
template <typename T>
T StringToEnum(const FString& str);

template <typename T>
FString EnumToString(const T& e);

namespace Helper
{
	// JSON �迭�� FVector�� ��ȯ�ϴ� �Լ�
	FORCEINLINE FVector JsonToVector(const TArray<TSharedPtr<FJsonValue>>& arrValue)
	{
		check(arrValue.Num() == 3);

		FVector vec;
		for (int i = 0; i < 3; ++i)
		{
			vec[i] = arrValue[i]->AsNumber();
		}

		return vec;
	}
}