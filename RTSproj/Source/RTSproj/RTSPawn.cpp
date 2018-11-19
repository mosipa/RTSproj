// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSPawn.h"
#include "Camera/CameraComponent.h"

// Sets default values
ARTSPawn::ARTSPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	Camera = CreateDefaultSubobject<UCameraComponent>(FName("CameraComponent"));
	SetRootComponent(Camera);
	Camera->SetWorldLocation(FVector(0.f, 0.f, 800.f));
	Camera->SetWorldRotation(FRotator(-60.f, 0.f, 0.f));
}

// Called when the game starts or when spawned
void ARTSPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARTSPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARTSPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

