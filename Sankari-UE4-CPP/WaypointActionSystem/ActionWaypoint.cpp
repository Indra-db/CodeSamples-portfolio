#include "BaseWaypoint.h"
#include <Components/SphereComponent.h>
#include <Components/BillboardComponent.h>
#include "WaypointPatrolComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <GameFramework/Character.h>
#include <Components/ArrowComponent.h>

// ---- Constructors ----
AActionWaypoint::AActionWaypoint()
{
    // Deactivate tick on start, because it is not needed
    PrimaryActorTick.bStartWithTickEnabled = false;
    PrimaryActorTick.bCanEverTick = true;

    ResetValues();
    ClearPawnData();

    // Set default values
    m_TriggerSphereRadius = 20.0f;
    m_pCurrentAnimMontage = nullptr;
    m_NewSpeed = 600.0f;
    m_WaitTime = 1.0f;
    m_pWaypointPatrolComponent = nullptr;

    // Disable other actor's unused vars
    SetCanBeDamaged(false);	// actor's property

    bFindCameraComponentWhenViewTarget = false;

    // Create collision component
    m_pSphereCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("WaypointCollision"));
    if(m_pSphereCollisionComponent != nullptr)
    {
        // Add a function that handles the begin overlap event on the trigger
        m_pSphereCollisionComponent->OnComponentBeginOverlap.AddDynamic(this , &AActionWaypoint::OnBeginOverlap);

        // Ignore all object types, except pawn
        m_pSphereCollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
        m_pSphereCollisionComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn , ECollisionResponse::ECR_Overlap);

        // Disable other unnecessary things
        m_pSphereCollisionComponent->SetCanEverAffectNavigation(false);
        m_pSphereCollisionComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
        m_pSphereCollisionComponent->AreaClass = nullptr;
        m_pSphereCollisionComponent->SetHiddenInGame(true);
        m_pSphereCollisionComponent->bVisibleInReflectionCaptures = false;
        m_pSphereCollisionComponent->SetRenderInMainPass(true);
        m_pSphereCollisionComponent->bRenderInDepthPass = false;
        m_pSphereCollisionComponent->bVisibleInRayTracing = false;
        m_pSphereCollisionComponent->bReceivesDecals = false;
        m_pSphereCollisionComponent->SetEnableGravity(false);
        m_pSphereCollisionComponent->bApplyImpulseOnDamage = false;

        // Set the radius and another properties
        m_pSphereCollisionComponent->InitSphereRadius(m_TriggerSphereRadius);
        m_pSphereCollisionComponent->ShapeColor = FColor(100 , 255 , 100 , 255);

        // Set it as root component
        SetRootComponent(m_pSphereCollisionComponent);

        // Position the actor a little bit higher than the floor
        AddActorWorldTransform(FTransform(FVector(0.0f , 0.0f , 80.0f)));
    }

    // Create arrow component, used for the rotation.
    m_pArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("WaypointArrow"));
    if(m_pArrowComponent != nullptr)
    {
        m_pArrowComponent->ArrowColor = FColor(150 , 200 , 255);	// for example
        m_pArrowComponent->bTreatAsASprite = true;
        m_pArrowComponent->bIsScreenSizeScaled = true;
        m_pArrowComponent->SetHiddenInGame(true);
        m_pArrowComponent->bVisibleInReflectionCaptures = false;
        m_pArrowComponent->SetRenderInMainPass(true);
        #if ENGINE_MINOR_VERSION >= 24
        m_pArrowComponent->bRenderInDepthPass = false;
        #endif
        m_pArrowComponent->bVisibleInRayTracing = false;
        m_pArrowComponent->SetReceivesDecals(false);
        m_pArrowComponent->SetComponentTickEnabled(false);
        m_pArrowComponent->SetupAttachment(m_pSphereCollisionComponent);
    }

    // Create billboard component
    m_pBillboardComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("WaypointBillboard"));
    if(m_pBillboardComponent != nullptr)
    {
        m_pBillboardComponent->SetRelativeScale3D(FVector(0.5f , 0.5f , 0.5f));
        m_pBillboardComponent->bIsScreenSizeScaled = true;
        m_pBillboardComponent->SetHiddenInGame(true);
        m_pBillboardComponent->bVisibleInReflectionCaptures = false;
        m_pBillboardComponent->SetRenderInMainPass(true);
        m_pBillboardComponent->bRenderInDepthPass = false;
        m_pBillboardComponent->bVisibleInRayTracing = false;
        m_pBillboardComponent->SetReceivesDecals(false);
        m_pBillboardComponent->SetComponentTickEnabled(false);
        m_pBillboardComponent->SetupAttachment(m_pSphereCollisionComponent);
    }

    SetActorHiddenInGame(true);

}

