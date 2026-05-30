// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Components/BoxComponent.h"

APawn* PlayerPawn;

// Sets default values
AEnemy::AEnemy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	auto Box = CreateDefaultSubobject<UBoxComponent>(FName("Box"));
	RootComponent = Box;
	
	auto StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	if (PlayerPawn == nullptr)
	{
		PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (PlayerPawn == nullptr) return;
	
	auto dir = (PlayerPawn->GetActorLocation() - GetActorLocation()).Normalize();
	SetActorLocationAndRotation(GetActorLocation() + dir * Speed, FRotator::ZeroRotator);
}

