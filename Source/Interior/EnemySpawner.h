// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class INTERIOR_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TSubclassOf<class AEnemy> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float SpawnInterval = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TArray<AActor*> SpawnPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TArray<int32> WaveEnemyCounts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float TimeBetweenWaves = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	TSubclassOf<class ABoss> BossClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	AActor* BossSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundBase* StageBgmSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundBase* BossBgmSound;

private:
	float SpawnTimer = 0.0f;
	float WaveDelayTimer = 0.0f;
	int32 CurrentWaveIndex = -1;
	int32 SpawnedInCurrentWave = 0;
	int32 AliveEnemies = 0;
	bool bWaitingForNextWave = false;
	bool bBossBattleStarted = false;

	UPROPERTY()
	class UAudioComponent* BgmAudioComponent;

	void SpawnEnemy();
	FVector GetSpawnLocation() const;
	void StartNextWave();
	void BeginBossBattle();
	void PlayBgm(class USoundBase* NewBgmSound);

	UFUNCTION()
	void OnSpawnedEnemyDestroyed(AActor* DestroyedActor);
};
