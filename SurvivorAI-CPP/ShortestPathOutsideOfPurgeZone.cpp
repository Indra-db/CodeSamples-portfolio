const bool IsLocationInPurgeZone(const PurgeZoneInfo& purgeZone , const Vector2& location)
{
    return (Square(purgeZone.Center.x - location.x) + Square(purgeZone.Center.y - location.y) < Square(purgeZone.Radius))
}

const Vector2 GetPointOnLine(const Vector2& startPoint , const Vector2& endPoint , float distance)
{
    const Vector2 unitVec = GetNormalized(startPoint - endPoint);
    return {startPoint - (distance * unitVec)};
}

BehaviorState SetShortestPathOutsidePurgeZone(Blackboard* pBlackboard)
{
    PurgeZoneInfo purgeZone{};
    const AgentInfo* pAgent{};
    AgentTargets* agentTargets{nullptr};
    const IExamInterface* pInterface{nullptr};

    bool dataAvailable =
        pBlackboard->GetData("Agent" , pAgent) &&
        pBlackboard->GetData("ActivePurgeZone" , purgeZone) &&
        pBlackboard->GetData("AgentTargets" , agentTargets) &&
        pBlackboard->GetData("Interface" , pInterface);

    if(!dataAvailable) return BehaviorState::Failure;

    //using fov range as it suited quite well
    const float distance = purgeZone.Radius + (pAgent->FOV_Range);

    //gets us a point fov-range outside of circle on the line going from the center of the purge zone to the player.
    Vector2 nextTarget = GetPointOnLine(purgeZone.Center , pAgent->Position , distance);

    //if the new target is not within world bounds
    if(!IsTargetInWorld(pInterface->World_GetInfo() , nextTarget))
    {
          //go to the other side
        nextTarget = GetPointOnLine(purgeZone.Center , pAgent->Position , -distance);
    }

    //next target where the agent will move to
    agentTargets->emplace_back(typeLocation::WAYPOINT , nextTarget);
    return BehaviorState::Success;
}