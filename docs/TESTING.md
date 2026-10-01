# Test checklist

**No tests have been run yet.** A written checklist is not evidence of a passing build.

Use a small controlled scenario without other gameplay mods first. Record game/tools versions, addon revision, explosion resource, session type, and exact results. Start with widely separated soldiers and test chains only after the single-casualty behavior is reliable.

## Phase A: logging-only probe

- [ ] Addon project loads and the probe compiles without errors.
- [ ] Normal character behavior remains intact.
- [ ] Unconsciousness generates an identifiable life-state transition.
- [ ] Direct death generates an identifiable life-state transition.
- [ ] Unconsciousness followed by death shows the expected separate transitions.
- [ ] The enum values in logs are mapped to the installed enum definition.
- [ ] Server/client callback differences are recorded, including JIP behavior.

The probe does not detonate anything. Its logs alone cannot prove authority, replication, or explosion correctness.

## Phase B: local detonation acceptance

| Test | Required result | Status |
| --- | --- | --- |
| Healthy soldier becomes unconscious | One damaging explosion with expected effects | Not run |
| Healthy soldier dies outright | One explosion without requiring unconsciousness first | Not run |
| Unconscious soldier subsequently dies | No second explosion from the same entity | Not run |
| Same entity revived then knocked out again | No new explosion under the once-per-entity rule | Not run |
| Duplicate/unchanged state notification | No additional blast | Not run |
| New player character after respawn | New entity can detonate once | Not run |
| Casualty already present at scenario initialization | No spontaneous blast from initialization | Not run |
| Spawned pre-unconscious/dead entity | No initialization-triggered blast | Not run |
| Nearby soldiers injured by first explosion | Each newly unconscious/dead entity explodes at most once | Not run |
| Spaced-out control group | No blast without a triggering casualty | Not run |
| Soldier inside a vehicle | Location, occupant/vehicle damage, and effects documented | Not run |
| Soldier on a slope/in a building | Blast placement and occlusion behavior inspected | Not run |
| Character removed immediately after transition | No invalid-object access; deletion policy honored | Not run |
| Missing/invalid explosion resource | Clear failure diagnostic, no repeated retry storm | Not run |
| Small then larger casualty chain | No runaway recursion/duplicates; load and timing recorded | Not run |

Test casualty entry from gunfire, blast damage, falls, and bleeding where feasible. These may reach different parts of damage handling.

## Phase C: networking

Run a listen server with a second client, then a dedicated server before claiming dedicated-server support.

- [ ] Authority creates one damage-producing blast per accepted casualty.
- [ ] Both clients see/hear appropriate effects without duplicated damage.
- [ ] AI, host player, and remote player casualties all behave correctly.
- [ ] Joining with existing dead/unconscious soldiers causes no new blasts.
- [ ] Joining during a chain reaction does not cause replayed detonations.
- [ ] Disconnect/reconnect does not re-detonate existing casualties.
- [ ] Respawn produces a fresh entity and preserves the once-per-entity rule.
- [ ] Server/client logs show no missing resources, script exceptions, or RPC errors.
- [ ] Attribution, friendly-fire, and teamkill behavior match the chosen policy.

## Phase D: optional claims

Only claim these after testing:

- [ ] Save/load does not re-detonate previously processed casualties.
- [ ] Authority migration preserves the rule, if this workflow is relevant/supported.
- [ ] Medical/damage overhaul compatibility, with addon names and versions recorded.
- [ ] Large-battle performance, with population, blast count, timing, and server symptoms recorded.

## Test-run record

Copy this template for each meaningful run. Keep sensitive server details out of committed records.

```text
Run label:
Game version/branch:
Tools version/branch:
Addon revision (commit or "uncommitted"):
Explosion resource/configuration:
Session (standalone/listen/dedicated), client count:
Other addons and versions:
Scenario/terrain and soldier setup:
Test case and exact reproduction steps:
Expected result:
Observed result:
Server/client log excerpts:
Pass / fail / inconclusive:
Follow-up:
```

Commit small, useful records under `docs/test-runs/` if desired. Raw logs can stay in the ignored `/logs/` directory; extract relevant lines into the record.
