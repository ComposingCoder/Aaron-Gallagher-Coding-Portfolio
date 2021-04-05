/* Aaron Gallagher's Gameplay Code Samples */
/* All samples are coded to Unreal Engine coding standards */

/* Special Ability Mechanics (Spells) */

/* These functions relate to a spell that is fired from a distance, spawns an object on an enemy, and the object then causes damage over a period of time */

// Detaches collision capsule and launches the capsule to make contact with enemy object
void ADamageOverTimeSpellBase::LaunchSpell()
{
	Super::LaunchSpell();
	SpellCollisionCapsule->RemoveFromRoot();
	SpellBlastParticle->RemoveFromRoot();
	SpellBlastParticle->AttachToComponent(SpellCollisionCapsule, FAttachmentTransformRules::SnapToTargetIncludingScale);
	SpellCollisionCapsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SpellCollisionCapsule->SetSimulatePhysics(true);
	SpellCollisionCapsule->AddImpulse(SpellSpeed * GetOwner()->GetActorForwardVector());
	bHasFiredSpell = true;
}

// Spawns an object that is attached to the enemy object detected by the collision capsule
// Object begins dealing damage to enemy object according to Damage Aount and Damage Type variables from the Stats struct 
void ADamageOverTimeSpellBase::SpawnDotParticle(ACharacter * Hit)
{
	FActorSpawnParameters SpawnParams;
	if (!Hit || (Hit == GetOwner())) return;

	if (!ensure(DotParticleClass != nullptr)) return;
	DotParticle = GetWorld()->SpawnActor<ADotParticleBase>(DotParticleClass, Hit->GetActorLocation(), Hit->GetActorRotation(), SpawnParams);
	if (!ensure(DotParticle != nullptr)) return;
	DotParticle->SetEffectAmount(Stats.DamageAmount);
	DotParticle->SetDamageType(Stats.DamageType);
	DotParticle->GetWorld()->GetTimerManager().SetTimer(DotParticle->_OTLifeTimer, DotParticle, &ADotParticleBase::EndOverTimeEffect, DotLifeTime, false); // sets rate of damage being delt with a maximum lifetime
	DotParticle->LifeTime = DotLifeTime; // sets maximum lifetime of object (garbage collection)
	DotParticle->SetOwner(Hit);
	DotParticle->AttachToActor(Hit, FAttachmentTransformRules::SnapToTargetIncludingScale);
}

// When the collision capsule overlaps with an enemy object, the Dot Particle Object is spawned and attached to the enemy object
// Collision Capsule is returned to the original position and reattached to the parent
void ADamageOverTimeSpellBase::OnSpellOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!bHasFiredSpell) return;
	if (OtherActor && OtherActor != GetOwner())// ensurance that DealDamageOverTime() only effects enemy objects and not the object firing the spell
	{
		CharacterHit = Cast<ACharacter>(OtherActor);
		if (CharactersHit.Contains(OtherActor)) return;
		if (OtherActor != CharacterHit) return; // prevents the enemy object from being detected multiple times
		CharactersHit.Add(CharacterHit);
		HitResult = SweepResult;
		SpawnDotParticle(CharactersHit[0]); // ensures that only the first enemy object will be "hit" by the spell
		ReturnSpellMesh();
	}
	CharactersHit.Empty();
	CharacterHit = nullptr;
}

/* These functions are used for a spell that, instead of casting, is channeled as long as the player is pressing the corresponding button */

// Enables collision and particle effects as well as sets the timer that calls the function which deals damage on collision detected
void AChannelSpellBase::BeginCasting()
{
	Super::BeginCasting();
	SpellCollisionCapsule->Activate();
	if (!SpellBlastParticle) return;
	GetWorld()->GetTimerManager().SetTimer(_SBTimer, this, &AChannelSpellBase::ActivateSpellBlastParticle, SBActivationTime, false);
}

// Disables collision and particle effects as well as ends the timer for dealing damage on collision detected
void AChannelSpellBase::EndCasting()
{
	Super::EndCasting();
	SpellCollisionCapsule->Deactivate();
	bIsCasting = false;
	if (!SpellBlastParticle) return;
	SpellBlastParticle->Deactivate();
	StopCastingAnimation();
}



/* Inventory System Mechanics */



