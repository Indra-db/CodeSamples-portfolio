// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnPoint.h"
#include "NavigationSystem.h"

// ---- Constructors ----
ASpawnPoint::ASpawnPoint()
{
    PrimaryActorTick.bCanEverTick = false;
}

// ---- Functionality ----
// -- Virtual Functions --
void ASpawnPoint::BeginPlay()
{
    Super::BeginPlay();

    m_AmountSPawnPoints = m_SpawnPoints.Num();
}

void ASpawnPoint::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

