class XRPHYSICS_API CDamagableItem
{
protected:
	u16		m_levels_num;
	float	m_max_health;
	u16		m_level_applied;
	float	m_health;

public:
					  CDamagableItem();
	virtual		void  Init(float max_health, u16 level_num);
				void  HitEffect();
				void  RestoreEffect();
				float DamageLevelToHealth(u16 dl);

				void  Hit(float P);
	inline		void  SetHealth(float health) noexcept { m_health = health; }
protected:
				u16   DamageLevel();
	inline		float Health()			const noexcept { return m_health; }
				void  ApplyDamage(u16 level);
};