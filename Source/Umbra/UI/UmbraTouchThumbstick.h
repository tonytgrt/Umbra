#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UmbraTouchThumbstick.generated.h"

class AUmbraPawn;

/**
 *  Virtual thumbstick widget for mobile touch input.
 *  Placed on the left side of the screen. Outputs a normalized
 *  FVector2D that drives pawn movement.
 */
UCLASS()
class UUmbraTouchThumbstick : public UUserWidget
{
    GENERATED_BODY()

public:
    /** The current stick direction (length 0..1). Read by the controller each tick. */
    UFUNCTION(BlueprintPure, Category = "Touch")
    FVector2D GetStickInput() const { return StickInput; }

protected:
    virtual void NativeConstruct() override;
    virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
        int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

    virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry,
        const FPointerEvent& InGestureEvent) override;
    virtual FReply NativeOnTouchMoved(const FGeometry& InGeometry,
        const FPointerEvent& InGestureEvent) override;
    virtual FReply NativeOnTouchEnded(const FGeometry& InGeometry,
        const FPointerEvent& InGestureEvent) override;

    // Mouse handlers for PC debugging (forward to the same stick logic)
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry,
        const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseMove(const FGeometry& InGeometry,
        const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry,
        const FPointerEvent& InMouseEvent) override;

private:
    /** Radius of the outer base circle in pixels. */
    UPROPERTY(EditAnywhere, Category = "Thumbstick")
    float BaseRadius = 80.f;

    /** Radius of the inner knob circle in pixels. */
    UPROPERTY(EditAnywhere, Category = "Thumbstick")
    float KnobRadius = 30.f;

    /** Dead zone as a fraction of BaseRadius (0..1). Below this, input is zero. */
    UPROPERTY(EditAnywhere, Category = "Thumbstick", meta = (ClampMin = "0", ClampMax = "0.5"))
    float DeadZone = 0.1f;

    /** Opacity of the thumbstick when idle / when touched. */
    UPROPERTY(EditAnywhere, Category = "Thumbstick")
    float IdleOpacity = 0.3f;

    UPROPERTY(EditAnywhere, Category = "Thumbstick")
    float ActiveOpacity = 0.7f;

    /** Center of the base circle in local widget space. */
    FVector2D BaseCenter;

    /** Current knob offset from BaseCenter (pixels). */
    FVector2D KnobOffset = FVector2D::ZeroVector;

    /** Normalized stick output. */
    FVector2D StickInput = FVector2D::ZeroVector;

    /** Whether a finger is currently on the stick. */
    bool bIsTouching = false;

    /** Finger index we're tracking (to ignore other fingers). */
    int32 TrackedFingerIndex = -1;

    /** Recalculate StickInput from KnobOffset. */
    void UpdateStickInput();
};