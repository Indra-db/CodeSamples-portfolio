#include "PotentiaPCH.h"

// - Project includes -
#include "IncludeComponents.h"
#include "MathFunctions.h"
#include "BaseCommands.h"
#include "EnumHelpers.h"
#include "SceneManager.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Scene.h"

// ---- Destructor ----
CollisionCheckComponent::~CollisionCheckComponent()
{
    for(auto& [key , command] : m_CollisionActions)
    {
        SafeDelete(command);
    }
    m_CollisionActions.clear();
}

// ---- Functionality ----
void CollisionCheckComponent::AddCollisionAction(CollisionType type , Command* const newCommand) noexcept
{
    for(auto& [key , command] : m_CollisionActions)
    {
        if(command == newCommand)
        {
            return;
        }
    }
    m_CollisionActions[type] = newCommand;
}
void CollisionCheckComponent::RemoveCollisionAction(CollisionType type) noexcept
{
    Command* deleteCommand = m_CollisionActions[type];
    SafeDelete(deleteCommand);
    m_CollisionActions.erase(type);
}

// -- Virtual Functions --
void CollisionCheckComponent::Initialize() noexcept
{
    m_OwnerCollisionBoxes = GetEntityOwner()->GetAllComponentOfType<RectCollisionComponent>();
}

void CollisionCheckComponent::Update(float) noexcept
{
    if(m_IsActive)
    {
        CheckCollisionWithOtherCollisionComp();
    }
}

/// @brief Checks if it has collided with others and if there are events/commands/actions for it
/// @detail goes over all the collision boxes of the owner of this component, compares them to another entity it's collision boxes that are active
/// @detail while checking if there is an action/command/event responds to specific collisions between 2 collision types
/// @detail this way we can precisely change the behavior between entities, and prevent them from colliding with each other 
/// @detail (for e.g player collides with enemy, but enemy does not collide with enemy), all the collision between those two are handles just by one entity (having this component), and not both
void CollisionCheckComponent::CheckCollisionWithOtherCollisionComp() noexcept
{
    if(m_ShouldUpdateCollisionBoxes)
    {
        m_OwnerCollisionBoxes = GetEntityOwner()->GetAllComponentOfType<RectCollisionComponent>();
    }

    const auto& EntitiesWithCollision = SceneManager::GetInstance().GetActiveScene()->GetEntityManager()->GetEntities();
    for(auto& collisionBoxOwner : m_OwnerCollisionBoxes)
    {
        for(auto& entityCol : EntitiesWithCollision)
        {
            auto& collChecks = entityCol->GetAllComponentOfType<RectCollisionComponent>();

            for(auto& collCheck : collChecks)
            {
                                          //if not active, continue
                if(!(static_cast<RectCollisionComponent*>(collCheck)->GetIsActive()))
                {
                    continue;
                }
                                        //if not overlapping, continue
                if(!(PotentiaMath::CheckRectangleOverlap(static_cast<RectCollisionComponent*>
                    (collisionBoxOwner)->GetCollidorBox() , static_cast<RectCollisionComponent*>(collCheck)->GetCollidorBox())))
                {
                    continue;
                }

                for(auto& [collisionType , command] : m_CollisionActions)
                {
                    CollisionType collisionMask = static_cast<CollisionType>(static_cast<RectCollisionComponent*>
                        (collisionBoxOwner)->GetTag() | static_cast<RectCollisionComponent*>(collCheck)->GetTag());
                                                //if there is a collision mask, execute
                    if(collisionType == collisionMask)
                    {
                        command->Execute(entityCol);
                        return;
                    }
                }

            }
        }
    }
}