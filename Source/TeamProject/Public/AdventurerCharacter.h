// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AdventurerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class TEAMPROJECT_API AAdventurerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAdventurerCharacter();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	

    //キャラ移動関数
    void Move(const FInputActionValue& Value);
    //カメラ操作関数
    void Look(const FInputActionValue& Value);

    //カメラアームコンポーネント
    UPROPERTY(VisibleAnywhere, Category = "Camera")
    USpringArmComponent* CameraBoom;

    //カメラコンポーネント
    UPROPERTY(VisibleAnywhere, Category = "Camera")
    UCameraComponent* FollowCamera;

    //カメラアーム距離
    UPROPERTY(EditAnywhere, Category = "Camera")
    float DefaultArmLength = 400.f;

    //インプットアクションコンポーネント
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    //移動アクション
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* MoveAction;

    //カメラアクション
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* LookAction;

    //ジャンプアクション
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* JumpAction;

    //プレイヤーの移動管理用
    UPROPERTY()
    UCharacterMovementComponent* MoveSetting;
};
