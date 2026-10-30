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
class UTrapPlacementComponent; //トラップ設置Comp
class ATrapBase; //トラップベース
class APickUpItem; //アイテム
class UAnimSequence; //アニメーション
class AGamePlayModeBase;

//プレイヤーの状態管理用
UENUM(BlueprintType)
enum class EAdvennturerState : uint8
{
    PlayMode,
    TrapMode,
    Inoperable,
};

UCLASS()
class TEAMPROJECT_API AAdventurerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAdventurerCharacter();

    //外部からのプレイヤーの状態参照
    UFUNCTION(BlueprintPure, Category = "Player")
    EAdvennturerState GetAdvennturerState() const { return AdvennturerState; }

    UFUNCTION(BlueprintPure, Category = "Player")
    int32 GetPlayerIndex() const { return PlayerIndex; }

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

    UFUNCTION(BlueprintCallable, Category = "Player")
    void PlayerReset();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    virtual void PossessedBy(AController* NewController) override;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

  
private:	

#pragma region InputFunction

    //キャラ移動関数
    UFUNCTION()
    void Move(const FInputActionValue& Value);

    //視点操作関数
    UFUNCTION()
    void Look(const FInputActionValue& Value);

    //トラップ設置操作関数
    UFUNCTION()
    void PlaceTrap();

    //トラップ回転操作関数
    UFUNCTION()
    void TrapRotate();

    //トラップスロット操作関数
    UFUNCTION()
    void TrapSlotChange();

    UFUNCTION()
    void Cancel();

    //アイテム使用関数
    UFUNCTION()
    void UseHeldItem();

#pragma endregion InputFunction

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
#pragma region InputAction

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

    //トラップスロット右アクション
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* RightTrapSlotAction;

    //トラップスロット左アクション
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* LeftTrapSlotAction;

#pragma endregion InputAction

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
    
    //プレイヤーのトラップ設置上限
    UPROPERTY(EditAnywhere, Category = "Trap")
    int32 TrapPlaceMax = 5;

    //プレイヤーのトラップ設置回数
    UPROPERTY(EditAnywhere, Category = "Trap")
    int32 TrapPlaceCount = 0;

    //プレイヤーの状態リセット
    UPROPERTY(EditAnywhere, Category = "Character")
    EAdvennturerState AdvennturerState = EAdvennturerState::PlayMode;

    
#pragma endregion CharacterSetting

    UPROPERTY()
    UTrapPlacementComponent* TrapPlacement;

    //ゲームモードクラス
    UPROPERTY()
    AGamePlayModeBase* GamePlayMode; 

    //アニメーション
    UPROPERTY()
    TObjectPtr<UAnimSequence>CurrentAnim;

    //初期位置(リスポーン位置)
    FTransform StartTransform;

    UPROPERTY(EditAnywhere, Category = "Trap")
    TArray  <TSubclassOf<ATrapBase>>TrapList;

    UPROPERTY(VisibleAnywhere)
    TSubclassOf<ATrapBase> SelectedTrap;

    UPROPERTY(VisibleAnywhere)
    int32 PlayerIndex = INDEX_NONE;
};
