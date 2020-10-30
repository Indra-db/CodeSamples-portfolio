const Vector2 GetPointOnLine(const Vector2& startPoint , const Vector2& endPoint , float distance)
{
    const Vector2 unitVec = GetNormalized(startPoint - endPoint);
    return {startPoint - (distance * unitVec)};
}

const Vector2 GetMidpoint(const Vector2& startPoint , const Vector2& endPoint)
{
    return Vector2{((startPoint.x + endPoint.x) / 2),((startPoint.y + endPoint.y) / 2)};
}

const std::optional<std::tuple<Vector2 , Vector2>> FindLineCircleIntersections(const PurgeZoneInfo& purgeZone , const Vector2& point1 , const Vector2& point2)
{

    const float A = dx * dx + dy * dy;
    const float B = 2 * (dx * (point1.x - purgeZone.Center.x) + dy * (point1.y - purgeZone.Center.y));
    const float C = (point1.x - purgeZone.Center.x) * (point1.x - purgeZone.Center.x) + (point1.y - purgeZone.Center.y) * (point1.y - purgeZone.Center.y) - purgeZone.Radius * purgeZone.Radius;

    const float discriminant = B * B - 4 * A * C;

    if(discriminant > 0)
    {
          // Two solutions.
        float t = static_cast<float>(((-B + sqrt(discriminant)) / (2 * A)));
        const Vector2 intersection1{point1.x + t * dx , point1.y + t * dy};
        t = static_cast<float>(((-B - sqrt(discriminant)) / (2 * A)));
        const Vector2 intersection2{point1.x + t * dx , point1.y + t * dy};
        return std::make_optional<std::tuple<const Vector2 , const Vector2>>(std::make_tuple(intersection1 , intersection2));
    }
    return std::nullopt;
}

BehaviorState CollisionAvoidPurgeZone(Blackboard* pBlackboard)
{
    PurgeZoneInfo purgeZone{};
    AgentTargets* agentTargets{nullptr};
    std::tuple< Vector2 , Vector2>* pIntersectionPoints{};

    bool dataAvailable =
        pBlackboard->GetData("ActivePurgeZone" , purgeZone) &&
        pBlackboard->GetData("AgentTargets" , agentTargets) &&
        pBlackboard->GetData("IntersectionPoints" , intersectionPoints);

    if(!dataAvailable) { return BehaviorState::Failure; }

    const int offset{8};
    // has to be auto : https://dominikberner.ch/structured-bindings/
    auto&& [intersPoint1 , intersPoint2] = *intersectionPoints;
    const Vector2 midPoint = GetMidpoint(intersPoint1 , intersPoint2);
    const float distance = purgeZone.Radius + offset;
    const Vector2 nextTarget = GetPointOnLine(purgeZone.Center , midPoint , distance);
    agentTargets->emplace_back(typeLocation::WAYPOINT , nextTarget);

    #ifdef DEBUGRENDERING
    pBlackboard->ChangeData("PurgeZoneNewTarget" , nextTarget);
    pBlackboard->ChangeData("PurgeZoneIntersMidPoint" , midPoint);
    #endif // DEBUGRENDERING

    return BehaviorState::Success;
}

// used to check if the intersection points are between the AI and the target location bounding box. 
// otherwise it could give wrong results as lines go on infinite 
// and the world 'loops' when going out of the bounding of the world
bool AreThere2IntersectionPoints(Blackboard* pBlackboard) //called in behavior tree
{
    AgentTargets* agentTargets{nullptr};
    ISteeringBehavior* pCurrentBehavior = nullptr;
    AgentInfo* pAgent;
    PurgeZoneInfo purgeZone{};

    bool dataAvailable =
        pBlackboard->GetData("CurrentBehavior" , pCurrentBehavior) &&
        pBlackboard->GetData("Agent" , pAgent) &&
        pBlackboard->GetData("ActivePurgeZone" , purgeZone) &&
        pBlackboard->GetData("AgentTargets" , agentTargets);


    if(!dataAvailable) { return false; }

    const Elite::Vector2& target = agentTargets->back().location;

    const std::optional<std::tuple< Vector2 , Vector2>> intersectionPoints = FindLineCircleIntersections(purgeZone , pAgent->Position , target);
    if(intersectionPoints)
    {
        const std::tuple< Vector2 , Vector2>& interPoints = intersectionPoints.value();
        // has to be auto : https://dominikberner.ch/structured-bindings/
        auto&& [intersPoint1 , intersPoint2] = interPoints;
        const bool intersectPointsOnLine = target.y > pAgent->Position.y ?
            AreIntersectionPointsOnLine(target , pAgent->Position , intersPoint1 , intersPoint2) :
            AreIntersectionPointsOnLine(pAgent->Position , target , intersPoint1 , intersPoint2);

        if(intersectPointsOnLine)
        {
            pBlackboard->ChangeData("IntersectionPoints" , interPoints);
            #ifdef DEBUGRENDERING
            pBlackboard->ChangeData("AreThere2IntersPoints" , true);
            #endif // DEBUGRENDERING
            return true;
        }
    }
    #ifdef DEBUGRENDERING
    pBlackboard->ChangeData("AreThere2IntersPoints" , false);
    #endif // DEBUGRENDERING
    return false;
}