#include "Serpant.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include <Math/UnrealMathUtility.h>
#include "Components/SphereComponent.h"
#include "Components/SplineMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/SplineComponent.h"
#include "Math/TransformNonVectorized.h"
#include "Materials/MaterialInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMesh.h"
#include "Math/Color.h"

#if WITH_EDITOR
#include <DrawDebugHelpers.h>
#endif
#include <Engine/EngineTypes.h>

// ---- Constructors ----

ASerpant::ASerpant()
{
    PrimaryActorTick.bCanEverTick = true;

    USceneComponent* SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    AddOwnedComponent(SceneRootComponent);
    SceneRootComponent->SetMobility(EComponentMobility::Movable);
    SetRootComponent(SceneRootComponent);

}

// ---- Functionality ----

void ASerpant::AddCollisionSpheres()
{

    float tempRadius{0.0f};
    float tempDistance{0.0f};
    float nodeScaleRatio{m_AmountNodes * m_StartScalingDownRatio};

    //add collision spheres and give it a radius according to the scale ratio.
    //e.g. for a smaller ratio towards the tail
    for(int32 indexNode = 0; indexNode < m_AmountNodes; ++indexNode)
    {
        if((float) indexNode < nodeScaleRatio)
        {
            tempRadius = m_Radius;
        }
        else
        {
            const float scaleBaseddIndexCurNode = (float) indexNode - nodeScaleRatio;
            const float scaleDownBaseddIndexCurNode = m_AmountNodes * (1.0f - m_StartScalingDownRatio);
            const float ClampedScaleRatio = FMath::Clamp(scaleBaseddIndexCurNode / scaleDownBaseddIndexCurNode , 0.0f , 1.0f);
            const float radiusLastSPhere = m_Radius * m_LastSphereRadiusRatio;
            const float lerpValue = FMath::Lerp(m_Radius , radiusLastSPhere , ClampedScaleRatio);
            tempRadius = FGenericPlatformMath::Max(lerpValue , 0.01f);
        }

        USphereComponent* pSphereComponent = NewObject<USphereComponent>(this);
        if(!pSphereComponent)
        {
            return;
        }
        pSphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic , ECollisionResponse::ECR_Block);
        AddOwnedComponent(pSphereComponent);
        pSphereComponent->SetupAttachment(RootComponent);
        pSphereComponent->SetMobility(EComponentMobility::Movable);
        pSphereComponent->SetEnableGravity(false);
        pSphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        pSphereComponent->RegisterComponent();
        pSphereComponent->SetRelativeLocation({tempDistance,0.0f,0.0f});
        pSphereComponent->SetSphereRadius(tempRadius , true);
        tempDistance += (tempRadius * 2) + m_Spacing;
        pSphereComponent->SetLinearDamping(m_LinearDamping);
        pSphereComponent->SetAngularDamping(m_AngularDamping);
        pSphereComponent->SetPhysMaterialOverride(m_pPhysicsMaterial);
        pSphereComponent->SetSimulatePhysics(true);
        m_pSphereCollisions.Push(pSphereComponent);
    }
    m_pHeadRef = m_pSphereCollisions[0];
}

void ASerpant::AddPhysicsConstraints()
{
    int32 amountSphereCollisions = m_pSphereCollisions.Num();
    if(amountSphereCollisions == 0)
    {
        return;
    }

    int32 tempIndex{0};
    for(USphereComponent* pSphereComp : m_pSphereCollisions)
    {
        if(amountSphereCollisions - 1 == tempIndex)
        {
            return;
        }

        //add physics constraint between two collision spheres
        UPhysicsConstraintComponent* pPhysicsConstraintComponent = NewObject<UPhysicsConstraintComponent>(this);
        if(!pPhysicsConstraintComponent)
        {
            return;
        }

        AddOwnedComponent(pPhysicsConstraintComponent);
        pPhysicsConstraintComponent->SetupAttachment(RootComponent);
        pPhysicsConstraintComponent->SetMobility(EComponentMobility::Movable);

        pPhysicsConstraintComponent->RegisterComponent();
        USphereComponent* pNextSphereComp = m_pSphereCollisions[tempIndex + 1];
        FVector worldLoc = (pSphereComp->GetComponentLocation() + pNextSphereComp->GetComponentLocation()) * 0.5f;
        pPhysicsConstraintComponent->SetWorldLocation(worldLoc);

        pPhysicsConstraintComponent->SetConstrainedComponents(pNextSphereComp , "None" , pSphereComp , "None");

        m_pPhysicsConstraints.Push(pPhysicsConstraintComponent);

        ++tempIndex;
    }
}

