// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectRogueCharacter.h"
#include "ProjectRogueProjectile.h"
#include "ProjectRogueGameMode.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "CollisionShape.h"

#include "PartyController.h"
#include "PlayerHUD.h"
#include "MonsterBase.h"
#include "Spell.h"

#include "ItemManager.h"
#include "Item.h"
#include "Ammo.h"
#include "Armor.h"
#include "Consumable.h"
#include "EffectItem.h"
#include "Weapon.h"
#include "Armor.h"
#include "Quiver.h"

#include "DoorComponent.h"
#include "Building.h"
#include "Room.h"
#include "Shopkeeper.h"
#include "TavernKeeper.h"
#include "ReviewBoard.h"
#include "SaveData.h"
#include "LootObject.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

constexpr int32 IntMax = 2147483647;

//////////////////////////////////////////////////////////////////////////
// AProjectRogueCharacter

AProjectRogueCharacter::AProjectRogueCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(45.f, 60.0f);
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetCapsuleComponent());

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(StaticMesh);
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f)); // Position the camera
	FirstPersonCameraComponent->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	FirstPersonCameraComponent->bUsePawnControlRotation = false;

	KeyboardSelectedAdventurer = -1;
}

void AProjectRogueCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	if (HUD)
	{
		HUD->SetInputMode(EInputMode::GameAndUI);
	}
	PartyController = NewObject<UPartyController>();
	PartyController->Player = this;
	Context = EContext::Guild;
	//T: merchant index starts as invalid because its only used for keyboard controls.
	// the first time the player cycles through the merchant inventory, it will be 0
	// if they go down the list, and Inventory.Num if they go up the list
	MerchantInventoryIndex = -1;

	// M: MAKE IT WORK
	AItemManager* ItemManager = GetWorld()->SpawnActor<AItemManager>(AItemManager::StaticClass());
	AItemManager::Init(ItemManager);

	// Create ALL the items!
	CreateAllItems();
	CreateAllSpells();

	LoadGame();
}

//////////////////////////////////////////////////////////////////////////
// Input
//////////////////////////////////////////////////////////////////////////
void AProjectRogueCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAction("MoveForwardOneUnit", IE_Pressed, this, &AProjectRogueCharacter::MoveOneUnit);
	PlayerInputComponent->BindAction("MoveBackwardOneUnit", IE_Pressed, this, &AProjectRogueCharacter::ReverseOneUnit);
	PlayerInputComponent->BindAction("TurnRight", IE_Pressed, this, &AProjectRogueCharacter::TurnRight);
	PlayerInputComponent->BindAction("TurnLeft", IE_Pressed, this, &AProjectRogueCharacter::TurnLeft);
}

const TArray<UAdventurer*>& AProjectRogueCharacter::GetParty() const
{
	check(PartyController);
	return PartyController->GetParty();
}

TArray<UItem*> AProjectRogueCharacter::GetInventory(int32 PartyIndex)
{
	check(PartyController);
	UAdventurer* Adventurer = PartyController->GetAdventurer(PartyIndex);
	return Adventurer->GetInventory();
}

TArray<ASpell*> AProjectRogueCharacter::GetSpells(EClass InClass)
{
	check(Spells.Num() > 0);
	return Spells.FindOrAdd(InClass);
}

int32 AProjectRogueCharacter::GetKeyboardSelectedAdventurer() const
{
	if (KeyboardSelectedAdventurer == -1)
	{
		return 0;
	}
	return KeyboardSelectedAdventurer;
}

void AProjectRogueCharacter::RefreshInventory(UAdventurer* Adventurer)
{
	check(HUD);
	HUD->RefreshInventory(Adventurer);
}

bool AProjectRogueCharacter::IsNameAvailable(UAdventurer* TempAdventurer, const FText& InName) const
{
	check(PartyController);
	return PartyController->IsNameAvailable(TempAdventurer, InName);
}

void AProjectRogueCharacter::SaveGame()
{
	auto ProjectName = TEXT("ProjectRogue");
	USaveData* SaveData = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(ProjectName, 0))
	{
		SaveData = Cast<USaveData>(UGameplayStatics::LoadGameFromSlot(ProjectName, 0));
	}
	else
	{
		SaveData = Cast<USaveData>(UGameplayStatics::CreateSaveGameObject(USaveData::StaticClass()));
	}
	SaveData->SavePartyData(this);
	UGameplayStatics::SaveGameToSlot(SaveData, ProjectName, 0);
}

void AProjectRogueCharacter::LoadGame()
{
	auto ProjectName = TEXT("ProjectRogue");
	if (UGameplayStatics::DoesSaveGameExist(ProjectName, 0))
	{
		USaveGame* SaveGame = UGameplayStatics::LoadGameFromSlot(ProjectName, 0);
		if (USaveData* SaveData = Cast<USaveData>(SaveGame))
		{
			SaveData->LoadPartyData(this);
		}
	}
}

void AProjectRogueCharacter::MoveOneUnit()
{
	if (!bIsInRoom)
	{
		TryMove(true);
	}
}

void AProjectRogueCharacter::ReverseOneUnit()
{
	if (bIsInRoom)
	{
		//trun around and tryMovefront
		SetActorRotation(GetActorRotation()+FRotator(0,180,0));
		TryMove(true);
	}
	else
	{
		// M: Slither Street Hack
		auto pos = GetActorLocation();
		auto forward = GetActorForwardVector();
		if (FMath::IsNearlyEqual(forward.Y, -1.f, 0.2f) && pos.Equals(FVector(1500, 1200, 44), 2.f))
		{
			return;
		}
		TryMove(false);
	}
}

void AProjectRogueCharacter::TurnRight()
{
	if (!bIsInRoom)
	{
		TryTurn(true);
	}
}

void AProjectRogueCharacter::TurnLeft()
{
	if (!bIsInRoom)
	{
		TryTurn(false);
	}
}

void AProjectRogueCharacter::PreMove()
{
	if (Context == EContext::Tavern || Context == EContext::Shop)
	{
		HUD->CloseMerchant();
	}

	SetContext(EContext::None);
}

