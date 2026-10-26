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

//アイテム
class APickUpItem;

//アニメーション
class UAnimSequence;

//プレイヤーの状態管理用
UENUM(BlueprintType)
enum class EAdvennturerState : uint8
{
    PlayMode,
    Trapmode,
    Inoperable,
};


UCLASS()
class TEAMPROJECT_API AAdventurerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAdventurerCharacter();

    //外部からのトラップの状態参照
    UFUNCTION(BlueprintPure, Category = "Player")
    EAdvennturerState GetAdvennturerState() const { return AdvennturerState; }


    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    //手に持っているアイテム
    UPROPERTY()
    APickUpItem* HeldItem = nullptr;

    //バフ効果があるか
    bool bBuffActive = false;

    //アニメーション
    UPROPERTY(EditAnywhere, Category = "Animation")
    TObjectPtr<UAnimSequence> IdleAnim;

    UPROPERTY(EditAnywhere, Category = "Animation")
    TObjectPtr<UAnimSequence> WalkAnim;

    UPROPERTY(EditAnywhere, Category = "Animation")
    TObjectPtr<UAnimSequence> JumpAnim;

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

    //アイテム使用アクション
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* UseAction;

    //アイテム使用関数
    void UseHeldItem();

    //プレイヤーの移動管理用
    UPROPERTY()
    UCharacterMovementComponent* MoveSetting;

    //キャラクタ移動スピード
    UPROPERTY(EditAnywhere, Category = "Character")
    float MoveSpeed = 0;
    
    //キャラクターの空中コントロール
    UPROPERTY(EditAnywhere, Category = "Character")
    float AirControllPower = 0;

    //カメラアーム距離

    UPROPERTY(EditAnywhere, Category = "Camera")
    float DefaultArmLength = 400.f;

    //アニメーション
    UPROPERTY()
    TObjectPtr<UAnimSequence>CurrentAnim;


    //カメラの移動スピード
    UPROPERTY(EditAnywhere, Category = "Camera")
    float CameraSpeed = 0;

    EAdvennturerState AdvennturerState = EAdvennturerState::PlayMode;
};
