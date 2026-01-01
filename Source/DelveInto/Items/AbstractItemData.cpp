// Fill out your copyright notice in the Description page of Project Settings.


#include "AbstractItemData.h"

FText UAbstractItemData::GetCurrentDescription()
{
	return Description;
}

FText UAbstractItemData::GetCurrentName()
{
	return DisplayName;
}

TObjectPtr<const UTexture2D> UAbstractItemData::GetCurrentIcon()
{
	return Icon;
}

FPrimaryAssetId UAbstractItemData::GetItemId() const
{
	return GetPrimaryAssetId();
}
