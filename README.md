# ExplosiveCasualties

A very silly **Arma Reforger** addon: soldiers explode when they become unconscious or die outright. Nearby casualties can trigger chain reactions. Medics may wish to stand back.

## Status

**Experimental casualty explosions work locally. Dedicated-server/client verification is next.** This is no longer a logging-only addon.

Jake's 2026-10-01 gameplay run produced actual explosions on incapacitation and direct death. Each displayed character spawned once; subsequent deaths skipped, and recovery preserved the latch. Multiple squad casualties produced individual blasts, consistent with intended chain reactions. The prefab activates automatically: timerLeft=0.05 s during configuration, without a manual trigger. See the [local explosion record](docs/test-runs/2026-10-01-local-explosions.md). Runtime proves loading/execution; a full compiler transcript was not supplied. Earlier [logging-only diagnostic results](docs/test-runs/2026-10-01-local-authority-dry-run.md) are retained separately.

**Start with [RESUME.md](docs/RESUME.md)** for the exact checkpoint, policies and next tests. No package, Workshop publication, license choice or multiplayer test has occurred.

## Experimental behavior

- First qualifying unconsciousness or death requests one blast per character entity.
- Recovery never resets the latch; a new character is independently eligible.
- Requires a non-proxy default damage hit zone; proxy observations cannot request a blast.
- Defers the spawn outside the current damage callback, with a second authority/existence check.
- Uses the installed resource copied from Workbench:
  `{72BEEF40AF179763}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Large.et`.
- Spawns at captured character origin + 0.3 m, independent of character/vehicle hierarchy.
- Credits the captured current instigator, falling back to the casualty if absent; ignores only the source character in its own explosion damage container.
- Lets the prefab's timer activate it. No duplicate manual trigger, custom damage RPC or particle-only replacement.
- Allows intentional nearby casualty chains.
- Cancels if the source is removed or loses authority before queued execution. Failed/cancelled requests never retry or reset eligibility.

Local timer activation passed. Cleanup, exact strength, client effects, attribution, initialization/JIP and networking still need testing. The JIP filter is provisional, not proof of replay safety. Persistence/authority migration and medical-overhaul support are not claimed.

## Development workflow

1. Open existing `ExplosiveCasualties/addon.gproj`; keep scripts/resources inside the addon root.
2. Stop Play, reload changed files from disk and compile Game scripts with Shift+F7.
3. Use Everon's `GM_Eden.ent` scenario for an isolated local blast test. Do not save edits to base-game resources.
4. Record actual effects/damage as well as `[ExplosiveCasualties][trigger]` and `[ExplosiveCasualties][blast]` logs. A spawn log alone is not a passing detonation test.
5. Test an experimental build on Jake's empty dedicated server plus his normal game client. No second Steam account or separate logging-only release is required. Restarts during iteration are acceptable.

See [SETUP.md](docs/SETUP.md), [DESIGN.md](docs/DESIGN.md), and [TESTING.md](docs/TESTING.md). Compile/runtime tests and publication are performed by Jake, not the assistant. Start chains with only a few soldiers; scaling is unverified.

## Repository layout

```text
README.md
AGENTS.md
.gitattributes
.gitignore
ExplosiveCasualties/
  addon.gproj                    Workbench-generated project descriptor
  resourceDatabase.rdb           Workbench-generated resource database
  Scripts/Game/ExplosiveCasualties/
    EC_CharacterLifeStateProbe.c  Gameplay trigger; historical filename retained
    EC_CasualtyExplosion.c        Deferred vanilla explosion dispatcher
docs/
  RESUME.md
  SETUP.md
  DESIGN.md
  TESTING.md
  source-notes/
  test-runs/
```

Repository root and addon root are deliberately separate. Preserve generated project/resource identifiers and required engine metadata. Do not invent GUIDs, copy base-game archives or publish repository-private files. The old top-level `Scripts/` copy was removed earlier.

## References

- [Bohemia: Mod Project Setup](https://community.bistudio.com/wiki/Arma_Reforger:Mod_Project_Setup)
- [Bohemia: Scripting Modding](https://community.bistudio.com/wiki/Arma_Reforger:Scripting_Modding)
- [Character damage-manager API](https://community.bistudio.com/wikidata/external-data/arma-reforger/ArmaReforgerScriptAPIPublic/interfaceSCR__CharacterDamageManagerComponent.html)
- [Timer trigger API](https://community.bistudio.com/wikidata/external-data/arma-reforger/ArmaReforgerScriptAPIPublic/interfaceTimerTriggerComponent.html)
- [Explosion configuration discussion](https://reforger.armaplatform.com/news/modding-update-sept-10-2024)
- [Mod Publishing Process](https://community.bistudio.com/wiki/Arma_Reforger:Mod_Publishing_Process)

Installed source/assets take precedence over public mirrors. Exact selected resource was supplied by Jake; no activation/replication guarantee is inferred from its name.

## License and distribution

No project license selected. Choose deliberately before Workshop publication. Reference vanilla assets via the addon dependency; do not redistribute base-game archives. Unlisted experimental distribution is the proposed route, not an action already performed.
