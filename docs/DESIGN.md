# Design and implementation plan

## Behavior contract

An ordinary soldier should produce one large explosion when they enter unconsciousness or death. Direct death must work without a preceding unconscious state. The later death of an already-detonated unconscious character must not explode again.

The latch belongs to the character entity, not to a player account, squad, or network connection. A newly spawned character can detonate once. Revival of the same character does not reset the latch for the initial release.

Nearby soldiers may be injured or killed by the blast and trigger their own explosions. This is intentional. All damage must still be authoritative, with no duplicate client-generated blasts.

Replayed state on initialization, JIP, or loading existing casualties must not cause explosions. Persistence/save-load support is not assumed; its semantics need separate validation before being advertised.

## Scope

**First release:** vanilla AI and player soldiers, one fixed verified explosion, one detonation per entity, correct local and multiplayer behavior.

**Later:** user-selectable explosion resources, AI/player filtering, settings UI, compatibility integrations, and optional chain-reaction load limits.

**Not needed initially:** replacement soldier models, new animations, custom particles, a full game mode, or edits to base-game files.

An anti-tank mine is a candidate, not a claim about current blast strength. Select by observed damage, impulse, and effect, not merely by the weapon name. Mortar impact effects and mine effects may have different configuration/placement requirements.

## Proposed integration

Extend `SCR_CharacterDamageManagerComponent` through Enforce Script's `modded class` mechanism. Preserve the original handler. This avoids making a replacement prefab for every soldier, but remains a broad script modification whose coverage and compatibility must be tested.

### Event flow — pseudocode, not implemented

```text
life-state callback:
    preserve the original handler
    reject replay/initialization callbacks
    reject unchanged states and transitions other than unconsciousness/death
    reject non-authoritative execution
    reject an entity that has already requested detonation
    capture blast position, selected resource, and attribution data
    set the per-entity latch BEFORE queueing work
    queue a one-shot detonation outside this damage callback

queued operation:
    use captured data with a verified vanilla explosion-spawning path
    generate damage exactly once on the authority
    let the verified networking path deliver effects to clients
    report resource/spawn failures clearly
```

No explosion code exists yet. Do not translate this directly into API calls without checking the installed source.

## Important decisions and caveats

### Authority and replication

The logging probe deliberately runs on all instances so callback behavior can be observed. The final blast must not. Determine an authority test from vanilla code that works for standalone, listen-server, and dedicated-server sessions. Avoid an improvised RPC that causes each client to apply explosion damage.

### Initialization and JIP

The callback supplies `isJIP`, but its exact semantics need inspection. A false value is not automatically proof of a newly incurred casualty. Test pre-existing corpses, spawned casualties, and any supported save/load workflow. If initialization uses indistinguishable transitions, add an explicit readiness/initialization guard based on verified engine lifecycle behavior.

### Reentrancy and chain reactions

Latch before scheduling; never spawn a damage-producing blast synchronously inside another damage callback. Deferred chain reactions can still create heavy load in a densely packed crowd. Start with small groups and measure a larger case. If a delay or burst limit is needed, document how it changes timing rather than silently claiming unlimited safe scale.

### Character deletion and queued work

Capturing position is not enough if the queued method itself belongs to a deleted component. Check callback lifetime handling. A stable dispatcher may be required so deletion either safely cancels the operation or permits a captured-data detonation according to an explicit policy. The desired first-release behavior is one blast for an accepted casualty even if the corpse is cleaned up immediately, where the engine can safely support it.

### Attribution

Open decision: should a secondary casualty blast credit the original attacker, the casualty, or an unattributed environmental cause? This affects score, teamkill handling, and chain-reaction attribution. Do not pick an instigator merely to satisfy a non-null API argument.

### Save/load and authority migration

An unsynchronized latch may suffice on a fixed authority but does not prove persistence or migration support. Explicitly test or declare these unsupported. JIP clients must not be allowed to initiate damage regardless of latch replication.

### Compatibility

Other addons can modify the same damage-manager class. Preserve `super`, test without unrelated mods first, and never claim medical-overhaul compatibility without observed results. Nonstandard characters using another damage component may require separate integration.

## Verification table

| Item | Current evidence | Next verification |
| --- | --- | --- |
| Life-state hook | Public API documents `OnLifeStateChanged(ECharacterLifeState, ECharacterLifeState, bool)` | Installed implementation/caller and actual callback logs |
| State constants | Older public source examples use `INCAPACITATED` and `DEAD` | Check installed enum; probe does not hard-code these |
| Class extension | Official scripting guide documents script modding | Compile the included override in this project |
| Secondary explosion | API documents `SecondaryExplosion(ResourceName, Instigator, EntitySpawnParams)` | Installed implementation, call site, resource compatibility, networking |
| Explosion resource | None selected | Copy real resource identifier from Workbench |
| Authority gate | Not selected | Confirm vanilla pattern on local/listen/dedicated sessions |
| Deferred execution | Not selected | Verify queue API, lifecycle, and deletion behavior |
| Initialization suppression | `isJIP` parameter exists; coverage unverified | Test JIP, initialization, and spawned/pre-existing casualties |
| Attribution | Undecided | Choose behavior and inspect instigator API |

## Milestones

- [x] Repository documentation and Git hygiene.
- [x] Logging-only probe written; not compiled.
- [ ] Workbench addon created with a real project descriptor and base-game dependency.
- [ ] Probe compiles and logs unconsciousness plus direct death.
- [ ] Explosion resource and vanilla spawn/replication path verified.
- [ ] Authoritative, deferred, once-per-entity detonation implemented.
- [ ] Local acceptance tests pass.
- [ ] Listen-server and second-client tests pass.
- [ ] Dedicated-server/JIP tests pass before claiming those support levels.
- [ ] License and Workshop packaging/publication decisions made.

## References

- [Scripting Modding](https://community.bistudio.com/wiki/Arma_Reforger:Scripting_Modding)
- [Character damage-manager API](https://community.bistudio.com/wikidata/external-data/arma-reforger/ArmaReforgerScriptAPIPublic/interfaceSCR__CharacterDamageManagerComponent.html)
- [Explosion configuration discussion](https://reforger.armaplatform.com/news/modding-update-sept-10-2024)

Current installed source/assets take precedence over online examples. Record versions with each verification.