// ---- Functionality ----

void AActionWaypoint::BeginPlay()
{
    Super::BeginPlay();
}

void AActionWaypoint::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Check if target has been changed
    CheckPawnTarget();

    if(m_bPlayingActions)
    {
        // If there are more actions to execute
        if(GetActionsLen() > m_CurrentActionIndex)
        {
            ExecuteActions(DeltaTime , m_CurrentActionIndex);
        }
        // No more actions
        else
        {
            if(m_pOverlappingPawn != nullptr)
            {
                m_pWaypointPatrolComponent->SetNextWaypoint();
                SetActorTickEnabled(false);
                ResetValues();
                ClearPawnData();
            }
        }
    }
}

void AActionWaypoint::OnBeginOverlap(UPrimitiveComponent* OverlappedComp , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult)
{
    UE_LOG(LogTemp , Warning , TEXT(__FUNCSIG__ " - overlap event"));

    // Other Actor is the actor that triggered the event. Check that is not ourself. 
    // Check this to only allow the first overlapping pawn to execute the actions
    if(OtherActor != nullptr && OtherActor != this && m_pOverlappingPawn == nullptr)
    {
        // Actor has to be a pawn
        m_pOverlappingPawn = Cast<APawn>(OtherActor);
        if(m_pOverlappingPawn == nullptr)
        {
            return;
        }

        UE_LOG(LogTemp , Warning , TEXT(__FUNCSIG__ " - Actor overlapping is pawn"));

        // Pawn must have WaypointPatrolComponent
        m_pWaypointPatrolComponent = m_pOverlappingPawn->FindComponentByClass<UWaypointPatrolComponent>();
        if(m_pWaypointPatrolComponent == nullptr)
        {
            m_pOverlappingPawn = nullptr;
            return;
        }

        UE_LOG(LogTemp , Warning , TEXT(__FUNCSIG__ " - actor has waypoint patrol component"));
        AActionWaypoint* currentWaypoint = m_pWaypointPatrolComponent->GetCurrentWaypoint();

        // Check if the current waypoint is this waypoint, because if not, this waypoint is not in its patrol
        if(currentWaypoint != nullptr && currentWaypoint == this)
        {
            //arrived at waypoints start actions

            // If there are actions to execute, enable tick
            // If not, select and set the next waypoint
            if(GetActionsLen() > 0)
            {
                UE_LOG(LogTemp , Warning , TEXT(__FUNCSIG__ " - actor has actions"));
                m_bPlayingActions = true;
                SetActorTickEnabled(true);

                // Stop movement if there are actions, to avoid relocation
                AController* overlappingPawnController = m_pOverlappingPawn->GetController();
                if(overlappingPawnController != nullptr)
                {
                    overlappingPawnController->StopMovement();
                }
            }
            else
            {
                UE_LOG(LogTemp , Warning , TEXT(__FUNCSIG__ " - No actions on this waypoint"));
                m_pWaypointPatrolComponent->SetNextWaypoint();
                ClearPawnData();
            }
        }
        else
        {
            // If this is not the waypoint pawn is looking for, clear some data
            UE_LOG(LogTemp , Warning , TEXT(__FUNCSIG__ " - Not the waypoint of this pawn"));
            ClearPawnData();
        }
    }
}

// - Editor -

