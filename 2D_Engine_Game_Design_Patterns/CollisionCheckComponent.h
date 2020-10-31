// =============================================================================
//                          CODE THAT BELONGS WITH THE COMPONENT
// =============================================================================

//source : Effective Modern C++ by Scott Meyers.
template <typename E>
constexpr auto to_underlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}
//cool alternative in C++20 might be inline constexpr std::vector<int> , each vector representing a state of X object

//code from this source, modified it to constexpr
//https://stackoverflow.com/questions/1448396/how-to-use-enums-as-flags-in-c/58068168#58068168
/// @brief a define to make bit operations possible between enums
#define ENUM_FLAG_OPERATORS(T)  \
    inline T operator~ (T a) { return static_cast<T>( ~to_underlying(a) ); }   \
    constexpr inline T operator| (T a, T b) { return static_cast<T>( to_underlying(a) | to_underlying(b) ); }   \
    inline T operator& (T a, T b) { return static_cast<T>( to_underlying(a) & to_underlying(b) ); }   \
    inline T operator^ (T a, T b) { return static_cast<T>( to_underlying(a) ^ to_underlying(b) ); }   \
    inline T& operator|= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) |= to_underlying(b) ); }   \
    inline T& operator&= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) &= to_underlying(b) ); }   \
    inline T& operator^= (T& a, T b) { return reinterpret_cast<T&>( reinterpret_cast<std::underlying_type<T>::type&>(a) ^= to_underlying(b) ); }


/// @brief all the collisions that will be used for the game BubbleBobble
enum class CollsionMask
{
    PLAYER = 1 ,
    ENEMY = 2 ,
    PLAYERPROJECTILE = 4 ,
    GOLDBAG = 8 ,
    GOLDLOOT = 16 ,
    EMERALD = 32
};
ENUM_FLAG_OPERATORS(CollsionMask);

/// @brief the possible collisions that can happen between entities in Bubble bobble
enum class CollisionType
{
    NO_COLLISION = 0 ,
    //player enemy collision
    PLAYER_ENEMY_COLLISION = (int) CollsionMask::PLAYER | (int) CollsionMask::ENEMY ,
    //projectiles
    ENEMY_PROJECTILE_COLLISION = (int) CollsionMask::ENEMY | (int) CollsionMask::PLAYERPROJECTILE ,
    //loot
    PLAYER_GOLDBAG_COLLISION = (int) CollsionMask::PLAYER | (int) CollsionMask::GOLDBAG ,
    PLAYER_GOLDLOOT_COLLISION = (int) CollsionMask::PLAYER | (int) CollsionMask::GOLDLOOT ,
    PLAYER_EMERALD_COLLISION = (int) CollsionMask::PLAYER | (int) CollsionMask::EMERALD ,
};

// =============================================================================
//                               HEADER
// =============================================================================

#pragma once
// - Inheritance includes - 
#include "BaseComponent.h"

// - Standard includes -
#include <unordered_map>

// - Inheritance includes -
enum class CollisionType;
class RectCollisionComponent;
class Command;

/// @brief An entity who holds this component checks the collision with other entities and if they match a specific collisionType, perform an action
class CollisionCheckComponent final : public BaseComponent
{

public:

      // ---- Constructors ----
    CollisionCheckComponent() = default;

    // ---- Destructor ----
    virtual ~CollisionCheckComponent();

    // ---- Copy/Move ----
    CollisionCheckComponent(const CollisionCheckComponent& other) = default; //copy constructor
    CollisionCheckComponent(CollisionCheckComponent&& other) noexcept = default; //move constructor
    CollisionCheckComponent& operator=(const CollisionCheckComponent& other) = default; // copy assignment
    CollisionCheckComponent& operator=(CollisionCheckComponent&& other) noexcept = default; //move assignment

    // ---- Functionality ----
    void AddCollisionAction(CollisionType type , Command* const newCommand) noexcept;
    void RemoveCollisionAction(CollisionType button) noexcept;

    // -- Virtual Functions --
    void Initialize() noexcept override;
    void Update(float) noexcept override;
    void Render() const noexcept override {};

    // -- Setters --
    /// @attention call this function when a collision component gets removed from an entity
    void SetShouldUpdateColBoxesTrue() { m_ShouldUpdateCollisionBoxes = true; }

private:
      // -- Private Functions --
    void CheckCollisionWithOtherCollisionComp() noexcept;

    // ---- Data members ----
    std::vector<BaseComponent*> m_OwnerCollisionBoxes{};
    std::unordered_map<CollisionType , Command*> m_CollisionActions{};

    bool m_IsActive{true};
    bool m_ShouldUpdateCollisionBoxes{false};

};