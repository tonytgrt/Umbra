#include "UmbraTouchThumbstick.h"
#include "Rendering/DrawElements.h"

void UUmbraTouchThumbstick::NativeConstruct()
{
    Super::NativeConstruct();
    SetVisibility(ESlateVisibility::Visible);
}

int32 UUmbraTouchThumbstick::NativePaint(const FPaintArgs& Args,
    const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
    FSlateWindowElementList& OutDrawElements, int32 LayerId,
    const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    const FVector2D LocalSize = AllottedGeometry.GetLocalSize();
    const FVector2D Center = LocalSize * 0.5f;
    const float Opacity = bIsTouching ? ActiveOpacity : IdleOpacity;

    // Draw base circle
    const FLinearColor BaseColor(1.f, 1.f, 1.f, Opacity * 0.4f);
    // UMG doesn't have a circle primitive �� use a box brush as placeholder.
    // For proper visuals, override in a Widget Blueprint child with a circular image.
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(
            FVector2f(BaseRadius * 2.f, BaseRadius * 2.f),
            FSlateLayoutTransform(FVector2f(Center.X - BaseRadius, Center.Y - BaseRadius))
        ),
        FCoreStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        BaseColor
    );

    // Draw knob
    const FVector2D KnobCenter = Center + KnobOffset;
    const FLinearColor KnobColor(1.f, 1.f, 1.f, Opacity);
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId + 1,
        AllottedGeometry.ToPaintGeometry(
            FVector2f(KnobRadius * 2.f, KnobRadius * 2.f),
            FSlateLayoutTransform(FVector2f(KnobCenter.X - KnobRadius, KnobCenter.Y - KnobRadius))
        ),
        FCoreStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        KnobColor
    );

    return LayerId + 2;
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
        StickInput = KnobOffset.GetSafeNormal() * FMath::Clamp(Remapped, 0.f, 1.f);
    }
}