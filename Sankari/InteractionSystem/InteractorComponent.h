// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractorComponent.generated.h"

USTRUCT()
struct FInteractionData
{
    GENERATED_BODY()

    // ---- Data members ----

    //The current interactable component we're viewing, if there is one
    UPROPERTY()
    class UInteractionComponent* m_pViewedInteractionComponent = nullptr;

        //The time when we last checked for an interactable
    UPROPERTY()
        float m_LastInteractionCheckTime{0.0f};

        //Whether the local player is holding the interact key
    UPROPERTY()
        bool m_bInteractHeld = false;
};

UCLASS(ClassGroup = (Interaction) , meta = (BlueprintSpawnableComponent))
class SANKARI_API UInteractorComponent final : public UActorComponent
{
    GENERATED_BODY()

public:

    // ---- Constructors ----

    UInteractorComponent();

    // ---- Functionality ----
    void BeginPlay() final;
    void TickComponent(float DeltaTime , ELevelTick TickType , FActorComponentTickFunction* ThisTickFunction) final;

    UFUNCTION(BlueprintCallable , Category = "Interaction")
    void BeginInteract();

    UFUNCTION(BlueprintCallable , Category = "Interaction")
    void EndInteract();

    // -- Getters --

    //true if we're interacting with an interactable that has an interaction time (eg. a lamp that takes 2 secs to turn on)
    UFUNCTION(BlueprintCallable , Category = "Interaction")
    bool IsInteracting() const;

    //get the time till we interact with the current interactable
    UFUNCTION(BlueprintCallable , Category = "Interaction")
    float GetRemainingInteractTime() const;

    // ---- Data members ----

    //How often in seconds to check for an interactable object. Set this to zero if you want to check every tick.
    UPROPERTY(EditDefaultsOnly , Category = "Interaction" , meta = (ClampMin = "0.0" , UIMin = "0.0" , ClampMax = "100000.0" , UIMax = "100000.0" , DisplayName = "Interaction check frequency"))
    float m_InteractionCheckFrequency = 0.0f;

        //How far we'll trace when we check if the player is looking at the interactable object
    UPROPERTY(EditDefaultsOnly , Category = "Interaction" , meta = (ClampMin = "0.1" , UIMin = "0.1" , ClampMax = "1000000.0" , UIMax = "1000000.0" , DisplayName = "Interaction check distance"))
    float m_InteractionCheckDistance = 1000.0f;

private:

    // - Interactions -
    void Interact();
    void PerformInteractionCheck();
    void CouldntFindInteractable();
    void FoundNewInteractable(UInteractionComponent* Interactable);

    // -- Getters --
    //Helper function to make grabbing interactable faster
    FORCEINLINE class UInteractionComponent* GetInteractable() const { return m_InteractionData.m_pViewedInteractionComponent; }

    // ---- Data members ----
    //information about the current state of the players interaction
    UPROPERTY()
    FInteractionData m_InteractionData;

    FTimerHandle m_TimerHandle_Interact;
};
