#include "UmbraTouchThumbstick.h"
#include "Rendering/DrawElements.h"

void UUmbraTouchThumbstick::NativeConstruct()
{
    Super::NativeConstruct();
    SetVisibility(ESlateVisibility::Visible);
    SetMinimumDesiredSize(FVector2D(BaseRadius * 2.f, BaseRadius * 2.f));
}

int32 UUmbraTouchThumbstick::NativePaint(const FPaintArgs& Args,
    const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
    FSlateWindowElementList& OutDrawElements, int32 LayerId,
    const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    const FVector2D LocalSize = AllottedGeometry.GetLocalSize();
    const FVector2D Center = LocalSize * 0.5f;
    const float Opacity = bIsTouching ? ActiveOpacity : IdleOpacity;
    constexpr int32 NumSegments = 32;

    // Helper lambda: build a circle as a line loop
    auto MakeCirclePoints = [&](const FVector2D& CircleCenter, float Radius) -> TArray<FVector2f>
    {
        TArray<FVector2f> Points;
        Points.Reserve(NumSegments + 1);
        for (int32 i = 0; i <= NumSegments; ++i)
        {
            const float Angle = (static_cast<float>(i) / NumSegments) * 2.f * PI;
            Points.Add(FVector2f(
                CircleCenter.X + FMath::Cos(Angle) * Radius,
                CircleCenter.Y + FMath::Sin(Angle) * Radius));
        }
        return Points;
    };

    // Draw base circle outline
    const FLinearColor BaseColor(1.f, 1.f, 1.f, Opacity * 0.4f);
    TArray<FVector2f> BasePoints = MakeCirclePoints(Center, BaseRadius);
    FSlateDrawElement::MakeLines(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(),
        BasePoints,
        ESlateDrawEffect::None,
        BaseColor,
        true,  // bAntialias
        2.f    // Thickness
    );

    // Draw knob circle (filled via concentric rings)
    const FVector2D KnobCenter = Center + KnobOffset;
    const FLinearColor KnobColor(1.f, 1.f, 1.f, Opacity * 0.5f);
    // Fill by drawing concentric circles from center outward
    for (float R = 1.f; R <= KnobRadius; R += 2.f)
    {
        TArray<FVector2f> FillPoints = MakeCirclePoints(KnobCenter, R);
        FSlateDrawElement::MakeLines(
            OutDrawElements,
            LayerId + 1,
            AllottedGeometry.ToPaintGeometry(),
            FillPoints,
            ESlateDrawEffect::None,
            KnobColor,
            true,
            2.f
        );
    }

    // Draw knob outline
    const FLinearColor KnobOutline(1.f, 1.f, 1.f, Opacity);
    TArray<FVector2f> KnobPoints = MakeCirclePoints(KnobCenter, KnobRadius);
    FSlateDrawElement::MakeLines(
        OutDrawElements,
        LayerId + 2,
        AllottedGeometry.ToPaintGeometry(),
        KnobPoints,
        ESlateDrawEffect::None,
        KnobOutline,
        true,
        2.f
    );

    return LayerId + 3;
}

FReply UUmbraTouchThumbstick::NativeOnTouchStarted(const FGeometry& InGeometry,
    const FPointerEvent& InGestureEvent)
{
    if (bIsTouching) return FReply::Unhandled();

    const FVector2D LocalPos = InGeometry.AbsoluteToLocal(
        InGestureEvent.GetScreenSpacePosition());
    const FVector2D LocalSize = InGeometry.GetLocalSize();
    BaseCenter = LocalSize * 0.5f;

    const float Dist = FVector2D::Distance(LocalPos, BaseCenter);
    if (Dist > BaseRadius * 1.5f) return FReply::Unhandled();

    bIsTouching = true;
    TrackedFingerIndex = InGestureEvent.GetPointerIndex();

    KnobOffset = LocalPos - BaseCenter;
    if (KnobOffset.Size() > BaseRadius)
    {
        KnobOffset = KnobOffset.GetSafeNormal() * BaseRadius;
    }
    UpdateStickInput();

    return FReply::Handled().CaptureMouse(GetCachedWidget().ToSharedRef());
}