// Replicated function that finds an item from the item data table using the interactable object seen by the player and adds it to the inventory
// Checks the interacatble object's struct for stackability
// If an element is found and has not reached the max stack size, the stack size is increased
// If the max stack size has been reached, a new element is created and the quantity is set to the leftover amount
// If the Object is not stackable, a number of new elements are added equal to the Object's quantity
void UInventoryComponent::AddItemtoInventoryByID_Implementation(FName ID)
{
	if (IsInventoryStatic()) // Checks to see which type of inventory system is being used
	{
		AddItemToInventoryWhenStaticByID(ID); // Implements a different version of AddItemToInventoryByID
		return;
	}
	int32 Index = 0;
	APickup* Pickup = Cast<APickup>(CurrentInteractable);

	AInventoryGameMode* GameMode = (AInventoryGameMode*)GetWorld()->GetAuthGameMode();
	if (!GameMode) return;

	UDataTable* ItemTable = GameMode->GetItemDB();
	if (!ItemTable) return;

	FInventoryItem* ItemToAdd = ItemTable->FindRow<FInventoryItem>(ID, "");
	if (!ItemToAdd) return;

	ItemToAdd->Quantity = Pickup->Quantity;

	if (ItemToAdd->bIsStackable)
	{
		if (DoesInventoryContainID(ID))
		{
			for (int32 i = 0; i < Inventory.Num(); i++)
			{
				if ((Inventory[i].ItemID == ID) && (Inventory[i].Quantity < MaxStackSize))
				{
					Index = i;
					break;
				}
			}
			if (Inventory[Index].ItemID == ID)
			{
				// Calls this function when there is already an Item that is stackable with the same ID in the Inventory
				// Element's Quantity must be less than the Maximum Stack Size
				IncreaseQuantityAtIndex(*ItemToAdd, Index);
			}
		}
		else if (!DoesInventoryContainID(ID))
		{
			// Function called when there is no item with the same ID in the Inventory
			// Similar to IncreaseQuantityAtIndex function
			AddStackableItem(*ItemToAdd);
		}
	}
	else if (ItemToAdd && !ItemToAdd->bIsStackable)
	{
		int32 Leftover = ItemToAdd->Quantity;
		do
		{
			ItemToAdd->Quantity = 1;
			Inventory.Add(*ItemToAdd);
			if (Pickup->bIsModifiedPickup)
			{
				int AddedItemIndex = -1;
				for (int i = 0; i < Inventory.Num(); i++)
				{
					if ((Inventory[i].ItemID == ItemToAdd->ItemID) && !Inventory[i].bHaveStatsBeenSet)
						AddedItemIndex = i;
				}

				if (AddedItemIndex >= 0)
				{
					Inventory[AddedItemIndex].PickupCharacterStats = Pickup->PickupCStats;
					Inventory[AddedItemIndex].bIsModifiedItem = true;
					Inventory[AddedItemIndex].bHaveStatsBeenSet = true;
				}
			}
			--Leftover;
		} while ((Leftover > 0) && !IsInventoryFull());
		if ((Leftover > 0) && IsInventoryFull())
		{
			// Called when there is an excess quantity of the item being added and no room is left in the Inventory
			DropItemAtLocation(*ItemToAdd, Leftover);
		}
	}
	bIsOverwieght = IsEncombered();
}

bool UInventoryComponent::AddItemtoInventoryByID_Validate(FName ID)
{
	if (!IsInventoryFull()) return true;
	else return false;
}

//Function that increases the quantity variable of a specific copy of the inventory instruct that is located in the inventory
// Once the struct variable quantity reaches the maximum stack size, a new item is added to the inventory with the remaining quantity
void UInventoryComponent::IncreaseQuantityAtIndex(FInventoryItem ItemToAdd, int32 Index)
{
	if (!ItemToAdd.bIsStackable || !Inventory[Index].bIsStackable) return;
	int32 Leftover;
	if (ItemToAdd.Quantity + Inventory[Index].Quantity > MaxStackSize)
	{
		do
		{
			Leftover = ItemToAdd.Quantity + Inventory[Index].Quantity - MaxStackSize;
			Inventory[Index].Quantity = MaxStackSize;
			ItemToAdd.Quantity = Leftover;
			if (ItemToAdd.Quantity <= MaxStackSize && !IsInventoryFull())
			{
				Inventory.Add(ItemToAdd);
				Leftover = 0;
			}
			else if (!IsInventoryFull())
			{
				Leftover = ItemToAdd.Quantity - MaxStackSize;
				ItemToAdd.Quantity = MaxStackSize;
				Inventory.Add(ItemToAdd);
				ItemToAdd.Quantity = Leftover;
			}
		} while ((Leftover > 0) && !IsInventoryFull());
		if ((Leftover > 0) && IsInventoryFull())
		{
			DropItemAtIndex(Index, Leftover);
		}
	}
	else if (!IsInventoryFull())
	{
		Inventory[Index].Quantity = Inventory[Index].Quantity + ItemToAdd.Quantity;
	}
	else DropItemAtIndex(Index, ItemToAdd.Quantity); // Finds the item using the Index and then calls DropItemAtLocation
	//	Prevents the incorrect element from being dropped
}

