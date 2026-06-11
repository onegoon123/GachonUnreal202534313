// Fill out your copyright notice in the Description page of Project Settings.

#include "Boss.h"

#include "EnemyBullet.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"

namespace
{
	void SpawnTemporaryNiagara(AActor* Owner, UNiagaraSystem* Effect, const FVector& Location, const FVector& Scale, float LifeTime)
	{
		if (Owner == nullptr || Effect == nullptr || Owner->GetWorld() == nullptr) return;

		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			Owner, Effect, Location, FRotator::ZeroRotator, Scale, true, true);
		if (NiagaraComponent == nullptr) return;

		FTimerHandle TimerHandle;
		FTimerDelegate CleanupDelegate = FTimerDelegate::CreateWeakLambda(NiagaraComponent, [NiagaraComponent]()
		{
			if (IsValid(NiagaraComponent))
			{
				NiagaraComponent->DeactivateImmediate();
				NiagaraComponent->DestroyComponent();
			}
		});
		Owner->GetWorldTimerManager().SetTimer(TimerHandle, CleanupDelegate, LifeTime, false);
	}
}

ABoss::ABoss()
{
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;
	Box->SetCollisionProfileName(TEXT("Enemy"));
	Box->SetBoxExtent(FVector(120.0f, 120.0f, 80.0f));

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);
	StaticMesh->SetRelativeScale3D(FVector(3.0f));

	ConstructorHelpers::FObjectFinder<UStaticMesh> TempMesh(TEXT("/Script/Engine.StaticMesh'/Game/Drone/Drone_low.Drone_low'"));
	if (TempMesh.Succeeded())
	{
		StaticMesh->SetStaticMesh(TempMesh.Object);
	}

	ConstructorHelpers::FObjectFinder<USoundBase> TempExplosionSound(TEXT("/Script/Engine.SoundWave'/Game/Sounds/Explosion.Explosion'"));
	if (TempExplosionSound.Succeeded())
	{
		ExplosionSound = TempExplosionSound.Object;
	}

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> TempDeathEffect(TEXT("/Script/Niagara.NiagaraSystem'/Game/Fire_EXP_Vol01_Free/Niagara/EXP/NS_Sub_EXP_Large_001_01.NS_Sub_EXP_Large_001_01'"));
	if (TempDeathEffect.Succeeded())
	{
		DeathEffect = TempDeathEffect.Object;
	}

	EnemyBulletClass = AEnemyBullet::StaticClass();
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	EntranceTargetLocation = StartLocation + FVector(0.0f, 0.0f, -EntranceDistance);
	CurrentHealth = MaxHealth;

	if (GetWorld() != nullptr && GetWorld()->GetFirstPlayerController() != nullptr)
	{
		PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	}
}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsEntering)
	{
		MoveEntrance(DeltaTime);
		return;
	}

	MovePattern(DeltaTime);

	AimShotTimer += DeltaTime;
	if (AimShotTimer >= AimShotInterval)
	{
		AimShotTimer = 0.0f;
		FireAimShot();
	}

	SpreadShotTimer += DeltaTime;
	if (SpreadShotTimer >= SpreadShotInterval)
	{
		SpreadShotTimer = 0.0f;
		FireSpreadShot();
	}
}

void ABoss::MoveEntrance(float DeltaTime)
{
	const FVector CurrentLocation = GetActorLocation();
	const FVector NextLocation = FMath::VInterpConstantTo(CurrentLocation, EntranceTargetLocation, DeltaTime, EntranceSpeed);
	SetActorLocation(NextLocation, true);

	if (FVector::DistSquared(NextLocation, EntranceTargetLocation) <= 1.0f)
	{
		bIsEntering = false;
		StartLocation = EntranceTargetLocation;
		SetActorLocation(EntranceTargetLocation, true);
	}
}

void ABoss::HitByPlayerBullet()
{
	CurrentHealth--;

	if (CurrentHealth <= 0)
	{
		Die();
	}
}

void ABoss::MovePattern(float DeltaTime)
{
	const FVector NewLocation = GetActorLocation() + FVector(0.0f, MoveDirection * MoveSpeed * DeltaTime, 0.0f);
	SetActorLocation(NewLocation, true);

	const float DistanceFromStart = FMath::Abs(GetActorLocation().Y - StartLocation.Y);
	if (DistanceFromStart >= MoveRange)
	{
		MoveDirection *= -1.0f;
	}
}

void ABoss::FireAimShot()
{
	if (EnemyBulletClass == nullptr || PlayerPawn == nullptr || GetWorld() == nullptr) return;

	const FVector SpawnLocation = GetActorLocation() + FVector(0.0f, 0.0f, -120.0f);
	const FVector Direction = (PlayerPawn->GetActorLocation() - SpawnLocation).GetSafeNormal();

	if (AEnemyBullet* Bullet = GetWorld()->SpawnActor<AEnemyBullet>(EnemyBulletClass, SpawnLocation, FRotator::ZeroRotator))
	{
		Bullet->MoveDirection = Direction;
	}
}

void ABoss::FireSpreadShot()
{
	if (EnemyBulletClass == nullptr || GetWorld() == nullptr) return;

	const FVector SpawnLocation = GetActorLocation() + FVector(0.0f, 0.0f, -120.0f);
	const int32 Count = FMath::Max(1, SpreadShotCount);
	const float StartAngle = -45.0f;
	const float EndAngle = 45.0f;

	for (int32 Index = 0; Index < Count; Index++)
	{
		const float Alpha = Count == 1 ? 0.5f : static_cast<float>(Index) / static_cast<float>(Count - 1);
		const float Angle = FMath::DegreesToRadians(FMath::Lerp(StartAngle, EndAngle, Alpha));
		const FVector Direction(0.0f, FMath::Sin(Angle), -FMath::Cos(Angle));
		if (AEnemyBullet* Bullet = GetWorld()->SpawnActor<AEnemyBullet>(EnemyBulletClass, SpawnLocation, FRotator::ZeroRotator))
		{
			Bullet->MoveDirection = Direction;
		}
	}
}

void ABoss::Die()
{
	if (ExplosionSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}
	if (DeathEffect != nullptr)
	{
		SpawnTemporaryNiagara(this, DeathEffect, GetActorLocation(), FVector(1.0f), 3.0f);
	}

	Destroy();
}
