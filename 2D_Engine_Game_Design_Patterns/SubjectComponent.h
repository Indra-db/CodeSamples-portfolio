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
#include "ObserverComponent.h"
#include <algorithm>

template<typename T>
class SubjectComponent final : public BaseComponent
{

public:

    SubjectComponent() = default;
    ~SubjectComponent() = default;

    // ---- Copy/Move ----
    SubjectComponent(const SubjectComponent& other) = default; //copy constructor
    SubjectComponent(SubjectComponent&& other) noexcept = default; //move constructor
    SubjectComponent& operator=(const SubjectComponent& other) = default; // copy assignment
    SubjectComponent& operator=(SubjectComponent&& other) noexcept = default; //move assignment

    void Initialize() {};
    void Update(float) {};
    void Render() const {};

    void Attach(ObserverComponent<T>* const pObserver);
    void Detach(ObserverComponent<T>* const pObserver);
    void Notify(Entity* pEntity , T event);
    void Clear();
private:

    std::vector<ObserverComponent<T>*> m_pObservers;

};

template<typename T>
void SubjectComponent<T>::Attach(ObserverComponent<T>* const pObserver)
{
    m_pObservers.push_back(pObserver);
}

template<typename T>
void SubjectComponent<T>::Detach(ObserverComponent<T>* const pObserver)
{
    m_pObservers.erase(std::remove(m_pObservers.begin() , m_pObservers.end() , pObserver) , m_pObservers.end());
}

template<typename T>
void SubjectComponent<T>::Clear()
{
    m_pObservers.clear();
}

template<typename T>
void SubjectComponent<T>::Notify(Entity* pEntity , T event)
{
    for(ObserverComponent<T>* const pObserver : m_pObservers)
    {
        pObserver->OnNotify(pEntity , event);
    }
}