#if WITH_EDITOR
void AActionWaypoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    const FName changedPropertyName = PropertyChangedEvent.MemberProperty ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;

    if(changedPropertyName == GET_MEMBER_NAME_CHECKED(AActionWaypoint , m_TriggerSphereRadius) && m_pSphereCollisionComponent != nullptr)
    {
        m_pSphereCollisionComponent->SetSphereRadius(m_TriggerSphereRadius);
    }
}
#endif

// - Actions -

void AActionWaypoint::ExecuteActions_Implementation(const float deltaTime , const int32 actionIndex)
{
    UE_LOG(LogTemp , Error , TEXT(__FUNCSIG__ " - Execute actions not implemented, override it in blueprints"));
}

int32 AActionWaypoint::GetActionsLen_Implementation()
{
    UE_LOG(LogTemp , Error , TEXT(__FUNCSIG__ " - Actions length not implemented, override it in blueprints"));
    return 0;
}

void AActionWaypoint::Wait()
{
    // If the specified wait time is minimum 0.01 seconds
    if((FMath::IsNearlyZero(m_WaitTime)))
    {
        GoToNextAction();
        return;
    }

    // If is not waiting yet
    if(!m_bWaiting)
    {
        // Set a timer with the specified time
        float finalWaitTime = UKismetMathLibrary::RandomFloatInRange(m_WaitTimeDeviationMin , m_WaitTimeDeviationMax) + m_WaitTime;

        #if !UE_BUILD_SHIPPING
        if(FMath::IsNearlyZero(finalWaitTime))
        {
            UE_LOG(LogTemp , Warning , TEXT(__FUNCSIG__ " - Wait Time went under or near zero, please re-evaluate your deviation and wait time values"));
            finalWaitTime = 0;
        }
        #endif

        if(GetWorld() != nullptr)
        {
            GetWorldTimerManager().SetTimer(m_TimerHandle , this , &AActionWaypoint::OnWaitEnd , finalWaitTime , false);
        }

        m_bWaiting = true;
    }
}

void AActionWaypoint::ChangeSpeed()
{
    // Check unexpected nullptr
    if(m_pOverlappingPawn == nullptr)
    {
        GoToNextAction();

        return;
    }

    // Check if pawn is a character to try to get one component or another
    ACharacter* character = Cast<ACharacter>(m_pOverlappingPawn);
    if(character != nullptr)
    {
        UCharacterMovementComponent* pCharacterMovementComponent = m_pOverlappingPawn->FindComponentByClass<UCharacterMovementComponent>();

        if(pCharacterMovementComponent == nullptr)
        {
            UE_LOG(LogTemp , Warning , TEXT("AActionWaypoint::ChangeSpeed character movement component does not exist"))
        }
        else
        {
            pCharacterMovementComponent->MaxWalkSpeed = m_NewSpeed;
        }
    }

    GoToNextAction();
}

void AActionWaypoint::PlayAnims()
{
    // If there are no animations to play and there are still no other animations to play
    if(!(GetAnimsLen() > 0 && GetAnimsLen() > m_CurrentAnimIndex))
    {
        GoToNextAction();
        return;
    }

    // Check unexpected nullptr
    if(m_pOverlappingPawn == nullptr)
    {
        UE_LOG(LogTemp , Warning , TEXT(__FUNCSIG__ " - Pawn is null"));
        GoToNextAction();

        return;
    }

    ACharacter* pCharacter = Cast<ACharacter>(m_pOverlappingPawn);

    // Check if pawn is a character
    if(pCharacter == nullptr)
    {
        UE_LOG(LogTemp , Warning , TEXT(__FUNCSIG__ " - AActionWaypoint::PlayAnims failed, not a character"));
        GoToNextAction();
        return;
    }

    // If the anims are not currently playing
    if(m_bPlayingAnims)
    {
        return;
    }

    m_pCurrentAnimMontage = m_pAnimsToPlay[m_CurrentAnimIndex];

    #if !UE_BUILD_SHIPPING
    if(m_pCurrentAnimMontage == nullptr)
    {
        // If the anim is null then go to next animation
        UE_LOG(LogTemp , Error , TEXT(__FUNCSIG__ " - anim montage: nullptr") , *(m_pCurrentAnimMontage->GetName()));
        OnCurrentAnimMontageEnd();
        return;
    }
    #endif

    // Play current anim and get the duration
    const float currentAnimDuration = pCharacter->PlayAnimMontage(m_pCurrentAnimMontage);

    // Set a timer with the duration time of the animation
    if(GetWorld() != nullptr)
    {
        GetWorldTimerManager().SetTimer(m_TimerHandle , this , &AActionWaypoint::OnCurrentAnimMontageEnd , currentAnimDuration , false);
    }

    m_bPlayingAnims = true;
}

