// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionWidget.h"
#include "InteractionComponent.h"

/// @brief call this when interaction card values gets updated
/// @param pInteractionComponent pointer to the current
void UInteractionWidget::UpdateInteractionWidget(class UInteractionComponent* pInteractionComponent)
{
    m_pOwningInteractionComponent = pInteractionComponent;
    OnUpdateInteractionWidget();
}