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
	
	UPROPERTY(EditAnywhere)
	float Speed;
	
private:
	FVector Direction;
	bool IsUpgraded;
	
	void MoveHorizontal(float value);
	void MoveVertical(float value);
	void Shoot();
	
	void MoveAnimation(float DeltaTime);
};