void AProjectRogueCharacter::PreTurn()
{
	if (Context == EContext::Tavern || Context == EContext::Shop)
	{
		HUD->CloseMerchant();
	}

	SetContext(EContext::None);
}

void AProjectRogueCharacter::PostMove()
{
	if (Context == EContext::Tavern || Context == EContext::Shop)
	{
		HUD->CloseMerchant();
	}

	SetContext(EContext::None);

	ShowHealthbarsForVisibleEnemies();
	UpdateContextSensitiveUI();

	auto pos = GetActorLocation();
	//Check shopkeeper/tavern keeper/review board, etc
	UE_LOG(LogTemp, Display, TEXT("Pos:(%d,%d)"), pos.X, pos.Y);

	// Slither street TP
	if (pos.Equals(FVector(1500, 1300, 44), 2.f))
	{
		SetActorLocation(FVector(1500, 1100, 44));
	}

	if (IsValid(SlitherBlocker))
	{
		SlitherBlocker->Destroy();
		SlitherBlocker = nullptr;
	}
}

void AProjectRogueCharacter::PostTurn()
{
	ShowHealthbarsForVisibleEnemies();
	UpdateContextSensitiveUI();

	if (IsValid(SlitherBlocker))
	{
		SlitherBlocker->Destroy();
		SlitherBlocker = nullptr;
	}
};

void AProjectRogueCharacter::Interact()
{
	FHitResult hit;

	bool result = LineTraceFront(hit);
	if (result)	//something at front
	{
		//check if it's a door
		auto p = hit.Component;
		auto door = Cast<UDoorComponent>(p);
		if (door)
		{
			auto pOwner = door->GetOwner();
			//if it's a door of a building, go in
			auto pBuilding = Cast<ABuilding>(pOwner);
			if (pBuilding)
			{
				auto canEnter = pBuilding->Enter(this);
				if (canEnter)
				{
					bIsInRoom = true;
					PostMove();
				}
				return;
			}
			//if it's a door in a room, go out
			auto pRoom = Cast<ARoom>(pOwner);
			if (pRoom)
			{
				FVector endPos;
				EDirection outDir = EDirection::East;
				bool canLeave = pRoom->Leave(GetDirection(), endPos, outDir);
				if (canLeave)
				{
					UE_LOG(LogTemp, Warning, TEXT("Leave to (%f,%f)"), endPos.X, endPos.Y);
					endPos.Z = GetActorLocation().Z;
					SetActorLocation(endPos);
					SetRotation(outDir);
				}
				bIsInRoom = false;
				PostMove();
				return;
			}
		}

		if (hit.Actor->GetName() == "DungeonExit")
		{
			EnterTown();
		}
	}
}

void AProjectRogueCharacter::ShowHealthbarsForVisibleEnemies()
{
	FCollisionShape Sphere = FCollisionShape::MakeSphere(10000);
	TArray<FHitResult> AreaHitResults;
	FCollisionObjectQueryParams ObjectParams;
	//look for monsters
	ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
	//look for buildings so we know if we dont have line of sight
	ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	FVector Location = GetActorLocation();
	UE_LOG(LogTemp, Display, TEXT("Attempting to sweep for healthbars..."));
	GetWorld()->SweepMultiByObjectType(AreaHitResults, Location, Location, FQuat(), ObjectParams, Sphere, TraceParams);
	if (AreaHitResults.Num() == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Error showing healthbars: Hit results empty"));
	}
	for (const auto& Result : AreaHitResults)
	{
		AMonsterBase* Monster = Cast<AMonsterBase>(Result.Actor);
		if (Monster)
		{
			FHitResult ViewResult;
			FVector Start = GetActorLocation();
			FVector End = Monster->GetActorLocation();
			GetWorld()->LineTraceSingleByObjectType(ViewResult, Start, End, ObjectParams, TraceParams);

			AMonsterBase* MonsterInFront = Cast<AMonsterBase>(ViewResult.Actor);
			ARoom* Room = Cast<ARoom>(ViewResult.Actor);

			//if another object is detected between the player and the enemy, hide health bar
			if ((MonsterInFront != Monster) ||	!MonsterInFront || Room)
			{
				Monster->HideHealthbar();
			}
			else
			{
				Monster->ShowHealthbar();
				// tell the monster to check if they see the player.
				Monster->CheckPlayer();
			}
		}
	}
}

void AProjectRogueCharacter::PopulatePartyInterface()
{
	check(HUD);
	HUD->PopulatePartyInterface(GetParty());
}

void AProjectRogueCharacter::UpdatePartyInterface()
{
	TArray<UAdventurer*> Party = PartyController->GetParty();
	for (auto& Adventurer : Party)
	{
		HUD->UpdatePartyMember(Adventurer);
	}
}

void AProjectRogueCharacter::UpdateContextSensitiveUI()
{
	LineTraceFront(HitResult);
	auto Actor = HitResult.Actor;
	auto shopKeeper = Cast<AShopkeeper>(Actor);
	auto tavernKeeper = Cast<ATavernKeeper>(Actor);
	auto reviewBoard = Cast<AReviewBoard>(Actor);
	ALootObject* LootObject = Cast<ALootObject>(Actor);
	if (shopKeeper)
	{
		GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Shopkeeper: Hello!"));
		SetContext(EContext::Shop);
	}
	else if (tavernKeeper)
	{
		GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Tavern Keeper: Hey there!"));
		SetContext(EContext::Tavern);
	}
	else if (reviewBoard)
	{
		GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Review Adventurer"));
		SetContext(EContext::Review);
	}
	else if (LootObject)
	{
		GEngine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Lootable object"));
		SetContext(EContext::Loot);
	}

	//after moving or turning, if we are in a building, we need to check the context
	if (bIsInRoom)
	{
		if (Context == EContext::Shop)
		{
			HUD->OpenMerchant(Shop);
		}
		else if (Context == EContext::Tavern)
		{
			HUD->OpenMerchant(Tavern);
		}
		else
		{
			HUD->CloseMerchant();
		}
	}
}

void AProjectRogueCharacter::ShowCreateAdventurer()
{
	check(HUD);
	Context = EContext::CreateAdventurer;
	HUD->ShowCreateAdventurer();
}

