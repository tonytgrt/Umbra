// Umbra - Light & Shadow Puzzle Game

#include "UmbraHUDWidget.h"
#include "Components/TextBlock.h"

void UUmbraHUDWidget::SetLevelName(const FText& Name)
{
    if (Level)
    {
        Level->SetText(Name);
    }
}