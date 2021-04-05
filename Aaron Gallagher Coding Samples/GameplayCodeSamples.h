/* Aaron Gallagher's Gameplay Code Samples */
/* All samples are coded to Unreal Engine coding standards */


/* Special Ability Mechanics (Spells) */


/* Sample base class for spells that are fired from a distance, spawns an object on an enemy, and the object then causes damage over a period of time */ 
UCLASS()
class RPGFIX_API ADamageOverTimeSpellBase : public ASpellBase
{
	GENERATED_BODY()

public:

	ADamageOverTimeSpellBase(const FObjectInitializer& ObjectInitializer);

	// Detaches collision capsule and launches the capsule to make contact with enemy 
	virtual void LaunchSpell() override;

	// Spawns an object that is attached to the enemy object detected by the collision capsule
	// Object begins dealing damage to enemy object according to Damage Aount and Damage Type variables from the Stats struct
	void SpawnDotParticle(ACharacter* Hit);

	// When the collision capsule overlaps with an enemy object, the Dot Particle Object is spawned and attached to the enemy object
	// Collision Capsule is returned to the original position and reattached to the parent
	UFUNCTION()
	void OnSpellOverlap(
	UPrimitiveComponent * OverlappedComp, AActor * OtherActor, 
	UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult & SweepResult
	);

};

/* Sample base class for spells that, instead of casting, is channeled as long as the player continues pressing the corresponding button */
UCLASS()
class RPGFIX_API AChannelSpellBase : public ASpellBase
{
	GENERATED_BODY()

public:

	AChannelSpellBase(const FObjectInitializer& ObjectInitializer);

	// Enables collision and particle effects as well as sets the timer that calls the function which deals damage on collision detected
	virtual void BeginCasting() override;
	// Disables collision and particle effects as well as ends the timer for dealing damage on collision detected
	virtual void EndCasting() override;

};



/* Inventory System Mechanics */


/* Sample Inventory Component Class, not the full class */
/* Relevant forward declarations are included */
#define Interactable        ECC_GameTraceChannel1

enum class EItemType : uint8;
struct FInventoryItem;
struct FArmorStats;

class AInteractable;
class APickup;
class UWeaponComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INVENTORY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UInventoryComponent();

	// Replicated function that finds an item from the item data table using the interactable object seen by the player and adds it to the inventory
	// Checks the interacatble object's struct for stackability
	// If an element is found and has not reached the max stack size, the stack size is increased
	// If the max stack size has been reached, a new element is created and the quantity is set to the leftover amount
	// If the Object is not stackable, a number of new elements are added equal to the Object's quantity
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Utils")
	void AddItemtoInventoryByID(FName ID);

	//Function that increases the quantity variable of a specific copy of the inventory instruct that is located in the inventory
	// Once the struct variable quantity reaches the maximum stack size, a new item is added to the inventory with the remaining quantity
	void IncreaseQuantityAtIndex(FInventoryItem ItemToAdd, int32 Index);

	// Removes an amount of one element from the Inventory and spawns an object into the world with the quantity set to the amount dropped
	// If the element had a CharacterStats struct that was modified, the object spawn recieves the modified struct variables
	void DropItemAtLocation(FInventoryItem ItemToDrop, int32 Amount);

	// Checks if the WeaponComponent is already equipped and calls the unequip function if necessary
	// Spawns an object from the AWeaponBase template, WeaponToSet and sets the CharacterStats struct on the weapon with the values from a data table
	UFUNCTION(BlueprintCallable)
	void EquipWeapon(UWeaponComponent* WeaponToEquip, TSubclassOf<class AWeaponBase> WeaponToSet, FCharacterStats StatsToEquip);

	// Matches the Weapon ID to the Inventory Item's ID
	// Checks if the struct, CharacterStats, is identical between both the Inventory Item and the equipped weapon
	// If a match is found, the weapon object is destroyed and the information in the Inventory Item is updated to no longer be equipped
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Utils")
	void UnEquipWeaponFromIndex(int32 WeaponIndex);

};