void AProjectRogueCharacter::ShowEditParty(EPartyEditType EditType)
{
	check(HUD);
	switch (EditType)
	{
		case EPartyEditType::Delete: Context = EContext::DeleteScreen; break;
		case EPartyEditType::Rename: Context = EContext::RenameScreen; break;
		case EPartyEditType::Reorder: Context = EContext::ReorderScreen; break;
	}
	
	HUD->ShowEditParty(EditType);
}

void AProjectRogueCharacter::RenameClicked(int32 Index)
{
	Context = EContext::RenameAdventurer;
	HUD->RenameClicked(Index);
}

UAdventurer* AProjectRogueCharacter::AddAdventurer()
{
	check(PartyController);
	UAdventurer* Adventurer = PartyController->AddAdventurer();
	Adventurer->AddInventorySlots(StartingInventorySize - 1);
	return Adventurer;
}

void AProjectRogueCharacter::InfoButtonClicked(UAdventurer* Adventurer)
{
	HUD->InfoButtonClicked(Adventurer);
}

void AProjectRogueCharacter::InventoryButtonClicked(UAdventurer* Adventurer)
{
	HUD->InventoryButtonClicked(Adventurer);
}

void AProjectRogueCharacter::AttackButtonClicked(UAdventurer* Adventurer)
{
	UWeapon* Weapon = Cast<UWeapon>(Adventurer->GetWeapon());
	int32 WeaponRange = MeleeRange;

	//T: instead of branching for melee weapons, just give weapons 100 range by default
	//because 1 tile is 100 units
	if (Weapon)
	{
		WeaponRange = Weapon->GetRange();
	}

	//only the first two adventurer's can use melee attacks
	//so dont do anything else if the party memeber is in the back without a ranged weapon
	if (Adventurer->GetPartyIndex() > 1 && WeaponRange < BowRange)
		return;

	LineTraceFront(HitResult, WeaponRange);	
	AMonsterBase* Monster = Cast<AMonsterBase>(HitResult.Actor);
	if (!IsValid(Monster))
		return;

	UQuiver* Quiver = nullptr;
	float Damage = 0.f;
	//calculate damage = weapon + str modifier
	if (Weapon)
	{
		FText WeaponName = Weapon->GetItemName();
		//check for projectile ammo. for now, just check for arrows for a bow
		if (WeaponName.ToString() == "Bow")
		{
			auto& Inventory = Adventurer->GetInventory();
			int32 InventoryIndex = 0;
			//loop through the 8 slot inventory to find a quiver
			for (auto& Item : Inventory)
			{
				if (Item->GetType() == EItemType::Container)
				{
					Quiver = Cast<UQuiver>(Item);
					break;
				}
				++InventoryIndex;
			}
			//if it exists in our inventory
			if (Quiver)
			{
				check(Quiver->GetCount() > 0);
				if (Quiver->RemoveOne())
				{
					//we have succesfully shot an arrow
					//if the quiver is empty, remove it from the inventory
					if (Quiver->GetCount() == 0)
					{
						Adventurer->DropItem(InventoryIndex);
					}
					HUD->RefreshInventory(Adventurer);
				}
			}
			//if we dont have a quiver, we cant shoot
			else
			{
				return;
			}
		}

		Damage += Weapon->GetDamage();
	}

	Damage += static_cast<float>(Adventurer->GetStatModifier(EStat::Strength));

	//T: if we have a negative modifier, this will reset it so it doesnt heal the monster
	if (Damage < 0)
		Damage = 0;

	//update party member interface
	FTimerDelegate AttackDelegate;
	AttackDelegate.BindLambda([this, Adventurer]()
		{
			if (Adventurer->GetCurrentHealth() > 0)
			{
				//T: this will re-enable the attack button when the delay is over
				HUD->UpdatePartyMember(Adventurer);
			}
		});
	Adventurer->OnAttack(GetWorld()->GetTimerManager(), AttackDelegate);
	HUD->UpdatePartyMember(Adventurer);

	//update enemy
	Monster->SetPlayer(this);
	Monster->TakeWeaponDamage(Damage);
	HUD->SetInputMode(EInputMode::GameAndUI);
}

void AProjectRogueCharacter::CastButtonClicked(UAdventurer* Adventurer)
{
	//if we're already keeping track of a caster
	if (SpellCaster)
	{
		//if we're clicking the same cast button
		if (SpellCaster == Adventurer)
		{
			//invalidate caster
			SpellCaster = nullptr;
		}
		//if we're not clicking the same cast button
		else
		{
			//update caster
			SpellCaster = Adventurer;
		}
	}
	//if we're not keeping track of a caster
	else
	{
		SpellCaster = Adventurer;
	}
	HUD->CastButtonClicked(Adventurer);
}

void AProjectRogueCharacter::ContextButtonClicked(UAdventurer* Adventurer)
{
	switch (Context)
	{
		case EContext::Review: 
		{
			PartyController->SetReviewBoard(Cast<AReviewBoard>(HitResult.Actor));
		}
		break;
		case EContext::Tavern:
		{
			// M: Tavern and Adventurers use effects only, do the easy thing
			UEffectItem* Item = Cast<UEffectItem>(AItemManager::GetItem(ClickedSlot.Name.ToString()));
			if (!Item)
				return;

			if (Item->GetItemName().ToString() == "Special")
			{
				if (bHasEnteredDungeon)
				{
					EnterDungeon();
				}
				else
				{
					HUD->EnterSpecialRequest();
				}
			}
			else if (PartyController->GetGold() >= Item->GetValue() &&
				ClickedSlot.Character != Adventurer)
			{
				Item->OnEffect(Adventurer);
				PartyController->AdjustGold(-Item->GetValue());
			}
		}
		break;
		case EContext::Shop: 
		{
			if (Adventurer->GetInventoryFreeSlots() == 0)
			{
				GEngine->AddOnScreenDebugMessage(2, 2, FColor::Red, "No inventory slots available");
				return;
			}

			TArray<UItem*> Inventory = Shop->GetInventory();
			UItem* Item = Inventory[ClickedSlot.InventoryIndex];
			int32 Count = 1;
			if (Item->GetType() == EItemType::Container)
			{
				UQuiver* Quiver = Cast<UQuiver>(Item);
				Item = Quiver->GetContainedItem();
				Count = Quiver->GetCount();
			}

			int32 Cost = Item->GetValue() * Count;
			if (Item && (PartyController->GetGold() >= Cost) &&
				ClickedSlot.Character != Adventurer)
			{
				Adventurer->AddItem(Item->GetItemName().ToString(), Count);
				PartyController->AdjustGold(-Cost);
			}
		}
		break;
		case EContext::Loot:
		{
			ALootObject* LootObject = Cast<ALootObject>(HitResult.Actor);
			if (LootObject)
			{
				//add any dropped loot to the first party member
				const auto& Loot = LootObject->GetItems();
				//spawn loot chest
				for (const auto& Item : Loot)
				{
					Adventurer->AddItem(Item);
					GEngine->AddOnScreenDebugMessage(2, 2, FColor::Green, "Picked up " + Item->GetItemName().ToString());
				}
				LootObject->Destroy();
			}
			
			SetContext(EContext::None);
			HUD->SetInputMode(EInputMode::GameAndUI);
			HUD->RefreshInventory(Adventurer);
		}
	}
	
	HUD->ContextButtonClicked(Adventurer);
}

