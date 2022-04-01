#include "Adventurer.h"
#include "PartyController.h"
#include "ProjectRogueCharacter.h"
#include "DiceBag.h"

UAdventurer::UAdventurer()
{
    Stats.Add(EStat::Strength, 0);
    Stats.Add(EStat::Intelligence, 0);
    Stats.Add(EStat::Wisdom, 0);
    Stats.Add(EStat::Dexterity, 0);
    Stats.Add(EStat::Constitution, 0);
}

UAdventurer::~UAdventurer()
{
}

void UAdventurer::Update(FText InName, ERace InRace, EClass InClass, FSlateBrush InIcon)
{
    Name = InName;
    Race = InRace;
    Class = InClass;
    Icon = InIcon;

    ClearAllowedEquipment();
    Spells.Empty();

    switch (Class)
    {
        case EClass::Magician:  
        {
            AddAllowedEquipment("Staff");
            AddAllowedEquipment("Dagger");
            AddAllowedEquipment("Robes");
            AddAllowedEquipment("Mithril Staff");
            AddAllowedEquipment("Mithril Dagger");
            AddAllowedEquipment("Mithril Robes");
            
            CurrentMana = TentativeSpellPoints; 

            Spells = PartyController->GetPlayer()->GetSpells(EClass::Magician);
            break;
        }
        case EClass::Cleric:    
        {
            AddAllowedEquipment("Staff");
            AddAllowedEquipment("Dagger");
            AddAllowedEquipment("Robes");
            AddAllowedEquipment("Mithril Staff");
            AddAllowedEquipment("Mithril Dagger");
            AddAllowedEquipment("Mithril Robes");
            CurrentMana = TentativePrayerPoints; 

            Spells = PartyController->GetPlayer()->GetSpells(EClass::Cleric);
            break;
        }
    }

    MaxMana = CurrentMana;
}

void UAdventurer::AddSpell(ASpell* Spell)
{
    check(Spell);
    Spells.Add(Spell);
}

void UAdventurer::OnAttack(FTimerManager& TimerManager, FTimerDelegate AttackDelegate)
{
    TimerDelay = AttackDelay - static_cast<float>(GetStatModifier(EStat::Dexterity));
    Super::OnAttack(TimerManager,AttackDelegate);
}

bool UAdventurer::OnEquip(const UItem* InItem)
{
    FString ItemName = InItem->GetItemName().ToString();
    if (AllowedEquipment.Num() > 0)
    {
        for (auto& Item : AllowedEquipment)
        {
            if (Item == ItemName)
            {
                GEngine->AddOnScreenDebugMessage(2, 2, FColor::Green, "Equipped " + ItemName + "!");
                return true;
            }
        }
        GEngine->AddOnScreenDebugMessage(2, 3, FColor::Yellow, "Cannot equip " + ItemName + ".");
        return false;
    }
    GEngine->AddOnScreenDebugMessage(2, 2, FColor::Green, "Equipped " + ItemName + "!");
	return true;
}

void UAdventurer::AddAllowedEquipment(const FString& InName)
{
    auto Itr = AllowedEquipment.Find(InName);
    if (Itr == INDEX_NONE)
    {
        AllowedEquipment.Add(InName);
    }
}

void UAdventurer::RandomizeStats()
{
    // Stats
    UDiceBag* DiceBag = NewObject<UDiceBag>();
    DiceBag->SetDice({6, 6, 6});
    for (auto& Stat : Stats)
    {
        // We will ALWAYS roll 3D6 for stats
        Stat.Value = DiceBag->Roll();
    }

    // Combat Data
    auto& Data = GetCombatData(Race);
    DiceBag->Clear();

    if (Data.Num() == 0)
    {
        //M: If there's no combat data established, we let the
        //  engine run, but warn the user
        UE_LOG(LogTemp, Display, TEXT("Combat Data not populated! Please fill in this data on the Character blueprint."))
        return;
    }    

    //T: get the specific dice to roll for each combat stat
    auto& HealthDice = *Data.Find(ECombatDataType::HitPoints);
    TArray<int32> Dice;
    Dice.Init(HealthDice.DieFaces, HealthDice.DiceThrown);
    DiceBag->SetDice(Dice); 

    MaxHealth = DiceBag->Roll() + GetStatModifier(EStat::Constitution);
    if (MaxHealth < 1)
        MaxHealth = 1;
    CurrentHealth = MaxHealth;

    TentativeSpellPoints = RollManaPoints(ECombatDataType::SpellPoints, EStat::Intelligence);
    TentativePrayerPoints = RollManaPoints(ECombatDataType::PrayerPoints, EStat::Wisdom);
    
    // Gold
    DiceBag->Clear();
    DiceBag->SetDice({4});
    Gold = DiceBag->Roll();

    Update(Name, Race, Class, Icon);
}

