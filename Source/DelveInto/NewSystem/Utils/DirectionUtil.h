#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "Types/Direction.h"

UCLASS()
class DELVEINTO_API USotaDirectionUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * 방향 비트마스크(ESotaDirection)를 읽기 쉬운 문자열로 변환합니다.
	 */
	UFUNCTION(BlueprintPure, Category = "Direction | Utility")
	static FString GetDirectionName(ESotaDirection InDir)
	{
		// 1. 완전 비어있는 경우 처리
		if (InDir == ESotaDirection::Empty) return TEXT("Empty");

		TArray<FString> ActiveNames;

		// 2. 각 비트가 켜져 있는지 확인 (조합 대응)
		if (EnumHasAnyFlags(InDir, ESotaDirection::Forward))  ActiveNames.Add(TEXT("Forward"));
		if (EnumHasAnyFlags(InDir, ESotaDirection::Backward)) ActiveNames.Add(TEXT("Backward"));
		if (EnumHasAnyFlags(InDir, ESotaDirection::Right))    ActiveNames.Add(TEXT("Right"));
		if (EnumHasAnyFlags(InDir, ESotaDirection::Left))     ActiveNames.Add(TEXT("Left"));
		if (EnumHasAnyFlags(InDir, ESotaDirection::Center))   ActiveNames.Add(TEXT("Center"));

		// 3. 결과 합치기 (예: "Forward | Right")
		return FString::Join(ActiveNames, TEXT(" "));
	}
};