FReply UUmbraTouchThumbstick::NativeOnTouchMoved(const FGeometry& InGeometry,
    const FPointerEvent& InGestureEvent)
{
    if (!bIsTouching || InGestureEvent.GetPointerIndex() != TrackedFingerIndex)
        return FReply::Unhandled();

    const FVector2D LocalPos = InGeometry.AbsoluteToLocal(
        InGestureEvent.GetScreenSpacePosition());

    KnobOffset = LocalPos - BaseCenter;
    if (KnobOffset.Size() > BaseRadius)
    {
        KnobOffset = KnobOffset.GetSafeNormal() * BaseRadius;
    }
    UpdateStickInput();

    return FReply::Handled();
}

FReply UUmbraTouchThumbstick::NativeOnTouchEnded(const FGeometry& InGeometry,
    const FPointerEvent& InGestureEvent)
{
    if (InGestureEvent.GetPointerIndex() != TrackedFingerIndex)
        return FReply::Unhandled();

    bIsTouching = false;
    TrackedFingerIndex = -1;
    KnobOffset = FVector2D::ZeroVector;
    StickInput = FVector2D::ZeroVector;

    return FReply::Handled().ReleaseMouseCapture();
}

// --- Mouse handlers for PC debugging ---

FReply UUmbraTouchThumbstick::NativeOnMouseButtonDown(const FGeometry& InGeometry,
    const FPointerEvent& InMouseEvent)
{
    if (bIsTouching) return FReply::Unhandled();

    const FVector2D LocalPos = InGeometry.AbsoluteToLocal(
        InMouseEvent.GetScreenSpacePosition());
    const FVector2D LocalSize = InGeometry.GetLocalSize();
    BaseCenter = LocalSize * 0.5f;

    const float Dist = FVector2D::Distance(LocalPos, BaseCenter);
    if (Dist > BaseRadius * 1.5f) return FReply::Unhandled();

    bIsTouching = true;
    TrackedFingerIndex = 0;

    KnobOffset = LocalPos - BaseCenter;
    if (KnobOffset.Size() > BaseRadius)
    {
        KnobOffset = KnobOffset.GetSafeNormal() * BaseRadius;
    }
    UpdateStickInput();

    return FReply::Handled().CaptureMouse(GetCachedWidget().ToSharedRef());
}

FReply UUmbraTouchThumbstick::NativeOnMouseMove(const FGeometry& InGeometry,
    const FPointerEvent& InMouseEvent)
{
    if (!bIsTouching) return FReply::Unhandled();

    const FVector2D LocalPos = InGeometry.AbsoluteToLocal(
        InMouseEvent.GetScreenSpacePosition());

    KnobOffset = LocalPos - BaseCenter;
    if (KnobOffset.Size() > BaseRadius)
    {
        KnobOffset = KnobOffset.GetSafeNormal() * BaseRadius;
    }
    UpdateStickInput();

    return FReply::Handled();
}

FReply UUmbraTouchThumbstick::NativeOnMouseButtonUp(const FGeometry& InGeometry,
    const FPointerEvent& InMouseEvent)
{
    if (!bIsTouching) return FReply::Unhandled();

    bIsTouching = false;
    TrackedFingerIndex = -1;
    KnobOffset = FVector2D::ZeroVector;
    StickInput = FVector2D::ZeroVector;

    return FReply::Handled().ReleaseMouseCapture();
}

void UUmbraTouchThumbstick::UpdateStickInput()
{
    const float Magnitude = KnobOffset.Size() / BaseRadius;
    if (Magnitude < DeadZone)
    {
        StickInput = FVector2D::ZeroVector;
    }
    else
    {
        // Remap [DeadZone..1] to [0..1]
        const float Remapped = (Magnitude - DeadZone) / (1.f - DeadZone);
        const FVector2D Dir = KnobOffset.GetSafeNormal() * FMath::Clamp(Remapped, 0.f, 1.f);
        // Screen space: X=right, Y=down.
        // Pawn expects: X=forward(screen-up), Y=right(screen-right).
        // Swap and negate to match.
        StickInput = FVector2D(-Dir.Y, Dir.X);
    }
}