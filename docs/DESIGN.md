# Design and implementation

Current checkpoint: [RESUME.md](RESUME.md). **Experimental gameplay source passed its first local explosion test.** See the [gameplay record](test-runs/2026-10-01-local-explosions.md): Jake observed real explosions; direct death/incapacitation spawn once, later death skips, and recovery retains the latch. Multiple squad casualties produce blasts consistent with intended chains. Dedicated-server/client, JIP, cleanup and performance are still unverified. Prior [diagnostic tests](test-runs/2026-10-01-local-authority-dry-run.md) remain separate historical evidence.

## Behavior contract

An ordinary soldier produces one large explosion on entering unconsciousness or death. Direct death must work without unconsciousness first. The later death of a previously processed unconscious character must not explode again.

Eligibility belongs to a character entity, not a player account. Revival never resets the initial-release latch; a newly spawned character can request its own blast. Nearby casualties may trigger their own explosions. Chains are intentional; duplicates/client-generated damage are not.

Existing casualties must not spontaneously explode on initialization/JIP. Current replay filtering is provisional. Persistence, save/load, authority migration and medical-overhaul compatibility are not claimed.

## Implemented experimental architecture

`modded class SCR_CharacterDamageManagerComponent`, in the historical `EC_CharacterLifeStateProbe.c`, preserves vanilla's handler and existing controller subscription. No replacement soldier prefabs or extra event subscription.

```text
life-state callback:
    reject JIP/replay, unchanged state, non-casualty transitions
    reject missing owner/hit zone or damage proxy
    reject replay-excluded/already-latched entity or non-game world
    capture position and attribution
    reserve latch BEFORE vanilla handler and queuing
    always call original handler and log decision
    queue one deferred static-dispatcher call

queued dispatcher:
    recheck source/world/damage manager/hit-zone authority
    validate captured instigator and selected resource
    spawn independent world-space vanilla explosion
    validate timer trigger and explosion damage container
    apply captured instigator and character-only damage ignore list
    log spawn/timer state; let prefab own activation and replication
```

The dispatcher, static queued call and spawn/configure APIs now have local runtime evidence in the installed build. Jake performed the test; a full compiler transcript was not supplied. This is not network or failure-path verification.

### Resource and activation

Jake copied `{72BEEF40AF179763}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Large.et` from Workbench. Inspected components include TimerTriggerComponent, HitEffectComponent and RplComponent. Screenshot confirms enabled/trigger-once damage, impulse, particle and decal effects. One visible base effect shows 1000 damage and 10 damage distance; several effects exist, so neither value describes the whole blast. Charge Weight 45000 is recorded without inferring units.

Use vanilla's spawn/configure pattern, WITHOUT an additional OnUserTrigger or SetLive call. Local runtime confirms automatic timer activation, with timerLeft=0.05 s and alreadyTriggered=0 during configuration. Sound and network effects remain unverified. Diagnostic prints timerLeft and alreadyTriggered; if it already fired during creation, attribution/ignore-list setup is too late and needs correction. Do not solve a silent blast by blindly layering multiple activation paths.

Delete On Trigger appears unchecked. The prefab may manage lifetime elsewhere; cleanup is unverified. Do not infer successful cleanup or a leak from that checkbox alone. Observe the spawned entity and inspect lifecycle if it remains after effects. No custom successful-spawn cleanup is implemented yet.

### Authority and once-per-entity latch

Vanilla source gates damage-related work on `HitZone.IsProxy()`. Current acceptance requires an existing non-proxy default hit zone; execution repeats this check. A local player controlling a character does not establish damage authority. No custom RPC broadcasts a second damage path.

The instance-local latch is reserved before calling vanilla or queuing work, and remains set after recovery, cancellation or failure. It is not replicated/persistent. It passed the earlier local logging-only tests, including second unconsciousness. Networking, migration and save/load remain unverified; unexpected client acceptance must be fixed before claiming support.

### Replays and initialization

JIP callbacks never queue a blast. A replayed casualty sets a separate conservative exclusion flag, even on a proxy; this is not acceptance. Later casualty transitions on that same local instance remain excluded even after recovery. Healthy replayed characters remain eligible for later real casualty entries.

This does not handle every possible initialization event: `isJIP=false` is not proof of a fresh live casualty. Existing/spawned casualties, engine dispatch and readiness semantics need tracing/testing.

