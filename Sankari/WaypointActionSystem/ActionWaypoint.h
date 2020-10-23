#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWaypoint.generated.h"

class USphereComponent;
class UBillboardComponent;
class UWaypointPatrolComponent;
class UAnimMontage;
class UArrowComponent;

UCLASS(ClassGroup = (WaypointActionSystem))
class SANKARI_API AActionWaypoint final : public AActor
{
    GENERATED_BODY()

    public:

    // ---- Constructors ----

    AActionWaypoint();

    // ---- Functionality ----

    void BeginPlay() final;

    void Tick(float DeltaSeconds) final;

    UFUNCTION(Category = Default)
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComp , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult);

    // - Actions -

    //Executes the available actions
    UFUNCTION(BlueprintNativeEvent , Category = "Waypoint")
    void ExecuteActions(const float deltaTime , const int32 actionIndex);

    // Call needed to make actions go ahead to the next one.
    UFUNCTION(BlueprintCallable , Category = "Waypoint")
    void GoToNextAction() { m_CurrentActionIndex++; }

    //wait for a specified time
    UFUNCTION(BlueprintCallable , Category = "Waypoint")
    void Wait();

    // Changes pawn speed.
    UFUNCTION(BlueprintCallable , Category = "Waypoint")
    void ChangeSpeed();

    // Plays the animations on Anims to Play in order.
    UFUNCTION(BlueprintCallable , Category = "Waypoint")
    void PlayAnims();

    // Gets the number of animations.
    UFUNCTION(BlueprintPure , Category = "Waypoint")
    int32 GetAnimsLen() { return m_pAnimsToPlay.Num(); }

    // Rotates pawn to the same direction of the waypoint arrow component.
    UFUNCTION(BlueprintCallable , Category = "Waypoint")
    void RotateToArrow(const float deltaTime);

    // -- Getters --

    // Gets the waypoint trigger sphere radius.
    UFUNCTION(BlueprintPure , Category = "Waypoint")
    float GetSphereRadius() { return m_TriggerSphereRadius; }

    // Indicates if pawn is currently playing actions on the waypoint. 
    UFUNCTION(BlueprintPure , Category = "Waypoint")
    bool IsPlayingActions() { return m_bPlayingActions; }

    // Returns CollisionComponent. 
    UFUNCTION(BlueprintPure , Category = "Waypoint")
    USphereComponent* GetCollisionComponent() { return m_pSphereCollisionComponent; }

    // Returns SpriteComponent. 
    UFUNCTION(BlueprintPure , Category = "Waypoint")
    UBillboardComponent* GetSpriteComponent() { return m_pBillboardComponent; }

    // Gets the number of actions. Must be overriden in blueprint.
    UFUNCTION(BlueprintNativeEvent , BlueprintPure , Category = "Waypoint")
    int32 GetActionsLen();

    // -- Editor --

    #if WITH_EDITOR
    void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) final;
    #endif // WITH_EDITOR

    // ---- Data members ----

    // Waypoint sphere radius. 
    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Properties | Trigger" , meta = (ClampMin = "0.1" , UIMin = "0.1" , ClampMax = "100000.0" , UIMax = "100000.0", DisplayName = "Trigger Sphere Radius"))
    float m_TriggerSphereRadius;

    // Pawn that overlaps with the waypoint. Visible for debug purposes. 
    UPROPERTY(VisibleAnywhere , Category = "Properties | Debug " , Meta = (DisplayName = "Overlapping pawn"))
    APawn* m_pOverlappingPawn;

    // Shape component used for collision 
    UPROPERTY(VisibleAnywhere , BlueprintReadOnly , Category = "Properties | Trigger", Meta = (DisplayName = "Sphere collision component"))
    USphereComponent* m_pSphereCollisionComponent;

    // Billboard used to see the waypoint in the editor 
    UPROPERTY(EditDefaultsOnly , BlueprintReadOnly , Category = "Properties | Debug" , Meta = (DisplayName = "Billboard component"))
    UBillboardComponent* m_pBillboardComponent;

    // Indicates if pawn is currently playing actions on the waypoint. Visible for debug purposes. 
    UPROPERTY(VisibleAnywhere , BlueprintReadOnly , Category = "Properties | Debug" , Meta = (DisplayName = "Is playing actions"))
    bool m_bPlayingActions;

    // New speed for the pawn, if change speed action is scheduled.
    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Properties | Speed Action" , meta = (ClampMin = "0.0" , UIMin = "0.0" , ClampMax = "100000.0" , UIMax = "100000.0", DisplayName = "New speed"))
    float m_NewSpeed;

    // Time to wait in seconds, if wait action is called.
    UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = "Properties | Wait Action" , meta = (ClampMin = "0.01" , UIMin = "0.01" , ClampMax = "100000.0" , UIMax = "100000.0" , DisplayName = "Wait time"))
    float m_WaitTime;

    // Time to wait in seconds, if wait action is called.
    UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = "Properties | Wait Action" , meta = (ClampMin = "0.01" , UIMin = "0.01" , ClampMax = "100000.0" , UIMax = "100000.0" , DisplayName = "Wait Time min deviation"))
    float m_WaitTimeDeviationMin;

    // Time to wait in seconds, if wait action is called.
    UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = "Properties | Wait Action" , meta = (ClampMin = "0.01" , UIMin = "0.01" , ClampMax = "100000.0" , UIMax = "100000.0" , DisplayName = "Wait Time max deviation"))
    float m_WaitTimeDeviationMax;

    // Array of all anims to play, if the action is called.
    UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = "Properties | Anim Action" , Meta = (DisplayName = "Animations to play"))
    TArray<UAnimMontage*> m_pAnimsToPlay;

    // Rotation time that pawn will get if it has to rotate 180 degrees, so the less angle to rotate the less time.
    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Properties | Rotate to Arrow action" , meta = (ClampMin = "0.1" , UIMin = "0.1" , ClampMax = "100000.0" , UIMax = "100000.0", DisplayName = "Max Rotation Time"))
    float m_MaxRotationTime;

    // Arrow component, indicates where to rotate to.
    UPROPERTY(EditDefaultsOnly , BlueprintReadOnly , Category = "Properties | Arrow" , Meta = (DisplayName = "Arrow component"))
    UArrowComponent* m_pArrowComponent;

private:

    // -- Private Functions --

    void ResetValues();
    void ClearPawnData();
    void CheckPawnTarget();
    void OnWaitEnd();
    void OnCurrentAnimMontageEnd();

    // ---- Data members ----

    FTimerHandle m_TimerHandle;	// used for timers

    UPROPERTY()
    UWaypointPatrolComponent* m_pWaypointPatrolComponent; //cached

    UPROPERTY()
    UAnimMontage* m_pCurrentAnimMontage;

    float m_RotationInterpolationSpeed;
    bool m_bWaiting;
    bool m_bPlayingAnims;
    uint8 m_CurrentAnimIndex;
    uint8 m_CurrentActionIndex;

};