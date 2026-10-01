# ExplosiveCasualties

A very silly mod for **Arma Reforger**: soldiers explode when they become unconscious or die outright. Nearby casualties can trigger a chain reaction. Medics may wish to stand back.

## Status

**Repository scaffold and diagnostic script only. No explosions are implemented yet.**

The repository contains documentation and an initial Enforce Script life-state probe. The probe has **not been compiled or tested in Workbench**. There is no Workbench-generated `.gproj`, selected explosion resource, packaged addon, or Workshop release yet.

## Intended first release

- Trigger on a real transition into **unconsciousness or death**, including instant death.
- Detonate **once per character entity**, not once per state change. An unconscious soldier's later death must not trigger another blast.
- Reuse a vanilla explosion's damage, sound, visuals, and physical impulse. An anti-tank mine is the initial candidate; a mortar or another suitably absurd explosion is also acceptable.
- Allow casualty-driven chain reactions.
- Apply gameplay effects on the authority only; show effects correctly to connected clients.
- Do not detonate existing casualties because a player joins or the world initializes.
- Initially target ordinary vanilla soldiers, both AI and player characters. Validate this scope in testing rather than assuming every character uses the same component.

The explosion resource and exact invocation are deliberately undecided until we inspect the installed game assets and scripts. We are not assuming which explosive is currently the most powerful.

## Start here

1. Install Arma Reforger and **Arma Reforger Tools** on a Windows development PC.
2. Follow [Workbench setup](docs/SETUP.md) to create the actual addon project.
3. Compile the included diagnostic probe and confirm life-state callbacks in a small local test.
4. Inspect the vanilla explosion implementation, then implement authority-only, deferred detonation.
5. Run the [test checklist](docs/TESTING.md), including a second client, before publishing.

The included probe only logs state changes. **It does not spawn explosives or change damage.** Do not load it on a production server; it has not been validated and may produce substantial logging in large battles.

## Repository layout

```text
README.md
AGENTS.md                         Development guardrails for future assistant sessions
.gitattributes                    Text-file normalization
.gitignore                        Local clutter exclusions
Scripts/Game/ExplosiveCasualties/
  EC_CharacterLifeStateProbe.c     Uncompiled, logging-only prototype
docs/
  SETUP.md                        First Workbench session and source inspection
  DESIGN.md                       Behavior contract, architecture, open questions
  TESTING.md                      Acceptance tests and test-run template
```

Workbench will generate the project descriptor and any engine-managed files. Keep project/source metadata needed by the addon in Git; do not manufacture resource GUIDs or blanket-ignore `.meta` files.

## Development workflow

Use [DESIGN.md](docs/DESIGN.md) as the behavior contract and [TESTING.md](docs/TESTING.md) to record results. The first milestone is a **compiled probe with observed unconsciousness and direct-death transitions**, not a spectacular explosion.

Suggested commits after reviewing the changes:

```sh
git status
git add README.md AGENTS.md .gitignore .gitattributes docs Scripts
git commit -m "Scaffold ExplosiveCasualties and add life-state probe"
```

These are suggested commands; they have not been executed by the assistant.

## References

- [Bohemia: Mod Project Setup](https://community.bistudio.com/wiki/Arma_Reforger:Mod_Project_Setup)
- [Bohemia: Scripting Modding](https://community.bistudio.com/wiki/Arma_Reforger:Scripting_Modding)
- [Bohemia: Character damage-manager API](https://community.bistudio.com/wikidata/external-data/arma-reforger/ArmaReforgerScriptAPIPublic/interfaceSCR__CharacterDamageManagerComponent.html)
- [Bohemia: September 2024 modding update — explosion configuration](https://reforger.armaplatform.com/news/modding-update-sept-10-2024)
- [Bohemia: Mod Publishing Process](https://community.bistudio.com/wiki/Arma_Reforger:Mod_Publishing_Process)

Documentation is a starting point. The installed game version and its source/assets are the final check for this project's implementation.

## License and distribution

No project license has been chosen yet. Choose one before a public release. Reference vanilla assets through the addon dependency; do not copy base-game asset archives into this repository.
