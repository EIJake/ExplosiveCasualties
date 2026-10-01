# Installed secondary-explosion source inspection

**Historical asset evidence below concerns Explosion_Tnt_Large.et.** Jake subsequently reported Linux dedicated-server success with that build, then requested SMALL/repeat-trigger/2000 ms behavior. Current resource is `{2F690C7C59FB4DBF}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Small.et`; its activation/timer/cleanup are not yet tested. See [current design](../DESIGN.md) and [resume](../RESUME.md). Keep the original source and LARGE observations below as historical evidence, not claims about SMALL.

Recorded by **Element0**, 2026-10-01. Evidence: Jake supplied the installed `SCR_DamageManagerComponent.c` source in the conversation. Last observed runtime build: 1.8.0.13; no separate build/version check accompanied this source.

## Confirmed implementation

`SecondaryExplosion(ResourceName prefabName, notnull Instigator instigator, notnull EntitySpawnParams spawnParams)`:

- Returns if the default hit zone exists and is a proxy. Our caller must retain its stronger missing-hit-zone rejection.
- Loads the specified resource and returns if it is invalid.
- Defaults `spawnParams.Parent` to the damage manager's owner.
- Spawns the prefab in the parent's world and looks for `BaseTriggerComponent` on the spawned entity.
- Calls `trigger.SetInstigator(GetInstigator())`. **The supplied `instigator` argument is not used in this implementation.** Passing a captured instigator does not preserve that attribution; do not claim otherwise.
- Builds an ignore list containing the owner's root parent and its hierarchy.
- Removes vehicle occupants from that list only when the damage-manager owner itself has `SCR_BaseCompartmentManagerComponent`.
- Sets this ignore list on every `ExplosionDamageContainer` returned by `trigger.GetProjectileEffects(...)`.
- Contains no explicit trigger activation call, custom RPC, returned success value, or retry mechanism. The compatible prefab/engine must provide activation, effects, replication, and cleanup; those behaviors are not established by this method alone.

`FuelSecondaryExplosion(...)` obtains a position from `GetSecondaryExplosionPosition(...)`, writes it into `spawnParams.Transform[3]`, selects a configured explosion resource, and calls `SecondaryExplosion(...)`. It leaves parent selection to the helper. These call sites use owner-relative positions; do not pass an arbitrary world-space position as though it were local.

## Character-specific implications

- Do not choose a resource through the character's secondary-fuel configuration and assume it is present. The first experimental release needs one explicitly selected, installed vanilla explosion prefab.
- Select a standalone secondary-explosion `.et` with the expected trigger/damage configuration, not a particle-only `.ptc`, armed mine item, or ammunition prefab that requires another firing/activation path.
- A character may have a vehicle as its root parent. Blind reuse of the hierarchy ignore list could exempt more than the exploding soldier, including passengers. Inspect/test this explicitly, or use a character-specific spawn path with a deliberate ignore list.
- For a deferred call, choose current or captured attribution deliberately. The dispatcher captures the manager's instigator at acceptance, falling back to the casualty, and sets that captured object explicitly; it does not call the helper that ignores its argument.
- Do not infer successful detonation from calling this void method: it silently returns on several failures. Resource/trigger checks and clear diagnostics are needed in our implementation.
- Defer gameplay execution outside the damage callback, latch before queuing, and define character deletion behavior. Do not add client damage RPCs to compensate for unverified networking.

## Testing approach agreed with Jake

Jake has a dedicated server, currently empty, and one normal game client. No second Steam account or second game client is required for the first server/client test. Jake accepts restarts while iterating.

Implement the experimental explosion before network verification, after resolving the real prefab reference and spawn details. Test authority decisions, damage, client effects, duplication, chains, and JIP together. Separate logging-only Workshop publication and a maintenance-window procedure are not prerequisites. This changes test sequencing, not the authority/once-per-entity contract or the obligation to label unverified behavior honestly.

## Selected installed resource and experimental implementation

Jake copied this reference from Workbench:

`{72BEEF40AF179763}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Large.et`

Prefab instance inspection shows TimerTriggerComponent, HitEffectComponent and RplComponent. The supplied screenshot confirms enabled, trigger-once ExplosionDamageContainer, explosion damage/impulse effects, SpawnDecalEffect and SpawnParticleEffect. One visible base effect is 1000 damage with 10 damage distance; several effects exist, so these values do not establish the full radius/strength. Charge Weight shows 45000; units were not verified. Delete On Trigger appears unchecked.

Timer start/duration fields were not visible in the supplied screenshot. **Subsequent local runtime confirms automatic activation:** Jake observed real explosions, and each supplied spawn diagnostic reports timerLeft=0.05 s, alreadyTriggered=0 and damageContainers=1 during configuration. The source uses no OnUserTrigger/SetLive call; do not add a second activation path. See the [local explosion record](../test-runs/2026-10-01-local-explosions.md).

`EC_CasualtyExplosion.c` queues via a static dispatcher, rechecks source and non-proxy damage authority, spawns unparented/world-space, sets captured instigator and ignores only the casualty. It cancels safely on missing source/authority and never retries. This revision now has local runtime evidence for deferred spawning, real explosions, and latch behavior. Full compiler transcript was not supplied. Prefab lifetime/cleanup, deletion/queue edge cases, quantified damage, attribution and client effects remain unverified. No custom RPC or second damage path was added.

[Public timer-trigger API](https://community.bistudio.com/wikidata/external-data/arma-reforger/ArmaReforgerScriptAPIPublic/interfaceTimerTriggerComponent.html) documents GetTimer (seconds remaining), SetTimer, WasTriggered and SetInstigator. Its documentation does not prove the selected prefab's start configuration or replication behavior. Installed compilation/runtime checks remain necessary.
