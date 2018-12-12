// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSPlayerController.h"
#include "RTSHud.h"
#include "RTSprojCharacter.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Projectile.h"
#include "Engine/World.h"
#include "Engine/Classes/Kismet/KismetMathLibrary.h"

ARTSPlayerController::ARTSPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Hand;
}

void ARTSPlayerController::BeginPlay()
{
	HUDPtr = Cast<ARTSHud>(GetHUD());

	bSomeoneToStab = false;
	bSomeoneToShoot = false;
	bSomeoneToAid = false;
	Target = nullptr;
}

void ARTSPlayerController::Tick(float DeltaTime)
{
	//TODO change it to timers, there's no need to call it every tick
	if (bSomeoneToStab) { Knife(); }
	if (bSomeoneToShoot) { Pistol(); }
	if (bSomeoneToAid) { Aid(); }
}

void ARTSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (!InputComponent) { return; }

	InputComponent->BindAction("Select", IE_Pressed, this, &ARTSPlayerController::Select);
	InputComponent->BindAction("Select", IE_Released, this, &ARTSPlayerController::FinishSelecting);
	InputComponent->BindAction("Move", IE_Pressed, this, &ARTSPlayerController::Move);
	InputComponent->BindAction("Knife", IE_Pressed, this, &ARTSPlayerController::Knife);
	InputComponent->BindAction("Pistol", IE_Pressed, this, &ARTSPlayerController::Pistol);
	InputComponent->BindAction("Aid", IE_Pressed, this, &ARTSPlayerController::Aid);
}

void ARTSPlayerController::Select()
{
	if (!HUDPtr) { return; }
	UE_LOG(LogTemp, Warning, TEXT("Selecting"));
	HUDPtr->InitialPoint = HUDPtr->GetMousePosition2D();
	HUDPtr->bStartSelecting = true;
}

void ARTSPlayerController::FinishSelecting()
{
	if (!HUDPtr) { return; }
	UE_LOG(LogTemp, Warning, TEXT("FinishSelecting"));
	HUDPtr->bStartSelecting = false;
}

void ARTSPlayerController::Move()
{
	bSomeoneToStab = false;
	bSomeoneToShoot = false;
	bSomeoneToAid = false;
	Target = nullptr;

	if (!HUDPtr) { return; }
	if (HUDPtr->GetSelectedActors().Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Moving"));
		TArray<ARTSprojCharacter*> SelectedActors = HUDPtr->GetSelectedActors();

		FHitResult Hit;
		GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, Hit);
		FVector MoveTo = Hit.Location;

		for (auto Actor : SelectedActors)
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(Actor->GetController(), MoveTo);
		}
	}
}

void ARTSPlayerController::Knife()
{
	bSomeoneToShoot = false;
	bSomeoneToAid = false;

	if (!HUDPtr) { return; }

	if (HUDPtr->GetSelectedActors().Num() > 0)
	{
		TArray<ARTSprojCharacter*> SelectedActors = HUDPtr->GetSelectedActors();

		FHitResult Hit;

		if (GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, Hit))
		{
			//Check if target is character OR
			//there's someone to stab from previous order
			if (Hit.GetActor()->GetClass()->IsChildOf<ARTSprojCharacter>() || bSomeoneToStab)
			{
				//Check if you hover over another character and player did press a button again (1st time passes fine)
				if (this->WasInputKeyJustPressed(FKey(FName("A"))))
				{
					Target = Hit.GetActor();
				}
				
				for (auto Actor : SelectedActors)
				{
					if (Target && Actor->GetName().Equals(Target->GetName()))
					{
						continue; //Prevent knifing yourself
					}
					else
					{
						if (!Target) { return; }

						UE_LOG(LogTemp, Warning, TEXT("Knife"));
						UAIBlueprintHelperLibrary::SimpleMoveToActor(Actor->GetController(), Target);

						float Distance = GetDistance(Target->GetActorLocation(), Actor->GetActorLocation());
						UE_LOG(LogTemp, Warning, TEXT("Getting closer: %f"), Distance);

						float Damage;
						
						float DotProd = Target->GetDotProductTo(Actor);
						
						//Behind the target
						if (DotProd <= 0)
						{
							UE_LOG(LogTemp, Warning, TEXT("Backstab"));
							Damage = 100.f;
						}
						//In front of the target
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("Frontstab"));
							Damage = 50.f;
						}

						//TODO doesnt work with 2 characters
						//TODO decide if we want to have 2 characters stabbing someone
						if(Distance <= 150.f)
						{
							UE_LOG(LogTemp, Warning, TEXT("Deal dmg"));
							bSomeoneToStab = false;

							//Rotation of a body (in case of player is in range so he doesnt have to move around to the target location)
							FRotator BodyRotation = UKismetMathLibrary::FindLookAtRotation(Actor->GetActorLocation(), Target->GetActorLocation());
							Actor->SetActorRotation(BodyRotation);

							UGameplayStatics::ApplyDamage(
								Target,
								Damage,
								this,
								Actor,
								UDamageType::StaticClass()
							);

							Target = nullptr;
						}
						else
						{
							bSomeoneToStab = true;
						}
					}
				}
			}
		}
	}
}

float ARTSPlayerController::GetDistance(FVector A, FVector B)
{
	FVector VectorLength = A - B;
	float Distance = FMath::Sqrt(FMath::Pow(VectorLength.X, 2) + FMath::Pow(VectorLength.Y, 2) + FMath::Pow(VectorLength.Z, 2));
	return Distance;
}

