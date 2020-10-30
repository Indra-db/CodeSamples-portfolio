void SurvivorAI::CalculateWorldExplorationWaypoints()
{
    const WorldInfo& world = m_pInterface->World_GetInfo();
    const float startLocHor{m_Spacing * 0.5f};
    const float startLocVer{m_Spacing * 0.5f};
    const float wordDimXDiv2{world.Dimensions.x * 0.5f};
    const float wordDimYDiv2{world.Dimensions.y * 0.5f};
    float currPosY{wordDimYDiv2 - startLocVer};
    float currPosX{0.0f};
    const int amntGridCelsHor{static_cast<int>(world.Dimensions.x / m_Spacing)};
    const int amntGridCelsVer{static_cast<int>(world.Dimensions.y / m_Spacing)};
    bool didXPosChange{false};

    while(currPosY > -wordDimYDiv2)
    {
        currPosX = startLocHor;
        didXPosChange ? currPosX -= wordDimXDiv2 : currPosX = wordDimXDiv2 - m_Spacing;
        while(didXPosChange ? currPosX < wordDimXDiv2 : currPosX > -wordDimXDiv2)
        {
            m_AgentTargets.emplace_back(typeLocation::WAYPOINT , Vector2{currPosX,currPosY});
            didXPosChange ? currPosX += m_Spacing : currPosX -= m_Spacing;
        }

        //store the waypoints in reverse order every other time so the agent can traverse it easier
        didXPosChange = !didXPosChange;
        currPosY -= m_Spacing;
    }
}