// Removes an amount of one element from the Inventory and spawns an object into the world with the quantity set to the amount dropped
// If the element had a CharacterStats struct that was modified, the object spawn recieves the modified struct variables
void UInventoryComponent::DropItemAtLocation(FInventoryItem ItemToDrop, int32 Amount)
{
	if (Amount <= 0) { Amount = 1; }
	TSubclassOf<APickup> PickupToDrop = ItemToDrop.Pickup;
	bool bIsModified = ItemToDrop.bIsModifiedItem;
	FCharacterStats ModifiedStats = ItemToDrop.PickupCharacterStats;
	FVector DropLocation = GetOwner()->GetActorLocation();
	DropLocation.Y += FMath::RandRange(FMath::RandRange(-120.f, -80.f), FMath::RandRange(120.f, 80.f));
	DropLocation.X += FMath::RandRange(FMath::RandRange(-120.f, -80.f), FMath::RandRange(120.f, 80.f));
	DropLocation.Z += 10.f;
	if (UWorld* const World = GetWorld())
	{
		APickup* Pickup = World->SpawnActor<APickup>(PickupToDrop, DropLocation, FRotator::ZeroRotator);
		Pickup->Quantity = Amount;
		if (bIsModified)
		{
			Pickup->bIsModifiedPickup = true;
			Pickup->PickupCStats = ModifiedStats;
		}
	}
	bIsOverwieght = IsEncombered();
}

// Checks if the WeaponComponent is already equipped and calls the unequip function if necessary
// Spawns an object from the AWeaponBase template, WeaponToSet and sets the CharacterStats struct on the weapon with the values from a data table
void UInventoryComponent::EquipWeapon(UWeaponComponent* WeaponToEquip, TSubclassOf<class AWeaponBase> WeaponToSet, FCharacterStats StatsToEquip)
{
	if (!WeaponToEquip || !WeaponToSet) return;
	if (WeaponToEquip->GetWeapon() != nullptr)
	{
		UnEquipWeapon(WeaponToEquip);
	}
	WeaponToEquip->SetWeapon(WeaponToSet);
	WeaponToEquip->SpawnWeapon();

	APickup* Pickup = Cast<APickup>(CurrentInteractable);

	AInventoryGameMode* GameMode = (AInventoryGameMode*)GetWorld()->GetAuthGameMode();
	if (!GameMode) return;

	UDataTable* ItemTable = GameMode->GetItemDB();
	if (!ItemTable) return;

	FInventoryItem* WeaponItem = ItemTable->FindRow<FInventoryItem>(WeaponToEquip->GetWeapon()->WeaponStats.WeaponID, "");
	if (!WeaponItem) return;

	WeaponToEquip->GetWeapon()->WeaponStats.WeaponCharacterStats = StatsToEquip;
}

// Matches the Weapon ID to the Inventory Item's ID
// Checks if the struct, CharacterStats, is identical between both the Inventory Item and the equipped weapon
// If a match is found, the weapon object is destroyed and the information in the Inventory Item is updated to no longer be equipped
void UInventoryComponent::UnEquipWeaponFromIndex_Implementation(int32 WeaponIndex)
{
	if (!Inventory.IsValidIndex(WeaponIndex)) return;
	int32 WeaponCompIndex = -1; // Default value keeps WeaponComponents[0] from being selected automatically

	for (UWeaponComponent* Weapon : WeaponComponents)
	{
		if (Weapon->GetWeapon())
		{
			if ((Weapon->GetWeapon()->WeaponStats.WeaponID == Inventory[WeaponIndex].ItemID) && (Weapon->GetWeapon()->WeaponStats.WeaponCharacterStats == Inventory[WeaponIndex].PickupCharacterStats))
			{
				WeaponCompIndex = WeaponComponents.IndexOfByKey(Weapon);
			}
		}
	}

	if (!WeaponComponents.IsValidIndex(WeaponCompIndex)) return;
	Inventory[WeaponIndex].bIsEquipped = false;
	WeaponComponents[WeaponCompIndex]->GetWeapon()->Destroy();
	WeaponComponents[WeaponCompIndex]->RemoveWeapon();
}

bool UInventoryComponent::UnEquipWeaponFromIndex_Validate(int32 WeaponIndex)
{
	if (!Inventory.IsValidIndex(WeaponIndex)) return false;
	return true;
}