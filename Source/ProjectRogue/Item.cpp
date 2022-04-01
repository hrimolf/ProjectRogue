#include "Item.h"

UItem::UItem()
{
}

UItem::~UItem()
{
}

void UItem::Init(const FString& InName, float InValue, const TArray<int32> InDiceFaces, const FString& InDescription)
{
    DiceBag = NewObject<UDiceBag>();
    Name = FText::FromString(InName);
    Value = InValue;
    Description = FText::FromString(InDescription);
    DiceBag->SetDice(InDiceFaces);
    bConsumesSpace = true;
    SubType = EItemType::None;
}
