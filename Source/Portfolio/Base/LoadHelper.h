#pragma once
#include "CoreMinimal.h"

namespace LoadHelper
{
	// �ε� �ÿ� ����� �ֿ� ��� ����
	const FString PATH_DATA			= "/Game/Data/";
	const FString PATH_MESH			= "/Game/Mesh/";
	const FString PATH_FX			= "/Game/FX/";
	const FString PATH_TEXTURE		= "/Game/Texture/";
	const FString PATH_THUMBNAIL	= "/Game/Texture/WidgetImage/Thumbnail/";
	const FString PATH_SOUND		= "/Game/Sound/Cue/";
	const FString PATH_BLUEPRINT	= "/Game/Blueprint/";

	// ������ ������ ����� �� �ִ� �ε� �Լ�
	template <typename T>
	FORCEINLINE T* C_LoadObjectFromPath(const FName& path)
	{
		ConstructorHelpers::FObjectFinder<T> finder(*path.ToString());
		if (!finder.Succeeded())
		{
			UE_LOG(LogTemp, Log, TEXT("No object in path %s"), *path.ToString());
			return nullptr;
		}

		return finder.Object;
	}

	// ������ �ܿ����� ����� �� �ִ� �ε� �Լ�
	template <typename T>
	FORCEINLINE T* LoadObjectFromPath(const FName& path)
	{
		if (path == NAME_None)
		{
			return nullptr;
		}

		T* obj = Cast<T>(StaticLoadObject(UObject::StaticClass(), NULL, *path.ToString()));
		if (!obj)
		{
			UE_LOG(LogTemp, Log, TEXT("No object in path %s"), *path.ToString());
		}

		return obj;
	}
}
