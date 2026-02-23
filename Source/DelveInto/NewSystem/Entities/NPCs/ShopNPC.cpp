#include "ShopNPC.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "PaperSprite.h" // [수정] PaperSprite 헤더
#include "Blueprint/UserWidget.h"
#include "NewSystem/Billboards/FacingSpriteComponent.h"
#include "NewSystem/Entities/Characters/DelveCharacter.h"
#include "NewSystem/Widgets/Shops/ShopWidget.h"

AShopNPC::AShopNPC()
{
    PrimaryActorTick.bCanEverTick = false;

    // 1. Root 설정 (바닥 기준점)
    DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
    RootComponent = DefaultSceneRoot;

    // 2. 콜리전 박스 설정
    InteractCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractCollision"));
    InteractCollision->SetupAttachment(RootComponent);
    InteractCollision->SetBoxExtent(FVector(40.f, 40.f, 90.f)); 
    InteractCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    // 3. [수정] FacingSprite 컴포넌트 생성 및 부착
    NPCVisual = CreateDefaultSubobject<UFacingSpriteComponent>(TEXT("NPCVisual"));
    NPCVisual->SetupAttachment(RootComponent);
}

void AShopNPC::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (InteractCollision && NPCVisual)
    {
        // 1. 박스 바닥 고정 (높이 절반만큼 위로)
        float HalfHeight = InteractCollision->GetUnscaledBoxExtent().Z;
        InteractCollision->SetRelativeLocation(FVector(0.f, 0.f, HalfHeight));

        // 2. [수정] 스프라이트 비주얼 자동 크기 조절
        if (IdleSprite)
        {
            NPCVisual->SetSprite(IdleSprite);
            
            // 스프라이트 원본 데이터의 크기 추출
            FBoxSphereBounds SpriteBounds = IdleSprite->GetRenderBounds();
            float RawHeight = SpriteBounds.BoxExtent.Z * 2.0f;
            float BoxHeight = HalfHeight * 2.0f;

            if (RawHeight > 0.1f)
            {
                // 박스 높이에 맞게 스프라이트 배율 계산
                float FinalScale = BoxHeight / RawHeight;
                NPCVisual->SetRelativeScale3D(FVector(FinalScale));
            }
        }
        
        NPCVisual->SetRelativeLocation(FVector::ZeroVector);
    }
}

void AShopNPC::Interact_Implementation(AActor* Interactor)
{
    if (!ShopWidgetClass || !Interactor) return;

    // [핵심] 이미 상점이 열려 있고 화면에 떠 있다면 중복 생성을 차단합니다.
    if (ActiveShopWidget && ActiveShopWidget->IsInViewport())
    {
        UE_LOG(LogTemp, Warning, TEXT("이미 상점 UI가 활성화되어 있습니다."));
        return;
    }

    ADelveCharacter* PlayerChar = Cast<ADelveCharacter>(Interactor); //
    if (!PlayerChar) return;

    APlayerController* PC = Cast<APlayerController>(PlayerChar->GetController());
    if (!PC) return;

    // 위젯 생성
    UShopWidget* ShopUI = CreateWidget<UShopWidget>(GetWorld(), ShopWidgetClass); //
    if (ShopUI)
    {
        ShopUI->AddToViewport();
        ShopUI->SetBuyer(PlayerChar);

        // [추가] 플레이어의 이동 및 시선 회전 입력을 차단합니다.
        PC->SetIgnoreMoveInput(true);
        PC->SetIgnoreLookInput(true); // 마우스로 UI를 조작해야 하므로 시선 회전도 막는 것이 좋습니다.

        // 마우스 커서 및 입력 모드 설정
        FInputModeGameAndUI InputMode;
        // 생성한 위젯을 NPC가 기억하게 합니다.
        ActiveShopWidget = ShopUI;
        
        InputMode.SetWidgetToFocus(ShopUI->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }
}