// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

// Sets default values
AEnemy::AEnemy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Box = CreateDefaultSubobject<UBoxComponent>(FName("Box"));
	RootComponent = Box;
	Box->SetCollisionProfileName(TEXT("Enemy"));
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);

	Speed = 250.0f;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld() != nullptr && GetWorld()->GetFirstPlayerController() != nullptr)
	{
		PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (PlayerPawn == nullptr) return;

	const FVector Direction = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	SetActorLocation(GetActorLocation() + Direction * Speed * DeltaTime, true);
}

