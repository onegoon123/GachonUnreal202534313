// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyBullet.h"
#include "ShootPlayer.h"
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
AEnemyBullet::AEnemyBullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// box 컴포넌트 생성
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	RootComponent = Box;
	Box->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(RootComponent);
	StaticMesh->SetWorldScale3D(FVector(3.0f, 0.5f, 0.5f));
	// bodymesh에 cube static mesh 데이터 로드해서 할당하기
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (tempMesh.Succeeded() /* or tempMesh.Object != nullptr */)
	{
		StaticMesh->SetStaticMesh(tempMesh.Object);
	}

	ConstructorHelpers::FObjectFinder<USoundBase> TempImpactSound(TEXT("/Script/Engine.SoundWave'/Game/Sounds/Explosion.Explosion'"));
	if (TempImpactSound.Succeeded())
	{
		ImpactSound = TempImpactSound.Object;
	}

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> TempHitEffect(TEXT("/Script/Niagara.NiagaraSystem'/Game/Fire_EXP_Vol01_Free/Niagara/EXP/NS_Sub_EXP_Small_002.NS_Sub_EXP_Small_002'"));
	if (TempHitEffect.Succeeded())
	{
		HitEffect = TempHitEffect.Object;
	}
}

// Called when the game starts or when spawned
void AEnemyBullet::BeginPlay()
{
	Super::BeginPlay();
	
	Box->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBullet::Attack);
	SetLifeSpan(LifeTime);
}

// Called every frame
void AEnemyBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	SetActorLocation(GetActorLocation() + MoveDirection.GetSafeNormal() * Speed * DeltaTime, true);
}

void AEnemyBullet::Attack(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != nullptr && OtherActor->IsA<AShootPlayer>())
	{
		if (ImpactSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		}
		if (HitEffect != nullptr)
		{
			SpawnTemporaryNiagara(this, HitEffect, GetActorLocation(), FVector(2.0f), 2.0f);
		}

		OtherActor->Destroy();
		Destroy();
	}
}

