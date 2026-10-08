// Fill out your copyright notice in the Description page of Project Settings.


#include "AdventurerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
AAdventurerCharacter::AAdventurerCharacter()
{
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
	
	CameraBoom->TargetArmLength = DefaultArmLength;

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

}

// Called to bind functionality to input
void AAdventurerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (auto* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAdventurerCharacter::Move);
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAdventurerCharacter::Look);
		EIC->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EIC->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}
}

//移動関数
void AAdventurerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D Input = Value.Get<FVector2D>();
	const FRotator YawRot(0.f, GetControlRotation().Yaw, 0.f);

}

void AAdventurerCharacter::Look(const FInputActionValue& Value)
{

}