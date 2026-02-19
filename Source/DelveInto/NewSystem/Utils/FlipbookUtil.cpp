// Fill out your copyright notice in the Description page of Project Settings.


#include "FlipbookUtil.h"

#include "PaperFlipbook.h"

float UFlipbookUtil::GetAdjustedFlipbookDuration(const UPaperFlipbook* Flipbook, float PlayRate)
{
	if (!Flipbook)
	{
		return 0.0f;
	}

	float SafeRate = FMath::Max(0.01f, PlayRate); 
    
	return Flipbook->GetTotalDuration() / SafeRate;
}
