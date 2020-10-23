#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"


UCLASS(ClassGroup = (Interaction))
class SANKARI_API UInteractionWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    // ---- Functionality ----

    //call this when interaction card values gets updated
    UFUNCTION(BlueprintCallable , Category = "Interaction")
    void UpdateInteractionWidget(class UInteractionComponent* pInteractionComponent);

        // -- Event --
        //when this is called, use blueprint logic to update card
    UFUNCTION(BlueprintImplementableEvent , Category = "Interaction")
    void OnUpdateInteractionWidget();

        // ---- Data members ----

        //when spawned, we can put this (pointer) right in
    UPROPERTY(BlueprintReadOnly , Category = "Interaction" , Meta = (ExposeOnSpawn , DisplayName = "Owning Interaction Component"))
    class UInteractionComponent* m_pOwningInteractionComponent;
};
