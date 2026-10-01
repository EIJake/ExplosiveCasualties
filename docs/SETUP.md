# Workbench setup and current test workflow

For the latest checkpoint, start with [RESUME.md](RESUME.md). **Current source implements experimental explosions; it is not a harmless logging probe.** This revision still needs compilation/runtime testing.

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

Open existing `ExplosiveCasualties/addon.gproj`; do not create a replacement project or relocate/generated identifiers. Keep addon source/resources inside that root. Workbench has already discovered and compiled earlier scripts, and local diagnostic tests passed. Those results do not validate the new dispatcher.

## Compile current gameplay source

1. Stop Play.
2. Reload externally changed files from disk; do not overwrite them with stale editor buffers. Both `.c` files are now maintained.
3. Let Workbench generate needed source/resource metadata; do not invent GUIDs.
4. In Script Editor, Shift+F7 compiles/reloads Game scripts. Capture exact compiler text, line numbers and versions if it fails; never guess enum/API fixes.
5. Start a fresh Play session and use fresh characters. Do not reload between steps of a latch sequence.

New logs use `[ExplosiveCasualties][trigger]` and `[ExplosiveCasualties][blast]`, not the old `[dry-run]` prefix.

## Local runtime test

Open installed `ArmaReforger/Worlds/GameMaster/GM_Eden.ent` (Everon) in World Editor. Do not save changes to the base-game world. Use Play and place fresh soldiers via Game Master.

Begin with one isolated casualty, then a few soldiers for a damage/chain test. Expected: one accepted trigger and one queued/spawned explosion per source. Real damage/effects must be observed; a spawn line is not proof of timer activation or client effects.

Selected asset:

`{72BEEF40AF179763}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Large.et`

It has timer, replication and damage effects. Current code lets its timer activate it; no extra OnUserTrigger/SetLive. Record timerLeft/alreadyTriggered. If silent, investigate timer start/duration; if already triggered at configuration, inspect attribution/ignore-list timing. Successful-spawn cleanup is not yet verified (Delete On Trigger appeared unchecked).

Own source character is ignored by the damage container, not its root hierarchy. Test unconscious/recovery/death without another soldier's blast interfering. Vehicle/gear/impulse behavior and exact effective radius remain unverified. See [TESTING.md](TESTING.md).

## Resource Browser and prefab inspection reminders

- A folder arrow expands subfolders; click its NAME to display files.
- Script Editor searches source, not `.et` resources. Searches/navigation may open a different matching file; do not assume the active tab stays selected.
- Resource Manager's prefab preview/Details panel is not World Editor's component inspector. Base-game resource edit buttons can be disabled.
- To inspect components without editing the original prefab: open a disposable empty world in World Editor, drag the prefab into the viewport, select the placed instance in Hierarchy, and use Object Properties.
- Do not click Apply to prefab or save changes to base-game files. Discard the temporary inspection world when done.

## Dedicated-server iteration

Jake has an empty dedicated server and one normal client. After a local compile/single-blast test, choose the license deliberately and distribute/publish an experimental build. Unlisted Workshop distribution is the proposed route; no upload is performed by the assistant.

Test server acceptance, client proxy rejection/no extra spawns, AI and player casualties, effects/damage, small chains, respawn and JIP/reconnect together with explosions enabled. No second Steam account, separate diagnostic-only release or maintenance window is required. Correct unexpected client acceptance before claiming multiplayer support.

Keep private server configuration, credentials, account identifiers and unrelated repository files out of publication. Record addon revision, game versions, session role and exact results in `docs/test-runs/`.

## Sources and remaining inspection

- [Installed lifecycle source](source-notes/2026-10-01-character-lifecycle.md): callback subscription/handler and hit-zone authority check.
- [Installed explosion source and asset evidence](source-notes/2026-10-01-secondary-explosion.md): spawning API, timer hypothesis and character-specific adaptation.
- [Mod Project Setup](https://community.bistudio.com/wiki/Arma_Reforger:Mod_Project_Setup)
- [Scripting Modding](https://community.bistudio.com/wiki/Arma_Reforger:Scripting_Modding)
- [Mod Publishing Process](https://community.bistudio.com/wiki/Arma_Reforger:Mod_Publishing_Process)

Engine initialization/isJIP semantics, authority migration, queue/reference lifetime, cleanup, score/friendly-fire and networking remain test/inspection work. Do not advertise untested support.

## Git housekeeping

Review status/diffs after Workbench creates metadata. Commit required descriptors/source/metadata and narrowly ignore observed disposable output. Do not blanket-ignore `.gproj` or `.meta`. No commit has been performed by Element0; no project license has been selected.