void AProjectRogueCharacter::OnTakeDamage(UAdventurer* Adventurer, int32 Amount)
{
	check(Adventurer);
	UArmor* Armor = Cast<UArmor>(Adventurer->GetArmor());
	int32 DamageAmount = Amount;
	if (Armor)
	{
		DamageAmount -= Armor->GetProtection();
	}

	if (DamageAmount < 0)
		DamageAmount = 0;

	Adventurer->OnTakeDamage(DamageAmount);
	HUD->UpdatePartyMember(Adventurer);
	HUD->ShowBlood();
	
	if (Adventurer->GetCurrentHealth() <= 0)
	{
		PartyController->PartyMemberDied(Adventurer->GetPartyIndex());
		PopulatePartyInterface();

		if (PartyController->IsPartyDead())
		{
			PartyController->RestoreParty();
			//T: move the player to the top of the adventurer's guild
			SetActorLocation(FVector(1300.f, 500.f, 150.f));
			HUD->PartyDied();
		}
	}

}

void AProjectRogueCharacter::OnSpellCasted(ASpell* Spell)
{
	float Range = (Spell->GetRange() * 100) + 25;
	LineTraceFront(HitResult, Range);
	//check if the adventurer has enough mana to cast the spell
	if (!IsValid(SpellCaster) || SpellCaster->GetCurrentMana() < Spell->GetManaCost())
		return;

	if (Spell->GetSpellType() == ESpellType::Prayer)
	{
		if (HUD->IsHealMenuVisible())
		{
			//its a prayer, so open a submenu next to spells
			//to determine which party member it targets
			UAdventurer* SpellTarget = Cast<UAdventurer>(Target);
			if (SpellTarget)
			{
				//do not heal the target if it is already max health
				if (Target->GetCurrentHealth() == SpellTarget->GetMaxHealth())
					return;

				if (Spell->RunCallback(SpellTarget))
				{
					SpellCaster->OnSpellCast(Spell);
					HUD->UpdatePartyMember(SpellTarget);
					HUD->HealCasted();
					HUD->UpdatePartyMember(SpellCaster);
					SpellCaster = nullptr;
				}
			}
		}
		else
		{
			HUD->ShowTargetMenu(SpellCaster, Spell);
		}
	}
	else
	{
		//otherwise, hit the monster in front of us
		AMonsterBase* Monster = Cast<AMonsterBase>(HitResult.Actor);
		if (Monster)
		{
			Monster->SetPlayer(this);
			if (Monster->TakeSpellDamage(Spell))
			{
				SpellCaster->OnSpellCast(Spell);
				HUD->UpdatePartyMember(SpellCaster);
			}
			if (Monster->IsPendingKill())
			{
				UpdateContextSensitiveUI();
			}
		}
	}
}

void AProjectRogueCharacter::SetContext(EContext NewContext)
{
	Context = NewContext;
	UpdatePartyInterface();
	ContextChanged(Context);
}

bool AProjectRogueCharacter::RenameAdventurer(UAdventurer* Adventurer, const FText& NewName)
{
	check(HUD);
	if (PartyController->RenameAdventurer(Adventurer->GetPartyIndex(), NewName))
	{
		HUD->RenameConfirmed(Adventurer);
		return true;
	}
	return false;
}

void AProjectRogueCharacter::HideEditParty()
{
	check(HUD);
	SetContext(EContext::Guild);
	HUD->HideEditParty();
}

void AProjectRogueCharacter::DeleteAdventurer(int32 Index)
{
	PartyController->DeleteAdventurer(Index);
	if (GetPartySize() == 0)
	{
		HideEditParty();
	}
}

void AProjectRogueCharacter::ItemSlotClicked(const FItemSlotInfo& InItemSlot)
{
	check(HUD);
	//update hud before setting ClickedSlot to the new clicked slot
	if (InItemSlot.Character == Shop || InItemSlot.Character == Tavern)
	{
		MerchantInventoryIndex = InItemSlot.InventoryIndex;
	}
	HUD->ItemSlotClicked(InItemSlot, &ClickedSlot);
	ClickedSlot = InItemSlot;
}

void AProjectRogueCharacter::DropSelectedItem()
{
	UAdventurer* Source = Cast<UAdventurer>(ClickedSlot.Character);
	check(Source);
	Source->DropItem(ClickedSlot.InventoryIndex);
	HUD->ItemDropped(ClickedSlot);
	//T: Reset clicked slot after dropping an item so that we can select the currently
	//highlighted item
	ClickedSlot = FItemSlotInfo();
}

void AProjectRogueCharacter::GiveItemClicked()
{
	UAdventurer* Adventurer = Cast<UAdventurer>(ClickedSlot.Character);
	check(Adventurer);
	HUD->ShowTargetMenu(Adventurer, nullptr);
}