void UAdventurer::TransferGoldToParty()
{
    PartyController->AdjustGold(Gold);
    Gold = 0;
}

int32 UAdventurer::GetStatModifier(EStat Stat)
{
    int32 Value = Stats.FindOrAdd(Stat);

    if(Value <= 3)
        return -3;
    else if(Value >= 18)
        return 3;

    switch (Value)
    {
        case 4:
        case 5: return -2;
        case 6:
        case 7:
        case 8: return -1;
        case 9:
        case 10:
        case 11:
        case 12: return 0;
        case 13:
        case 14:
        case 15: return 1;
        case 16:
        case 17: return 2;
    }
    return 0;
}

const TMap<ECombatDataType, FCombatDice>& UAdventurer::GetCombatData(ERace InRace)
{
    return PartyController->GetRaceCombatData(InRace);
}

const TMap<ECombatDataType, FCombatDice>& UAdventurer::GetCombatData(EClass InClass)
{
    return PartyController->GetClassCombatData(InClass);
}

bool UAdventurer::UseItem(const int32 InventoryIndex)
{
    if (InventoryItems.Num() == 0)
        return false;
    check(InventoryIndex < InventoryItems.Num());
    auto Item = InventoryItems[InventoryIndex];
    if (!Item)
        return false;
    Item->OnUse(this, InventoryIndex);
    if (Item->GetType() == EItemType::Consumable || Item->GetType() == EItemType::Effect)
    {
        InventoryItems.RemoveAt(InventoryIndex);
    }
    return true;
}

int32 UAdventurer::RollManaPoints(ECombatDataType ManaType, EStat Stat)
{
    auto& Data = GetCombatData(Race);
    auto& ManaDice = *Data.Find(ManaType);
    UDiceBag* DiceBag = NewObject<UDiceBag>();
    
    TArray<int32> Dice;
    Dice.Init(ManaDice.DieFaces, ManaDice.DiceThrown);
    DiceBag->SetDice(Dice);
    
    int32 Mana = DiceBag->Roll() + GetStatModifier(Stat);
    if (Mana < 1)
        Mana = 1;

    return Mana;
}

bool UAdventurer::CanLevelUp()
{
    check(PartyController);
    return PartyController->CanLevelUp(this);
}

int UAdventurer::GetExpToLevel()
{
    check(PartyController);
    return PartyController->GetExpToLevel(this);
}

void UAdventurer::AddStatPoint(EStat Stat)
{
    if (!CanLevelUp())
        return;
    ++Stats.FindOrAdd(Stat);
    LevelUp();
}

bool UAdventurer::LevelUp()
{
    check(PartyController);
    if (PartyController->LevelUp(this))
    {
        AddLevel();
        auto& Data = GetCombatData(Class);
        UDiceBag* DiceBag = NewObject<UDiceBag>();
        TArray<int32> Dice;
        int32 ManaIncrease = 0;
        if (Class == EClass::Magician)
        {
            ManaIncrease = RollManaIncrease(Data, ECombatDataType::SpellPoints, DiceBag);

        }
        else if (Class == EClass::Cleric)
        {
            ManaIncrease = RollManaIncrease(Data, ECombatDataType::PrayerPoints, DiceBag);
        }

        if (ManaIncrease < 0)
            ManaIncrease = 0;
        IncreaseMaxMana(ManaIncrease);

        auto HealthDice = *Data.Find(ECombatDataType::HitPoints);
        Dice.Init(HealthDice.DieFaces, HealthDice.DiceThrown);
        DiceBag->SetDice(Dice);
        int32 HealthIncrease = DiceBag->Roll() + GetStatModifier(EStat::Constitution);
        if (HealthIncrease < 0)
            HealthIncrease = 0;
        IncreaseMaxHealth(HealthIncrease);
    }

    return CanLevelUp();
}

int32 UAdventurer::RollManaIncrease(const TMap<ECombatDataType, FCombatDice>& Data, ECombatDataType DataType, UDiceBag* DiceBag)
{
    TArray<int32> Dice;
    auto& ManaDice = *Data.Find(DataType);
    Dice.Init(ManaDice.DieFaces, ManaDice.DiceThrown);
    DiceBag->SetDice(Dice);
    if (DataType == ECombatDataType::SpellPoints)
    {
        return DiceBag->Roll() + GetStatModifier(EStat::Intelligence);
    }
    else
    {
        return DiceBag->Roll() + GetStatModifier(EStat::Wisdom);
    }
}
