#include "ShopNPC.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "PaperSprite.h" 
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

    // 2. 콜리전 박스 설정 (Root의 자식)
    InteractCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractCollision"));
    InteractCollision->SetupAttachment(RootComponent);
    InteractCollision->SetBoxExtent(FVector(40.f, 40.f, 90.f)); 
    InteractCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    // 3. [핵심 수정] 스프라이트 비주얼을 콜리전 박스(InteractCollision)의 자식으로 부착!
    NPCVisual = CreateDefaultSubobject<UFacingSpriteComponent>(TEXT("NPCVisual"));
    NPCVisual->SetupAttachment(InteractCollision);
}

void AShopNPC::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (InteractCollision && NPCVisual)
    {
        // 1. 박스의 "실제" 높이 계산 (BoxExtent * 스케일값 반영)
        // GetScaledBoxExtent()를 사용하면 스케일 툴(R)로 줄인 크기까지 정확히 반영됩니다.
        float ActualBoxHalfHeight = InteractCollision->GetScaledBoxExtent().Z;

        // 2. 박스를 바닥에 딱 맞게 올림
        InteractCollision->SetRelativeLocation(FVector(0.f, 0.f, ActualBoxHalfHeight));

        // 3. 스프라이트 크기 조절 (World Scale 강제 적용)
        if (IdleSprite)
        {
            NPCVisual->SetSprite(IdleSprite);
            
            // 스프라이트 원본 이미지의 쌩(Raw) 높이
            FBoxSphereBounds SpriteBounds = IdleSprite->GetRenderBounds();
            float RawHeight = SpriteBounds.BoxExtent.Z * 2.0f;
            
            // 박스의 전체 높이
            float TargetBoxHeight = ActualBoxHalfHeight * 2.0f;

            if (RawHeight > 0.1f)
            {
                // 최종 배율 계산
                float FinalScale = TargetBoxHeight / RawHeight;
                
                // [핵심 변경] 부모 눈치 보지 말고, 월드(절대) 스케일로 강제로 크기를 세팅합니다!
                // 박스가 어떻게 변형되었든 우리가 원하는 배율로 출력됩니다.
                NPCVisual->SetWorldScale3D(FVector(FinalScale));
            }
        }
        
        // 4. 위치 정렬 (스프라이트가 박스 중앙에 오도록)
        NPCVisual->SetRelativeLocation(FVector::ZeroVector);
    }
}

void AShopNPC::Interact_Implementation(AActor* Interactor)
{
    if (!ShopWidgetClass || !Interactor) return;

    // 중복 생성 차단
    if (ActiveShopWidget && ActiveShopWidget->IsInViewport())
    {
        UE_LOG(LogTemp, Warning, TEXT("이미 상점 UI가 활성화되어 있습니다."));
        return;
    }

    ADelveCharacter* PlayerChar = Cast<ADelveCharacter>(Interactor);
    if (!PlayerChar) return;

    APlayerController* PC = Cast<APlayerController>(PlayerChar->GetController());
    if (!PC) return;

    // 위젯 생성
    UShopWidget* ShopUI = CreateWidget<UShopWidget>(GetWorld(), ShopWidgetClass); 
    if (ShopUI)
    {
        ShopUI->AddToViewport();
        ShopUI->SetBuyer(PlayerChar);

        // 플레이어의 이동 및 시선 회전 입력을 차단
        PC->SetIgnoreMoveInput(true);
        PC->SetIgnoreLookInput(true); 

        // 생성한 위젯을 NPC가 기억하게 합니다.
        ActiveShopWidget = ShopUI;
        
        // 키보드 포커스(스페이스바 구매 등)를 잡기 위해 명시적으로 포커스 설정
        ShopUI->SetKeyboardFocus();

        // 마우스 커서 및 입력 모드 설정
        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(ShopUI->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }
}