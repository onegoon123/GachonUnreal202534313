// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootPlayer.h"

#include "Boss.h"
#include "Bullet.h"
#include "Enemy.h"
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

// Sets default values
AShootPlayer::AShootPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	RootComponent = Box;
	Box->SetCollisionProfileName(TEXT("Player"));
	Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Box->SetGenerateOverlapEvents(true);
	Box->SetBoxExtent(FVector(70.0f, 70.0f, 70.0f));

	Pivot = CreateDefaultSubobject<USceneComponent>("PivotComponent");
	Pivot->SetupAttachment(RootComponent);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Pivot);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Game/SpaceShip/Spaceship_ARA.Spaceship_ARA'"));
	if (tempMesh.Succeeded() /* or tempMesh.Object != nullptr */)
	{
		Mesh->SetStaticMesh(tempMesh.Object);
	}
	
	FirePosition = CreateDefaultSubobject<USceneComponent>("FirePosition");
	FirePosition->SetupAttachment(Mesh);
	FirePosition->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));

	Speed = 600.0f;
	BulletClass = ABullet::StaticClass();

	ConstructorHelpers::FObjectFinder<USoundBase> TempShootSound(TEXT("/Script/Engine.SoundWave'/Game/Sounds/Shoot1.Shoot1'"));
	if (TempShootSound.Succeeded())
	{
		ShootSound = TempShootSound.Object;
	}

	ConstructorHelpers::FObjectFinder<USoundBase> TempBgmSound(TEXT("/Script/Engine.SoundWave'/Game/Sounds/BGM1.BGM1'"));
	if (TempBgmSound.Succeeded())
	{
		BgmSound = TempBgmSound.Object;
	}

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> TempHitEffect(TEXT("/Script/Niagara.NiagaraSystem'/Game/Fire_EXP_Vol01_Free/Niagara/EXP/NS_Sub_EXP_Small_002.NS_Sub_EXP_Small_002'"));
	if (TempHitEffect.Succeeded())
	{
		HitEffect = TempHitEffect.Object;
	}

	bAutoPlayBgm = false;
}

// Called when the game starts or when spawned
void AShootPlayer::BeginPlay()
{
	Super::BeginPlay();

	Box->OnComponentBeginOverlap.AddDynamic(this, &AShootPlayer::OnPlayerOverlap);

	if (bAutoPlayBgm && BgmSound != nullptr)
	{
		UGameplayStatics::PlaySound2D(this, BgmSound);
	}
	
}

void AShootPlayer::OnPlayerOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr || OtherActor == this || IsActorBeingDestroyed()) return;

	const bool bHitByEnemy = OtherActor->IsA<AEnemy>() || OtherActor->IsA<ABoss>() || OtherActor->IsA<AEnemyBullet>();
	if (!bHitByEnemy) return;

	if (HitEffect != nullptr)
	{
		SpawnTemporaryNiagara(this, HitEffect, GetActorLocation(), FVector(2.0f), 2.0f);
	}

	Destroy();
}

// Called every frame
void AShootPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	MoveWithCollision(DeltaTime);
	MoveAnimation(DeltaTime);
}

// Called to bind functionality to input
void AShootPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis(TEXT("Horizontal"), this, &AShootPlayer::MoveHorizontal);
	PlayerInputComponent->BindAxis(TEXT("Vertical"), this, &AShootPlayer::MoveVertical);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &AShootPlayer::Shoot);
	
}

void AShootPlayer::MoveHorizontal(float value)
{
	Direction.Y = value;
	Direction = Direction.GetClampedToMaxSize(1.0f);
}

void AShootPlayer::MoveVertical(float value)
{
	Direction.Z = value;
	Direction = Direction.GetClampedToMaxSize(1.0f);
}

void AShootPlayer::MoveAnimation(float DeltaTime)
{
	auto rot = Pivot->GetRelativeRotation();
	double yaw = FMath::Lerp(rot.Yaw, Direction.Y * -45.0, 0.1);
	double pitch = FMath::Lerp(rot.Pitch, Direction.Z * -10.0, 0.1);
	Pivot->SetRelativeRotation(FRotator(pitch, yaw, 0.0));
}

void AShootPlayer::Shoot()
{
	if (BulletClass == nullptr || GetWorld() == nullptr) return;

	FActorSpawnParameters spawnParams;
	GetWorld()->SpawnActor<ABullet>(BulletClass, FirePosition->GetComponentLocation(), FRotator::ZeroRotator, spawnParams);

	if (ShootSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShootSound, FirePosition->GetComponentLocation());
	}
}

void AShootPlayer::MoveWithCollision(float DeltaTime)
{
	const FVector ClampedDirection = Direction.GetClampedToMaxSize(1.0f);
	const FVector Delta = ClampedDirection * Speed * DeltaTime;

	if (!FMath::IsNearlyZero(Delta.Y))
	{
		AddActorWorldOffset(FVector(0.0f, Delta.Y, 0.0f), true);
	}

	if (!FMath::IsNearlyZero(Delta.Z))
	{
		AddActorWorldOffset(FVector(0.0f, 0.0f, Delta.Z), true);
	}
}
