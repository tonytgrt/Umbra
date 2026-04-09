#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UmbraPauseWidget.generated.h"

class UButton;

UCLASS(Abstract)
class UUmbraPauseWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ResumeButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ExitToMenuButton;

    UFUNCTION()
    void OnResumeClicked();

    UFUNCTION()
    void OnExitToMenuClicked();
};