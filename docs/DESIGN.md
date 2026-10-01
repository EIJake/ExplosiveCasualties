# Design and implementation

Current checkpoint: [RESUME.md](RESUME.md). Previous large-TNT/once-per-character code passed local gameplay tests and Jake reported success on his Linux dedicated server with approximately 130 other mods, publication and commit/push. See the [server report](test-runs/2026-10-01-linux-dedicated-large-tnt.md). **Jake ran the small/repeat/delayed revision locally and on the Linux server: triggers work. The initial silence was corrected locally by saving a native SoundComponent enabled override in our inherited prefab. Jake confirmed audio in World Editor/Game Master, and Element0 verified `Enabled 1` on disk. Corrected server-client audio still needs retesting.**

## Behavior contract — revised 2026-10-01

Each real life-state transition into INCAPACITATED or DEAD requests a small TNT explosion about two seconds later. No once-per-character gate:

- ALIVE -> INCAPACITATED: request one.
- INCAPACITATED -> DEAD: request another, even if the first remains pending.
- ALIVE -> DEAD: request one.
- INCAPACITATED -> ALIVE: no new request and no cancellation.
- Recovered character -> INCAPACITATED again: request another.
- Same-state notification, JIP/replay callback or damage proxy: no request.

Nearby casualty chains are intentional. Repeat explosions for distinct transitions on one character are now intentional too; client-generated or unchanged-notification duplicates are not. The user's request to remove the gate removes the lifetime latch, NOT damage authority or JIP filtering. The old permanent replay-casualty flag is removed: later real transitions are eligible.

## Architecture

The historical `EC_CharacterLifeStateProbe.c` modifies SCR_CharacterDamageManagerComponent, preserving vanilla's handler and existing controller subscription. No extra subscription/replacement soldier prefabs.

```text
life-state callback:
    reject JIP, unchanged state, non-casualty entries
    require owner, current game world and non-proxy default hit zone
    capture attribution; increment diagnostic request number
    always call original handler and log decision
    queue an independent static dispatcher call with 2000 ms delay

queued dispatcher:
    recheck source/world/component and non-proxy damage authority
    validate captured instigator and selected resource
    sample current source position + 0.3 m world-up
    spawn world-space vanilla explosion; validate timer/damage container
    apply captured instigator and source-only ignore list
    log spawn/timer state; let prefab own activation and replication
```

No per-character acceptance latch, last-request gate, queue replacement or request coalescing. The counter labels events, not eligibility. No `Remove()` call cancels older pending blasts. Multiple calls for one source have independent captured instigators/labels and due times. All damage stays outside the current damage callback.

### Resource and activation

Jake supplied vanilla `{2F690C7C59FB4DBF}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Small.et`; its blasts triggered locally/on the server but were silent. The dispatcher now selects his addon-owned derivative, `{B0DC0394D3820463}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Small_Inherited.et`. Its generated `.meta` matches the reference, and its saved `.et` now overrides the inherited SoundComponent with `Enabled 1`. Jake confirmed sound in World Editor/Game Master. Corrected audio on the server's client remains a follow-up test. No global vanilla override. Runtime Activate was removed; native-component presence/IsActive diagnostics remain without manual sound playback/RPC.

Jake identified a Workbench persistence quirk: the disposable empty inspection world needed saving for object changes to persist. Apply/save overrides to the addon-owned prefab and verify the actual `.et`, rather than relying solely on the inspector checkbox or Apply button. Do not save modifications to vanilla worlds/prefabs. See the [audio investigation](test-runs/2026-10-01-small-tnt-silent.md).

Keep vanilla spawn/configure pattern without additional OnUserTrigger/SetLive. LARGE automatically activated with a 0.05 s timer at configuration; SMALL inspector showed a 0.050 s timer, and Jake reports proper triggering in both environments. Exact delayed-detonation timing remains unmeasured. Logs include timerLeft/alreadyTriggered; already-triggered configuration is a warning that attribution/ignore setup may be too late. Investigate rather than layer activation paths.

### Authority and replay safety

Vanilla gates damage-related work on HitZone.IsProxy(). Require an existing non-proxy default hit zone at acceptance AND execution. Client control does not establish damage authority. No custom damage RPC.

JIP/replay callbacks never queue a blast. Unlike the earlier build, they do not permanently blacklist a character: a later real casualty entry can request independently. Same-state callbacks skip. This does not prove every initialization path is filtered; pre-existing/spawned casualties with isJIP=false remain targeted tests. Duplicate notification of an identical changed-state pair is not explicitly deduplicated by a new event-ID mechanism.

### Delay, position and source lifetime

`DEFER_MS=2000` delays SPAWN. Actual detonation is 2000 ms plus engine scheduling and the prefab timer, not an exact two-second fuse. Do not alter timer APIs just to subtract an unverified SMALL timer duration.

Position is sampled at execution, source origin + 0.3 m world-up. This intentionally changes the previous captured-position policy so a falling, transported, or recovered soldier carries the pending blast to its current location. Spawn is unparented/world-space.

Recovery and later casualty transitions do not cancel or postpone a pending request. Source/component deletion, world change, loss of damage authority or unavailable attribution cancels that request with a diagnostic and no retry. Immediate-deletion blast guarantee and authority-migration/persistence support are not implemented. Longer delay increases exposure to source deletion; test it.

### Attribution and own-source exclusion

Capture current instigator at each transition; fallback to casualty if absent. Apply that captured object to the explosion trigger, rather than reading changed attribution two seconds later. Score/friendly-fire attribution is not separately verified.

Ignore ONLY source character in ExplosionDamageContainer, not root hierarchy/vehicle/passengers. Own blast need not kill a recoverable soldier; other characters' blasts, gear, impulse and other effects can still affect it. Test rather than promise survival. Repeat transitions can produce more chain-wave blasts than the earlier build.

### Failure handling, cleanup and load

Resource/spawn/missing timer/damage-container failures log explicitly. Delete malformed spawned entities; do not retry. The new SMALL prefab cleanup and queue/reference lifetime need tests. No custom successful-spawn cleanup or load/concurrency cap is added. A spawn log alone is not proof of effects, damage, cleanup or cross-client replication.

## Test route and evidence limits

Stop Play, reload both files, compile and locally check small/repeat/delay behavior. Jake can upload the revised Workshop version and restart his Linux server with the normal client/full mod stack; no second Steam account or diagnostic-only release is required.

Previous server report is aggregate success with approximately 130 mods, without process-labelled logs or exact mod list. It does not individually establish JIP/reconnect, proxy rejection, cleanup, performance, vehicle behavior or compatibility with every medical overhaul. Keep historical latch records as history, not the current contract.

## Sources

- [Installed lifecycle notes](source-notes/2026-10-01-character-lifecycle.md)
- [Installed explosion source and LARGE asset evidence](source-notes/2026-10-01-secondary-explosion.md)
- [Previous local gameplay test](test-runs/2026-10-01-local-explosions.md)
- [User-reported Linux server test](test-runs/2026-10-01-linux-dedicated-large-tnt.md)
- [Scripting Modding](https://community.bistudio.com/wiki/Arma_Reforger:Scripting_Modding)
- [Character damage-manager API](https://community.bistudio.com/wikidata/external-data/arma-reforger/ArmaReforgerScriptAPIPublic/interfaceSCR__CharacterDamageManagerComponent.html)
- [Timer trigger API](https://community.bistudio.com/wikidata/external-data/arma-reforger/ArmaReforgerScriptAPIPublic/interfaceTimerTriggerComponent.html)

Installed source/runtime takes precedence over online mirrors. Written code/checklists are not test results.
