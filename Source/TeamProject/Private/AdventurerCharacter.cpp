// Fill out your copyright notice in the Description page of Project Settings.


#include "AdventurerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

//アイテム
#include "PickUpItem.h"

//アニメーション
#include"Animation/AnimSequence.h"

using enum EAdvennturerState;

// Sets default values
AAdventurerCharacter::AAdventurerCharacter()
{
	//アニメーション
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//キャラクターの移動管理用
	MoveSetting = GetCharacterMovement();

	//キャラクターがカメラの向きを向かないように
	bUseControllerRotationYaw = false;
	MoveSetting->bOrientRotationToMovement = true;

	//カメラアームコンポーネント追加
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	//カメラアーム設定
	CameraBoom->TargetArmLength = DefaultArmLength;
	CameraBoom->bUsePawnControlRotation = true;

	//カメラコンポーネント追加
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//カメラ設定
	FollowCamera->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void AAdventurerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//初期位置を保存
	StartTransform = GetActorTransform();

	//プレイヤー・カメラのステータス設定
	CameraBoom->TargetArmLength = DefaultArmLength;
	MoveSetting->AirControl = AirControllPower;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Called every frame
void AAdventurerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//アニメーション
	UAnimSequence* Next = IdleAnim;
	if (MoveSetting->IsFalling())
	{
		Next = JumpAnim;
	}
	else if (GetVelocity().Size2D() > 3.f)
	{
		Next = WalkAnim;
	}
	//変わった時だけ再生しなおす(舞フレーム呼ぶと最初で止まる)
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
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAdventurerCharacter::Move);
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAdventurerCharacter::Look);
		EIC->BindAction(LookAction, ETriggerEvent::Started, this, &AAdventurerCharacter::PlaceTrap);
		EIC->BindAction(LookAction, ETriggerEvent::Started, this, &AAdventurerCharacter::TrapRotate);
		EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//アイテム
		EIC->BindAction(UseAction, ETriggerEvent::Started, this, &AAdventurerCharacter::UseHeldItem);
	}
}

//移動関数
void AAdventurerCharacter::Move(const FInputActionValue& Value)
{
	if (AdvennturerState == Inoperable)
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
	AddControllerPitchInput(-Input.Y);
}

void AAdventurerCharacter::PlaceTrap()
{
	if (AdvennturerState != TrapMode)
		return;
}

void AAdventurerCharacter::TrapRotate()
{
	if (AdvennturerState != TrapMode)
		return;
}

void AAdventurerCharacter::TrapSlotChange()
{
	if (AdvennturerState != TrapMode)
		return;
}

//アイテム使用関数
void AAdventurerCharacter::UseHeldItem()
{
	if (HeldItem&&!bBuffActive)
	{
		HeldItem->Use();
		HeldItem = nullptr;//一回きり
	}
}

//リスポーン
void AAdventurerCharacter::RespawnAtStart()
{
	MoveSetting->StopMovementImmediately();
	TeleportTo(StartTransform.GetLocation(), StartTransform.Rotator());
}
