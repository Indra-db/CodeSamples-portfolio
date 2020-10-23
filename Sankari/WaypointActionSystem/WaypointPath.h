#pragma once
#include "CoreMinimal.h"
#include "GameplayTagsModule.h"
#include "GameFramework/Actor.h"
#include "WaypointPath.generated.h"

class AActionWaypoint;

UCLASS(ClassGroup = (WaypointActionSystem))
class SANKARI_API AWaypointPath final: public AActor
{
    GENERATED_BODY()

public:

    // ---- Constructors ----

    AWaypointPath();

    // ---- Functionality ----

    void BeginPlay() final;

    void Tick(float DeltaTime) final;

    UFUNCTION(BlueprintPure , Category = "Waypoint Path")
    const TArray<AActionWaypoint*>& GetWayPoints() const { return m_pWayPoints; }

    // ---- Data members ----

    UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = "Waypoint" , Meta = (DisplayName = "waypoints"))
    TArray<AActionWaypoint*> m_pWayPoints;

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Properties" , Meta = (DisplayName = "tag waypoint path"))
    FGameplayTag m_WaypointPathTag;

};
