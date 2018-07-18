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
};