void ASerpant::AddSplineComponent()
{
    m_pSplineComp = NewObject<USplineComponent>(this);
    if(!m_pSplineComp)
    {
        return;
    }
    AddOwnedComponent(m_pSplineComp);
    m_pSplineComp->SetupAttachment(RootComponent);
    m_pSplineComp->SetMobility(EComponentMobility::Movable);
    m_pSplineComp->RegisterComponent();
    FTransform splineTransform{FRotator{0.0f,0.0f,0.0f},FVector{0.0f,0.0f,0.0f},FVector{1.0f,1.0f,1.0f}};
    m_pSplineComp->SetRelativeTransform(splineTransform);
    m_pSplineComp->SetAbsolute(true , true , true);
    m_pSplineComp->ClearSplinePoints(true);
}

void ASerpant::AddSplineMeshes()
{

    int32 amountSphereCollisions = m_pSphereCollisions.Num();

    if(amountSphereCollisions == 0)
    {
        return;
    }

    int32 tempIndex{0};
    for(const USphereComponent* pSphereComp : m_pSphereCollisions)
    {
        if(amountSphereCollisions - 1 <= tempIndex)
        {
            continue;
        }

        //create and add mesh
        USplineMeshComponent* pSplineMeshComponent = NewObject<USplineMeshComponent>(this);

        if(!pSplineMeshComponent)
        {
            return;
        }

        AddOwnedComponent(pSplineMeshComponent);
        pSplineMeshComponent->SetMobility(EComponentMobility::Movable);
        pSplineMeshComponent->RegisterComponent();
        m_pSplineMeshes.Push(pSplineMeshComponent);

        //decide the scale according to the value set + position in the body
        float currentSphereRadiusDiv50 = pSphereComp->GetUnscaledSphereRadius() * 0.02f;
        pSplineMeshComponent->SetStartScale({currentSphereRadiusDiv50,currentSphereRadiusDiv50} , true);

        const USphereComponent* pNextSphereComp = m_pSphereCollisions[tempIndex + 1];
        float nextSphereRadiusDiv50 = pNextSphereComp->GetUnscaledSphereRadius() * 0.02f;
        pSplineMeshComponent->SetEndScale({nextSphereRadiusDiv50,nextSphereRadiusDiv50} , true);

        pSplineMeshComponent->SetStartAndEnd(pSphereComp->GetComponentLocation() , this->GetActorForwardVector() , pNextSphereComp->GetComponentLocation() , this->GetActorForwardVector() , true);

        //decide which mesh and material to use
        UStaticMesh* pSelectedMesh{nullptr};
        UMaterialInterface* pSelectedMaterial{nullptr};

        if(tempIndex == 0) //head
        {
            pSelectedMesh = m_pHeadMesh;
            pSelectedMaterial = m_pHeadMaterial;
        }
        else if(tempIndex == m_AmountNodes - 2) //tail
        {
            pSelectedMesh = m_pTailMesh;
            pSelectedMaterial = m_pTailMaterial;
        }
        else //body
        {
            pSelectedMesh = m_pBodyMesh;
            pSelectedMaterial = m_pBodyMaterial;
        }

        //set mesh and material
        pSplineMeshComponent->SetStaticMesh(pSelectedMesh);
        pSplineMeshComponent->SetMaterial(0 , pSelectedMaterial);

        ++tempIndex;
    }

}

// -- Virtual Functions --

void ASerpant::BeginPlay()
{
    Super::BeginPlay();

    ClearOnConstructValues();

    m_pSphereCollisions.Reserve(m_AmountNodes);
    m_SplinePointsLocAndTang.Reserve(m_AmountNodes);
    m_SplinePointsLocAndTang.Init(FSplinePointLocTangent{} , m_AmountNodes);

    AddCollisionSpheres();
    AddSplineMeshes();
    AddPhysicsConstraints();
    AddSplineComponent();
}

