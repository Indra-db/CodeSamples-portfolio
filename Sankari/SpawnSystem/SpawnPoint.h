// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnPoint.generated.h"

UCLASS(ClassGroup = (SpawnSystem))
class SANKARI_API ASpawnPoint final : public AActor
{
    GENERATED_BODY()

public:

    // ---- Constructors ----

    ASpawnPoint();

    // ---- Functionality ----
    // -- Virtual Functions --
    void Tick(float DeltaTime) final;
    void BeginPlay() final;

    // ---- Data members ----
    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "SpawnPoints" , Meta = (DisplayName = "SpawnLocations" , MakeEditWidget))
    TArray<FVector> m_SpawnPoints;

    UPROPERTY(VisibleAnywhere , BlueprintReadOnly , Category = "SpawnPoints" , Meta = (DisplayName = "Spawn Locations Size" , MakeEditWidget))
    int32 m_AmountSPawnPoints { 0 };

};