#include "InteractionComponent.h"
#include "InteractorComponent.h"
#include "InteractionWidget.h"

// ---- Constructors ----
UInteractionComponent::UInteractionComponent()
{
    Space = EWidgetSpace::Screen;
    DrawSize = FIntPoint(600 , 100);
    bDrawAtDesiredSize = true;

    SetComponentTickEnabled(false);
    SetActive(true);
    SetHiddenInGame(true);
}

// ---- Functionality ----

/// @brief When started viewing the interactable actor, BeginFocus is called
/// @param pInteractor the actor which started looking/focusing
void UInteractionComponent::BeginFocus(class UInteractorComponent* pInteractor)
{
    if(!IsActive() || !GetOwner() || !pInteractor) return;

    //call delegate 
    OnBeginFocus.Broadcast(pInteractor);

    SetHiddenInGame(false);

    //grab any visual component (prim comp)
    const TSet<UActorComponent*>& pOwnerComps = GetOwner()->GetComponents();
    for(UActorComponent* const visualComponent : pOwnerComps)
    {
        if(UPrimitiveComponent* const pPrimitive = Cast<UPrimitiveComponent>(visualComponent))
        {
            pPrimitive->SetRenderCustomDepth(true); //visuals outline shader
        }
    }

    RefreshWidget();
}

/// @brief When stopped viewing the interactable actor, EndFocus is called
/// @param pInteractor the actor which stopped looking/focusing
void UInteractionComponent::EndFocus(class UInteractorComponent* pInteractor)
{
    if(!IsActive() || !GetOwner() || !pInteractor) return;

    //call delegate 
    OnEndFocus.Broadcast(pInteractor);

    SetHiddenInGame(true);

    //grab any visual component (prim comp)
    const TSet<UActorComponent*>& ownerComponents = GetOwner()->GetComponents();
    for(UActorComponent* pComponent : ownerComponents)
    {
        UPrimitiveComponent* pPrimitive = Cast<UPrimitiveComponent>(pComponent);
        if(pPrimitive == nullptr)
        {
            continue;
        }
        pPrimitive->SetRenderCustomDepth(false); //outline
    }

}

/// @brief called when the interaction begins
/// @param pInteractor the interactor
void UInteractionComponent::BeginInteract(class UInteractorComponent* pInteractor)
{
    if(!IsActive() || !GetOwner() || !pInteractor)
    {
        return;
    }

    m_pInteractors.AddUnique(pInteractor);
    OnBeginInteract.Broadcast(pInteractor);
}


/// @brief called when the interaction ends
/// @param pInteractor the interactor
void UInteractionComponent::EndInteract(class UInteractorComponent* pInteractor)
{
    if(!IsActive() || !GetOwner() || !pInteractor)
    {
        return;
    }
    m_pInteractors.RemoveSingle(pInteractor);
    OnEndInteract.Broadcast(pInteractor);
}


/// @brief called when the interaction happens
/// @param pInteractor the interactor
void UInteractionComponent::Interact(class UInteractorComponent* pInteractor)
{
    if(!IsActive() || !GetOwner() || !pInteractor)
    {
        return;
    }
    OnInteract.Broadcast(pInteractor);
}

// -- Getters --

/// @brief Gets the interaction percentage for UI purposes.
/// @return float value that has a range of [0.0f, 1.0f]
float UInteractionComponent::GetInteractPercentage() const
{
    if(!(m_InteractionTime > 0.0f)) // no negative + no 0
    {
        return 0.0f;
    }

    if(!m_pInteractors.IsValidIndex(0))
    {
        return 0.0f;
    }

    const UInteractorComponent* pInteractor = m_pInteractors[0];

    if(pInteractor == nullptr)
    {
        return 0.0f;
    }

    if(!(pInteractor && pInteractor->IsInteracting()))
    {
        return 0.0f;
    }

    return FMath::Clamp(1.0f - pInteractor->GetRemainingInteractTime() / m_InteractionTime , 0.0f , 1.0f);
}

// -- Setters --

/// @brief sets the name that is used on the UI
/// @param newNameText the new name to use on the UI
void UInteractionComponent::SetInteractableNameText(const FText& newNameText)
{
    m_InteractableNameText = newNameText;
    RefreshWidget();
}

/// @brief sets the action text that is used on the UI
/// @param newActionText the new action text to use UI
void UInteractionComponent::SetInteractableActionText(const FText& newActionText)
{
    m_InteractableActionText = newActionText;
    RefreshWidget();
}

// -- Private Functions --

/// @brief function to update the widget only when necessary
void UInteractionComponent::RefreshWidget()
{
    //interaction card not hidden
    if(bHiddenInGame)
    {
        return;
    }

    //make sure the widget is initialized, and that we are displaying the right values (these may have changed)

    #if !UE_BUILD_SHIPPING
    if(UInteractionWidget* pInteractionWidget = Cast<UInteractionWidget>(GetUserWidgetObject()))
    {
        pInteractionWidget->UpdateInteractionWidget(this);
    }
    #elif
    if(UInteractionWidget* pInteractionWidget = CastChecked<UInteractionWidget>(GetUserWidgetObject()))
    {
        pInteractionWidget->UpdateInteractionWidget(this);
    }
    #endif

}

/// @brief Deactivate the component and make it non-interactable
void UInteractionComponent::Deactivate()
{
    Super::Deactivate();

    int32 i = m_pInteractors.Num() - 1;
    for(i; i >= 0; --i)
    {
        if(UInteractorComponent* pInteractor = m_pInteractors[i])
        {
            EndFocus(pInteractor);
            EndInteract(pInteractor);
        }
    }

    m_pInteractors.Empty();
}