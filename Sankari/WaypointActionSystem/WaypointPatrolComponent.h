#pragma once

#include "CoreMinimal.h"
#include "GameplayTagsModule.h"
#include "Components/ActorComponent.h"
#include "WaypointPatrolComponent.generated.h"

class AActionWaypoint;
class AWaypointPath;
class UBlackboardComponent;

UCLASS(ClassGroup = (WaypointActionSystem) , meta = (BlueprintSpawnableComponent))
class SANKARI_API UWaypointPatrolComponent final : public UActorComponent
{
    GENERATED_BODY()

public:

    // ---- Constructors ----

    UWaypointPatrolComponent();

    // ---- Functionality ----

    void BeginPlay() final;

    void TickComponent(float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction) final;

    // -- Getters --

    UFUNCTION(BlueprintPure , Category = "Waypoint Patrol Component")
    FORCEINLINE UBlackboardComponent* GetBlackboardComponent() const { return m_pBlackboardComponent; }

    UFUNCTION(BlueprintPure , Category = "Waypoint Patrol Component")
    int32 GetCurrentWaypointIndex() const { return m_CurrentWaypointIndex; }

    UFUNCTION(BlueprintPure , Category = "Waypoint Patrol Component")
    FORCEINLINE AActionWaypoint* GetCurrentWaypoint() const { return m_pCurrentWaypoint; }

    // -- Setters --

    UFUNCTION(BlueprintCallable , Category = "Waypoint Patrol Component")
    void SetNextWaypoint();

    // ---- Data members ----

    UPROPERTY(VisibleAnywhere , BlueprintReadOnly , Category = "Properties | Debug" , Meta = (DisplayName = "waypoint path"))
    AWaypointPath* m_pWaypointPath;

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Properties | Path" , Meta = (DisplayName = "waypoint path tag to match to"))
    FGameplayTag m_TagPathToAssign;

private:

    // -- Private Functions --

    bool CacheBlackboard();

    // ---- Data members ----

    UPROPERTY() // Cached blackboard component
    UBlackboardComponent* m_pBlackboardComponent { nullptr };

    UPROPERTY()
    AActionWaypoint* m_pCurrentWaypoint { nullptr };
    int32 m_CurrentWaypointIndex{0};

};
