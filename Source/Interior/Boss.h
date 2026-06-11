// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Boss.generated.h"

UCLASS()
class INTERIOR_API ABoss : public AActor
{
	GENERATED_BODY()

public:
	ABoss();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	void HitByPlayerBullet();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* Box;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	int32 MaxHealth = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float MoveSpeed = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float MoveRange = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Entrance")
	float EntranceDistance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Entrance")
	float EntranceSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float AimShotInterval = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	float SpreadShotInterval = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	int32 SpreadShotCount = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss")
	TSubclassOf<class AEnemyBullet> EnemyBulletClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundBase* ExplosionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* DeathEffect;

private:
	UPROPERTY()
	APawn* PlayerPawn;

	int32 CurrentHealth;
	FVector StartLocation;
	FVector EntranceTargetLocation;
	float MoveDirection = 1.0f;
	float AimShotTimer = 0.0f;
	float SpreadShotTimer = 0.0f;
	bool bIsEntering = true;

	void MoveEntrance(float DeltaTime);
	void MovePattern(float DeltaTime);
	void FireAimShot();
	void FireSpreadShot();
	void Die();
};
