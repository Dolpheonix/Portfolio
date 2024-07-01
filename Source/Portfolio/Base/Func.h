#pragma once
#include "CoreMinimal.h"

// Json 파싱 시 string을 enum 객체로 변환
template <typename T>
T StringToEnum(const FString& str);
// enum을 string으로 변환
template <typename T>
FString EnumToString(const T& e);

namespace Helper
{
	// Json 객체를 FVector로 변환
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