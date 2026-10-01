# Linux dedicated-server gameplay report — 2026-10-01

Recorded by **Element0** from Jake's report. This is evidence for the PREVIOUS large-TNT, once-per-character implementation, not the new small/repeat/delayed revision.

## User-reported outcome

Jake published the addon and ran it on his local Linux dedicated Arma Reforger server alongside a pre-existing stack of approximately 130 other mods. He reported: "It works exactly as it was designed." He also reported committing and pushing the changes.

- Publication: reported performed by Jake; Workshop ID, version, visibility and license not supplied.
- Linux dedicated-server play test with existing mod stack: reported successful.
- Git commit/push: reported performed by Jake; commit hash/branch not supplied or independently inspected.
- Element0 did not operate the server, publish, play-test or commit.

## Limits

No server/client logs or exact mod list were supplied for this run. Do not mark individual proxy-rejection, JIP/reconnect, player/AI, cleanup, scoreboard, performance or medical-overhaul cases as separately verified. The report is meaningful aggregate evidence that the previous implementation worked in Jake's actual server environment; it is not a universal compatibility claim.

## Next iteration requested by Jake

1. Switch to `{2F690C7C59FB4DBF}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Small.et` to reduce damage.
2. Remove the once-per-character latch: every real unconsciousness/death entry can trigger, including death after unconsciousness.
3. Add approximately two seconds before the explosion.

The new revision retains authority/JIP/unchanged-state checks, schedules independent calls with a 2000 ms spawn delay, and samples current source position at execution. It is NOT yet compiled or tested. See [resume](../RESUME.md) and [current tests](../TESTING.md).
