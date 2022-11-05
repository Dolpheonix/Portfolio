// Fill out your copyright notice in the Description page of Project Settings.


#include "Chain.h"

// Sets default values
AChain::AChain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> chainMesh(TEXT("/Game/ShootingGame/Asset/ChainSegment.ChainSegment"));
	if (chainMesh.Succeeded())
	{
		ChainMesh = chainMesh.Object;

		for (int i = 0; i < Segments; i++)
		{
			FString chainIndex = TEXT("Chain_") + FString::FromInt(i);
			FString jointIndex = TEXT("Joint_") + FString::FromInt(i-1) + TEXT("_") + FString::FromInt(i);

			Chains.Add(CreateDefaultSubobject<UStaticMeshComponent>(FName(chainIndex)));
			Chains[i]->SetStaticMesh(ChainMesh);
			Chains[i]->SetRelativeLocationAndRotation(FVector(200.0f * i, 0.0f, 0.0f), FRotator(0.0f, 90.0f, 0.0f));
			Chains[i]->SetupAttachment(RootComponent);
			Chains[i]->SetSimulatePhysics(true);

			if (i > 0)
			{
				Joints.Add(CreateDefaultSubobject<UPhysicsConstraintComponent>(FName(jointIndex)));
				Joints[i-1]->SetRelativeLocation(FVector(200.0f * i, 0.0f, 0.0f));

				FConstrainComponentPropName c1;
				c1.ComponentName = FName(Chains[i - 1]->GetName());
				FConstrainComponentPropName c2;
				c2.ComponentName = FName(Chains[i]->GetName());

				Joints[i-1]->ComponentName1 = c1;
				Joints[i-1]->ComponentName2 = c2;
				Joints[i-1]->SetupAttachment(RootComponent);
			}
		}
	}
}

// Called when the game starts or when spawned
void AChain::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AChain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChain::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FString Name = PropertyChangedEvent.Property->GetName();
	if (Name == TEXT("Segments"))
	{
		UpdateSegments();
	}
	else if (Name == TEXT("ChainMesh"))
	{
		for (int i = 0; i < Segments; i++)
		{
			Chains[i]->SetStaticMesh(ChainMesh);
		}
	}
	else if (Name == TEXT("L_BindActor"))
	{
		UpdateBind(true);
	}
	else if (Name == TEXT("R_BindActor"))
	{
		UpdateBind(false);
	}
}

void AChain::UpdateSegments()
{
	// Segments can't be smaller than 1
	if (Segments < 1)
	{
		Segments = Segments_before;
	}
	// Add Segment Components
	else if (Segments > Segments_before)
	{
		for (int i = Segments_before; i < Segments; i++)
		{
			FString chainIndex = TEXT("Chain_") + FString::FromInt(i);
			FString jointIndex = TEXT("Joint_") + FString::FromInt(i - 1) + TEXT("_") + FString::FromInt(i);

			Chains.Add(NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), FName(chainIndex)));
			Chains[i]->SetStaticMesh(ChainMesh);
			Chains[i]->SetRelativeLocationAndRotation(FVector(200.0f * i, 0.0f, 0.0f), FRotator(0.0f, 90.0f, 0.0f));
			Chains[i]->SetupAttachment(RootComponent);
			Chains[i]->SetSimulatePhysics(true);
			Chains[i]->RegisterComponent();

			Joints.Add(NewObject<UPhysicsConstraintComponent>(this, UPhysicsConstraintComponent::StaticClass(), FName(jointIndex)));
			Joints[i - 1]->SetRelativeLocation(FVector(200.0f * i, 0.0f, 0.0f));

			FConstrainComponentPropName c1;
			c1.ComponentName = FName(Chains[i - 1]->GetName());
			FConstrainComponentPropName c2;
			c2.ComponentName = FName(Chains[i]->GetName());

			Joints[i - 1]->ComponentName1 = c1;
			Joints[i - 1]->ComponentName2 = c2;
			Joints[i - 1]->SetupAttachment(RootComponent);
			Joints[i - 1]->RegisterComponent();
		}
	}
	// Remove Segment Components
	else
	{
		for (int i = Segments_before - 1; i >= Segments; i--)
		{
			Chains[i]->DestroyComponent();
			Chains.RemoveAt(i);
			Joints[i - 1]->DestroyComponent();
			Joints.RemoveAt(i - 1);
		}
	}

	if (R_BindActor)
	{
		FString lastChain = TEXT("Chain_") + FString::FromInt(Segments - 1);
		R_Constraint->ComponentName1.ComponentName = FName(lastChain);
	}

	Segments_before = Segments;
}

void AChain::UpdateBind(bool isLeft)
{
	if (isLeft)
	{
		if (!L_BindActor) // Binded Actor disappeared
		{
			L_Constraint->DestroyComponent();
		}
		else
		{
			L_Constraint = NewObject<UPhysicsConstraintComponent>(this, UPhysicsConstraintComponent::StaticClass(), TEXT("Left Binder"));
			L_Constraint->ConstraintActor1 = L_BindActor;
			L_Constraint->ComponentName2.ComponentName = TEXT("Chain_0");
			L_Constraint->SetupAttachment(RootComponent);
			L_Constraint->RegisterComponent();
		}
	}
	else
	{
		if (!R_BindActor)
		{
			R_Constraint->DestroyComponent();
		}
		else
		{
			FString lastChain = TEXT("Chain_") + FString::FromInt(Segments - 1);
			R_Constraint = NewObject<UPhysicsConstraintComponent>(this, UPhysicsConstraintComponent::StaticClass(), TEXT("Right Binder"));
			R_Constraint->ConstraintActor2 = R_BindActor;
			R_Constraint->ComponentName1.ComponentName = FName(lastChain);
			R_Constraint->SetupAttachment(RootComponent);
			R_Constraint->RegisterComponent();
		}
	}
	AdjustSegments();
}

void AChain::AdjustSegments()
{
	if (L_BindActor && R_BindActor)
	{
		SetActorLocation(L_BindActor->GetActorLocation());
		FVector dist = R_BindActor->GetActorLocation() - L_BindActor->GetActorLocation();
		if (dist.Size() > 200.0f * Segments)
		{
			int newSegments = FMath::CeilToInt(dist.Size() / 200.0f);
			Segments = newSegments;
			UpdateSegments();
		}

	}
	else if (L_BindActor)
	{
		SetActorLocation(L_BindActor->GetActorLocation());
	}
	else if (R_BindActor)
	{
		FVector origin = R_BindActor->GetActorLocation() - FVector(200.0f * Chains.Num(), 0.f, 0.f);
		SetActorLocation(origin);
		R_Constraint->SetRelativeLocation(FVector(200.0f * Segments, 0.f, 0.f));
	}
	else
	{

	}
}