void ASerpant::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateSplineComponent();
    UpdateSnakeMesh();

    #if !UE_BUILD_SHIPPING
    //! temporary key -> was for testing
    if(UGameplayStatics::GetPlayerController(GetWorld() , 0)->WasInputKeyJustPressed(EKeys::N))
    {
        SetMeshVisibility();
    }
    if(!m_bIsMeshVisible)
    {
        DrawDebugShapes();
    }
    #endif

}

void ASerpant::OnConstruction(const FTransform& Transform)
{
    if(GetWorld() == nullptr)
    {
        return;
    }

    ClearOnConstructValues();
    AddCollisionSpheres();
    AddSplineMeshes();
    AddPhysicsConstraints();

}

// -- Private Functions --

void ASerpant::UpdateSplineComponent()
{
    m_pSplineComp->ClearSplinePoints();

    int32 lastSphereColIndex = m_pSphereCollisions.Num() - 2;

    int32 tempIndex{0};
    for(const USphereComponent* pSphereComp : m_pSphereCollisions)
    {
        m_pSplineComp->AddSplinePoint(pSphereComp->GetComponentLocation() , ESplineCoordinateSpace::World , tempIndex == lastSphereColIndex ? true : false);

        ++tempIndex;
    }
}

void ASerpant::UpdateSnakeMesh()
{
    #pragma region method01
    /*
    int32 tempIndex{0};

    FVector currentSplinePointLocation{};
    FVector currentSplinePointTangent{};
    FVector nextSplinePointLocation{};
    FVector nextSplinePointTangent{};

    m_pSplineComp->GetLocationAndTangentAtSplinePoint(tempIndex , currentSplinePointLocation , currentSplinePointTangent , ESplineCoordinateSpace::Local);

    for(USplineMeshComponent* pSplineMesh : m_pSplineMeshes)
    {
        FVector startpos = currentSplinePointLocation - (currentSplinePointTangent * m_pSphereCollisions[tempIndex]->GetScaledSphereRadius());

        m_pSplineComp->GetLocationAndTangentAtSplinePoint(tempIndex + 1 , nextSplinePointLocation , nextSplinePointTangent , ESplineCoordinateSpace::Local);

        FVector endpos = nextSplinePointLocation - (nextSplinePointTangent * m_pSphereCollisions[tempIndex + 1]->GetScaledSphereRadius());

        pSplineMesh->SetStartAndEnd(startpos , currentSplinePointTangent , endpos , nextSplinePointTangent , false);

        FVector upVecAtSplinePoint = m_pSplineComp->GetUpVectorAtSplinePoint(tempIndex , ESplineCoordinateSpace::Local);

        pSplineMesh->SetSplineUpDir(upVecAtSplinePoint , true);

        currentSplinePointLocation = MoveTemp(nextSplinePointLocation);
        currentSplinePointTangent = MoveTemp(nextSplinePointTangent);

        ++tempIndex;
    }
    */
    #pragma endregion method01

    //method 2 is a refactored method 1 (which I'm keeping for safe keeping for now), since these days we have more memory,
    //but cpu power and memory access are mainly the bottleneck, so to get that improved I tried to align some data
    //I can still improve on it.
    #pragma region method02

    int32 tempIndex{0};
    int32 amountSplineMeshes = m_pSplineMeshes.Num();

    //+1 due to having one more sphere collision element
    for(int32 i{0}; i < amountSplineMeshes + 1; ++i)
    {
        m_pSplineComp->GetLocationAndTangentAtSplinePoint(i , m_SplinePointsLocAndTang[i].m_Location , m_SplinePointsLocAndTang[i].m_Tangent , ESplineCoordinateSpace::Local);
    }

    for(USplineMeshComponent* pSplineMesh : m_pSplineMeshes)
    {
        FVector startpos = m_SplinePointsLocAndTang[tempIndex].m_Location - (m_SplinePointsLocAndTang[tempIndex].m_Tangent.GetSafeNormal(0.0001f) * m_pSphereCollisions[tempIndex]->GetScaledSphereRadius());

        FVector endpos = m_SplinePointsLocAndTang[tempIndex + 1].m_Location - (m_SplinePointsLocAndTang[tempIndex + 1].m_Tangent.GetSafeNormal(0.0001f) * m_pSphereCollisions[tempIndex + 1]->GetScaledSphereRadius());

        pSplineMesh->SetStartAndEnd(startpos , m_SplinePointsLocAndTang[tempIndex].m_Tangent , endpos , m_SplinePointsLocAndTang[tempIndex + 1].m_Tangent , false);

        FVector upVecAtSplinePoint = m_pSplineComp->GetUpVectorAtSplinePoint(tempIndex , ESplineCoordinateSpace::Local);

        pSplineMesh->SetSplineUpDir(upVecAtSplinePoint , true);

        ++tempIndex;
    }

    #pragma endregion method02


}

