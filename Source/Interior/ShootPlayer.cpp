// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootPlayer.h"

#include "Bullet.h"
#include "UniversalObjectLocators/UniversalObjectLocatorUtils.h"

// Sets default values
AShootPlayer::AShootPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	RootComponent = Box;
	Pivot = CreateDefaultSubobject<USceneComponent>("PivotComponent");
	Pivot->SetupAttachment(RootComponent);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetupAttachment(Pivot);
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Game/SpaceShip/Spaceship_ARA.Spaceship_ARA'"));
	if (tempMesh.Succeeded() /* or tempMesh.Object != nullptr */)
	{
		Mesh->SetStaticMesh(tempMesh.Object);
	}
	
	FirePosition = CreateDefaultSubobject<USceneComponent>("FirePosition");
	FirePosition->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void AShootPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShootPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	SetActorLocation(GetActorLocation() + Direction * Speed * DeltaTime, true);
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
}

void AShootPlayer::MoveVertical(float value)
{
	Direction.Z = value;
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
	FActorSpawnParameters spawnParams;
	GetWorld()->SpawnActor<ABullet>(FirePosition->GetComponentLocation(), FRotator::ZeroRotator, spawnParams);
}