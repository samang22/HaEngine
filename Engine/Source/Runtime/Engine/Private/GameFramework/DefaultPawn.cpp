#include "GameFramework/DefaultPawn.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"

void InitializeDefaultPawnInputBindings()
{
    static bool bBindingsAdded = false;
    if (!bBindingsAdded)
    {
        bBindingsAdded = true;

        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping(TEXT("DefaultPawn_MoveForward"), EKeys::W, 1.f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping(TEXT("DefaultPawn_MoveForward"), EKeys::S, -1.f));

        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping(TEXT("DefaultPawn_MoveRight"), EKeys::A, -1.f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping(TEXT("DefaultPawn_MoveRight"), EKeys::D, 1.f));

        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping(TEXT("DefaultPawn_Turn"), EKeys::MouseX, 1.f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping(TEXT("DefaultPawn_LookUp"), EKeys::MouseY, -1.f));
    }
}

ADefaultPawn::ADefaultPawn()
{
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent0"));
    RootComponent = MeshComponent->As<USceneComponent>();
}

void ADefaultPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    InitializeDefaultPawnInputBindings();

    PlayerInputComponent->BindAxis(TEXT("DefaultPawn_MoveForward"), this, &ADefaultPawn::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("DefaultPawn_MoveRight"), this, &ADefaultPawn::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("DefaultPawn_Turn"), this, &ADefaultPawn::AddControllerYawInput);
    PlayerInputComponent->BindAxis(TEXT("DefaultPawn_LookUp"), this, &ADefaultPawn::AddControllerPitchInput);
}

void ADefaultPawn::MoveForward(float Val)
{
    if (Val != 0.f)
    {
        if (Controller)
        {
            FRotator const ControlSpaceRot = Controller->GetControlRotation();
            FTransform Transform = FTransform(ControlSpaceRot, FVector3D::Zero, FVector3D::One);
            FVector3D ForwardVector = Transform.GetScaledAxis(EAxis::X);

            FVector ActorLocation = GetActorLocation();
            ActorLocation = ActorLocation + ForwardVector * Val * Speed;
            SetActorLocation(ActorLocation);

            // transform to world space and add it
            //AddMovementInput( FRotationMatrix(ControlSpaceRot).GetScaledAxis( EAxis::X ), Val );
        }
    }
}

void ADefaultPawn::MoveRight(float Val)
{
    if (Val != 0.f)
    {
        if (Controller)
        {
            FRotator const ControlSpaceRot = Controller->GetControlRotation();
            FTransform Transform = FTransform(ControlSpaceRot, FVector3D::Zero, FVector3D::One);
            FVector3D ForwardVector = Transform.GetScaledAxis(EAxis::Y);

            FVector ActorLocation = GetActorLocation();
            ActorLocation = ActorLocation + ForwardVector * Val * Speed;
            SetActorLocation(ActorLocation);

            // transform to world space and add it
            //AddMovementInput(FRotationMatrix(ControlSpaceRot).GetScaledAxis(EAxis::Y), Val);
        }
    }
}

void ADefaultPawn::AddControllerYawInput(float Val)
{
    if (Val != 0.f && Controller /*&& Controller->IsLocalPlayerController()*/)
    {
        APlayerController* const PC = CastChecked<APlayerController>(Controller);
        PC->AddYawInput(Val);
    }
}

void ADefaultPawn::AddControllerPitchInput(float Val)
{
    if (Val != 0.f && Controller /*&& Controller->IsLocalPlayerController()*/)
    {
        APlayerController* const PC = CastChecked<APlayerController>(Controller);
        PC->AddPitchInput(Val);
    }
}