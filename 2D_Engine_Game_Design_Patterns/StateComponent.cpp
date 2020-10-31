#include "PotentiaPCH.h"
#include "StateComponent.h"

StateComponent::StateComponent(const std::string& stateID , State* const pStartState)
    : m_CurrentState{pStartState}
{
    m_pStates.emplace(stateID , pStartState);
}

StateComponent::~StateComponent()
{
    for(auto& [key , pState] : m_pStates)
    {
        SafeDelete(pState);
    }
    m_pStates.clear();
}

void StateComponent::Update(float deltaTime) noexcept
{
    m_CurrentState->Update(deltaTime);
}

void StateComponent::AddState(const std::string& stateID , State* const pState)
{
    m_pStates.emplace(stateID , pState);
}
