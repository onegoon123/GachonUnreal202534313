// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyBullet.h"
#include "ShootPlayer.h"


// Sets default values
AEnemyBullet::AEnemyBullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// box 컴포넌트 생성
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	RootComponent = Box;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(RootComponent);
	StaticMesh->SetWorldScale3D(FVector(3.0f, 0.5f, 0.5f));
	// bodymesh에 cube static mesh 데이터 로드해서 할당하기
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (tempMesh.Succeeded() /* or tempMesh.Object != nullptr */)
	{
		StaticMesh->SetStaticMesh(tempMesh.Object);
	}
}

// Called when the game starts or when spawned
void AEnemyBullet::BeginPlay()
{
	Super::BeginPlay();
	
	Box->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBullet::Attack);
}

// Called every frame
void AEnemyBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	SetActorLocation(GetActorLocation() + GetActorForwardVector() * DeltaTime * 100);
}

void AEnemyBullet::Attack(UPrimitiveComponent* OverlapComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->StaticClass() == AShootPlayer::StaticClass())
	{
		OtherActor->Destroy();
	}
}

