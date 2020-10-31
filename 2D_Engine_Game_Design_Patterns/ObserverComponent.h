//enum found in the game project
enum class EventType
{
    DIG ,
    COLEMERALD ,
    COLGOLDBAG ,
    COLENEMY ,
    EMERALDSCORE ,
    PLAYERDEAD ,
    ENEMYDEAD ,
    LOST ,
    WON
};

struct EventArg
{
    EventType eventType;
    void* pArg;
};

#pragma once
// - Inheritance includes - 
#include "BaseComponent.h"
#include "EnumHelpers.h"

template<typename T>
class ObserverComponent abstract : public BaseComponent
{

public:

    ObserverComponent() = default;
    virtual ~ObserverComponent() = default;

    // ---- Copy/Move ----
    ObserverComponent(const ObserverComponent& other) = delete; //copy constructor
    ObserverComponent(ObserverComponent&& other) noexcept = delete; //move constructor
    ObserverComponent& operator=(const ObserverComponent& other) = delete; // copy assignment
    ObserverComponent& operator=(ObserverComponent&& other) noexcept = delete; //move assignment

    virtual void Initialize() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render() const = 0;

    virtual void OnNotify(Entity* const pEntity , T type) = 0;

};


// =============================================================================
//                               EXAMPLE
// =============================================================================

template<typename T>
void PlayerObserverComponent<T>::OnNotify(Entity* const pEntity , T event)
{
    TransformComponent* transComp{nullptr};
    float entityWidth{};
    float entityHeight{};
    switch(event.eventType)
    {
        case EventType::DIG:
            if((this)->GetEntityOwner()->HasComponent<PixelTextureComponent>())
            {
                transComp = pEntity->GetFirstComponentOfType<TransformComponent>();
                const glm::vec3& pos = transComp->GetPosition();
                entityWidth = transComp->GetWidth();
                entityHeight = transComp->GetHeight();
                (this)->GetEntityOwner()->GetFirstComponentOfType<PixelTextureComponent>()->SetPixelRange({pos.x, pos.y} , static_cast<int>(entityWidth) , static_cast<int>(entityHeight) , SDL_Color{0,0,0});
            }
            break;
        case EventType::EMERALDSCORE:
            if((this)->GetEntityOwner()->HasComponent<ScoreComponent>())
            {
                (this)->GetEntityOwner()->GetFirstComponentOfType<ScoreComponent>()->Increase(emeraldScoreAmount);
                (this)->GetEntityOwner()->GetFirstComponentOfType<ScoreComponent>()->NeedsUpdate(true);
                (this)->GetEntityOwner()->GetFirstComponentOfType<ScoreComponent>()->AddEmerald();
            }
            break;
            //continued
            //...
    }
}