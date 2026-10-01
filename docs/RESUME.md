# Resume here — ExplosiveCasualties

Updated by **Element0**, 2026-10-01, after Jake's successful local gameplay explosion run.

**Current experimental gameplay source passed its first local explosion test.** Jake observed real explosions; the log confirms one deferred spawn per accepted character, latch rejection on subsequent death, and recovery preserving the latch. Multiple squad casualties produced individual blasts, consistent with intended chains. See the [local explosion record](test-runs/2026-10-01-local-explosions.md). Next: choose a license, publish/distribute this experimental build, and test on Jake's empty dedicated server plus his normal client. No second Steam account or separate diagnostic-only release is required. Network correctness is NOT established by this local run.

## Current checkpoint

- Workbench project: `ExplosiveCasualties/addon.gproj`; keep generated identifiers/database intact. Repository root and addon root are separate.
- Trigger: `ExplosiveCasualties/Scripts/Game/ExplosiveCasualties/EC_CharacterLifeStateProbe.c`. Filename retained, but this is no longer a harmless probe.
- New dispatcher: `ExplosiveCasualties/Scripts/Game/ExplosiveCasualties/EC_CasualtyExplosion.c`.
- Selected vanilla resource, copied by Jake:
  `{72BEEF40AF179763}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Large.et`.
- Inspected prefab components: TimerTriggerComponent, HitEffectComponent, RplComponent. Screenshot confirms enabled, trigger-once ExplosionDamageContainer with damage/impulse effects, plus decal and particle effects. One visible base effect shows damage 1000 and damage distance 10; these are NOT a complete blast-strength/radius specification. Charge Weight displayed 45000; do not infer undocumented units. Delete On Trigger appears unchecked.
- Local runtime confirms the prefab automatically activates without OnUserTrigger or SetLive. Spawn diagnostics show damageContainers=1, timerLeft=0.05 s and alreadyTriggered=0 during configuration. Sound, entity cleanup and network effects remain unverified.
- Previous local tests (1.8.0.13): direct death accepted once; incapacitation/recovery/re-incapacitation/death on one character accepted once, all NON_PROXY. Compiled constants: ALIVE=0, INCAPACITATED=1, DEAD=10. Use names, not numbers.
- Jake ran the new gameplay source locally and reported real explosions. Runtime proves loading/execution; a full compiler transcript was not supplied. No package, publication, license decision, or Git commit has been reported. Element0 cannot run Workbench and did not perform the runtime test.

## Implemented experimental policies

- Preserve vanilla handler; no duplicate event subscription.
- Reject JIP/unchanged/non-casualty observations and missing/proxy default hit zones.
- Conservative separate replay-casualty exclusion remains; initialization/JIP coverage is not proven.
- Reserve one request latch per character BEFORE vanilla handler and queuing. Recovery, cancellation and failed spawning never reset it. Start fresh sessions/characters after source changes.
- Capture origin + 0.3 m world-up and current instigator at acceptance. Credit existing instigator; use casualty itself only if none exists. Attribution effects on score/teamkills remain untested.
- Queue one static-dispatcher call with a nominal 1 ms delay (actual scheduling is engine/frame-dependent). Recheck source, game world, damage manager and non-proxy hit zone before spawning.
- Initial deletion policy: cancel if the source/component disappears or authority is lost before execution. The latch remains reserved; no retry. This deliberately does not guarantee a blast after immediate corpse deletion. Dispatcher/engine-reference lifetime behavior needs testing.
- Spawn independent world-space explosion; set captured instigator and ignore ONLY the source character, not its root hierarchy or vehicle occupants. Own-blast survival/gear and nearby-chain behavior need testing.
- Prefab timer owns activation/networking/lifecycle. No manual activation, broadcast damage RPC, or extra radial-damage path. Resource/spawn/trigger/damage-container failures log explicitly.
- Prefixes: `[ExplosiveCasualties][trigger]` and `[ExplosiveCasualties][blast]`. A spawn log is not proof of detonation, damage, sound or client effects.

## Immediate next steps

1. Choose a license deliberately, then publish an experimental Workshop build, preferably Unlisted. Do not infer publication or privacy from this plan. Describe it as an experimental casualty-explosion addon with multiplayer behavior under test.
2. Add the exact published revision to Jake's dedicated-server configuration and join using his regular client. Server OS/hosting/configuration and Workshop ID have not been supplied; ask rather than invent them. Restarts are acceptable.
3. Collect separately labelled server/client logs. First test one AI casualty, then a player casualty. Expect one authoritative request/spawn; observed client proxy callbacks must not accept or spawn. Absent callbacks are not an observed proxy-rejection pass. Local entity IDs are not cross-process IDs.
4. Check actual client explosion visuals/sound and nearby damage, then small chains. Verify respawn, joining/reconnecting with existing casualties, and no replayed blasts. Keep explosions enabled for these network tests.
5. Check successful-spawn entity cleanup. Delete On Trigger appeared unchecked; do not infer either a leak or cleanup without observation. Investigate if entities persist after effects.
6. Continue targeted vehicle/occupant, deletion, attribution, initialization and performance tests. The recovery/death decision for entity ending 1778 was truncated in the latest log; recovery latch retention is confirmed, its final skip is not captured.

For any future source change: stop Play, reload external changes from disk, compile with Shift+F7, start fresh characters/session, and never hot-reload midway through a latch sequence. No behavior changes were made while recording the successful local run.

## Evidence and limits

- [Latest local explosion test](test-runs/2026-10-01-local-explosions.md) covers the CURRENT gameplay revision: real explosions reported, direct-death/incapacitation acceptance, later-death latch rejection, recovery, and multi-casualty spawns. Damage causality for every casualty and cleanup were not instrumented.
- [Earlier local diagnostic tests](test-runs/2026-10-01-local-authority-dry-run.md) remain historical evidence for the logging-only revision.
- [Installed lifecycle source](source-notes/2026-10-01-character-lifecycle.md): controller subscription and hit-zone authority pattern; do not add another subscription.
- [Installed explosion source and selected asset](source-notes/2026-10-01-secondary-explosion.md): vanilla ignores its instigator argument and excludes root hierarchy; adapted implementation deliberately differs.
- [Design](DESIGN.md), [test matrix](TESTING.md), [setup](SETUP.md).

Authority migration, persistence, medical-overhaul compatibility, large chains, JIP and dedicated/client correctness remain unverified. Local entity IDs are not cross-process replication IDs. Keep account/server secrets out of docs and published assets.
