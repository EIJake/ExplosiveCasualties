# Test checklist — small TNT / repeat transitions / two-second delay

**Current checkpoint:** Jake ran the small/repeat/delayed revision in Workbench and on his Linux dedicated server; explosions triggered correctly but were initially silent. After persisting the native SoundComponent enabled override in our inherited prefab, Jake confirmed sound working in World Editor/Game Master. Element0 verified `Enabled 1` on disk. Corrected server-client audio remains to be retested. See the [audio investigation and resolution](test-runs/2026-10-01-small-tnt-silent.md). Detailed timing, repeat-event, JIP/proxy and cleanup cases are not individually established by these aggregate reports.

Previous large-TNT/once-per-character code passed local gameplay tests and user-reported Linux dedicated-server play with approximately 130 other mods, followed by publication and commit/push. See the [server record](test-runs/2026-10-01-linux-dedicated-large-tnt.md).

Start with [RESUME.md](RESUME.md). Current resource:

`{B0DC0394D3820463}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Small_Inherited.et`

This addon-owned prefab inherits vanilla SMALL and enables its native SoundComponent. Local audio passes; update the Workshop version with the saved prefab and metadata, then retest from the server's normal client.

## Local regression procedure

1. Stop Play; reload both `.c` files from disk and compile with Shift+F7. Record actual compiler errors/versions.
2. Start fresh Play in GM_Eden with widely spaced fresh soldiers. Do not hot-reload midway through a sequence.
3. Inspect `[ExplosiveCasualties][trigger]` and `[ExplosiveCasualties][blast]`. Request labels distinguish multiple pending blasts on the same local entity.
4. Confirm actual explosion/effects/damage, not only spawn logs. Record timerLeft/alreadyTriggered. SMALL's timer was observed as 0.050 s in the inspector; code delays SPAWN by 2000 ms, with detonation afterward according to prefab timer/frame scheduling. Precise actual timing remains unmeasured.
5. Check single casualties before a small chain. Own source character is ignored in its explosion damage container; other sources can still injure/kill it.

## Targeted acceptance matrix

Aggregate triggering is confirmed locally/on the server and corrected local audio passes. The detailed cases below remain follow-up checks unless individually recorded; do not label them all failed or all passed based solely on aggregate play success.

| Test | Expected result |
| --- | --- |
| Healthy soldier becomes unconscious | One request, delayMs=2000, small blast about two seconds later |
| Healthy soldier dies outright | One request and delayed small blast |
| Unconscious -> dead before first fuse expires | Same entity requests 1 and 2; two independent delayed spawns, neither replaced/cancelled |
| Unconscious -> dead after first blast | Another request/blast at death; no already-latched skip |
| Recover before a queued blast | Recovery requests nothing; pending blast STILL occurs |
| Recover and become unconscious again | Another independent request/blast |
| Move/transport the source during delay | Blast at CURRENT source origin + 0.3 m, not old event position |
| Multiple pending blasts on same moving source | Each samples source position when its own delay expires |
| Unchanged-state callback, if observed | No new request |
| Proxy callback, if observed | No acceptance or spawn on proxy |
| JIP casualty replay then later real transition | Replay skips; later genuine casualty entry remains eligible, no permanent blacklist |
| Delete source during the two seconds | Cancel safely with diagnostic; no retry/invalid access |
| Lose source authority or game world changes | Pending request cancels; no unauthorized damage |
| Missing resource/timer/damage container | Clear failure diagnostic; malformed spawned entity deleted |
| Small blast damage/effects/sound | Triggering confirmed locally/on server; corrected sound confirmed locally, server-client retest pending; exact damage/radius not measured |
| Explosion cleanup | Check entity lifetime after effects; no cleanup claim without evidence |
| Nearby small group/chain | New casualty transitions each schedule; repeat death after unconsciousness may add blasts intentionally |
| Vehicle occupants / building / slope | Position, occlusion, gear/impulse and passenger/vehicle damage observed |

If SMALL spawns without exploding, inspect its timer/activation instead of blindly adding OnUserTrigger or SetLive. If alreadyTriggered=1 during configuration, investigate attribution/ignore-list timing.

Unconsciousness and death can arrive close together and yield closely spaced blasts after the delay: this is EXPECTED under the revised contract. Recovery does not defuse a queued request. No custom duplicate-event ID or global load limiter exists.

## Linux dedicated-server iteration

Jake already published the previous build. After the local check, upload/update the revised Workshop version, update/restart the Linux dedicated server and join with the normal client. Existing mod stack testing is useful; a clean/minimal run is a troubleshooting option, not a prerequisite. No second Steam account or separate diagnostic-only release required.

Record server/client game versions, exact addon revision and full/mod-minimal context. Observe both AI and player casualties, including rapid unconsciousness/death and respawn.

- [ ] Authority alone schedules/spawns each accepted transition; no client-generated duplicate blasts.
- [ ] Client actually receives visuals/sound and damage is not duplicated.
- [ ] Two eligible transitions on one character produce TWO blasts with correct delay.
- [ ] Joining/reconnecting with existing casualties causes no replay blasts.
- [ ] Joining during the two-second pending interval does not create extra requests.
- [ ] Respawn behaves correctly for the new character and old pending-source deletion policy.
- [ ] Request counters and local IDs interpreted per process, not used as cross-process correlation IDs.
- [ ] Cleanup, error logs and server load inspected during small and larger chains.
- [ ] Attribution/score/friendly-fire and vehicle/occupant cases inspected.

Absence of a client callback is not an observed proxy-rejection pass. Compare one explicitly labelled casualty at a time if no stable replication ID is logged. Do not claim blanket compatibility, save/load or authority-migration support from a successful play session.

## Historical evidence — previous contracts

Do not rewrite earlier latch-test outcomes to the new repeat-trigger behavior:

- [Initial probe](test-runs/2026-09-30-local-probe.md)
- [Initial once-per-entity dry run](test-runs/2026-09-30-local-dry-run.md)
- [Non-proxy local authority/latch dry run](test-runs/2026-10-01-local-authority-dry-run.md)
- [Local large-TNT gameplay](test-runs/2026-10-01-local-explosions.md)
- [Aggregate Linux dedicated-server success](test-runs/2026-10-01-linux-dedicated-large-tnt.md)

Current-contract evidence:
- [Small TNT audio investigation and local resolution](test-runs/2026-10-01-small-tnt-silent.md)

## Test-run record

```text
Run label/date:
Game/tools versions and branches:
Addon revision (hash/version or uncommitted):
Explosion resource and configured delay:
Session (standalone/listen/dedicated), client count:
Other addons/version list or known approximate context:
Scenario/setup and exact reproduction:
Expected result:
Observed timing, effects, damage and request count:
Separately labelled server/client excerpts:
Pass / fail / inconclusive:
Follow-up:
```

Keep account identifiers, credentials and private configuration out of committed records. Raw logs can stay in ignored `/logs/`; retain useful redacted evidence under `docs/test-runs/`.
