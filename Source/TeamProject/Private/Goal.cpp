// Fill out your copyright notice in the Description page of Project Settings.


#include "Goal.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AGoal::AGoal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	//ÉÅÉbÉVÉÖçÏê¨
	GoalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
	GoalMesh->SetupAttachment(Root);
	GoalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//ìñÇΩÇËîªíËçÏê¨
	GoalCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("GoalCollision"));
	GoalCollision->SetupAttachment(Root);
	GoalCollision->SetBoxExtent(FVector(100.f, 100.f, 100.f));
	GoalCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	GoalCollision->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void AGoal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGoal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


