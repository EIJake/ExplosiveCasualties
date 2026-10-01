// ExplosiveCasualties: experimental damaging explosion dispatcher.
// Vanilla small TNT triggers locally/on server but is silent. Test an addon-owned
// inherited prefab with native sound enabled at creation; audio remains unverified.
// Spawn/configuration follows the supplied vanilla SecondaryExplosion pattern.
// The prefab's timer owns activation: do NOT also call OnUserTrigger/SetLive.
// Previous large-TNT build worked on Jake's Linux dedicated server/mod stack.

class EC_CasualtyExplosion
{
	static const ResourceName EXPLOSION_PREFAB = "{B0DC0394D3820463}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Small_Inherited.et";
	// Delay before spawning; the prefab adds its own timer before detonation.
	static const int DEFER_MS = 2000;

	// Static dispatcher, not a callback on the character's component.
	// Captured source must still exist on damage authority when this runs.
	// Initial deletion policy: cancel safely if removed before execution;
	// do not guess current authority for a deleted character. Never retry.
	static void Spawn(IEntity source, Instigator capturedInstigator, string characterLabel)
	{
		if (!GetGame() || !source)
		{
			Print(string.Format("[ExplosiveCasualties][blast] %1; cancelled: source removed or game unavailable", characterLabel), LogLevel.WARNING);
			return;
		}

		if (source.GetWorld() != GetGame().GetWorld())
		{
			Print(string.Format("[ExplosiveCasualties][blast] %1; cancelled: source outside current game world", characterLabel), LogLevel.WARNING);
			return;
		}

		SCR_CharacterDamageManagerComponent manager = SCR_CharacterDamageManagerComponent.Cast(source.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!manager)
		{
			Print(string.Format("[ExplosiveCasualties][blast] %1; cancelled: damage manager removed", characterLabel), LogLevel.WARNING);
			return;
		}

		HitZone defaultHitZone = manager.GetDefaultHitZone();
		if (!defaultHitZone || defaultHitZone.IsProxy())
		{
			Print(string.Format("[ExplosiveCasualties][blast] %1; cancelled: damage authority absent/lost", characterLabel), LogLevel.WARNING);
			return;
		}

		if (!capturedInstigator)
		{
			Print(string.Format("[ExplosiveCasualties][blast] %1; cancelled: missing captured instigator", characterLabel), LogLevel.WARNING);
			return;
		}

		Resource resource = Resource.Load(EXPLOSION_PREFAB);
		if (!resource || !resource.IsValid())
		{
			Print(string.Format("[ExplosiveCasualties][blast] %1; failed: invalid resource %2", characterLabel, EXPLOSION_PREFAB), LogLevel.ERROR);
			return;
		}

		// Sample current position after the delay, so the blast follows a falling,
		// moved or recovered character rather than its position two seconds ago.
		vector blastPosition = source.GetOrigin() + Vector(0, 0.3, 0);
		// Independent world-space entity: do not parent to the casualty/vehicle.
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		spawnParams.Transform[3] = blastPosition;
		IEntity explosion = GetGame().SpawnEntityPrefab(resource, source.GetWorld(), spawnParams);
		if (!explosion)
		{
			Print(string.Format("[ExplosiveCasualties][blast] %1; failed: SpawnEntityPrefab returned null", characterLabel), LogLevel.ERROR);
			return;
		}

		TimerTriggerComponent trigger = TimerTriggerComponent.Cast(explosion.FindComponent(TimerTriggerComponent));
		if (!trigger)
		{
			Print(string.Format("[ExplosiveCasualties][blast] %1; failed: expected TimerTriggerComponent missing; deleting spawned entity", characterLabel), LogLevel.ERROR);
			SCR_EntityHelper.DeleteEntityAndChildren(explosion);
			return;
		}

		// Preserve the instigator captured at casualty acceptance, unlike the
		// vanilla helper which ignores its argument and reads current attribution.
		trigger.SetInstigator(capturedInstigator);

		array<BaseProjectileEffect> damageContainers = {};
		trigger.GetProjectileEffects(ExplosionDamageContainer, damageContainers);
		if (damageContainers.IsEmpty())
		{
			Print(string.Format("[ExplosiveCasualties][blast] %1; failed: no ExplosionDamageContainer; deleting spawned entity", characterLabel), LogLevel.ERROR);
			SCR_EntityHelper.DeleteEntityAndChildren(explosion);
			return;
		}

		// Ignore ONLY the source character. Do not suppress damage to its vehicle,
		// other occupants, or nearby soldiers by ignoring the root hierarchy.
		// This preserves a chance of revival; other casualties' blasts may kill it.
		array<IEntity> ignoreList = {source};
		foreach (BaseProjectileEffect effect : damageContainers)
		{
			ExplosionDamageContainer container = ExplosionDamageContainer.Cast(effect);
			if (container)
				container.SetIgnoreList(ignoreList);
		}

		// Native sound must be configured on the addon prefab for all instances.
		// Presence/active-state diagnostics only: no runtime activation or manual
		// sound event, so this test isolates the saved prefab configuration.
		// A missing component on headless authority alone does not prove a client bug.
		SoundComponent sound = SoundComponent.Cast(explosion.FindComponent(SoundComponent));
		if (sound)
		{
			Print(string.Format("[ExplosiveCasualties][audio] %1; prefab-native SoundComponent present; active=%2; no manual activation/event", characterLabel, sound.IsActive()));
		}
		else
		{
			Print(string.Format("[ExplosiveCasualties][audio] %1; prefab-native SoundComponent missing on this instance; blast retained", characterLabel), LogLevel.WARNING);
		}

		Print(string.Format("[ExplosiveCasualties][blast] %1; spawned explosion=%2; position=%3; damageContainers=%4", characterLabel, explosion.GetID(), blastPosition, damageContainers.Count()));
		Print(string.Format("[ExplosiveCasualties][blast] %1; timerLeft=%2s; alreadyTriggered=%3; prefab timer owns activation (NO manual trigger)", characterLabel, trigger.GetTimer(), trigger.WasTriggered()));
		if (trigger.WasTriggered())
			Print(string.Format("[ExplosiveCasualties][blast] %1; WARNING: trigger already fired at configuration time; inspect timing before trusting attribution/ignore list", characterLabel), LogLevel.WARNING);

		// No improvised broadcast RPC or second damage path. Keep the prefab's
		// replication/lifecycle unchanged until observed on server and client.
		// A spawn log is NOT proof that the timer fired or clients saw effects.
	}
}
