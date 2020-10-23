#include "WaveSystem.h"
#include <Kismet/GameplayStatics.h>
#include "../CharacterBase.h"
#include <Containers/Map.h>
#include <Engine/World.h>
#include "SpawnPoint.h"
#include "Components/BoxComponent.h"
#include <DrawDebugHelpers.h>
#include <Kismet/KismetMathLibrary.h>

AWaveSystem::AWaveSystem()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AWaveSystem::BeginPlay()
{
    Super::BeginPlay();

    //get the player character for reference (single player support only)
    #if !UE_BUILD_SHIPPING
    m_pPlayer = Cast<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorld() , 0));
    #elif
        //production checked for guaranteed and tested
    m_pPlayer = CastChecked<ACharacterBase>(UGameplayStatics::GetPlayerCharacter(GetWorld() , 0));
    #endif

}

void AWaveSystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    SpawnIfPossible();
}

void AWaveSystem::SpawnIfPossible()
{
    if(!m_bShouldSpawn)
    {
        return;
    }
    if(m_CurrentAmountOfSpawns == m_MaxAmountOfSpawnsTotal)
    {
        return;
    }
    if(GetWorld()->TimeSince(m_LastCheckedTime) > m_SpawningCheckFrequency)
    {
        int32 amountOfSpawns = (m_MaxAmountOfSpawnsTotal - m_CurrentAmountOfSpawns);
        if(amountOfSpawns > m_MaxAmountOfSpawnsASpawn)
        {
            Spawn(m_MaxAmountOfSpawnsASpawn);
        }
        else
        {
            Spawn(amountOfSpawns);
        }
    }

}

void AWaveSystem::GetSpawnPointsNearPlayer(TArray<ASpawnPoint*>& spawnpointsOUT)
{

    if(m_SpawnPoints.Num() == 0)
    {
        UE_LOG(LogTemp , Error , TEXT(__FUNCSIG__ " - No spawnpoint !"));
        return;
    }

    spawnpointsOUT.Reserve(m_MaxAmountOfSpawnPointsASpawn);
    int32 amountSpawnPoints{0};

    SchuffleSpawnPoints();
    for(ASpawnPoint* const spawnPoint : m_SpawnPoints)
    {
        if(UKismetMathLibrary::IsPointInBox(spawnPoint->GetActorLocation() , m_pPlayer->GetActorLocation() + m_BoxOffset , m_BoxExtend))
        {
            spawnpointsOUT.Push(spawnPoint);
            ++amountSpawnPoints;
            if(m_MaxAmountOfSpawnPointsASpawn == amountSpawnPoints)
            {
                break;
            }
        }
    }
}

void AWaveSystem::Spawn(int32 amount)
{

    #if !defined UE_BUILD_SHIPPING
    if(m_ActorsToSpawn.Num() == 0)
    {
        m_LastCheckedTime = GetWorld()->GetTimeSeconds();
        UE_LOG(LogTemp , Error , TEXT(__FUNCSIG__ " - No actors to spawn!"));
    }
    #endif

    int32 sumCommon{0};

    //Find the total of the commonality
    for(const FTypeAI& spawnableActor : m_ActorsToSpawn)
    {
        sumCommon += spawnableActor.m_Commonality;
    }

    int32 randValue{0};
    int32 sumCheck{0};

    TArray<ASpawnPoint*> spawnPoints{};
    GetSpawnPointsNearPlayer(spawnPoints);

    int32 amountSpawnPoints = spawnPoints.Num();

    if(amountSpawnPoints == 0)
    {
        m_LastCheckedTime = GetWorld()->GetTimeSeconds();

        #if !defined(UE_BUILD_SHIPPING)
        UE_LOG(LogTemp , Warning , TEXT(__FUNCSIG__ " - can't spawn, no nearby spawn points"));
        #endif

        return;
    }

    for(int32 i = 0; i < amount; i++)
    {
        randValue = FMath::RandRange(0 , sumCommon);
        sumCheck = 0;

        for(const FTypeAI& spawnableActor : m_ActorsToSpawn)
        {
            sumCheck += spawnableActor.m_Commonality;

            if(randValue < sumCheck)
            {
                ASpawnPoint* pSpawnpoint = spawnPoints[FMath::RandRange(0 , amountSpawnPoints - 1)];
                const FVector& location = pSpawnpoint->m_SpawnPoints[FMath::RandRange(0 , pSpawnpoint->m_AmountSPawnPoints - 1)] + pSpawnpoint->GetActorLocation();
                FRotator rotation(0.0f , 0.0f , 0.0f);
                FActorSpawnParameters spawnInfo;

                #if !defined(UE_BUILD_SHIPPING)
                UE_LOG(LogTemp , Warning , TEXT(__FUNCSIG__ " - Spawning actor"));
                #endif

                //spawnableActor.m_SpawnableActor
                if(GetWorld()->SpawnActor<AActor>(spawnableActor.m_SpawnableActor , location , rotation , spawnInfo))
                {
                    m_CurrentAmountOfSpawns++;
                }
                m_LastCheckedTime = GetWorld()->GetTimeSeconds();
                break;
            }
        }
    }
}

void AWaveSystem::OnConstruction(const FTransform& Transform)
{
    if(GetWorld() == nullptr)
    {
        return;
    }

    UKismetSystemLibrary::FlushPersistentDebugLines(this->GetWorld());

    DrawDebugBox(GetWorld() , Transform.GetLocation() + m_BoxOffset , m_BoxExtend , Transform.GetRotation() , FColor::Purple , true , -1 , 0 , 10);
}

void AWaveSystem::SchuffleSpawnPoints()
{
    int32 amountSpawnPoints = m_SpawnPoints.Num();
    if(amountSpawnPoints > 0)
    {
        int32 LastIndex = amountSpawnPoints - 1;
        for(int32 i = 0; i <= LastIndex; ++i)
        {
            int32 Index = FMath::RandRange(i , LastIndex);
            if(i != Index)
            {
                m_SpawnPoints.Swap(i , Index);
            }
        }
    }
}
