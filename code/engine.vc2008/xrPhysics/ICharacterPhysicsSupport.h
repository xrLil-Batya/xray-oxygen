#pragma once
class CPHMovementControl;

class ICharacterPhysicsSupport
{
public:
	enum EType
	{
		etActor,
		etStalker,
		etBitting, 
		etEmpty
	};

	enum EState
	{
		esDead,
		esAlive,
		esRemoved
	};

public:
	virtual			~ICharacterPhysicsSupport	() = default;
	virtual	EType	Type						() { return etEmpty; }
	virtual	CPHMovementControl *movement		() { return nullptr; }
	virtual const CPHMovementControl *movement	() const { return nullptr; }
	virtual const Fvector MovementVelocity		() const = 0;
};