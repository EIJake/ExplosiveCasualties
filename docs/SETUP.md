# Workbench setup and current test workflow

For the latest checkpoint, start with [RESUME.md](RESUME.md). **Current source implements small, repeatable explosions with a 2000 ms spawn delay; it is not a harmless logging probe.** Jake ran this behavior locally and on his Linux dedicated server. Initially silent, audio now works in World Editor/Game Master after enabling the native SoundComponent in our inherited prefab; the saved override is verified on disk. Corrected server-client audio still needs retesting. Jake published, successfully ran and committed/pushed the previous large-TNT/latch build with approximately 130 other mods.

## Project and prerequisites

Windows development PC with Arma Reforger and Arma Reforger Tools; game/tools on matching branches. Last visible build: 1.8.0.13. Record actual versions when testing.

Workbench created an addon subdirectory when Jake selected the workspace as the project location:

```text
workspace/                       Repository/docs root
  ExplosiveCasualties/            Addon root
    addon.gproj
    resourceDatabase.rdb
    Scripts/Game/ExplosiveCasualties/
      EC_CharacterLifeStateProbe.c
      EC_CasualtyExplosion.c
```

Open existing `ExplosiveCasualties/addon.gproj`; do not create a replacement project or relocate generated identifiers. Keep addon source/resources inside that root. The small/repeat/delay revision has run locally and on the server; detailed timing, repeat-event and lifecycle edge cases remain separate targeted tests.

## Compile current gameplay source

1. Stop Play.
2. Reload externally changed files from disk; do not overwrite them with stale editor buffers. Both `.c` files are now maintained.
3. Let Workbench generate needed source/resource metadata; do not invent GUIDs.
4. In Script Editor, Shift+F7 compiles/reloads Game scripts. Capture exact compiler text, line numbers and versions if it fails; never guess enum/API fixes.
5. Start a fresh Play session and use fresh characters. Do not reload midway through a delayed/repeat-transition sequence.

New logs use `[ExplosiveCasualties][trigger]` and `[ExplosiveCasualties][blast]`, not the old `[dry-run]` prefix.

## Local runtime test

Open installed `ArmaReforger/Worlds/GameMaster/GM_Eden.ent` (Everon) in World Editor. Do not save changes to the base-game world. Use Play and place fresh soldiers via Game Master.

Begin with one isolated casualty, then a few soldiers for a damage/chain test. Expected: one independent delayed blast PER eligible transition, not per source. Unconsciousness then death queues two; recovery does not cancel a pending blast. Logs label each request. Real damage/effects must be observed; a spawn line is not proof of timer activation or client effects.

Selected asset:

`{B0DC0394D3820463}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Small_Inherited.et`

This addon-owned resource inherits vanilla SMALL and overrides its native SoundComponent with `Enabled 1`. The override is saved and local sound is confirmed. Include both `.et` and `.et.meta` when committing/publishing; retest sound on the server's client after updating the Workshop revision.

Current code delays spawn 2000 ms and lets the prefab timer activate it; no extra OnUserTrigger/SetLive. Actual detonation includes frame scheduling and the SMALL timer, observed as 0.050 s in the inspector but not precisely measured in play. LARGE's previous runtime timer was 0.05 s. Record timerLeft/alreadyTriggered. If already triggered at configuration, inspect attribution/ignore-list timing. Successful-spawn cleanup is not yet verified for SMALL.

Own source character is ignored by the damage container, not its root hierarchy. Position is sampled AFTER the delay, at current origin + 0.3 m. Recovery does not cancel; deletion/lost authority does. Test unconsciousness/death before the first delay expires to check independent queue entries. Vehicle/gear/impulse behavior and exact effective radius remain unverified. See [TESTING.md](TESTING.md).

## Resource Browser and prefab inspection reminders

- A folder arrow expands subfolders; click its NAME to display files.
- Script Editor searches source, not `.et` resources. Searches/navigation may open a different matching file; do not assume the active tab stays selected.
- Resource Manager's prefab preview/Details panel is not World Editor's component inspector. Base-game resource edit buttons can be disabled.
- To inspect components without editing the original prefab: open a disposable empty world in World Editor, drag the prefab into the viewport, select the placed instance in Hierarchy, and use Object Properties.
- Do not click Apply to prefab for base-game assets or save modifications to base-game files/worlds. Applying changes to an addon-owned inherited prefab is appropriate.
- **Persistence quirk found by Jake:** save the disposable empty inspection world for object edits to persist, then apply/save the override to OUR inherited prefab and verify its `.et` on disk. In this session, repeated Apply clicks and a checked Enabled box did not initially produce a persisted override; saving the test world resolved it. Do not confuse this with permission to save modified vanilla GM_Eden.
- The verified audio correction is native SoundComponent `Enabled 1` in `Explosion_Tnt_Small_Inherited.et`; no separate sound event/RPC or runtime activation is added. See the [investigation record](test-runs/2026-10-01-small-tnt-silent.md).

## Dedicated-server iteration

Jake has a Linux dedicated server and one normal client, and already published/tested the previous build with approximately 130 other mods. After local regression, upload the revised Workshop version, update/restart the server and join normally. Workshop ID/visibility/license terms were not supplied; do not invent or change them. No upload is performed by the assistant.

Test server acceptance, client proxy rejection/no extra spawns, AI and player casualties, effects/damage, small chains, respawn and JIP/reconnect together with explosions enabled. No second Steam account, separate diagnostic-only release or maintenance window is required. Correct unexpected client acceptance before claiming multiplayer support.

Keep private server configuration, credentials, account identifiers and unrelated repository files out of publication. Record addon revision, game versions, session role and exact results in `docs/test-runs/`.

## Sources and remaining inspection

- [Installed lifecycle source](source-notes/2026-10-01-character-lifecycle.md): callback subscription/handler and hit-zone authority check.
- [Installed explosion source and asset evidence](source-notes/2026-10-01-secondary-explosion.md): spawning API, timer hypothesis and character-specific adaptation.
- [Mod Project Setup](https://community.bistudio.com/wiki/Arma_Reforger:Mod_Project_Setup)
- [Scripting Modding](https://community.bistudio.com/wiki/Arma_Reforger:Scripting_Modding)
- [Mod Publishing Process](https://community.bistudio.com/wiki/Arma_Reforger:Mod_Publishing_Process)

Engine initialization/isJIP semantics, authority migration, delayed queue/reference lifetime, cleanup, score/friendly-fire and detailed server/client behavior remain targeted work. Aggregate previous server success is not a separately observed pass for each case. Do not advertise untested support.

## Git housekeeping

Review status/diffs after Workbench creates metadata. Commit required descriptors/source/metadata and narrowly ignore observed disposable output. Do not blanket-ignore `.gproj` or `.meta`. Jake reported committing/pushing the previous build; these new edits are not committed by Element0. Published license terms were not supplied.
