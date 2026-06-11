// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "ShootPlayer.generated.h"

UCLASS()
class INTERIOR_API AShootPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AShootPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	UBoxComponent* Box;
	
	UPROPERTY(EditAnywhere)
	USceneComponent* Pivot;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;
	
	UPROPERTY(EditAnywhere)
	USceneComponent* FirePosition;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Speed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooting")
	TSubclassOf<class ABullet> BulletClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundBase* ShootSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundBase* BgmSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	bool bAutoPlayBgm = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UNiagaraSystem* HitEffect;
	
private:
	FVector Direction;
	bool IsUpgraded;

	UFUNCTION()
	void OnPlayerOverlap(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	void MoveHorizontal(float value);
	void MoveVertical(float value);
	void Shoot();
	
	void MoveAnimation(float DeltaTime);
	void MoveWithCollision(float DeltaTime);
};
