# Local gameplay explosion test — 2026-10-01

Recorded by **Element0** from Jake's runtime log and report: “Haha! It works!”

## Environment and scope

- Session: local Workbench Play; dedicated server/client not tested in this run.
- Previously established tools build: 1.8.0.13; version was not printed again in this excerpt.
- Addon revision: experimental gameplay trigger + deferred dispatcher; no commit identifier supplied.
- Resource: `{72BEEF40AF179763}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Large.et`.
- Setup: two individual riflemen, then US/USSR rifle squads and additional individual soldiers.
- Runtime proves the new source compiled sufficiently to load and execute. Full compiler transcript was not supplied.

## Results

**Local explosion implementation passes its first runtime test.** Jake observed actual explosions, not just spawn logs.

- Incapacitation and direct death both accept one deferred TNT request per eligible character.
- Successful spawn diagnostics report `damageContainers=1`, `timerLeft=0.05s`, and `alreadyTriggered=0` before the prefab activates. No manual trigger is needed for this tested asset/build.
- All displayed damage roles are `NON_PROXY`; all displayed transitions have `isJIP=0`.
- Characters that later die after incapacitation are rejected as already latched; no second spawn for the same source is shown.
- Many distinct characters accept and spawn separate explosions during the squad test. The progression is consistent with intended casualty-driven chain reactions; the excerpt does not include damage contexts proving the cause of every casualty.
- A character ending `1778` became unconscious, spawned one blast, recovered, and retained its latch. A later death callback for that character appears at the end of the supplied excerpt, but the corresponding decision is truncated. Do not claim that final decision was captured.
- No addon failure/exception is visible in the supplied excerpt. This is not a performance benchmark, cleanup check, or network pass.

## Representative evidence

### Incapacitation followed by death: one request

```text
entity=0x40000000000001EA {}; life-state ALIVE(0) -> INCAPACITATED(1); isJIP=0; latched=1; replayCasualty=0
entity=0x40000000000001EA {}; damageRole=NON_PROXY
entity=0x40000000000001EA {}; accepted: deferred TNT blast requested
entity=0x40000000000001EA {}; spawned explosion=619; position=<5408.92,77.7712,5662.06>; damageContainers=1
entity=0x40000000000001EA {}; timerLeft=0.05s; alreadyTriggered=0; prefab timer owns activation (NO manual trigger)
entity=0x40000000000001EA {}; life-state INCAPACITATED(1) -> DEAD(10); isJIP=0; latched=1; replayCasualty=0
entity=0x40000000000001EA {}; skip: already latched
```

### Direct death on a different character

```text
entity=0x4000000000000227 {}; life-state ALIVE(0) -> DEAD(10); isJIP=0; latched=1; replayCasualty=0
entity=0x4000000000000227 {}; damageRole=NON_PROXY
entity=0x4000000000000227 {}; accepted: deferred TNT blast requested
entity=0x4000000000000227 {}; spawned explosion=641; position=<5408.72,77.5858,5654.13>; damageContainers=1
entity=0x4000000000000227 {}; timerLeft=0.05s; alreadyTriggered=0; prefab timer owns activation (NO manual trigger)
```

### Recovery after an accepted explosion

```text
entity=0x4000000000001778 {}; life-state ALIVE(0) -> INCAPACITATED(1); isJIP=0; latched=1; replayCasualty=0
entity=0x4000000000001778 {}; accepted: deferred TNT blast requested
entity=0x4000000000001778 {}; spawned explosion=6181; position=<5519.6,86.1345,5622.1>; damageContainers=1
entity=0x4000000000001778 {}; life-state INCAPACITATED(1) -> ALIVE(0); isJIP=0; latched=1; replayCasualty=0
entity=0x4000000000001778 {}; skip: not a casualty entry (latch preserved)
```

Entity identifiers are local, not cross-process replication IDs. `latched=1` is printed after acceptance/reservation and is expected on the first accepted transition.

## Other warnings

The excerpt contains a vanilla particle-material warning: outdated `AlphaToEmissiveLV`, converted automatically during loading. It also contains Game Master UI warnings concerning EditBoxFilterComponent and missing highlight widgets. None shown is an addon compiler error or a demonstrated blast failure. Do not modify vanilla assets merely to silence these warnings.

## Outstanding / next

- Publish an experimental build after deliberately selecting a license; no publication occurred as part of this test record.
- Use Jake's empty dedicated server + his ordinary client. Check authority/client behavior, AI and player casualties, effects/sound, nearby damage, duplicates, respawn, and JIP/reconnect together with explosions enabled.
- Verify entity cleanup, exact damage/attribution, vehicles/occupants, initialization replays, deletion and failure paths, and larger-chain performance separately.
- No extra activation call or blast-strength change is justified by this successful local run.

See [RESUME.md](../RESUME.md), [TESTING.md](../TESTING.md), and [the selected-asset source notes](../source-notes/2026-10-01-secondary-explosion.md).