#if WITH_EDITOR

void ASerpant::DrawDebugShapes()
{
    if(GetWorld() == nullptr)
    {
        return;
    }

    //clear debug from previous frame
    UKismetSystemLibrary::FlushPersistentDebugLines(this->GetWorld());


    FVector LineEnd = m_pHeadRef->GetComponentLocation() + (m_pHeadRef->GetForwardVector() * m_ArrowLength);
    DrawDebugDirectionalArrow(GetWorld() , m_pHeadRef->GetComponentLocation() , LineEnd , m_ArrowHeadSize , m_ArrowColor.ToFColor(true) , true , 0.0f , 0 , m_ArrowThickness);

    int32 tempIndex{0};
    int32 amountCollisionSphereMin1 = m_pSphereCollisions.Num() - 1;

    for(USphereComponent* pSphereComp : m_pSphereCollisions)
    {
        DrawDebugSphere(GetWorld() , pSphereComp->GetComponentLocation() , pSphereComp->GetScaledSphereRadius() , 16 , m_SphereColor.ToFColor(true) , true , 0.0f , 0 , m_SphereThickness);

        if(tempIndex < amountCollisionSphereMin1)
        {
            DrawDebugLine(GetWorld() , pSphereComp->GetComponentLocation() , m_pSphereCollisions[tempIndex + 1]->GetComponentLocation() , m_LineColor.ToFColor(true) , true , 0.0f , 0 , pSphereComp->GetScaledSphereRadius() * m_LineThickness);
        }

        ++tempIndex;
    }

    for(UPhysicsConstraintComponent* pPhysicsConstraintComp : m_pPhysicsConstraints)
    {
        DrawDebugSphere(GetWorld() , pPhysicsConstraintComp->GetComponentLocation() , m_Radius * 0.25f , 16 , m_ArrowColor.ToFColor(true) , true , 0.0f , 0 , m_SphereThickness);
    }
}

void ASerpant::SetMeshVisibility()
{
    m_bIsMeshVisible = !m_bIsMeshVisible;
    UKismetSystemLibrary::FlushPersistentDebugLines(this->GetWorld());
    for(USplineMeshComponent* pSplineMesh : m_pSplineMeshes)
    {
        pSplineMesh->SetVisibility(m_bIsMeshVisible);
    }
}

#endif //WITH_EDITOR

void ASerpant::ClearOnConstructValues()
{

    if(m_pSplineMeshes.Num() != 0)
    {
        for(USplineMeshComponent* pSplineMeshComp : m_pSplineMeshes)
        {
            if(pSplineMeshComp == nullptr)
            {
                continue;
            }

            pSplineMeshComp->DestroyComponent();
        }
        m_pSplineMeshes.Empty();
    }

    if(m_pSphereCollisions.Num() != 0)
    {
        for(USphereComponent* pSphereComp : m_pSphereCollisions)
        {
            if(pSphereComp == nullptr)
            {
                continue;
            }

            pSphereComp->DestroyComponent();
        }
        m_pSphereCollisions.Empty();
    }

    if(m_pPhysicsConstraints.Num() != 0)
    {
        for(UPhysicsConstraintComponent* pPhysicsConstraint : m_pPhysicsConstraints)
        {
            if(pPhysicsConstraint == nullptr)
            {
                continue;
            }

            pPhysicsConstraint->DestroyComponent();
        }
        m_pPhysicsConstraints.Empty();
    }
}
