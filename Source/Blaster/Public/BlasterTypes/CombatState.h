#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),//未占用
	ECS_Reloading UMETA(DisplayName = "Reloading"),//装弹中
	ECS_ThrowingGrenade UMETA(DisplayName = "ThrowingGrenade"),//投掷手雷
	ECS_SwappingWeapon UMETA(DisplayName = "SwappingWeapon"),//切换武器

	ECS_Max UMETA(DisplayName = "DefaultMax")
};
