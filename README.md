# ExplosiveCasualties

A very silly **Arma Reforger** addon: soldiers explode when they become unconscious or die. Nearby casualties can trigger chain reactions. Medics may wish to stand back.

## Status

**The previous large-TNT/once-per-character build worked locally and on Jake's Linux dedicated server alongside approximately 130 existing mods.** Jake published it and committed/pushed the changes. See the [server report](docs/test-runs/2026-10-01-linux-dedicated-large-tnt.md) and [local explosion record](docs/test-runs/2026-10-01-local-explosions.md). This is user-reported server success, not a guarantee for every mod or individually verified JIP/cleanup case.

**Current revision:** smaller TNT, repeat casualty triggers, and delayed blasts ran in Workbench and on Jake's dedicated server. Initially silent, the explosions now have **working sound in World Editor/Game Master**, confirmed by Jake after saving the native SoundComponent enabled override in our inherited prefab. Corrected audio on the server's client still needs retesting. See the [audio investigation and resolution](docs/test-runs/2026-10-01-small-tnt-silent.md). Detailed timing and individual edge-case checks remain pending.

**Start with [RESUME.md](docs/RESUME.md)** for the exact checkpoint, policies and next tests.

## Experimental behavior

- Every real transition into unconsciousness or death requests a blast; no lifetime latch.
- Unconsciousness followed by death queues two independent blasts. Recovery/repeated unconsciousness remains eligible; recovery does not cancel pending blasts.
- Requires a non-proxy default damage hit zone; proxy observations cannot request a blast.
- Delays each spawn by 2000 ms, outside the current damage callback, with a second authority/existence check. Actual detonation also includes scheduling and the prefab's own timer.
- Skips JIP/replay and unchanged-state notifications, but does not permanently exclude replayed characters from future real transitions.
- Currently selects our Workbench-generated inherited small-TNT resource:
  `{B0DC0394D3820463}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Small_Inherited.et`.
  It inherits vanilla `Explosion_Tnt_Small.et` and enables its native SoundComponent. The override is verified on disk; local audio is confirmed without a separate sound broadcast.
- Spawns at the character's CURRENT origin + 0.3 m after the delay, independent of character/vehicle hierarchy.
- Credits the captured current instigator, falling back to the casualty if absent; ignores only the source character in its own explosion damage container.
- Lets the prefab's timer activate it. No duplicate manual trigger, custom damage RPC or particle-only replacement.
- Allows intentional nearby casualty chains.
- Cancels a pending request if the source is removed or loses authority before execution; no retry. Later real transitions remain independently eligible.

SMALL explosions trigger locally and on the server; corrected audio is confirmed locally and awaits a server-client retest. Exact timing, cleanup, strength, attribution, initialization/JIP and detailed server/client behavior still need targeted tests. Persistence/authority migration and blanket medical-overhaul support are not claimed.

## Development workflow

1. Open existing `ExplosiveCasualties/addon.gproj`; keep scripts/resources inside the addon root.
2. Stop Play, reload changed files from disk and compile Game scripts with Shift+F7.
3. Use Everon's `GM_Eden.ent` scenario for an isolated local blast test. Do not save edits to base-game resources.
4. Record actual effects/damage as well as `[ExplosiveCasualties][trigger]` and `[ExplosiveCasualties][blast]` logs. A spawn log alone is not a passing detonation test.
5. Upload the revised Workshop version and test on Jake's Linux dedicated server plus his normal game client. No second Steam account or separate logging-only release is required. Restarts during iteration are acceptable.

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
    EC_CasualtyExplosion.c        Deferred explosion dispatcher
  Prefabs/Weapons/Warheads/Explosions/
    Explosion_Tnt_Small_Inherited.et
    Explosion_Tnt_Small_Inherited.et.meta
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

Jake published the previous build. Workshop ID/version, visibility and license terms were not supplied here; do not infer or change them. Reference vanilla assets via the addon dependency; do not redistribute base-game archives. Publication and Git commit/push were performed by Jake, not Element0.
