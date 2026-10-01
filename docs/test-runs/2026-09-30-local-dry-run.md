# Logging-only trigger dry run: local verification

Recorded by Element0 on 2026-09-30 (America/Chicago) from Jake's supplied screenshot `Screenshot 2026-09-30 221829.png` and confirmation that the dry run was working. Jake performed the Workbench compilation and gameplay; Element0 did not run Workbench.

## Environment and scope

- World Editor displays `Version 1.8.0.13 (production)`; the running game overlay displays `1.8.0.13 (Steam)`. Installation/branch settings were not separately inspected.
- Project: `ExplosiveCasualties/addon.gproj`.
- Script: `ExplosiveCasualties/Scripts/Game/ExplosiveCasualties/EC_CharacterLifeStateProbe.c`, named-state logging-only dry-run revision. Commit hash not recorded.
- Scenario: base-game `ArmaReforger/Worlds/GameMaster/GM_Eden.ent` (Everon), running in World Editor Play/Game Master.
- Characters: vanilla soldiers spawned through Game Master. Other addon inventory not recorded.
- Session: local Workbench Play; authority/topology not verified. No remote client or dedicated server tested.
- Explosion resource: none. No explosives, damage changes, deferred work, or authority gate implemented.

## Evidence

The screenshot shows the current revision executing, establishing that it compiled and loaded. A separate full compilation transcript for this revision was not supplied; do not claim a warning-free build.

Relevant lines transcribed from the screenshot below. Player account identifiers and unrelated log details are omitted. Entity IDs are session-local, not network identities.

```text
[ExplosiveCasualties][dry-run] compiled enum: ALIVE=0; INCAPACITATED=1; DEAD=10

[ExplosiveCasualties][dry-run] entity=0x40000000000001EB {}; life-state ALIVE(0) -> DEAD(10); isJIP=0; latched=1; replayCasualty=0
[ExplosiveCasualties][dry-run] entity=0x40000000000001EB {}; would detonate (LOCAL dry run only; authority NOT checked)

[ExplosiveCasualties][dry-run] entity=0x400000000000023A {}; life-state ALIVE(0) -> INCAPACITATED(1); isJIP=0; latched=1; replayCasualty=0
[ExplosiveCasualties][dry-run] entity=0x400000000000023A {}; would detonate (LOCAL dry run only; authority NOT checked)

[ExplosiveCasualties][dry-run] entity=0x400000000000023A {}; life-state INCAPACITATED(1) -> ALIVE(0); isJIP=0; latched=1; replayCasualty=0
[ExplosiveCasualties][dry-run] entity=0x400000000000023A {}; skip: not a casualty entry (latch preserved)

[ExplosiveCasualties][dry-run] entity=0x400000000000023A {}; life-state ALIVE(0) -> DEAD(10); isJIP=0; latched=1; replayCasualty=0
[ExplosiveCasualties][dry-run] entity=0x400000000000023A {}; skip: already latched
```

## Results

| Case | Observed result | Assessment |
| --- | --- | --- |
| Current dry run compiles/loads and executes | Named-state logs and decision lines appear | Confirmed locally by runtime evidence |
| Compiled enum constants | `ALIVE=0`, `INCAPACITATED=1`, `DEAD=10`; observed states are named, not `UNKNOWN` | Confirmed for this build; keep using named constants |
| Separate character enters direct death | Entity ending `01EB`: `ALIVE -> DEAD`, one `would detonate` shown | Passed for the displayed callback sequence |
| First incapacitation accepts once | Entity ending `023A`: `ALIVE -> INCAPACITATED`, `would detonate` | Passed locally |
| Recovery retains latch | Same entity: `INCAPACITATED -> ALIVE`, `latched=1`, skip | Passed locally |
| Death after recovery does not accept again | Same entity: `ALIVE -> DEAD`, `skip: already latched` | Passed locally |

The console also displays accumulated error/warning counters and unrelated GUI warnings. The screenshot does not establish the cause of all those entries. Do not attribute them to the addon or claim they are all harmless without inspecting the actual messages.

## Not established

- Incapacitated-to-dead without recovery, repeated incapacitation after recovery, duplicate/unchanged callbacks, player respawn, or full medical-behavior regression.
- JIP replay policy, unmarked initialization callbacks, spawned/pre-existing casualties, save/load, authority migration, or hot-reload latch behavior.
- Authority gating, multiplayer replication, deferred execution, explosion damage/effects, attribution, chain reactions, performance, or release readiness.

The local latch is unsynchronized and non-persistent. Multiple machines can independently print `would detonate`; this diagnostic is not permission to apply gameplay damage.

## Resume

See [RESUME.md](../RESUME.md) for the next-session checklist. Next work is installed-source inspection and an authority-gated logging-only pass before adding a verified, deferred vanilla explosion path.

## References

- Primary evidence: Jake's screenshot and confirmation in the 2026-09-30 conversation; no public URL. The attachment is not claimed to have been saved into the repository.
- [Bohemia: Scripting Modding](https://community.bistudio.com/wiki/Arma_Reforger:Scripting_Modding)
- [Bohemia: Character damage-manager API](https://community.bistudio.com/wikidata/external-data/arma-reforger/ArmaReforgerScriptAPIPublic/interfaceSCR__CharacterDamageManagerComponent.html)
- [Original probe observations](2026-09-30-local-probe.md)
