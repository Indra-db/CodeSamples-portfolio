#include "WaypointPath.h"

AWaypointPath::AWaypointPath()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AWaypointPath::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AWaypointPath::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

