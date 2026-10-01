# Local authority-gated dry run — 2026-10-01

Recorded by **Element0**, 2026-10-01 approximately 11:36 AM America/Chicago, from Jake's pasted runtime logs. Element0 did not compile or run Workbench.

## Context and limits

- Revision: hit-zone-authority-gated logging-only `EC_CharacterLifeStateProbe.c`; commit not recorded.
- Session: continuing local Workbench Game Master test. No remote client or dedicated server evidence supplied.
- Previously displayed game/tools build: 1.8.0.13; not independently reconfirmed in these excerpts.
- Current output confirms `ALIVE=0; INCAPACITATED=1; DEAD=10`.
- No explosion, damage, deferred task, or RPC implementation is active.
- Runtime output establishes that the current revision loaded/executed; a full new compiler transcript was not supplied.

## Results

| Character / event | Role | Decision | Result |
| --- | --- | --- | --- |
| Local entity ending `026E`: `ALIVE(0) -> DEAD(10)` | `NON_PROXY` | `would detonate` | Direct-death acceptance observed |
| Local entity ending `0366`: `ALIVE(0) -> INCAPACITATED(1)` | `NON_PROXY` | `would detonate` | First incapacitation accepted |
| Same `0366`: `INCAPACITATED(1) -> ALIVE(0)` | `NON_PROXY` | `skip: not a casualty entry (latch preserved)` | Recovery does not reset latch |
| Same `0366`: `ALIVE(0) -> INCAPACITATED(1)` | `NON_PROXY` | `skip: already latched` | Repeat incapacitation does not accept again |
| Same `0366`: `INCAPACITATED(1) -> DEAD(10)` | `NON_PROXY` | `skip: already latched` | Death while unconscious does not accept again |

All displayed callbacks report `isJIP=0; latched=1; replayCasualty=0`. The latch is reserved before diagnostic output, so `latched=1` on the first acceptance is expected. IDs are local diagnostics, not cross-process replication identifiers.

## Selected exact evidence

```text
[ExplosiveCasualties][dry-run] compiled enum: ALIVE=0; INCAPACITATED=1; DEAD=10
[ExplosiveCasualties][dry-run] entity=0x400000000000026E {}; life-state ALIVE(0) -> DEAD(10); isJIP=0; latched=1; replayCasualty=0
[ExplosiveCasualties][dry-run] entity=0x400000000000026E {}; damageRole=NON_PROXY
[ExplosiveCasualties][dry-run] entity=0x400000000000026E {}; would detonate (damage-authority dry run only; NO blast)
[ExplosiveCasualties][dry-run] entity=0x4000000000000366 {}; life-state ALIVE(0) -> INCAPACITATED(1); isJIP=0; latched=1; replayCasualty=0
[ExplosiveCasualties][dry-run] entity=0x4000000000000366 {}; damageRole=NON_PROXY
[ExplosiveCasualties][dry-run] entity=0x4000000000000366 {}; would detonate (damage-authority dry run only; NO blast)
[ExplosiveCasualties][dry-run] entity=0x4000000000000366 {}; life-state INCAPACITATED(1) -> ALIVE(0); isJIP=0; latched=1; replayCasualty=0
[ExplosiveCasualties][dry-run] entity=0x4000000000000366 {}; damageRole=NON_PROXY
[ExplosiveCasualties][dry-run] entity=0x4000000000000366 {}; skip: not a casualty entry (latch preserved)
[ExplosiveCasualties][dry-run] entity=0x4000000000000366 {}; life-state ALIVE(0) -> INCAPACITATED(1); isJIP=0; latched=1; replayCasualty=0
[ExplosiveCasualties][dry-run] entity=0x4000000000000366 {}; damageRole=NON_PROXY
[ExplosiveCasualties][dry-run] entity=0x4000000000000366 {}; skip: already latched
[ExplosiveCasualties][dry-run] entity=0x4000000000000366 {}; life-state INCAPACITATED(1) -> DEAD(10); isJIP=0; latched=1; replayCasualty=0
[ExplosiveCasualties][dry-run] entity=0x4000000000000366 {}; damageRole=NON_PROXY
[ExplosiveCasualties][dry-run] entity=0x4000000000000366 {}; skip: already latched
```

## Conclusion and next step

**Pass for the observed local non-proxy acceptance and latch paths.** The sequence additionally covers recovery followed by repeat incapacitation and death while unconscious. A fresh uninterrupted unconscious-to-dead sequence has not separately been demonstrated, although that final transition is observed here.

Still unverified: observed proxy rejection, missing-hit-zone rejection, remote-player/AI authority decisions, JIP/initialization, persistence, migration, and actual explosion behavior. No networked safety claim follows from this local result.

Next: establish available host/client test setup with Jake and test two networked instances with the same addon, still logging-only. Correlate one controlled casualty at a time using explicit labels, not local entity IDs. Only the appropriate damage-authority instance may accept; observed proxies must skip without setting the acceptance latch. Absence of a client callback is not a demonstrated proxy rejection. Stop on unexpected client acceptance.

References: [installed source notes](../source-notes/2026-10-01-character-lifecycle.md), [test checklist](../TESTING.md), [resume checkpoint](../RESUME.md).
