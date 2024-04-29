#pragma once
#include "CoreMinimal.h"

// Json 파일 형식의 게임 데이터나, sav 파일로 저장되는 enum class의 경우, 해당 템플릿을 사용해서 string <-> enum 변환.
template <typename T>
T StringToEnum(const FString& str);

template <typename T>
FString EnumToString(const T& e);

namespace Helper
{
	// JSON 배열을 FVector로 변환하는 함수
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