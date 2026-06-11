// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"

#include "Boss.h"
#include "Components/AudioComponent.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EnemyClass = AEnemy::StaticClass();
	BossClass = ABoss::StaticClass();

	WaveEnemyCounts = { 5, 8, 12 };

	ConstructorHelpers::FObjectFinder<USoundBase> TempStageBgmSound(TEXT("/Script/Engine.SoundWave'/Game/Sounds/BGM1.BGM1'"));
	if (TempStageBgmSound.Succeeded())
	{
		StageBgmSound = TempStageBgmSound.Object;
	}

	ConstructorHelpers::FObjectFinder<USoundBase> TempBossBgmSound(TEXT("/Script/Engine.SoundWave'/Game/Sounds/BGM2.BGM2'"));
	if (TempBossBgmSound.Succeeded())
	{
		BossBgmSound = TempBossBgmSound.Object;
	}
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	PlayBgm(StageBgmSound);
	StartNextWave();
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bBossBattleStarted) return;

	if (bWaitingForNextWave)
	{
		WaveDelayTimer -= DeltaTime;
		if (WaveDelayTimer <= 0.0f)
		{
			bWaitingForNextWave = false;
			StartNextWave();
		}
		return;
	}

	if (!WaveEnemyCounts.IsValidIndex(CurrentWaveIndex)) return;

	const int32 CurrentWaveEnemyCount = WaveEnemyCounts[CurrentWaveIndex];
	if (SpawnedInCurrentWave >= CurrentWaveEnemyCount) return;

	SpawnTimer += DeltaTime;
	if (SpawnTimer >= SpawnInterval)
	{
		SpawnTimer = 0.0f;
		SpawnEnemy();
	}
}

void AEnemySpawner::SpawnEnemy()
{
	if (EnemyClass == nullptr || GetWorld() == nullptr) return;

	const FVector SpawnLocation = GetSpawnLocation();
	if (AEnemy* SpawnedEnemy = GetWorld()->SpawnActor<AEnemy>(EnemyClass, SpawnLocation, FRotator::ZeroRotator))
	{
		SpawnedInCurrentWave++;
		AliveEnemies++;
		SpawnedEnemy->OnDestroyed.AddDynamic(this, &AEnemySpawner::OnSpawnedEnemyDestroyed);
	}
}

FVector AEnemySpawner::GetSpawnLocation() const
{
	if (SpawnPoints.Num() > 0)
	{
		const int32 Index = FMath::RandRange(0, SpawnPoints.Num() - 1);
		if (SpawnPoints[Index] != nullptr)
		{
			return SpawnPoints[Index]->GetActorLocation();
		}
	}

	const float RandomY = FMath::RandRange(-SpawnRadius, SpawnRadius);
	return GetActorLocation() + FVector(0.0f, RandomY, 0.0f);
}

void AEnemySpawner::StartNextWave()
{
	CurrentWaveIndex++;
	SpawnedInCurrentWave = 0;
	SpawnTimer = SpawnInterval;

	if (!WaveEnemyCounts.IsValidIndex(CurrentWaveIndex))
	{
		BeginBossBattle();
	}
}

void AEnemySpawner::BeginBossBattle()
{
	if (bBossBattleStarted || GetWorld() == nullptr) return;

	bBossBattleStarted = true;
	PlayBgm(BossBgmSound);

	if (BossClass == nullptr) return;

	const FVector SpawnLocation = BossSpawnPoint != nullptr ? BossSpawnPoint->GetActorLocation() : GetActorLocation();
	GetWorld()->SpawnActor<ABoss>(BossClass, SpawnLocation, FRotator::ZeroRotator);
}

void AEnemySpawner::PlayBgm(USoundBase* NewBgmSound)
{
	if (BgmAudioComponent != nullptr)
	{
		BgmAudioComponent->Stop();
		BgmAudioComponent = nullptr;
	}

	if (NewBgmSound != nullptr)
	{
		BgmAudioComponent = UGameplayStatics::SpawnSound2D(this, NewBgmSound, 1.0f, 1.0f, 0.0f, nullptr, false, false);
	}
}

void AEnemySpawner::OnSpawnedEnemyDestroyed(AActor* DestroyedActor)
{
	AliveEnemies = FMath::Max(0, AliveEnemies - 1);

	if (!WaveEnemyCounts.IsValidIndex(CurrentWaveIndex) || bBossBattleStarted) return;

	const bool bAllEnemiesSpawned = SpawnedInCurrentWave >= WaveEnemyCounts[CurrentWaveIndex];
	const bool bAllSpawnedEnemiesDefeated = AliveEnemies <= 0;
	if (bAllEnemiesSpawned && bAllSpawnedEnemiesDefeated)
	{
		bWaitingForNextWave = true;
		WaveDelayTimer = TimeBetweenWaves;
	}
}

