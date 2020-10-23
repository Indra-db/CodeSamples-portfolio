#include "InteractorComponent.h"
#include "InteractionComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

// ---- Constructors ----
UInteractorComponent::UInteractorComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

// ---- Functionality ----

void UInteractorComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UInteractorComponent::TickComponent(float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime , TickType , ThisTickFunction);

    if(GetWorld()->TimeSince(m_InteractionData.m_LastInteractionCheckTime) > m_InteractionCheckFrequency)
    {
        PerformInteractionCheck();
    }
}

void UInteractorComponent::BeginInteract()
{
      //hold interact key down
    m_InteractionData.m_bInteractHeld = true;

    //are we looking at an interactable

    if(UInteractionComponent* pInteractable = GetInteractable())
    {
          //begin interacting (does not interact)
        pInteractable->BeginInteract(this);

        if(FMath::IsNearlyZero(pInteractable->m_InteractionTime))
        {
            Interact(); //do the interaction
        }
        else
        {
              //if there is an interaction timer, set a timer, and queue it up to do it soon
            GetWorld()->GetTimerManager().SetTimer(m_TimerHandle_Interact , this , &UInteractorComponent::Interact , pInteractable->m_InteractionTime , false);
        }
    }

}

void UInteractorComponent::EndInteract()
{
      //when the e key is released
    m_InteractionData.m_bInteractHeld = false;

    GetWorld()->GetTimerManager().ClearTimer(m_TimerHandle_Interact);

    if(UInteractionComponent* pInteractable = GetInteractable())
    {
          //end interacting (does not interact)
        pInteractable->EndInteract(this);
    }
}

// -- Getters --

bool UInteractorComponent::IsInteracting() const
{
    return GetWorld()->GetTimerManager().IsTimerActive(m_TimerHandle_Interact);
}

float UInteractorComponent::GetRemainingInteractTime() const
{
    return GetWorld()->GetTimerManager().GetTimerRemaining(m_TimerHandle_Interact);
}

// -- Private Functions --

// - Interactions -

void UInteractorComponent::Interact()
{
    GetWorld()->GetTimerManager().ClearTimer(m_TimerHandle_Interact);

    if(UInteractionComponent* pInteractable = GetInteractable())
    {
        pInteractable->Interact(this);
    }
}

void UInteractorComponent::PerformInteractionCheck()
{
    if(GetOwner()->GetInstigatorController() == nullptr)
    {
        return;
    }

    m_InteractionData.m_LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

    FVector eyesLoc;
    FRotator eyesRot;
    GetOwner()->GetInstigatorController()->GetPlayerViewPoint(eyesLoc , eyesRot); //get rotation and location of player camera

    FVector traceStart = eyesLoc;
    FVector traceEnd = (eyesRot.Vector() * m_InteractionCheckDistance) + traceStart;
    FHitResult traceHit; //figure out where the camera is in some point in the distance

    FCollisionQueryParams queryParams;
    queryParams.AddIgnoredActor(GetOwner()); //ignore player

    //line trace , cast a line, could not find interactable
    if(!(GetWorld()->LineTraceSingleByChannel(traceHit , traceStart , traceEnd , ECC_Visibility , queryParams)))
    {
        CouldntFindInteractable();
        return;
    }

    //check if the actor is valid, if not return
    if(!(traceHit.GetActor()))
    {
        CouldntFindInteractable();
        return;
    }

    #if !UE_BUILD_SHIPPING
    UInteractionComponent* pInteractionComponent = Cast<UInteractionComponent>(traceHit.GetActor()->GetComponentByClass(UInteractionComponent::StaticClass()));
    #elif
    UInteractionComponent* pInteractionComponent = CastChecked<UInteractionComponent>(traceHit.GetActor()->GetComponentByClass(UInteractionComponent::StaticClass()));
    #endif

    //if there is no interactable component
    if(pInteractionComponent == nullptr)
    {
        CouldntFindInteractable();
        return;
    }

    //figure how far we are
    float distance = (traceStart - traceHit.ImpactPoint).Size();

    //check if we can interact with it and close enough
    if(pInteractionComponent != GetInteractable() && distance <= pInteractionComponent->m_InteractionDistance)
    {
        FoundNewInteractable(pInteractionComponent);
    }
    //if too far away and valid interactable
    else if(distance > pInteractionComponent->m_InteractionDistance && GetInteractable())
    {
        CouldntFindInteractable();
    }
}

void UInteractorComponent::CouldntFindInteractable()
{
      //if we were looking at an interactable, but cant find it anymore (stopped looking) so cancel it
      //We've lost focus on an interactable. Clear the timer
    if(GetWorld()->GetTimerManager().IsTimerActive(m_TimerHandle_Interact))
    {
        GetWorld()->GetTimerManager().ClearTimer(m_TimerHandle_Interact);
    }

    //tell the interactable we've stopped focusing on it, and clear the current interactable
    if(UInteractionComponent* pInteractable = GetInteractable())
    {
        pInteractable->EndFocus(this);

        if(m_InteractionData.m_bInteractHeld)
        {
            EndInteract();
        }
    }

    m_InteractionData.m_pViewedInteractionComponent = nullptr;
}

void UInteractorComponent::FoundNewInteractable(UInteractionComponent* pInteractable)
{
      //if we're looking at a new interactable (edge case)
    EndInteract();

    //if we had an old interactable (we were looking to another one before), stop focusing the old one

    if(UInteractionComponent* pOldInteractable = GetInteractable())
    {
        pOldInteractable->EndFocus(this);
    }

    //set new interactable
    m_InteractionData.m_pViewedInteractionComponent = pInteractable;
    pInteractable->BeginFocus(this); //focus on new interactable
}