void AProjectRogueCharacter::UseItemClicked()
{
	UAdventurer* Adventurer = Cast<UAdventurer>(ClickedSlot.Character);
	check(Adventurer);

	const auto& Inventory = ClickedSlot.Character->GetInventory();
	check(Inventory.Num() > 0);
	const auto& Item = Inventory[ClickedSlot.InventoryIndex];
	check(Item);
	Item->OnUse(Adventurer, ClickedSlot.InventoryIndex);
	check(HUD);
	HUD->ItemUsed(ClickedSlot);
	HUD->UpdatePartyMember(Adventurer);
}

void AProjectRogueCharacter::MonsterDied(AMonsterBase* Monster)
{
	//add experience to party
	int32 TotalXP = Monster->GetExperience();
	int32 PartySize = PartyController->GetLivingAdventurerCount();
	int32 ExpPerPerson = TotalXP / PartySize;
	auto& Party = PartyController->GetParty();

	Party[0]->AddGold(Monster->GetGold());
	Party[0]->TransferGoldToParty();
	for (auto& Adventurer : Party)
	{
		if (Adventurer->GetIsAlive())
		{
			Adventurer->AdjustXP(ExpPerPerson);
		}
	}

	//we have to tell the room that a monster died so the pointer can be set to nullptr
	if (bIsInRoom)
	{
		ARoom* Room = ABuilding::GetRoom();
		check(Room);
		Room->MonsterDied();
	}

	LineTraceFront(HitResult, 125.f);
	UpdateContextSensitiveUI();
}

void AProjectRogueCharacter::GiveItem()
{
	ClickedSlot.Character->GiveItem(ClickedSlot.InventoryIndex, Target);
	HUD->RefreshInventory(ClickedSlot.Character);
	HUD->HideTargetMenu();
	HUD->HideItemMenu();
}

void AProjectRogueCharacter::EnterAdventurersGuild()
{
	check(PartyController);
	PartyController->RestoreParty();
	HUD->ShowAdventurersGuild();
	SetContext(EContext::Guild);
}

void AProjectRogueCharacter::EnterTown()
{
	SetContext(EContext::None);
	PopulatePartyInterface();
	//if player is in dungeon, move player to outside tavern
	if (GetActorLocation().Z < -900)
	{
		//this is outside the tavern
		SetActorLocation(FVector(1000.f, 400.f, 70.f));
	}
	//else, move player outside adventurers guild
	else
	{
		SetActorLocation(FVector(1400.f, 500.f, 70.f));
	}
	SetRotation(EDirection::East);
	bIsInRoom = false;
	HUD->EnterTown();
	ShowHealthbarsForVisibleEnemies();
}

void AProjectRogueCharacter::EnterDungeon()
{
	//Check for lantern
	bool HasLantern = false;
	TArray<UAdventurer*> Adventurers = PartyController->Adventurers;
	for (int32 I = 0; I < Adventurers.Num(); ++I)
	{
		if (HasLantern)
			break;

		TArray<UItem*> Items = Adventurers[I]->GetInventory();
		for (int32 J = 0; J < Items.Num(); ++J)
		{
			if (Items[J]->GetItemName().ToString() == "Lantern")
			{
				HasLantern = true;
				break;
			}
		}
	}
	
	if (!HasLantern)
	{
		GEngine->AddOnScreenDebugMessage(0, 3000, FColor::Red, "A Lantern is required to enter the dungeon.");
		return;
	}

	auto GameMode = Cast<AProjectRogueGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode->RespawnDungeon();

	ABuilding::GetRoom()->ResetRoom();
	SetContext(EContext::None);
	bHasEnteredDungeon = true;
	bIsInRoom = false;
	SetActorLocation(FVector(0.f, 1000.f, -937));
	SetRotation(EDirection::East);
	HUD->ConfirmSpecialRequest();
	ShowHealthbarsForVisibleEnemies();
	HUD->SetInputMode(EInputMode::GameAndUI);
}

bool AProjectRogueCharacter::CheckKey()
{
	for (int32 I = 0; I < PartyController->GetPartySize(); ++I)
	{
		UAdventurer* Adventurer = PartyController->GetAdventurer(I);
		auto& Inventory = Adventurer->GetInventory();
		for (int32 J = 0; J < Inventory.Num(); ++J)
		{
			if (Inventory[J]->GetItemName().ToString() == FString("Key"))
			{
				Adventurer->DropItem(J);
				return true;
			}
		}
	}

	return false;
}

void AProjectRogueCharacter::MoveUpInMerchant()
{
	--MerchantInventoryIndex;
	if (MerchantInventoryIndex < 0)
	{
		if (Context == EContext::Shop)
		{
			const auto& Inventory = Shop->GetInventory();
			MerchantInventoryIndex = Inventory.Num() - 1;
		}
		else if (Context == EContext::Tavern)
		{
			const auto& Inventory = Tavern->GetInventory();
			MerchantInventoryIndex = Inventory.Num() - 1;
		}
	}
}

void AProjectRogueCharacter::MoveDownInMerchant()
{
	++MerchantInventoryIndex;
	if (Context == EContext::Shop)
	{
		const auto& Inventory = Shop->GetInventory();
		if (MerchantInventoryIndex == Inventory.Num())
		{
			MerchantInventoryIndex = 0;
		}
	}
	else if (Context == EContext::Tavern)
	{
		const auto& Inventory = Tavern->GetInventory();
		if (MerchantInventoryIndex == Inventory.Num())
		{
			MerchantInventoryIndex = 0;
		}
	}
}

