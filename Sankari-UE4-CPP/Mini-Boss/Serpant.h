// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Serpant.generated.h"

class UStaticMesh;
class USplineComponent;
class USphereComponent;
class UPhysicalMaterial;
class UMaterialInterface;
class USplineMeshComponent;
class UPhysicsConstraintComponent;
struct FLinearColor;

USTRUCT()
struct FSplinePointLocTangent
{
    GENERATED_USTRUCT_BODY();

    FVector m_Location{0.0f,0.0f,0.0f};
    FVector m_Tangent{0.0f,0.0f,0.0f};
};

UCLASS(ClassGroup = (Serpant))
class SANKARI_API ASerpant final : public AActor
{
    GENERATED_BODY()

public:

    // ---- Constructors ----
    ASerpant();

    // ---- Functionality ----

    /// @brief Add the required collision spheres
    UFUNCTION(BlueprintCallable , Category = "Serpant")
    void AddCollisionSpheres();

    /// @brief add the required physics constraints between each collision sphere
    /// @attention Call @See ASerpant::AddCollisionSpheres() first
    UFUNCTION(BlueprintCallable , Category = "Serpant")
    void AddPhysicsConstraints();

    /// @brief add spline component which is used to place the meshes
    UFUNCTION(BlueprintCallable , Category = "Serpant")
    void AddSplineComponent();

    /// @brief add head, body, tail meshes and materials
    UFUNCTION(BlueprintCallable , Category = "Serpant")
    void AddSplineMeshes();

    /// @brief Get the head collision sphere reference
    /// @note used for moving the creature
    UFUNCTION(BlueprintPure,BlueprintCallable , Category = "Serpant")
    FORCEINLINE USphereComponent* GetHeadReference() const { return m_pHeadRef; }

    // -- Virtual Functions --
    void BeginPlay() final;
    void Tick(float DeltaTime) final;
    void OnConstruction(const FTransform& Transform) final;

    // ---- Data members ----

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Properties | body" , meta = (ClampMin = "4" , UIMin = "4" , DisplayName = "Amount Nodes"))
    int m_AmountNodes{10};

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Properties | body" , meta = (ClampMin = "0.01" , UIMin = "0.01" , DisplayName = "Radius Body"))
    float m_Radius{20.0f};

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Properties | body" , meta = (ClampMin = "0.0" , UIMin = "0.0" , DisplayName = "Spacing Body"))
    float m_Spacing{20.0f};

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Properties | body" , meta = (ClampMin = "0.0" , UIMin = "0.0" , ClampMax = "1.0" , UIMax = "1.0" , DisplayName = "Start Scaling Down Ratio"))
    float m_StartScalingDownRatio{1.0f};

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Properties | body" , meta = (ClampMin = "0.01" , UIMin = "0.01" , ClampMax = "1.0" , UIMax = "1.0" , DisplayName = "Last Sphere Radius Ratio"))
    float m_LastSphereRadiusRatio{1.0f};

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Properties | body" , meta = (ClampMin = "0.0" , UIMin = "0.0" , DisplayName = "Linear Damping"))
    float m_LinearDamping{1.5f};

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Properties | body" , meta = (ClampMin = "0.0" , UIMin = "0.0" , DisplayName = "Angular Damping"))
    float m_AngularDamping{0.8f};

    #if WITH_EDITORONLY_DATA

    UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = "Properties | debug" , meta = (DisplayName = "Arrow Length"))
    int m_ArrowLength{-300};

    UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = "Properties | debug" , meta = (ClampMin = "0.1" , UIMin = "0.1" , DisplayName = "Arrow Head Size"))
    float m_ArrowHeadSize{5000.f};

    UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = "Properties | debug" , meta = (ClampMin = "0.1" , UIMin = "0.1" , DisplayName = "Arrow Thickness"))
    float m_ArrowThickness{15.0f};

    UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = "Properties | debug" , meta = (ClampMin = "0.1" , UIMin = "0.1" , DisplayName = "Sphere Thickness"))
    float m_SphereThickness{1.5f};

    UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = "Properties | debug" , meta = (ClampMin = "0.1" , UIMin = "0.1" , DisplayName = "Line Thickness"))
    float m_LineThickness{0.2f};

    UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = "Properties | debug" , meta = (DisplayName = "Sphere Color"))
    FLinearColor m_SphereColor { 1.0f,0.5f,0.0f,1.0f };

    UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = "Properties | debug" , meta = (DisplayName = "Line Color"))
    FLinearColor m_LineColor { 1.0f,0.0f,1.0f,1.0f };

    UPROPERTY(EditAnywhere , BlueprintReadOnly , Category = "Properties | debug" , meta = (DisplayName = "Arrow Color"))
    FLinearColor m_ArrowColor { 0.0f,1.0f,1.0f,1.0f };

    #endif //WITH_EDITORONLY_DATA

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Properties | physics" , meta = (DisplayName = "Physics Material"))
    UPhysicalMaterial* m_pPhysicsMaterial { nullptr };

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Properties | mesh" , meta = (DisplayName = "Head Mesh"))
    UStaticMesh* m_pHeadMesh { nullptr };

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Properties | mesh" , meta = (DisplayName = "Body Mesh"))
    UStaticMesh* m_pBodyMesh { nullptr };

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Properties | mesh" , meta = (DisplayName = "Tail Mesh"))
    UStaticMesh* m_pTailMesh { nullptr };

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Properties | material" , meta = (DisplayName = "Head Material"))
    UMaterialInterface* m_pHeadMaterial { nullptr };

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Properties | material" , meta = (DisplayName = "Body Material"))
    UMaterialInterface* m_pBodyMaterial { nullptr };

    UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "Properties | material" , meta = (DisplayName = "Tail Material"))
    UMaterialInterface* m_pTailMaterial { nullptr };

private:

    /// @brief update the spline which is used to place the meshes correctly
    void UpdateSplineComponent();

    /// @brief uses the spline to update the meshes in the correct rotation / position
    void UpdateSnakeMesh();

    #if WITH_EDITOR
    /// @brief turn mesh visibility on/off
    /// @note editor only
    void SetMeshVisibility();
    /// @brief draw debug shapes for the spherical shapes, spline, physics constraint and forward vector
    /// @note editor only
    void DrawDebugShapes();
    #endif //WITH_EDITOR

    /// @brief clear on construct values
    void ClearOnConstructValues();

    // ---- Data members ----

    #if WITH_EDITOR 
    bool m_bIsMeshVisible{true};
    #endif //WITH_EDITOR

    USphereComponent* m_pHeadRef;
    TArray<FSplinePointLocTangent> m_SplinePointsLocAndTang;

    UPROPERTY()
    USplineComponent* m_pSplineComp;

    UPROPERTY()
    TArray<USphereComponent*> m_pSphereCollisions;

    UPROPERTY()
    TArray<UPhysicsConstraintComponent*> m_pPhysicsConstraints;

    UPROPERTY()
    TArray<USplineMeshComponent*> m_pSplineMeshes;
};
