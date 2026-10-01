# First Workbench session

## Prerequisites

- Windows development PC with Arma Reforger installed.
- Arma Reforger Tools installed through Steam's Tools library.
- Tools and game on matching release branches. Record the actual versions used.
- This Git repository available locally to Workbench.

No external C++ compiler, custom art package, or engine source license is needed for the planned script addon.

## 1. Create the addon project

Follow Bohemia's [Mod Project Setup](https://community.bistudio.com/wiki/Arma_Reforger:Mod_Project_Setup) guide.

Use `ExplosiveCasualties` as the project name and add the base Arma Reforger project as a dependency. Prefer having the addon root coincide with this repository root so `Scripts/Game/ExplosiveCasualties/` is inside the addon.

**Do not overwrite this repository to satisfy a new-project dialog.** If Workbench requires an empty destination, create the project in a temporary directory outside the repository. Then bring its generated project files/resources into this root without replacing the existing README or Scripts directory. Reopen the project from its final location and verify paths/dependencies. If the dialog or generated layout differs from the guide, stop and share what it shows before guessing.

Let Workbench generate the `.gproj` and resource metadata. We have deliberately not supplied a guessed descriptor, GUID, or dependency identifier.

Confirm that this addon's `Scripts/Game` directory is included in the **Game** script module. See [Scripting Modding](https://community.bistudio.com/wiki/Arma_Reforger:Scripting_Modding). Do not alter the base game's source files to load the addon.

## 2. Compile the logging-only probe

The file is:

```text
Scripts/Game/ExplosiveCasualties/EC_CharacterLifeStateProbe.c
```

Its only change is a `modded class` override of `OnLifeStateChanged`, calling the original implementation and then logging the transition.

Compile/reload scripts using Workbench's script tooling. Menu names may vary with the installed version. This repository's probe has not been compiled yet, so compilation is a real checkpoint, not an expected formality.

If it fails, capture the complete error text, filename, line number, and game/tools versions. Do not "fix" enum values or engine signatures by guessing.

## 3. Make a small test world/scenario

Use a minimal editable test scenario with ordinary vanilla soldiers and a way to injure them. No other gameplay mods should be loaded for the first test. Keep the actual test world/scenario resources in the addon if we create them; none are supplied yet.

Test separately:

1. A soldier who becomes unconscious rather than immediately dying.
2. A healthy soldier who dies outright.
3. An unconscious soldier who subsequently dies.

Look for lines beginning:

```text
[ExplosiveCasualties][probe]
```

The probe logs all life-state callbacks, not just casualties. Map its printed state values to the enum in the installed source. Confirm that the intended events actually reach this damage manager.

**No explosions are expected at this stage.**

## 4. Inspect the source before implementing detonation

Use Workbench's source browser/search. We need:

- `SCR_CharacterDamageManagerComponent.OnLifeStateChanged`: implementation, caller/registration, callback timing, and meaning of `isJIP`.
- `ECharacterLifeState`: actual unconsciousness and death constants in this build.
- A vanilla authority check used in comparable damage/explosion code, including standalone and listen-server behavior.
- `SCR_DamageManagerComponent.SecondaryExplosion`: implementation and at least one vanilla call site.
- The selected mine/shell prefab and the actual explosion resource/configuration it uses.
- Explosion networking: how damage is applied and how clients receive visual/audio effects.
- The deferred-call mechanism and lifetime behavior if the character/component disappears.
- The intended instigator/kill-credit policy and the API needed to preserve it.

The online API documents a callback and a secondary-explosion method, but does not establish that every explosion resource can be passed to that method unchanged.

Record findings in the verification table in [DESIGN.md](DESIGN.md). The installed game is the authority for exact resource identifiers. Third-party source mirrors can be useful clues but may describe older versions.

## 5. Share enough information for the next iteration

Provide:

- Game and Tools version/branch.
- Generated project descriptor filename and whether the probe compiled.
- Probe output for unconsciousness and direct death.
- Relevant source excerpts from the callback and explosion call site.
- Selected explosion resource path and identifier, copied from Workbench rather than typed from memory.

Do not share unrelated private server configuration or credentials.

## Git housekeeping

Review `git status` after generating/opening the project. Commit necessary project descriptors, source, resources, and engine metadata. Add narrow ignore rules only for observed disposable caches/build output. The current `.gitignore` intentionally does not exclude `.gproj` or `.meta`.

Before publication, choose a project license and follow [Mod Publishing Process](https://community.bistudio.com/wiki/Arma_Reforger:Mod_Publishing_Process). Local development does not require publishing first.