void AProjectRogueCharacter::CreateAllItems()
{
	// Quest Items
	UConsumable* SecretNote = AItemManager::AddItem<UConsumable>("Secret Note", 0, "Contains a secret message.");
	SecretNote->SetConsumesSpace(false);
	SecretNote->SetUseFunction([&](UAdventurer* Adventurer) {
		GEngine->AddOnScreenDebugMessage(1, 10, FColor::Red, TEXT("Notice to all Town Monsters: \
			\nThe new secret password is \"devil whiskey\". \
			\nThe Tavernkeep should let you in, regardless of appearance. \
			\nEven the ugly ones. That means you. HAHA"));
		});

	AItemManager::AddItem<UItem>("Key", 0, "Unlocks a door.");

	// Shop Items

	UItem* Lantern = AItemManager::AddItem<UItem>("Lantern", 1, "Use to see in the dungeon, it lasts until you leave the dungeon.");
	Lantern->SetConsumesSpace(false);

	UConsumable* Ration = AItemManager::AddItem<UConsumable>("Ration", 1, "Use to heal 1 HP.");
	Ration->SetUseFunction([&](UAdventurer* Adventurer) {
		Adventurer->RestoreHealth(1);
		Adventurer->DropItem(ClickedSlot.InventoryIndex);
	});

	UWeapon* Staff = AItemManager::AddItem<UWeapon>("Staff", 2, "Equip to attack for 1 damage.");
	Staff->SetFixedDamage(1);

	AItemManager::AddItem<UWeapon>("Dagger", 4, "Equip to attack for 1d2 damage.", { 2 });
	AItemManager::AddItem<UWeapon>("Club", 6, "Equip to attack for 1d3 damage.", { 3 });
	AItemManager::AddItem<UWeapon>("Axe", 8, "Equip to attack for 1d4 damage.", { 4 });
	AItemManager::AddItem<UWeapon>("Sword", 30, "Equip to attack for 1d8 damage.", { 8 });
	AItemManager::AddItem<UWeapon>("Mace", 20, "Equip to attack for 1d6 damage.", { 6 });
	AItemManager::AddItem<UWeapon>("Battle Axe", 30, "Equip to attack for 1d8 damage.", { 8 });

	UWeapon* Bow = AItemManager::AddItem<UWeapon>("Bow", 20, "Equip to attack for 1d6 damage up to 3 units away.", { 6 });
	Bow->SetRange(BowRange);

	UAmmo* Arrow = AItemManager::AddItem<UAmmo>("Arrow", 0.25, "Shot by a Bow.");

	UArmor* Robes = AItemManager::AddItem<UArmor>("Robes", 6, "Equip to decrease damage by 1.");
	Robes->SetFixedProtection(1);

	AItemManager::AddItem<UArmor>("Leather Armor", 12, "Equip to decrease damage by 1d3.", { 3 });
	AItemManager::AddItem<UArmor>("Chainmail Armor", 36, "Equip to decrease damage by 1d6.", { 6 });
	AItemManager::AddItem<UArmor>("Platemail Armor", 60, "Equip to decrease damage by 1d12.", { 12 });

	// Tavern Items

	UEffectItem* Meat = AItemManager::AddItem<UEffectItem>("Meat", 1, "Heal all hit points.");
	Meat->SetEffectFunction([&](UAdventurer* Adventurer) {
			GEngine->AddOnScreenDebugMessage(2, 2, FColor::Green, "Health Restored!");
			Adventurer->RestoreHealth(IntMax);
		});

	UEffectItem* Wine = AItemManager::AddItem<UEffectItem>("Wine", 1, "Restore all prayer points.");
	Wine->SetEffectFunction([&](UAdventurer* Adventurer) {
			if (Adventurer->GetClass() == EClass::Cleric)
			{
				GEngine->AddOnScreenDebugMessage(2, 2, FColor::Green, "Prayer points Restored!");
				Adventurer->RestoreMana(IntMax);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(2, 7, FColor::White, "That drink was good, but didn't satisfy my needs.");
			}
		});

	UEffectItem* Ale = AItemManager::AddItem<UEffectItem>("Ale", 1, "Restore all spell points.");
	Ale->SetEffectFunction([&](UAdventurer* Adventurer) {
			if (Adventurer->GetClass() == EClass::Magician)
			{
				GEngine->AddOnScreenDebugMessage(2, 2, FColor::Green, "Spell points Restored!");
				Adventurer->RestoreMana(IntMax);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(2, 7, FColor::White, "That drink was good, but didn't satisfy my needs.");
			}
		});

	UEffectItem* Dinner = AItemManager::AddItem<UEffectItem>("Dinner", 2, "Restore all hit points, prayer points, and spell points.");
	Dinner->SetEffectFunction([&](UAdventurer* Adventurer) {
			GEngine->AddOnScreenDebugMessage(2, 3, FColor::Green, "Health and Points Restored!");
			Adventurer->RestoreHealth(IntMax);
			Adventurer->RestoreMana(IntMax);
		});

	UEffectItem* SpecialRequest = AItemManager::AddItem<UEffectItem>("Special", 0, "Make a special request.");
	SpecialRequest->SetEffectFunction([&](UAdventurer* Adventurer) {
			GEngine->AddOnScreenDebugMessage(0, 2, FColor::White, "Need something?");
		});

	// Dungeon Items
	UWeapon* MithrilStaff = AItemManager::AddItem<UWeapon>("Mithril Staff", 0, "Equip to attack for 6d4 damage.", {4, 4, 4, 4, 4, 4});
	MithrilStaff->SetSubType(EItemType::Special);

	UWeapon* MithrilDagger = AItemManager::AddItem<UWeapon>("Mithril Dagger", 0, "Equip to attack for 3d4 damage.", { 4, 4, 4 });
	MithrilDagger->SetSubType(EItemType::Special);

	UWeapon* MithrilSword = AItemManager::AddItem<UWeapon>("Mithril Sword", 0, "Equip to attack for 3d8 damage.", { 8, 8, 8 });
	MithrilSword->SetSubType(EItemType::Special);
	
	UWeapon* MithrilBattleAxe = AItemManager::AddItem<UWeapon>("Mithril Battle Axe", 0, "Equip to attack for 3d8 damage.", { 8, 8, 8 });
	MithrilBattleAxe->SetSubType(EItemType::Special);

	UWeapon* MithrilBow = AItemManager::AddItem<UWeapon>("Mithril Bow", 0, "Equip to attack for 3d6 damage up to 3 units away.", { 6, 6, 6 });
	MithrilBow->SetRange(BowRange);
	MithrilBow->SetSubType(EItemType::Special);

	UAmmo* MithrilArrow = AItemManager::AddItem<UAmmo>("Mithril Arrow", 0, "Shot by a Mithril Bow.");
	MithrilArrow->SetSubType(EItemType::Special);

	UArmor* MithrilRobes = AItemManager::AddItem<UArmor>("Mithril Robes", 0, "Equip to decrease damage by 3d12.", {12, 12, 12});
	MithrilRobes->SetSubType(EItemType::Special);

	UArmor* MithrilLeatherArmor = AItemManager::AddItem<UArmor>("Mithril Leather Armor", 0, "Equip to decrease damage by 3d4.", { 4, 4, 4 });
	MithrilLeatherArmor->SetSubType(EItemType::Special);

	UArmor* MithrilChainMailArmor = AItemManager::AddItem<UArmor>("Mithril Chainmail Armor", 0, "Equip to decrease damage by 1d6.", { 6 });
	MithrilChainMailArmor->SetSubType(EItemType::Special);

	UArmor* MithrilPlatemailArmor = AItemManager::AddItem<UArmor>("Mithril Platemail Armor", 0, "Equip to decrease damage by 1d12.", { 12 });
	MithrilPlatemailArmor->SetSubType(EItemType::Special);

	// Potions
	UConsumable* HealingPotion = AItemManager::AddItem<UConsumable>("Healing Potion", 0, "Restore 4d6 Health points.", {6, 6, 6, 6});
	HealingPotion->SetSubType(EItemType::Potion);
	HealingPotion->SetUseFunction([&](UAdventurer* Adventurer) {
		Adventurer->RestoreHealth(AItemManager::GetItem("Healing Potion")->GetDiceBag()->Roll());
		Adventurer->DropItem(ClickedSlot.InventoryIndex);
		GEngine->AddOnScreenDebugMessage(2, 2, FColor::Green, "Health Points Restored!");
	});

	UConsumable* ManaPotion = AItemManager::AddItem<UConsumable>("Mana Potion", 0, "Restore 4d6 Spell points.", { 6, 6, 6, 6 });
	ManaPotion->SetSubType(EItemType::Potion);
	ManaPotion->SetUseFunction([&](UAdventurer* Adventurer) {
		if (Adventurer->GetClass() == EClass::Magician)
		{
			Adventurer->RestoreMana(AItemManager::GetItem("Mana Potion")->GetDiceBag()->Roll());
			Adventurer->DropItem(ClickedSlot.InventoryIndex);
			GEngine->AddOnScreenDebugMessage(2, 2, FColor::Green, "Spell Points Restored!");
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(2, 5, FColor::White, "That potion doesn't satisfy my needs.");
		}
	});

	UConsumable* HolyWater = AItemManager::AddItem<UConsumable>("Holy Water", 0, "Restore 4d6 Prayer points.", { 6, 6, 6, 6 });
	HealingPotion->SetSubType(EItemType::Potion);
	HolyWater->SetUseFunction([&](UAdventurer* Adventurer) {
		if (Adventurer->GetClass() == EClass::Cleric)
		{
			Adventurer->RestoreMana(AItemManager::GetItem("Holy Water")->GetDiceBag()->Roll());
			Adventurer->DropItem(ClickedSlot.InventoryIndex);
			GEngine->AddOnScreenDebugMessage(2, 2, FColor::Green, "Prayer Points Restored!");
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(2, 5, FColor::White, "That potion doesn't satisfy my needs.");
		}
	});


	// Merchant Setup

	Shop = NewObject<UCharacterData>();
	Shop->RenameCharacter(FText::FromString("Merchant"));
	Shop->AddInventorySlots(20);
	Shop->AddItem(FString("Lantern"));
	Shop->AddItem(FString("Rations"));
	Shop->AddItem(FString("Staff"));
	Shop->AddItem(FString("Dagger"));
	Shop->AddItem(FString("Club"));
	Shop->AddItem(FString("Axe"));
	Shop->AddItem(FString("Sword"));
	Shop->AddItem(FString("Mace"));
	Shop->AddItem(FString("Battle Axe"));
	Shop->AddItem(FString("Bow"));
	Shop->AddItem(FString("Arrow"), 4);
	Shop->AddItem(FString("Robes"));
	Shop->AddItem(FString("Leather Armor"));
	Shop->AddItem(FString("Chainmail Armor"));
	Shop->AddItem(FString("Platemail Armor"));

	Tavern = NewObject<UCharacterData>();
	Tavern->RenameCharacter(FText::FromString("Tavern"));
	Tavern->AddInventorySlots(4);
	Tavern->AddItem(FString("Meat"));
	Tavern->AddItem(FString("Wine"));
	Tavern->AddItem(FString("Ale"));
	Tavern->AddItem(FString("Dinner"));
	Tavern->AddItem(FString("Special"));
}

