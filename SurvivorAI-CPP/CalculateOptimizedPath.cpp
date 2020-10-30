/// @brief Find the shortest location between houses, get the left over houses, insert them between the 2 most suitable houses.
void SurvivorAI::CalculateOptimizedPath()
{
    m_pBehaviorTree->GetBlackboard()->ChangeData("CurrentIndexStoredHouses" , static_cast<size_t>(0));

    House* pClosestHouse{nullptr};
    House* newestHouse{};
    HouseTargets optimizedPathHouses{};
    size_t amountStoredHouses{m_StoredHouses.size()};

    optimizedPathHouses.reserve(amountStoredHouses);

    float shortestDistanceNextHouse{(numeric_limits<float>::max)()};
    float distance{0.0f};
    float shortestDistanceToFirstHouse{(numeric_limits<float>::max)()};
    const int maxSpacingBetweenHouses{Square(static_cast<int>(2 * m_Spacing))};
    int ClosestHouseIndex{0};

    for(size_t i = 0; i < m_StoredHouses.size(); i++)
    {
        distance = DistanceSquared(m_pInterface->Agent_GetInfo().Position , m_StoredHouses[i].info.Center);
        if(distance < shortestDistanceToFirstHouse)
        {
            ClosestHouseIndex = i;
            shortestDistanceToFirstHouse = distance;
        }
    }
    optimizedPathHouses.push_back(std::move(m_StoredHouses[ClosestHouseIndex]));

    while(optimizedPathHouses.size() != amountStoredHouses)
    {
        shortestDistanceNextHouse = (numeric_limits<float>::max)();
        newestHouse = &optimizedPathHouses[optimizedPathHouses.size() - 1];
        for(House& storedHouse : m_StoredHouses) //not const due to std::move
        {
            if(storedHouse.info.Size.x != -1)
            {
                distance = DistanceSquared(storedHouse.info.Center , (*newestHouse).info.Center);
                if(distance < shortestDistanceNextHouse && distance < maxSpacingBetweenHouses)
                {
                    pClosestHouse = &storedHouse;
                    shortestDistanceNextHouse = distance;
                }
            }
        }
        if(pClosestHouse)
        {
            optimizedPathHouses.push_back(std::move(*pClosestHouse));
            pClosestHouse = nullptr;
        }
        else
        {
            --amountStoredHouses;
        }
    }

    amountStoredHouses = m_StoredHouses.size();
    size_t optimalHouseIndex{0};

    while(optimizedPathHouses.size() != amountStoredHouses)
    {
        shortestDistanceNextHouse = (numeric_limits<float>::max)();
        for(House& storedHouse : m_StoredHouses) //not const due to std::move
        {
            shortestDistanceNextHouse = (numeric_limits<float>::max)();
            if(storedHouse.info.Size.x != -1)
            {
                for(size_t i = 0; i < optimizedPathHouses.size(); i++)
                {
                    distance = DistanceSquared(storedHouse.info.Center , optimizedPathHouses[i].info.Center);
                    if(distance < shortestDistanceNextHouse)
                    {
                        optimalHouseIndex = i;
                        pClosestHouse = &storedHouse;
                        shortestDistanceNextHouse = distance;
                    }
                }
                if(optimalHouseIndex == 0 || (optimalHouseIndex == optimizedPathHouses.size() - 1))
                {
                    if(pClosestHouse)
                    {
                        optimizedPathHouses.push_back(*pClosestHouse);
                    }
                }
                else
                {
                    distance = DistanceSquared(storedHouse.info.Center , optimizedPathHouses[optimalHouseIndex - 1].info.Center);
                    if(distance < DistanceSquared(storedHouse.info.Center , optimizedPathHouses[optimalHouseIndex + 1].info.Center))
                    {
                        optimizedPathHouses.insert((optimizedPathHouses.begin() + (optimalHouseIndex)) , std::move(*pClosestHouse));
                    }
                }
            }
        }
    }
    m_StoredHouses = std::move(optimizedPathHouses);
}