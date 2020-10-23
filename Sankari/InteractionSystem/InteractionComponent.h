#pragma once
#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "InteractionComponent.generated.h"

// BeginInteract is called the moment the interaction process begins
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginInteract , UInteractorComponent* , pInteractor);
// EndInteract is called the moment the interaction ends 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndInteract , UInteractorComponent* , pInteractor);
// OnBeginFocus is called the moment the interactor looks at the actor that can be interacted with
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginFocus , UInteractorComponent* , pInteractor);
// OnEndFocus is called the moment the interactor stops looking at the actor that can be interacted with
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndFocus , UInteractorComponent* , pInteractor);
// OnInteract is called the moment the interaction gets done
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract , UInteractorComponent* , pInteractor);

// - Forward Declaration -
class UInteractorComponent;

UCLASS(ClassGroup = (Interaction) , meta = (BlueprintSpawnableComponent))
class SANKARI_API UInteractionComponent final : public UWidgetComponent
{
    GENERATED_BODY()

public:

    // ---- Constructors ----
    UInteractionComponent();

    // ---- Functionality ----

    //called when the player interaction check trace begins hitting this item, when you start looking at the interactable
    void BeginFocus(UInteractorComponent* pInteractor);
    //called when the player interaction check trace ends hitting this item, when you stop looking at the interactable
    void EndFocus(UInteractorComponent* pInteractor);
    //called when the player begins the interaction with the interactable
    void BeginInteract(UInteractorComponent* pInteractor);
    //called when the player ends the interaction with the interactable
    void EndInteract(UInteractorComponent* pInteractor);
    //called when the player interacts with the interactable
    void Interact(UInteractorComponent* pInteractor);

    // -- Getters --

    //Return a value from 0-1 -> how far through the interaction we are
    UFUNCTION(BlueprintCallable , BlueprintPure , Category = "Interaction")
    float GetInteractPercentage() const;

    // -- Setters --

    //call this to change the name text of the interactable. Will also refresh the interaction widget
    UFUNCTION(BlueprintCallable , Category = "Interaction")
    void SetInteractableNameText(const FText& newNameText);

        //call this to change the action text interactable. Will also refresh the interaction widget
    UFUNCTION(BlueprintCallable , Category = "Interaction")
    void SetInteractableActionText(const FText& newActionText);

    // ---- Delegates ----

    //called when the interactor presses the interact key whilst focusing on this interactable actor
    UPROPERTY(BlueprintCallable , BlueprintAssignable)
    FOnBeginInteract OnBeginInteract;

    //called when the Interactor releases the interact key, stops looking at the interactable actor, or gets too far way after starting an interact
    UPROPERTY(BlueprintCallable , BlueprintAssignable)
    FOnEndInteract OnEndInteract;

    //called when the Interactor pressed the interact key whilst focusing on the interactable actor
    UPROPERTY(BlueprintCallable , BlueprintAssignable)
    FOnBeginFocus OnBeginFocus;

    //called when the Interactor releases the interact key, stop looking at the interactable actor, or gets too far away after starting an interact
    UPROPERTY(BlueprintCallable , BlueprintAssignable)
    FOnEndFocus OnEndFocus;

    //called when the Interactor has interacted with the item for the required amount of time
    UPROPERTY(BlueprintCallable , BlueprintAssignable)
    FOnInteract OnInteract;

    // ---- Data members ----

    //the time the pInteractor must hold the interact key to interact with this object
    UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = "Interaction" , meta = (ClampMin = "0.1" , UIMin = "0.1" , ClampMax = "100000.0" , UIMax = "100000.0" , DisplayName = "Interaction time"))
    float m_InteractionTime = 0.0f;

    //The max distance the Interactor can be away from this actor before you can interact
    UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = "Interaction" , meta = (ClampMin = "0.1" , UIMin = "0.1" , ClampMax = "100000.0" , UIMax = "100000.0" , DisplayName = "Interaction distance"))
    float m_InteractionDistance = 200.0f;

    //The name that will come up when the player looks at the interactable
    UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = "Interaction" , Meta = (DisplayName = "Interactable name text"))
    FText m_InteractableNameText { FText::FromString("Interactable Object") };

    //The verb that describes how the interaction works
    UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = "Interaction" , Meta = (DisplayName = "Interactable action text"))
    FText m_InteractableActionText { FText::FromString("Interact") };

private:

    // ---- Functionality ----

    //refresh the interaction widget and its custom widgets
    void RefreshWidget();

    void Deactivate() final;

    // ---- Data members ----

    //holds the Interactor(s)
    TArray<UInteractorComponent*> m_pInteractors;

};
