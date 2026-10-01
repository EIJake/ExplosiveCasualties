# Original numeric probe: local Workbench observations

Recorded by Element0 from Jake's compilation output, runtime logs, screenshots, and descriptions in the 2026-09-30 session. Jake performed the compilation and gameplay; Element0 did not run Workbench.

## Environment and scope

- Game/Tools versions and branches: not recorded; still needed.
- Addon: `ExplosiveCasualties/addon.gproj`; original numeric logging probe, before the trigger dry-run revision. Commit/revision not recorded.
- Scenario: base-game `Worlds/GameMaster/GM_Eden.ent` (Everon), opened in World Editor and run using Play.
- Session: local Workbench play; multiplayer topology/authority not established. No second-client, dedicated-server, or JIP test reported.
- Soldiers: ordinary vanilla US/USSR characters spawned via Game Master.
- Other addon list: not recorded.
- Explosion resource: none; the probe was logging-only.

## Compilation evidence

Jake supplied:

```text
Succeeded: Game( Scripts/Game ) in 893ms
Compilation finished...
Reloading game scripts
Script validation
```

The supplied output contained deprecation warnings in base-game files, not errors naming the probe. Subsequent runtime messages with the addon's prefix demonstrate that its override was loaded, rather than relying on compilation output alone.

## Runtime observations

An initial `0 -> 1; isJIP=0` was described as outright death. This was not adequate evidence of direct death: subsequent testing identified `0 -> 1` as unconsciousness.

In the clarified sequence, Jake stated that one soldier went unconscious, recovered, and was finally killed. Pasted logs included:

```text
[ExplosiveCasualties][probe] life-state 0 -> 1; isJIP=0
[ExplosiveCasualties][probe] life-state 1 -> 0; isJIP=0
```

The final screenshot was transcribed during the session as:

```text
[ExplosiveCasualties][probe] life-state 0 -> 10; isJIP=0
```

That last line is a screenshot transcription, not independently extracted file content. Confirm it against the installed enum and the updated diagnostic's compiled-enum output. The original probe did not print entity IDs, so matching these transitions to one character relies on Jake's observation, not machine-verifiable identity in the log.

## Conclusions and corrections

- **Confirmed locally:** addon override executes and observes unconsciousness, recovery, and death callbacks as reported by Jake.
- **Not confirmed:** a separate fresh-character death without preceding unconsciousness/recovery; unconscious-to-dead without recovery; enum definition in the installed build; initialization/JIP behavior; authority/replication; any once-only guard or explosion.
- Element0 initially called `0 -> 1` death, then quoted `DEAD=2` from a public source mirror. Neither was valid evidence for the installed build. The screenshot's reported `10` conflicts with that mirror's implicit enum ordering. Do not resolve this with hard-coded numbers.
- The updated dry run uses named constants, prints their compiled values, and labels unmatched values `UNKNOWN`. Any mismatch requires installed-source/caller inspection before gameplay code is added.
- No claim of unmodified medical behavior beyond the observed sequence, multiplayer compatibility, or explosion correctness follows from this test.

## Subsequent verification

The updated dry run was subsequently compiled/loaded and exercised by Jake in the same evening. Its compiled-enum output confirmed `ALIVE=0`, `INCAPACITATED=1`, `DEAD=10` on the displayed 1.8.0.13 build. A separate direct-death callback and the once-per-character unconsciousness/recovery/death latch sequence were observed. See the [dry-run record](2026-09-30-local-dry-run.md) for evidence; this original record's conclusions describe the earlier test only. Continue from [RESUME.md](../RESUME.md).

## References

- [Bohemia scripting guide](https://community.bistudio.com/wiki/Arma_Reforger:Scripting_Modding)
- [Bohemia character damage-manager API](https://community.bistudio.com/wikidata/external-data/arma-reforger/ArmaReforgerScriptAPIPublic/interfaceSCR__CharacterDamageManagerComponent.html)
- [Public enum source mirror](https://arexplorer.zeroy.com/_e_character_life_state_8c_source.html) — useful for member names, not evidence of installed numeric values.
- Primary test evidence: Jake's supplied logs/screenshots and observations in this session; no public URL. Player UUIDs and unrelated log details intentionally omitted.
