#pragma once
/* Aaron Gallagher Struct Samples */
/* All samples are coded to Unreal Engine coding standards */

/* Struct used to give weapons, armor, and characters modifyable attributes in Aaron Gallagher's Inventory Plugin */ 
USTRUCT(BlueprintType)
struct FCharacterStats : public FTableRowBase
{

	GENERATED_USTRUCT_BODY()

public:

	FCharacterStats()
	{
		Armor = 0;
		PhysicalAttack = 0;
		Fortitude = 0;
		Agility = 0;
		MagicAttack = 0;
		DamageAmount = 0.f;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Stats")
		int32 Armor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Stats")
		int32 PhysicalAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Stats")
		int32 Fortitude;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Stats")
		int32 Agility;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Stats")
		int32 MagicAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Stats")
		float DamageAmount;

	bool operator == (const FCharacterStats& StatB) const
	{
		if (
			(Agility == StatB.Agility) &&
			(Armor == StatB.Armor) &&
			(Fortitude == StatB.Fortitude) &&
			(DamageAmount == StatB.DamageAmount) &&
			(MagicAttack == StatB.MagicAttack) &&
			(PhysicalAttack == StatB.PhysicalAttack)
			) return true;
		else return false;
	}

	void operator += (FCharacterStats& StatB) const
	{
		StatB.Agility += Agility;
		StatB.Armor += Armor;
		StatB.Fortitude += Fortitude;
		StatB.DamageAmount += DamageAmount;
		StatB.MagicAttack += MagicAttack;
		StatB.PhysicalAttack += PhysicalAttack;
	}
};


/* Struct used for making new items in Aaron Gallagher's Inventory Plugin */
USTRUCT(BlueprintType)
struct FInventoryItem : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	FInventoryItem()
	{
		ItemID = FName("ID");
		Pickup = nullptr;
		ItemName = FText::FromString("Item");
		ItemAction = FText::FromString("Use");
		ItemDescription = FText::FromString("Please enter a description for this Item.");
		ItemValue = 0;
		Quantity = 0;
		Weight = 0.0f;
		Thumbnail = nullptr;
		ItemMesh = nullptr;
		ItemType = EItemType::ET_NONE;
		bIsStackable = false;
		bCanBeUsed = false;
		WeaponClass = nullptr;
		bIsEquippable = false;
		bIsEquipped = false;
		bIsModifiedItem = false;
		bHaveStatsBeenSet = false;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Item Info")
		FName ItemID;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Item Info")
		TSubclassOf<class APickup> Pickup; //APickup is the base class of all pickups, used to spawn new objects

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Item Info")
		FText ItemName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Item Info")
		int32 ItemValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Item Info")
		int32 Quantity;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Item Info")
		float Weight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Item Info")
		FText ItemDescription;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Item Info")
		FText ItemAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Item Info")
		class UTexture2D* Thumbnail;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Item Info")
		class UStaticMesh* ItemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Item Info")
		EItemType ItemType; // Enum used identify the type of item

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Item Info")
		bool bIsStackable;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Item Info")
		bool bCanBeUsed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Equipment")
		TSubclassOf<class AWeaponBase> WeaponClass; // AWeaponBase is the weapon base class, used to spawn new weapon objects on equip

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Equipment")
		FCharacterStats PickupCharacterStats; // Modifyable attributes, see above 

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Equipment")
		bool bIsEquippable;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Equipment")
		bool bIsEquipped;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Equipment")
		bool bIsModifiedItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "Equipment")
		bool bHaveStatsBeenSet;

	bool operator == (const FInventoryItem& Item) const
	{
		if (ItemID == Item.ItemID)
		{
			return true;
		}
		else return false;
	}
};