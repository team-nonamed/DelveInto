#include "MinimapRoomIcon.h"
#include "Components/Image.h"

void UMinimapRoomIcon::SetupIcon(UTexture2D* BorderTexture, UTexture2D* CenterTexture, float Size)
{
	// 테두리 설정
	if (BorderImage && BorderTexture)
	{
		BorderImage->SetBrushFromTexture(BorderTexture);
	}

	// 중앙 아이콘 설정
	if (CenterIconImage && CenterTexture)
	{
		CenterIconImage->SetBrushFromTexture(CenterTexture);
	}
    
	// 위젯 전체 크기 조절 로직은 블루프린트의 Size Box나 
	// 부모 캔버스 슬롯 설정을 통해 처리하는 것이 유연합니다.
}