void AProjectRogueCharacter::CreateAllSpells()
{
	//empty spells before adding so we dont have garbage data from previous runs
	Spells.Empty();
	TArray<ASpell*> MagicianSpells;
	UWorld* pWorld = GetWorld();
	ASpell* Pain = pWorld->SpawnActor<ASpell>();
	ASpell* Fireball = pWorld->SpawnActor<ASpell>();
	ASpell* LightningBolt = pWorld->SpawnActor<ASpell>();
	ASpell* RagingInferno = pWorld->SpawnActor<ASpell>();

	Pain->Init(this, ESpellType::Spell, 1, 1, 1, "Pain", "Damage one enemy 1d4.",
		[](UCharacterData* Character)
		{
			//T: in the event that enemies arent removed when they die, verify they are alive
			if (Character->GetCurrentHealth() == 0)
				return false;

			UDiceBag* DiceBag = NewObject<UDiceBag>();
			DiceBag->AddDice({ 4 });
			Character->OnTakeDamage(DiceBag->Roll());
			return true;
		});
	Fireball->Init(this, ESpellType::Spell, 2, 2, 2, "Fireball", "Damage one enemy for 1d8.",
		[](UCharacterData* Character)
		{
			if (Character->GetCurrentHealth() == 0)
				return false;

			UDiceBag* DiceBag = NewObject<UDiceBag>();
			DiceBag->AddDice({ 8 });
			Character->OnTakeDamage(DiceBag->Roll());
			return true;
		});
	LightningBolt->Init(this, ESpellType::Spell, 3, 3, 3, "LightningBolt", "Damage one enemy for 1d12.",
		[](UCharacterData* Character)
		{
			if (Character->GetCurrentHealth() == 0)
				return false;

			UDiceBag* DiceBag = NewObject<UDiceBag>();
			DiceBag->AddDice({ 12 });
			Character->OnTakeDamage(DiceBag->Roll());
			return true;
		});
	RagingInferno->Init(this, ESpellType::Spell, 4, 4, 3, "RagingInferno", "Damage all enemies for 1d8.",
		[this, pWorld](UCharacterData* Character)
		{
			if (Character->GetCurrentHealth() == 0)
				return false;

			UDiceBag* DiceBag = NewObject<UDiceBag>();
			DiceBag->AddDice({ 8 });
			Character->OnTakeDamage(DiceBag->Roll());
			//T: Character is the unit in front of us, so we have to get all enemies
			//within a 3 tile radius, which can be done with a box trace by visibility channel
			//we use visibility so we dont hit enemies around corners
			FCollisionShape Box = FCollisionShape::MakeBox(FVector(900.f, 900.f, 0.f));
			TArray<FHitResult> AreaHitResults;
			FCollisionQueryParams TraceParams;
			TraceParams.AddIgnoredActor(this);
			FVector Location = GetActorLocation();
			pWorld->SweepMultiByChannel(AreaHitResults, Location, Location, GetActorQuat(), ECC_Visibility, Box, TraceParams);
			for (const auto& Result : AreaHitResults)
			{
				//TODO do a line trace on hit monsters for visibility
				AMonsterBase* Monster = Cast<AMonsterBase>(Result.Actor);
				if (Monster)
				{
					Monster->TakeDamage(DiceBag->Roll(), FDamageEvent(), nullptr, this);
				}
			}
			return true;
		});

	MagicianSpells.Add(Pain);
	MagicianSpells.Add(Fireball);
	MagicianSpells.Add(LightningBolt);
	MagicianSpells.Add(RagingInferno);

	TArray<ASpell*> ClericSpells;
	ASpell* Comfort = pWorld->SpawnActor<ASpell>();
	ASpell* Heal = pWorld->SpawnActor<ASpell>();
	ASpell* Restore = pWorld->SpawnActor<ASpell>();
	ASpell* HealAll = pWorld->SpawnActor<ASpell>();
	ASpell* RaiseDead = pWorld->SpawnActor<ASpell>();

	Comfort->Init(this, ESpellType::Prayer, 1, 1, 0, "Comfort", "Heal one living hero for 1d4.",
		[](UCharacterData* Character)
		{
			if (Character->GetCurrentHealth() == 0)
				return false;

			UDiceBag* DiceBag = NewObject<UDiceBag>();
			DiceBag->AddDice({ 4 });
			Character->RestoreHealth(DiceBag->Roll());
			return true;
		});
	Heal->Init(this, ESpellType::Prayer, 2, 2, 0, "Heal", "Heal one living hero for 4d4.",
		[](UCharacterData* Character)
		{
			if (Character->GetCurrentHealth() == 0)
				return false;

			UDiceBag* DiceBag = NewObject<UDiceBag>();
			DiceBag->AddDice({ 4, 4, 4, 4 });
			Character->RestoreHealth(DiceBag->Roll());
			return true;
		});
	Restore->Init(this, ESpellType::Prayer, 3, 3, 0, "Restore", "Heal all hit points for one living hero.",
		[](UCharacterData* Character)
		{
			if (Character->GetCurrentHealth() == 0)
				return false;

			Character->RestoreHealth(Character->GetMaxHealth());
			return true;
		});
	HealAll->Init(this, ESpellType::Prayer, 4, 4, 0, "HealAll", "Restore every living hero's hit points.",
		[this](UCharacterData* Character)
		{
			if (Character->GetCurrentHealth() == 0)
				return false;

			TArray<UAdventurer*> Party = GetParty();
			for (auto Adventurer : Party)
			{
				Adventurer->RestoreHealth(Adventurer->GetMaxHealth());
			}
			return true;
		});
	RaiseDead->Init(this, ESpellType::Prayer, 5, 5, 0, "RaiseDead", "Bring one adventurer back to life.",
		[](UCharacterData* Character)
		{
			if (Character->GetCurrentHealth() > 0)
				return false;

			Character->RestoreHealth(Character->GetMaxHealth());
			return true;
		});

	ClericSpells.Add(Comfort);
	ClericSpells.Add(Heal);
	ClericSpells.Add(Restore);
	ClericSpells.Add(HealAll);
	ClericSpells.Add(RaiseDead);

	Spells.Add(EClass::Magician, MagicianSpells);
	Spells.Add(EClass::Cleric, ClericSpells);
}

