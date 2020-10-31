#pragma once
#include "BaseComponent.h"
#include "State.h"
#include <unordered_map>

class StateComponent final : public BaseComponent
{

public:

      // ---- Constructors ----
    StateComponent(const std::string& stateID , State* const pStartState);

    // ---- Destructor ----
    ~StateComponent();

    // ---- Copy/Move ----
    StateComponent(const StateComponent& other) = delete; //copy constructor
    StateComponent(StateComponent&& other) noexcept = delete; //move constructor
    StateComponent& operator=(const StateComponent& other) = delete; // copy assignment
    StateComponent& operator=(StateComponent&& other) noexcept = delete; //move assignment

    // -- Virtual Functions --
    void Initialize() noexcept override {};
    void Update(float deltaTime) noexcept override;
    void Render() const noexcept override {};
    void AddState(const std::string& stateID , State* const pState);

    // -- Getters --
    const State& const GetState() const noexcept;

    // -- Setters --
    void SetState(const std::string& stateID) noexcept;

private:

      // ---- Data members ----
    State* m_CurrentState;
    std::unordered_map<std::string , State*> m_pStates;
};

// =============================================================================
//                               Inline Definitions
// =============================================================================

// -- Getters --
inline const State& StateComponent::GetState() const noexcept
{
    return *m_CurrentState;
}

// -- Setters --
inline void StateComponent::SetState(const std::string& stateID) noexcept
{
    m_CurrentState = m_pStates[stateID];
    m_CurrentState->OnEnter(GetEntityOwner());
}