void ARTSPlayerController::Pistol()
{
	bSomeoneToStab = false;
	bSomeoneToAid = false;

	if (!HUDPtr) { return; }

	if (HUDPtr->GetSelectedActors().Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Pistol"));

		TArray<ARTSprojCharacter*> SelectedActors = HUDPtr->GetSelectedActors();

		FHitResult Hit;

		GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, Hit);
		if (Hit.GetActor()->GetClass()->IsChildOf<ARTSprojCharacter>() || bSomeoneToShoot)
		{
			if (this->WasInputKeyJustPressed(FKey(FName("G"))))
			{
				Target = Hit.GetActor();
			}

			for (auto Actor : SelectedActors)
			{
				if (Target->GetName().Equals(Actor->GetName()))
				{
					continue; //Prevent shooting yourself
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Aiming at other character"));

					float Distance = GetDistance(Target->GetActorLocation(), Actor->GetActorLocation());
					
					if (Distance >= 450.f)
					{
						//TODO doesnt work with 2 characters selected
						UE_LOG(LogTemp, Warning, TEXT("Too far"));
						
						bSomeoneToShoot = true;
						UAIBlueprintHelperLibrary::SimpleMoveToLocation(Actor->GetController(), Target->GetActorLocation());
					}
					else
					{
						//TODO doesnt work with 2 characters selected
						UE_LOG(LogTemp, Warning, TEXT("Close enough"));

						//Rotation of a body (in case of player is in range so he doesnt have to move around to the target location)
						FRotator BodyRotation = UKismetMathLibrary::FindLookAtRotation(Actor->GetActorLocation(), Target->GetActorLocation());
						Actor->SetActorRotation(BodyRotation);

						Actor->GetMovementComponent()->StopMovementImmediately();
						bSomeoneToShoot = false;

						auto Projectile = GetWorld()->SpawnActor<AProjectile>(
							Actor->GetActorLocation() + Actor->GetActorForwardVector() * 100.f,
							Actor->GetActorRotation()
							);

						Target = nullptr;

						if (!Projectile) { return; }
						Projectile->LaunchProjectile();
					}
				}
			}
		}
	}
}

void ARTSPlayerController::Aid()
{
	bSomeoneToStab = false;
	bSomeoneToShoot = false;

	if (!HUDPtr) { return; }

	if (HUDPtr->GetSelectedActors().Num() > 0)
	{
		TArray<ARTSprojCharacter*> SelectedActors = HUDPtr->GetSelectedActors();

		FHitResult Hit;

		GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, Hit);
		if (Hit.GetActor()->GetClass()->IsChildOf<ARTSprojCharacter>() || bSomeoneToAid)
		{
			if (this->WasInputKeyJustPressed(FKey(FName("C"))))
			{
				Target = Hit.GetActor();
				AidState = EAidState::Cleansing;
			}

			if (this->WasInputKeyJustPressed(FKey(FName("H"))))
			{
				Target = Hit.GetActor();
				AidState = EAidState::Healing;
			}
			
			//Only do it if Target is bleeding or is injured
			if (Cast<ARTSprojCharacter>(Target)->IsCharacterBleeding()
				|| Cast<ARTSprojCharacter>(Target)->IsCharacterInjured())
			{
				for (auto Actor : SelectedActors)
				{
					float Distance = GetDistance(Target->GetActorLocation(), Actor->GetActorLocation());

					if (Distance > 150)
					{
						UE_LOG(LogTemp, Warning, TEXT("Too far for aiding target"));
						bSomeoneToAid = true;

						UAIBlueprintHelperLibrary::SimpleMoveToActor(Actor->GetController(), Target);
					}
					else
					{
						//TODO selfheal and selfcleanse takes longer
						UE_LOG(LogTemp, Warning, TEXT("Aid"));

						//Rotation of a actor's body (in case of player is in range so he doesnt have to move around to the target location)
						//Rotation of target's body (faces actor that's aiding him)
						//Only do it if the target and actor isn't the same character
						if (!Target->GetName().Equals(Actor->GetName()))
						{
							FRotator ActorBodyRotation = UKismetMathLibrary::FindLookAtRotation(Actor->GetActorLocation(), Target->GetActorLocation());
							Actor->SetActorRotation(ActorBodyRotation);

							FRotator TargetBodyRotation = UKismetMathLibrary::FindLookAtRotation(Target->GetActorLocation(), Actor->GetActorLocation());
							Target->SetActorRotation(TargetBodyRotation);
						}

						bSomeoneToAid = false;
						PerformAid();
					}
				}
			}
		}
	}
}

void ARTSPlayerController::PerformAid()
{
	if (AidState == EAidState::Cleansing)
	{
		GetWorld()->GetTimerManager().SetTimer(CleansingTimerHandle, this, &ARTSPlayerController::Cleansing, 5.f, false);
	}
	if (AidState == EAidState::Healing)
	{
		GetWorld()->GetTimerManager().SetTimer(HealingTimerHandle, this, &ARTSPlayerController::Healing, 2.f, false);
	}
}

void ARTSPlayerController::Cleansing()
{
	if (Target)
	{
		Cast<ARTSprojCharacter>(Target)->StopBleeding();
		UE_LOG(LogTemp, Warning, TEXT("Target: %s cleansed"), *(Target->GetName()));
	}
	GetWorld()->GetTimerManager().ClearTimer(CleansingTimerHandle);
	Target = nullptr;
}

void ARTSPlayerController::Healing()
{
	if (Target)
	{
		Cast<ARTSprojCharacter>(Target)->AddHealth(20.f);
		UE_LOG(LogTemp, Warning, TEXT("Target: %s healed for 20.f. Health left: %f"), *(Target->GetName()), Cast<ARTSprojCharacter>(Target)->GetHealth());
	}
	GetWorld()->GetTimerManager().ClearTimer(HealingTimerHandle);
	Target = nullptr;
}