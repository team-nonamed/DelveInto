// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperFlipbook.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FlipbookUtil.generated.h"

/**
 * 프로젝트 전역에서 사용할 정적 헬퍼 함수 모음
 */
UCLASS()
class DELVEINTO_API UFlipbookUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * 플립북의 원래 길이와 재생 속도를 기반으로 실제 재생 시간을 계산합니다.
	 * @param Flipbook 대상 플립북
	 * @param PlayRate 재생 속도 (기본 1.0)
	 * @return 조정된 재생 시간 (초)
	 */
	UFUNCTION(BlueprintPure, Category = "Flipbook | Duration")
	static float GetAdjustedFlipbookDuration(const UPaperFlipbook* Flipbook, float PlayRate = 1.0f);
};
