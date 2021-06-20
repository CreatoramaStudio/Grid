#pragma once

#include "CoreMinimal.h"
#include "HexagonGridSettings.generated.h"

class FEdModeGridEditor;

UCLASS()
class UHexagonGridSettings : public UObject
{
	GENERATED_BODY()

public:
	UHexagonGridSettings();
	virtual ~UHexagonGridSettings() override;

	void SetParent(FEdModeGridEditor* GridEditorMode);

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bShowGrids;

	UPROPERTY(EditAnywhere, Category = "Settings")
	int32 GridShowRange = 50;

	UPROPERTY(EditAnywhere, Category = "Settings")
	FVector GridCenter;

	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bGridSnap;

	UPROPERTY(EditAnywhere, Category = "Settings")
	float GridSize = 100;

	UPROPERTY(EditAnywhere, Category = "Settings")
	UMaterialInterface* DecalMaterial;

	UPROPERTY(EditAnywhere, Category = "Settings")
	UMaterialInterface* GridSensingVisualizerMaterial;

private:
	FEdModeGridEditor* ParentMode;
};
