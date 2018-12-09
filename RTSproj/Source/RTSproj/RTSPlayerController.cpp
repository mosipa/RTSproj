// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSPlayerController.h"
#include "RTSHud.h"
#include "RTSprojCharacter.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"

ARTSPlayerController::ARTSPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ARTSPlayerController::BeginPlay()
{
	HUDPtr = Cast<ARTSHud>(GetHUD());

	bSomeoneToStab = false;
	Target = nullptr;
}

void ARTSPlayerController::Tick(float DeltaTime)
{
	if (bSomeoneToStab)
	{
		Knife();
	}
}

void ARTSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (!InputComponent) { return; }

	InputComponent->BindAction("Select", IE_Pressed, this, &ARTSPlayerController::Select);
	InputComponent->BindAction("Select", IE_Released, this, &ARTSPlayerController::FinishSelecting);
	InputComponent->BindAction("Move", IE_Pressed, this, &ARTSPlayerController::Move);
	InputComponent->BindAction("Knife", IE_Pressed, this, &ARTSPlayerController::Knife);
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
	if (!HUDPtr) { return; }

	if (HUDPtr->GetSelectedActors().Num() > 0)
	{
		TArray<ARTSprojCharacter*> SelectedActors = HUDPtr->GetSelectedActors();

		FHitResult Hit;

		if (GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, false, Hit))
		{
			//Check if target is character OR
			//We already checked that but still there's someone to stab
			if (Hit.GetActor()->GetClass()->IsChildOf<ARTSprojCharacter>() || bSomeoneToStab)
			{
				if (Hit.GetActor()->GetClass()->IsChildOf<ARTSprojCharacter>())
				{
					//Check if you hover over another character and player did press a button again (1st time passes fine)
					if (this->WasInputKeyJustPressed(FKey(FName("A"))))
					{
						Target = Hit.GetActor();
					}
				}
				
				for (auto Actor : SelectedActors)
				{
					if (Target && Actor->GetName().Equals(Target->GetName()))
					{
						continue; //TODO decide if we want to have friendly fire (if not put break)
					}
					else
					{
						if (!Target) { return; }

						UE_LOG(LogTemp, Warning, TEXT("Knife"));
						UAIBlueprintHelperLibrary::SimpleMoveToActor(Actor->GetController(), Target);

						FVector VectorLength = Target->GetActorLocation() - Actor->GetActorLocation();
						float Distance = FMath::Sqrt(FMath::Pow(VectorLength.X, 2) + FMath::Pow(VectorLength.Y, 2) + FMath::Pow(VectorLength.Z, 2));
						UE_LOG(LogTemp, Warning, TEXT("Getting closer: %f"), Distance);

						float Damage;
						
						float DotProd = Target->GetDotProductTo(Actor);
						
						if (DotProd <= 0)
						{
							UE_LOG(LogTemp, Warning, TEXT("Backstab"));
							Damage = 100.f;
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("Frontstab"));
							Damage = 50.f;
						}

						if(Distance <= 150.f)
						{
							UE_LOG(LogTemp, Warning, TEXT("Deal dmg"));
							bSomeoneToStab = false;

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