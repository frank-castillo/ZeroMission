// Fill out your copyright notice in the Description page of Project Settings.


#include "NPlayerController.h"
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
#include <Engine/LocalPlayer.h>
#include <GameFramework/PlayerController.h>
#include <UMG/Public/Components/Widget.h>
#include "UMG/Public/Blueprint/UserWidget.h"
#include "NTopDownCharacter.h"

ANPlayerController::ANPlayerController()
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;
}

void ANPlayerController::TogglePauseMenu()
{
    if (PauseMenuInstance && PauseMenuInstance->IsInViewport())
    {
        PauseMenuInstance->RemoveFromViewport();
        PauseMenuInstance = nullptr;

        bShowMouseCursor = true;
        SetInputMode(FInputModeGameOnly());
        return;
    }

    PauseMenuInstance = CreateWidget<UUserWidget>(this, PauseMenuClass);

    if (PauseMenuInstance)
    {
        PauseMenuInstance->AddToViewport(100);

        bShowMouseCursor = true;
        SetInputMode(FInputModeUIOnly());
    }
}

void ANPlayerController::SetPawn(APawn* InPawn)
{
    Super::SetPawn(InPawn);

    OnPawnChanged.Broadcast(InPawn);
}

void ANPlayerController::BeginPlayingState()
{
    BlueprintBeginPlayingState();
}

void ANPlayerController::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    
    OnPlayerStateReceived.Broadcast(PlayerState);
}

void ANPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
    {
        // Map the actions here
        // Moving
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANPlayerController::Move);

        // Evading
        EnhancedInputComponent->BindAction(EvadeAction, ETriggerEvent::Triggered, this, &ANPlayerController::Evade);

        // Shooting
        EnhancedInputComponent->BindAction(WeaponShootAction, ETriggerEvent::Triggered, this, &ANPlayerController::Shoot);

        // In-Game Menu
        EnhancedInputComponent->BindAction(OpenInGameMenuAction, ETriggerEvent::Completed, this, &ANPlayerController::TogglePauseMenu);
    }
}

void ANPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Add Input Mapping Context
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }
}

void ANPlayerController::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();

    // Find which way is forward
    const FRotator Rotation = GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    UE_LOG(LogTemp, Verbose, TEXT("YawRotation values are: %s"), *YawRotation.ToString());

    // Get forward vector
    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

    UE_LOG(LogTemp, Verbose, TEXT("Forward direction values are: %s"), *ForwardDirection.ToString());

    // Get right vector
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    UE_LOG(LogTemp, Verbose, TEXT("RightDirection values are: %s"), *RightDirection.ToString());

    // Add movement - Need to get the Pawn this controller is attached to
    if (APawn* MyPawn = GetPawn())
    {
        MyPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
        MyPawn->AddMovementInput(RightDirection, MovementVector.X);
    }
}

void ANPlayerController::Evade(const FInputActionValue& Value)
{

}

void ANPlayerController::Shoot(const FInputActionValue& Value)
{

}