#pragma once
// - Inheritance includes - 
#include "BaseComponent.h"

// - Project includes -
#include "EnumHelpers.h"

// - Standard includes -
#include <unordered_map>

// - Forward declarations -
class Command;

class InputComponent final : public BaseComponent
{
public:
      // ---- Constructors ----
    InputComponent() = default;

    // ---- Destructor ----
    ~InputComponent();

    // ---- Copy/Move ----
    InputComponent(const InputComponent& other) = delete; //copy constructor
    InputComponent(InputComponent&& other) noexcept = delete; //move constructor
    InputComponent& operator=(const InputComponent& other) = delete; // copy assignment
    InputComponent& operator=(InputComponent&& other) noexcept = delete; //move assignment

    // -- Virtual Functions --
    void Initialize() noexcept override {};
    void Update(float deltatime) noexcept override;
    void Render() const noexcept override {};

    void UpdateInput();
    void AddInputs(const std::string& inputName , Command* command);
private:
    std::unordered_map<std::string , Command*> m_Inputs;
};