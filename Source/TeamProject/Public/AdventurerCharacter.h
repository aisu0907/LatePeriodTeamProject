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
class UTrapPlacementCompoent; //トラップ設置Comp
class UTrapBase; //トラップベース
class APickUpItem; //アイテム
class UAnimSequence; //アニメーション


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

    //トラップに当たったらリスポーン
    UFUNCTION(BlueprintCallable, Category = "Player")
    void RespawnAtStart();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

  
private:	

    //キャラ移動関数
    void Move(const FInputActionValue& Value);
    //カメラ操作関数
    void Look(const FInputActionValue& Value);

    //void PlaceTrap();

    //アイテム使用関数
    void UseHeldItem();

    //追加コンポーネントまとめ
#pragma region Component
    //カメラアームコンポーネント
    UPROPERTY(VisibleAnywhere, Category = "Camera")
    USpringArmComponent* CameraBoom;

    //カメラコンポーネント
    UPROPERTY(VisibleAnywhere, Category = "Camera")
    UCameraComponent* FollowCamera;

    //プレイヤーの移動管理用
    UPROPERTY()
    UCharacterMovementComponent* MoveSetting;
#pragma endregion Component

    //プレイヤーIputActionまとめ
#pragma region Input

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

    //トラップ設置アクション
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* PlaceTrapAction;

    //トラップ回転アクション
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* TrapRotateAction;

    //キャンセルアクション
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* CancelAction;
#pragma endregion Input

    //キャラクターのステータス設定
#pragma region CharacterSetting

    //キャラクタ移動スピード
    UPROPERTY(EditAnywhere, Category = "Character")
    float MoveSpeed = 0;
    
    //キャラクターの空中コントロール
    UPROPERTY(EditAnywhere, Category = "Character")
    float AirControllPower = 0;

    //カメラアーム距離

    UPROPERTY(EditAnywhere, Category = "Camera")
    float DefaultArmLength = 400.f;

    //カメラの移動スピード
    UPROPERTY(EditAnywhere, Category = "Camera")
    float CameraSpeed = 0;

#pragma endregion CharacterSetting

    //アニメーション
    UPROPERTY()
    TObjectPtr<UAnimSequence>CurrentAnim;

    //プレイヤーの状態リセット
    EAdvennturerState AdvennturerState = EAdvennturerState::PlayMode;

    //初期位置(リスポーン位置)
    FTransform StartTransform;

    

};
