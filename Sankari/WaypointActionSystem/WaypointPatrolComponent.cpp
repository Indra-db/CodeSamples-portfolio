#include "WaypointPatrolComponent.h"
#include "WaypointPath.h"
#include "BaseWaypoint.h"
#include <BehaviorTree/BlackboardComponent.h>
#include <GameFramework/Controller.h>
#include <GameFramework/Pawn.h>
#include <Kismet/GameplayStatics.h>

// ---- Constructors ----

UWaypointPatrolComponent::UWaypointPatrolComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

// ---- Functionality ----

void UWaypointPatrolComponent::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> foundPaths;
    UGameplayStatics::GetAllActorsOfClass(GetWorld() , AWaypointPath::StaticClass() , foundPaths);

    for(AActor* pPath : foundPaths)
    {
        #if !UE_BUILD_SHIPPING
        AWaypointPath* const pWaypointPath = Cast<AWaypointPath>(pPath);
        #elif
        AWaypointPath* const pWaypointPath = CastChecked<AWaypointPath>(pPath);
        #endif
        if(pWaypointPath->m_WaypointPathTag.MatchesTag(m_TagPathToAssign))
        {
            m_pWaypointPath = pWaypointPath;
            if(m_pWaypointPath->GetWayPoints().IsValidIndex(0))
            {
                m_pCurrentWaypoint = m_pWaypointPath->GetWayPoints()[0];
            }
            else
            {
                UE_LOG(LogTemp , Warning , TEXT(__FUNCSIG__ " - no valid index waypoint"));
            }
        }
    }
}

void UWaypointPatrolComponent::TickComponent(float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime , TickType , ThisTickFunction);
}

// -- Setters --
void UWaypointPatrolComponent::SetNextWaypoint()
{
    UE_LOG(LogTemp , Warning , TEXT(__FUNCSIG__ " - Setting next way point"));
    const auto& pWaypoints = m_pWaypointPath->GetWayPoints();
    if(m_CurrentWaypointIndex < pWaypoints.Num() - 1)
    {
        m_pCurrentWaypoint = pWaypoints[m_CurrentWaypointIndex];
        ++m_CurrentWaypointIndex;
    }
    else
    {
        m_CurrentWaypointIndex = 0;
        m_pCurrentWaypoint = pWaypoints[m_CurrentWaypointIndex];
    }
}

// -- Private Functions --
bool UWaypointPatrolComponent::CacheBlackboard()
{
   // Only try to get the blackboard it is not already cached
    if(m_pBlackboardComponent != nullptr)
    {
        return true;
    }

    if(GetOwner() == nullptr)
    {
        return false;
    }
    APawn* cachedOwner = ExactCast<APawn>(GetOwner());

    if(cachedOwner == nullptr || cachedOwner->GetController() == nullptr)
    {
        return false;
    }

    AController* cachedAIController = ExactCast<AController>(cachedOwner->GetController());

    if(cachedAIController == nullptr)
    {
        return false;
    }

    m_pBlackboardComponent = cachedAIController->FindComponentByClass<UBlackboardComponent>();

    if(m_pBlackboardComponent == nullptr)
    {
        m_pBlackboardComponent = cachedOwner->FindComponentByClass<UBlackboardComponent>();
    }

    return m_pBlackboardComponent != nullptr;
}