### Defer, placement and deletion

One static `EC_CasualtyExplosion.Spawn` call is queued with nominal 1 ms delay. Scheduling is engine/frame-dependent; gameplay damage must not occur synchronously in the current life-state/damage callback.

Position is captured at acceptance: entity origin + 0.3 m world-up. Spawn is unparented/world-space, not relative to a character or vehicle. Pose, occlusion, floors and vehicle placement need testing.

**Initial deletion policy:** cancel when source/component no longer exists, game world differs, or source loses damage authority before execution. Do not retry. A static dispatcher avoids requiring a still-live component method, but engine reference/queue lifetime needs empirical testing. This conservative first implementation does not guarantee the desired eventual blast when a corpse is immediately deleted. Revisit with a stable authority context if needed.

### Attribution and source exclusion

Capture `GetInstigator()` at acceptance; if absent, create an instigator for the casualty. Apply that captured object to the explosion trigger. This deliberately differs from vanilla SecondaryExplosion, which ignores its supplied argument and reads the manager's current instigator. Original-attacker credit is the initial policy, including chains; actual scoreboard/friendly-fire behavior remains untested.

Ignore only the source character in ExplosionDamageContainer. Do not ignore its root hierarchy: that could exempt vehicles/passengers. Own-blast survival is intended to leave revival possible, but carried gear, impulse, other effects and subsequent chains can still affect the casualty. Verify rather than promise survival.

### Failures and chains

Log invalid resource, failed spawn, missing timer/damage container, missing attribution, deletion and authority loss. Delete a malformed spawned entity with the vanilla helper. No retry storm or latch reset.

No custom concurrency/load limits yet. Latches and deferral prevent synchronous self-recursion, not large-wave performance costs. Test a single isolated casualty, then a few nearby soldiers before larger chains.

## Test route agreed with Jake

Use his empty dedicated server + one normal client, after a local compile/single-blast check. Implement explosions before multiplayer verification; combine authority, damage/effects, duplication, chains and JIP tests. No second Steam account, separate logging-only Workshop release or maintenance-window procedure required. A listen-server/two-client run remains optional additional coverage, not claimed by a dedicated-server pass.

## Verification and milestones

| Item | Evidence/status | Next |
| --- | --- | --- |
| Project/layout and original probe | Verified locally; generated metadata retained | Do not recreate project |
| Enum members | ALIVE=0, INCAPACITATED=1, DEAD=10 on 1.8.0.13 | Continue named comparisons |
| Subscription/authority pattern | Installed source supplied and inspected | Engine dispatch/init and network verification |
| Prior local diagnostic latch | Direct death and incapacitation/recovery/re-incapacitation/death passed NON_PROXY | Regression with real blasts |
| Resource | Local real explosions reported; damageContainers=1, timerLeft=0.05 s, alreadyTriggered=0 at configuration | Quantified damage/attribution, client effects and cleanup |
| Gameplay trigger/dispatcher | Local runtime pass: direct death/incapacitation, later-death latch rejection, recovery latch retention, multiple distinct casualty spawns | Publish experimental build; dedicated-server/client tests |
| Attribution/deletion/ignore policies | Explicitly implemented, untested | Score/gear/vehicle and immediate deletion tests |
| Dedicated/client/JIP | Not tested | Experimental build on Jake's server/client |
| Packaging/license | Not done; no license selected | Choose before publication |

## Sources

- [Installed lifecycle notes](source-notes/2026-10-01-character-lifecycle.md)
- [Installed explosion source and asset evidence](source-notes/2026-10-01-secondary-explosion.md)
- [Scripting Modding](https://community.bistudio.com/wiki/Arma_Reforger:Scripting_Modding)
- [Character damage-manager API](https://community.bistudio.com/wikidata/external-data/arma-reforger/ArmaReforgerScriptAPIPublic/interfaceSCR__CharacterDamageManagerComponent.html)
- [Timer trigger API](https://community.bistudio.com/wikidata/external-data/arma-reforger/ArmaReforgerScriptAPIPublic/interfaceTimerTriggerComponent.html)
- [Explosion configuration discussion](https://reforger.armaplatform.com/news/modding-update-sept-10-2024)

Installed runtime/source takes precedence over mirrors. Written code and checklists are not test results.
