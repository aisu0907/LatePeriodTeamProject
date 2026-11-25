// Fill out your copyright notice in the Description page of Project Settings.
//AdventurerCharacter.cpp

#include "AdventurerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TimerManager.h"

// クラス
#include "PickUpItem.h" // アイテム
#include "TrapPlacementComponent.h"
#include "GamePlayModeBase.h"
#include "Goal.h"
#include "TrapBase.h"

// アニメーション
#include"Animation/AnimSequence.h"

using enum EAdventurerState;

// Sets default values
AAdventurerCharacter::AAdventurerCharacter()
{
	// アニメーション
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// キャラクターの移動管理用
	MoveSetting = GetCharacterMovement();

	// キャラクターがカメラの向きを向かないように
	bUseControllerRotationYaw = true; 
	MoveSetting->bOrientRotationToMovement = false;

	//// カメラアームコンポーネント追加
	//CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(RootComponent);
	//// カメラアーム設定
	//CameraBoom->TargetArmLength = DefaultArmLength;
	//CameraBoom->bUsePawnControlRotation = true;

	// カメラコンポーネント追加
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(GetCapsuleComponent());
	FollowCamera->SetRelativeLocation(FVector(0.f, 0.f, 64.f)); //目の高さ
	// カメラ設定
	FollowCamera->bUsePawnControlRotation = true;

	// 自分の全身メッシュはカメラに映さない
	GetMesh()->SetOwnerNoSee(true);

	// TrapPlacementコンポーネント
	TrapPlacement = CreateDefaultSubobject<UTrapPlacementComponent>(TEXT("TrapPlacementComp"));
}

// Called when the game starts or when spawned
void AAdventurerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// GamePlayMode取得s
	GamePlayMode = GetWorld()->GetAuthGameMode<AGamePlayModeBase>();

	// Overlapを登録
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(
		this, &AAdventurerCharacter::OnOverlapBegin);

	// 初期位置を保存
	StartTransform = GetActorTransform();

	// プレイヤー・カメラのステータス設定
	//CameraBoom->TargetArmLength = DefaultArmLength;
	MoveSetting->AirControl = AirControllPower;
}

// Called every frame
void AAdventurerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 死亡中はアニメを切り替えない
	if (AdventurerState == Inoperable)
		return;


	// アニメーション
	UAnimSequence* Next = IdleAnim;
	if (MoveSetting->IsFalling())
	{
		Next = JumpAnim;
	}
	else if (GetVelocity().Size2D() > 3.f)
	{
		Next = WalkAnim;
	}

	// 変わった時だけ再生しなおす(舞フレーム呼ぶと最初で止まる)
	if (Next && Next != CurrentAnim)
	{
		CurrentAnim = Next;
		GetMesh()->PlayAnimation(Next, true);
	}

}

// Called to bind functionality to input
void AAdventurerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (auto* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 移動
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAdventurerCharacter::Move);
		// ジャンプ
		EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		// カメラ操作
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAdventurerCharacter::Look);
		// トラップ設置
		EIC->BindAction(PlaceTrapAction, ETriggerEvent::Started, this, &AAdventurerCharacter::PlaceTrap);
		// トラップ回転
		EIC->BindAction(TrapRotateAction, ETriggerEvent::Started, this, &AAdventurerCharacter::TrapRotate);
		// トラップスロットチェンジ
		EIC->BindAction(TrapSlotChangeAction, ETriggerEvent::Triggered, this, &AAdventurerCharacter::TrapSlotChange);
		// TrapPhaseスキップ用
		EIC->BindAction(PhaseSkipAction, ETriggerEvent::Started, this, &AAdventurerCharacter::PhaseSkip);

		// アイテム
		EIC->BindAction(UseAction, ETriggerEvent::Started, this, &AAdventurerCharacter::UseHeldItem);
	}
}

void AAdventurerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		// ローカルプレイヤーのインデックス(0, 1, ...)を取得
		if (const ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			PlayerIndex = LP->GetLocalPlayerIndex();

			if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}

		}
	}

	UE_LOG(LogTemp, Warning, TEXT("PlayerIndex = %d"), PlayerIndex);
}

void AAdventurerCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (AdventurerState != PlayMode) { return; }

	//ゴールに触れている時
	if (AGoal* Goal = Cast<AGoal>(OtherActor))
	{
		if (Goal->IgnorePlayerIndex != PlayerIndex)
		{;
			GamePlayMode->RoundWin(this);
		}
	}


}

void AAdventurerCharacter::Move(const FInputActionValue& Value)
{
	if (AdventurerState == Inoperable)
		return;

	const FVector2D Input = Value.Get<FVector2D>();
	const FRotator YawRot(0.f, GetControlRotation().Yaw, 0.f);

	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	AddMovementInput(Forward, Input.Y);
	AddMovementInput(Right, Input.X);
}

void AAdventurerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();
	AddControllerYawInput(Input.X);
	AddControllerPitchInput(Input.Y);
}

void AAdventurerCharacter::PlaceTrap()
{
	if (AdventurerState != TrapMode) { return; }

	// 設置数の上限チェック
	if (TrapPlaceCount >= TrapPlaceMax)
	{
		UE_LOG(LogTemp, Warning, TEXT("Max Trap"));
		return;
	}

	// 設置に成功したときだけカウントを増やす
	if (TrapPlacement && TrapPlacement->PlaceTrap())
	{
		++TrapPlaceCount;
	}

}

void AAdventurerCharacter::TrapRotate()
{
	if (AdventurerState != TrapMode)
		return;
}

void AAdventurerCharacter::TrapSlotChange(const FInputActionValue& Value)
{
	if (AdventurerState != TrapMode)
		return;

	// 数値を取得
	const float Axis = Value.Get<float>();

	// +-判定
	const int32 Dir = (Axis > 0.f) ? 1 : -1;

	// TrapPlacementがあった場合
	if (TrapPlacement)
	{
		TrapPlacement->ChangeSelectTrapIndex(Dir);
	}
}

void AAdventurerCharacter::UseHeldItem()
{
	if (HeldItem&&!bBuffActive)
	{
		HeldItem->Use();
		HeldItem = nullptr;//一回きり
	}
}

void AAdventurerCharacter::Cancel()
{

}

void AAdventurerCharacter::PhaseSkip()
{
	if (GamePlayMode)
	{

		GamePlayMode->TrapPhaseEnd();
	}
}
//プレイヤーリセット関数
void AAdventurerCharacter::PlayerReset(EAdventurerState NewState)
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerReset"));

	ChangeState(NewState); // 状態変更
	// トラップフェーズの時だけ
	if(GamePlayMode->GetGamePhase() == EGamePhase::Trap)
		TrapPlaceCount = 0; // トラップ設置数リセット
	SetActorTransform(StartTransform); // 位置を初期位置にリセット
	MoveSetting->StopMovementImmediately(); // 速度・加速度を0に
}

void AAdventurerCharacter::ChangeState(EAdventurerState NewState)
{
	//同じじゃない場合
	if (AdventurerState == NewState)
		return;

	//状態更新
	AdventurerState = NewState;
}

//リスポーン
void AAdventurerCharacter::RespawnAtStart()
{
	MoveSetting->StopMovementImmediately();
	TeleportTo(StartTransform.GetLocation(), StartTransform.Rotator());
}