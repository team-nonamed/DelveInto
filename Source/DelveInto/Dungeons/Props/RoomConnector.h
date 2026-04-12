#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomConnector.generated.h"

UCLASS(Abstract, ClassGroup=(DungeonProp))
class DELVEINTO_API ARoomConnector : public AActor
{
	GENERATED_BODY()

public:
	ARoomConnector();

	// ==============================================================
	// [핵심] 룸에서 커넥터를 제어하기 위한 공통 인터페이스
	// ==============================================================
    
	/** 커넥터를 엽니다 (예: 문 열기, 장애물 제거) */
	virtual void OpenConnector(bool bPlaySound = true) {}

	/** 커넥터를 닫습니다 (예: 문 닫기, 보스방 봉쇄) */
	virtual void CloseConnector() {}
	
#if WITH_EDITORONLY_DATA
protected:
	/** * 기획자가 지정하는 에디터 미리보기용 대표 메쉬 
	 * (이 문이 다른 방의 에디터 뷰포트에 어떻게 보일지 결정합니다) 
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Editor Preview")
	TObjectPtr<UStaticMesh> EditorPreviewMesh;

public:
	/** 대표 메쉬를 반환합니다. */
	UStaticMesh* GetEditorPreviewMesh() const { return EditorPreviewMesh; }
#endif
};