#include "PotentiaPCH.h"
#include "InputComponent.h"

// - Project includes -
#include "BaseCommands.h"
#include "InputManager.h"

InputComponent::~InputComponent()
{
    for(auto& [key , command] : m_Inputs)
        SafeDelete(command);
}

void InputComponent::Update(float) noexcept
{}

void InputComponent::AddInputs(const std::string& inputName , Command* const command)
{
    m_Inputs.emplace(inputName , command);
}

void InputComponent::UpdateInput()
{
    for(const auto& [key , command] : m_Inputs)
    {
        if(InputManager::GetInstance().IsPressed(key))
        {
            command->Execute(GetEntityOwner());
            break;
        }
    }
}