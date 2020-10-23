#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Math/BoxSphereBounds.h>
#include "WaveSystem.generated.h"

class ASpawnPoint;
class ACharacterBase;
class UBoxComponent;

USTRUCT(BlueprintType)
struct FTypeAI
{
    GENERATED_BODY()

    FTypeAI() = default;

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Spawn" , Meta = (DisplayName = "how common"))
        int m_Commonality{0};

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Spawn" , Meta = (DisplayName = "Type Actors to spawn"))
        TSubclassOf<AActor> m_SpawnableActor;
};

UCLASS(ClassGroup = (SpawnSystem))
class SANKARI_API AWaveSystem final : public AActor
{
    GENERATED_BODY()

    public:

    // ---- Constructors ----
    AWaveSystem();

    // ---- Functionality ----

    UFUNCTION(BlueprintCallable , Category = "SpawnSystem")
    void AddSingleSpawn() { ++m_CurrentAmountOfSpawns; }

    UFUNCTION(BlueprintCallable , Category = "SpawnSystem")
    void RemoveSingleSpawn() { --m_CurrentAmountOfSpawns; }

    UFUNCTION(BlueprintCallable , Category = "SpawnSystem")
    void AddAmountCurrentlySpawned(int32 amount) { m_CurrentAmountOfSpawns += amount; }

    UFUNCTION(BlueprintCallable , Category = "SpawnSystem")
    void Spawn(int32 amount);

    UFUNCTION(BlueprintCallable , Category = "SpawnSystem")
    void GetSpawnPointsNearPlayer(TArray<ASpawnPoint*>& spawnpointsOUT);

    // -- Virtual Functions --

    virtual void BeginPlay() final;

    virtual void Tick(float DeltaTime) final;

    virtual void OnConstruction(const FTransform& Transform) override;

    // ---- Data members ----

    //How often in seconds to check for an interactable object. Set this to zero if you want to check every tick.
    UPROPERTY(EditDefaultsOnly , BlueprintReadWrite , Category = "Spawn" , meta = (ClampMin = "0.0" , UIMin = "0.0" , ClampMax = "100000.0" , UIMax = "100000.0" , DisplayName = "Spawning Frequency check"))
    float m_SpawningCheckFrequency = 2.0f;

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Spawn" , Meta = (DisplayName = "Should Spawn?"))
    bool m_bShouldSpawn{false};

    // this contains all spawn points for a given system
    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Spawn" , Meta = (DisplayName = "Assigned Waypoints" , MakeEditWidget))
    TArray<ASpawnPoint*> m_SpawnPoints;

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Spawn" , Meta = (DisplayName = "Box spawning Extend"))
    FVector m_BoxExtend;

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Spawn" , Meta = (DisplayName = "Box spawning Offset"))
    FVector m_BoxOffset;

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Spawn" , Meta = (DisplayName = "Actors to spawn and how common"))
    TArray<FTypeAI> m_ActorsToSpawn;

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Spawn" , Meta = (DisplayName = "Max amount of spawns"))
    int32 m_MaxAmountOfSpawnsTotal;

    // Pawn that overlaps with the waypoint. Visible for debug purposes. 
    UPROPERTY(VisibleAnywhere , Category = "Spawn | Debug " , Meta = (DisplayName = "Current amount of spawns"))
    int32 m_CurrentAmountOfSpawns;

    UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = "Spawn" , Meta = (DisplayName = "Max spawns per spawn"))
    int32 m_MaxAmountOfSpawnsASpawn { 20 };

    UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = "Spawn" , Meta = (DisplayName = "Max Spawnspoints per spawn"))
    int32 m_MaxAmountOfSpawnPointsASpawn { 10 };

private:

    // -- Private Functions --
    void SpawnIfPossible();
    void SchuffleSpawnPoints();
    // ---- Data members ----
    ACharacterBase* m_pPlayer{nullptr};
    FTimerHandle m_TimerHandleSpawning;
    float m_LastCheckedTime;
};
