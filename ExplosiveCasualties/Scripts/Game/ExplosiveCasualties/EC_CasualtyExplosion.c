// ExplosiveCasualties: experimental damaging explosion dispatcher.
// Resource copied by Jake from the installed Workbench assets (1.8.0.13).
// Spawn/configuration follows the supplied vanilla SecondaryExplosion pattern.
// The prefab's timer owns activation: do NOT also call OnUserTrigger/SetLive.
// Runtime activation, cleanup, replication, and queue lifetime need testing.

class EC_CasualtyExplosion
{
	static const ResourceName EXPLOSION_PREFAB = "{72BEEF40AF179763}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Large.et";
	static const int DEFER_MS = 1;

	// Static dispatcher, not a callback on the character's component.
	// Captured source must still exist on damage authority when this runs.
	// Initial deletion policy: cancel safely if removed before execution;
	// do not guess current authority for a deleted character. Never retry.
	static void Spawn(IEntity source, vector capturedPosition, Instigator capturedInstigator, string characterLabel)
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

		// Independent world-space entity: do not parent to the casualty/vehicle.
		EntitySpawnParams spawnParams = new EntitySpawnParams();
		spawnParams.TransformMode = ETransformMode.WORLD;
		spawnParams.Transform[3] = capturedPosition;
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

		Print(string.Format("[ExplosiveCasualties][blast] %1; spawned explosion=%2; position=%3; damageContainers=%4", characterLabel, explosion.GetID(), capturedPosition, damageContainers.Count()));
		Print(string.Format("[ExplosiveCasualties][blast] %1; timerLeft=%2s; alreadyTriggered=%3; prefab timer owns activation (NO manual trigger)", characterLabel, trigger.GetTimer(), trigger.WasTriggered()));
		if (trigger.WasTriggered())
			Print(string.Format("[ExplosiveCasualties][blast] %1; WARNING: trigger already fired at configuration time; inspect timing before trusting attribution/ignore list", characterLabel), LogLevel.WARNING);

		// No improvised broadcast RPC or second damage path. Keep the prefab's
		// replication/lifecycle unchanged until observed on server and client.
		// A spawn log is NOT proof that the timer fired or clients saw effects.
	}
}