UAdventurer* AProjectRogueCharacter::AddRandomAdventurer()
{
	UAdventurer* Adventurer = AddAdventurer();
	if (Adventurer == nullptr)
		return nullptr;

	Adventurer->Update(FText::FromString("RandomAdventurer"), (ERace)FMath::RandRange(0, 3), (EClass)FMath::RandRange(0, 2), FSlateBrush());
	Adventurer->RandomizeStats();
	Adventurer->TransferGoldToParty();
	const EClass Class = Adventurer->GetClass();

	//Debug items
	Adventurer->AddItem("Arrow", 20);
	Adventurer->AddItem("Bow");
	Adventurer->AddItem("Key");
	Adventurer->AddItem("Secret Note");
	Adventurer->AddItem("Leather Armor");
	Adventurer->AddItem("Healing Potion");
	Adventurer->AddItem("Mana Potion");
	Adventurer->AddItem("Holy Water");
	Adventurer->AdjustXP(500000);

	// Godmode and other Debug junk
	Adventurer->IncreaseMaxHealth(1337);
	Adventurer->IncreaseMaxMana(1234);
	Adventurer->AddLevel();
	PartyController->AdjustGold(5000);

	return Adventurer;
}

void AProjectRogueCharacter::SetKeyboardSelectedAdventurer(int32 Index)
{
	if (Index < 0 || Index >= PartyController->GetPartySize())
	{
		Index = -1;
	}
	KeyboardSelectedAdventurer = Index;
}