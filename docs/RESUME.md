# Resume here — ExplosiveCasualties

Updated by **Element0**, 2026-10-01, after Jake resolved the inherited-prefab save issue and confirmed local explosion audio.

## Current checkpoint

**The prior large-TNT/once-per-character build was published, successfully play-tested on Jake's Linux dedicated server alongside approximately 130 other mods, and committed/pushed by Jake.** Exact Workshop revision, commit hash, mod list, visibility and license terms were not supplied. See the [server report](test-runs/2026-10-01-linux-dedicated-large-tnt.md). Earlier local logs remain historical evidence.

**Current revision:** small TNT, repeat casualty transitions, approximately two-second delay. Jake reported proper triggering in Workbench and on his dedicated server, initially without sound. After saving the native SoundComponent enabled override in our inherited prefab, he confirmed **working audio in World Editor/Game Master**. Element0 verified the saved override on disk. Corrected audio on the server's client remains to be retested. See the [audio investigation and resolution](test-runs/2026-10-01-small-tnt-silent.md). Exact timing and individual repeat/JIP/cleanup cases were not separately documented.

- Project: `ExplosiveCasualties/addon.gproj`; repository root and addon root are separate. Preserve generated identifiers/metadata.
- Trigger: `ExplosiveCasualties/Scripts/Game/ExplosiveCasualties/EC_CharacterLifeStateProbe.c` (historical filename; gameplay code).
- Dispatcher: `ExplosiveCasualties/Scripts/Game/ExplosiveCasualties/EC_CasualtyExplosion.c`.
- Current selected asset supplied by Jake and verified against generated metadata:
  `{B0DC0394D3820463}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Small_Inherited.et`.
  It inherits vanilla `{2F690C7C59FB4DBF}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Small.et`.
  Saved `.et` now contains the inherited SoundComponent override with `Enabled 1`; verified by Element0. Jake confirmed local sound. Preserve the generated component/resource identifiers.
- Last observed build: 1.8.0.13; named constants ALIVE=0, INCAPACITATED=1, DEAD=10.

## Current behavior contract

- Every real transition into INCAPACITATED or DEAD creates an independent request. No lifetime latch: unconsciousness -> death queues TWO; recovery -> unconsciousness can queue another.
- Skip JIP/replay callbacks, unchanged states, non-casualty entries, missing owner/hit zone, proxies and non-game-world sources. No permanent replay-casualty exclusion: a later real transition on a replayed/recovered character is eligible.
- Keep damage authority gating at acceptance and execution. "Remove the gate" was interpreted as removing the once-per-character latch, not allowing client-generated damage.
- Call original vanilla handler; no duplicate event subscription.
- Delay each spawn by `DEFER_MS=2000`. Each pending request survives recovery/later transitions; no coalescing/removal. Actual detonation includes engine/frame scheduling and the prefab timer. Both LARGE and SMALL inspector/runtime evidence show a 0.05 s timer (SMALL observed in inspector; not a precise detonation-time measurement).
- Capture instigator at transition (current attacker, casualty fallback). Sample source position at delayed execution: current origin + 0.3 m world-up. Blast follows movement/falling/recovery during the delay, rather than using a stale event position.
- Cancel a request if source/component disappears, game world changes, authority is lost, or attribution is unavailable before execution. No retry; no blast guarantee after immediate corpse deletion.
- Spawn unparented/world-space and ignore ONLY source character, not its vehicle/root hierarchy/other occupants. Revival remains possible, not guaranteed.
- Prefab timer owns activation/networking/lifecycle; no manual trigger or second damage RPC.
- Logs include entity/request labels and delayMs=2000. Counters are diagnostic only, never eligibility gates. IDs/request numbers are local, not cross-process identifiers.

## Immediate next steps

**Next: publish the sound-enabled inherited asset with its metadata and retest audio from the normal client on Jake's Linux dedicated server.** Local audio is confirmed; no further local audio diagnosis is required unless the problem returns. No new gameplay code is needed for that test.

Audio correction uses the existing native SOUND_EXPLOSION path in the addon-owned inherited prefab, with SoundComponent `Enabled 1`. The unsuccessful runtime Activate attempt was removed; presence/IsActive diagnostics remain, with no manual sound event or sound RPC. A missing component on headless authority alone does not establish a client bug.

**Workbench persistence reminder:** Jake found that saving the disposable empty inspection world was necessary for the object's changes to persist. Save that addon-owned/test world, apply/save changes to OUR prefab, and verify its `.et` on disk. Do not save modified vanilla GM_Eden or apply changes to base-game prefabs. The earlier UI/file disagreement was an editor persistence quirk, not user error. This workflow produced the verified SoundComponent override and working local audio.

Resume the targeted behavior checks below as useful; aggregate play success is not instrumented coverage of every case.

1. After changes, stop Play, reload both `.c` files from disk, compile with Shift+F7, start a fresh session. Jake already ran the current revision in both environments; Element0 has not compiled/run it.
2. Isolated unconsciousness: one request, about two seconds then a SMALL blast. Check timerLeft/alreadyTriggered and actual effects, not just spawn logs.
3. Unconsciousness then death BEFORE the first fuse expires: same character must log requests 1 and 2; both spawn independently near their own due times.
4. Recovery before a pending blast: recovery does not cancel it. Later unconsciousness creates a new request. Move the source during the delay to verify updated placement.
5. Check direct death, source deletion during delay, and small chains. More than one blast per character is now intentional; extra unchanged/proxy/replay blasts are not.
6. Jake uploads the updated Workshop version, updates/restarts his Linux server and checks the same behavior with his normal client/full mod stack. No second Steam account, separate diagnostic release or maintenance-window prerequisite.
7. Record exact game/addon revisions and separately labelled server/client evidence where feasible. JIP/reconnect, client non-acceptance, cleanup, attribution/vehicle behavior and performance remain targeted follow-up work, not individually proven by the aggregate server report.

## Evidence

- [Linux dedicated-server report: previous large-TNT build](test-runs/2026-10-01-linux-dedicated-large-tnt.md)
- [Local large-TNT gameplay log](test-runs/2026-10-01-local-explosions.md)
- [Earlier local authority diagnostic](test-runs/2026-10-01-local-authority-dry-run.md)
- [Installed lifecycle source](source-notes/2026-10-01-character-lifecycle.md)
- [Installed explosion source / LARGE asset evidence](source-notes/2026-10-01-secondary-explosion.md)
- [Design](DESIGN.md), [current tests](TESTING.md), [setup](SETUP.md)

Do not retroactively rewrite historical once-per-character test outcomes to the new repeat-trigger contract. No project identifiers/resource databases were altered, and Element0 did not publish or commit these changes. Keep server/account secrets out of docs and published assets.