void AActionWaypoint::RotateToArrow(const float deltaTime)
{
    // Check unexpected nullptr
    if(m_pOverlappingPawn == nullptr)
    {
        GoToNextAction();
        return;
    }

    const FRotator arrowRotation = m_pArrowComponent->GetComponentRotation();

    // Go to next action if rotation is completed
    if(FMath::IsNearlyZero(m_pOverlappingPawn->GetActorRotation().Yaw , arrowRotation.Yaw))
    {
        GoToNextAction();
        return;
    }

    const FRotator pawnRotation = m_pOverlappingPawn->GetActorRotation();

    // Get rotation interpolation speed if it's not already calculated
    if(FMath::IsNearlyZero(m_RotationInterpolationSpeed))
    {
        // Get angle to rotate and clamp it
        float angleToRotate = arrowRotation.Yaw - pawnRotation.Yaw;
        angleToRotate += (angleToRotate > 180.0f) ? -360.0f : (angleToRotate < -180.0f) ? 360.0f : 0.0f;

        const float rotationTime = (m_MaxRotationTime * angleToRotate) / 179.99f; //edge case not 180
        m_RotationInterpolationSpeed = FMath::Abs(angleToRotate / rotationTime);
    }

    // Get the the interpolated rotation of the overlapping character
    const FRotator nextRotation = FMath::RInterpConstantTo(pawnRotation , arrowRotation , deltaTime , m_RotationInterpolationSpeed);
    m_pOverlappingPawn->SetActorRotation(nextRotation);
}

// -- Private Functions --

void AActionWaypoint::ResetValues()
{
    if(m_TimerHandle.IsValid() && GetWorld() != nullptr)
    {
        GetWorldTimerManager().ClearAllTimersForObject(this);
    }

    m_bPlayingActions = false;
    m_CurrentActionIndex = 0;
    m_bWaiting = false;
    m_CurrentAnimIndex = 0;
    m_bPlayingAnims = false;
    m_pCurrentAnimMontage = nullptr;
    m_RotationInterpolationSpeed = 0.0f;
}

void AActionWaypoint::ClearPawnData()
{
    m_pOverlappingPawn = nullptr;
    m_pWaypointPatrolComponent = nullptr;
}

void AActionWaypoint::CheckPawnTarget()
{
    // Stop all actions and reset values if the pawn changes his target to another
    if(m_pOverlappingPawn == nullptr && m_pWaypointPatrolComponent == nullptr && m_pWaypointPatrolComponent->GetCurrentWaypoint() == this)
    {
        return;
    }

    SetActorTickEnabled(false);

    // Stop anim montage if needed and if the pawn is a character
    if(m_pCurrentAnimMontage != nullptr)
    {
        ACharacter* character = Cast<ACharacter>(m_pOverlappingPawn);
        if(character != nullptr) character->StopAnimMontage(m_pCurrentAnimMontage);
    }

    ResetValues();
    ClearPawnData();
}

void AActionWaypoint::OnWaitEnd()
{
    m_bWaiting = false;
    GoToNextAction();
}

void AActionWaypoint::OnCurrentAnimMontageEnd()
{
    m_bPlayingAnims = false;
    m_pCurrentAnimMontage = nullptr;
    m_CurrentAnimIndex++;	// next anim
}

