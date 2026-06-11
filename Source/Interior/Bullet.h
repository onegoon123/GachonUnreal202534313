// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Bullet.generated.h"

UCLASS()
class INTERIOR_API ABullet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void Attack(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* Box;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Speed = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float LifeTime = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FVector MoveDirection = FVector(0.0f, 0.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* HitEffect;
};
