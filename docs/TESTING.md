# Test checklist

**CURRENT experimental deferred TNT source passed its first local gameplay test.** Jake observed actual explosions; the log confirms direct-death/incapacitation acceptance, one spawn per displayed character, later-death latch rejection, and recovery latch retention. Multiple squad casualties produced individual blasts consistent with intentional chains. See the [local explosion record](test-runs/2026-10-01-local-explosions.md). Next test networking with explosions enabled on Jake's empty dedicated server + his normal client. No second Steam account or diagnostic-only publication required. Historical diagnostic phases below concern earlier source, not network proof.

Jake exercised the previous authority-gated logging-only revision locally: direct death and incapacitation/recovery/re-incapacitation/death passed, all NON_PROXY. Runtime output confirmed loading/execution; a full new compiler transcript was not supplied. See the [current local authority-gated record](test-runs/2026-10-01-local-authority-dry-run.md). Network/proxy/JIP behavior remains unverified. See the [2026-09-30 dry-run record](test-runs/2026-09-30-local-dry-run.md) and [earlier probe record](test-runs/2026-09-30-local-probe.md). Version displayed: 1.8.0.13. Named enum values, direct-death acceptance, and the unconsciousness/recovery/death latch sequence are verified locally. The current gameplay revision has now been tested locally; no multiplayer test has been reported. A written checklist is not evidence of a passing build. Resume from [RESUME.md](RESUME.md).

Use a small controlled scenario without other gameplay mods first. Record game/tools versions, addon revision, explosion resource, session type, and exact results. Start with widely separated soldiers and test chains only after the single-casualty behavior is reliable.

## Phase A: logging-only probe

- [x] Addon project loads and the original probe compiles without errors (base-game deprecation warnings were present).
- [ ] Normal character behavior remains intact (unconsciousness/recovery/death observed; full regression checks not run).
- [x] Unconsciousness generates an identifiable transition: `0 -> 1`, confirmed by Jake's observation.
- [x] Recovery generates an identifiable transition: `1 -> 0`, confirmed by Jake's observation.
- [x] Death after recovery generates a callback, now named `ALIVE(0) -> DEAD(10)` in the dry run.
- [x] Separate character's direct death generates `ALIVE(0) -> DEAD(10)` and one acceptance in the displayed dry-run sequence.
- [ ] Unconsciousness followed by death without recovery shows the expected separate transitions.
- [x] Compiled enum constants are logged: `ALIVE=0`, `INCAPACITATED=1`, `DEAD=10`. Installed source/caller inspection remains necessary for lifecycle semantics.
- [ ] Server/client callback differences are recorded, including JIP behavior.

The probe does not detonate anything. Its logs alone cannot prove authority, replication, or explosion correctness.

## Phase A2: logging-only trigger dry run (core local cases verified)

Compile/reload the updated diagnostic. Start a new play session and use **freshly spawned characters** so old damage history or hot-reloaded component state cannot confound the latch test. Do not reload scripts between steps of a single sequence. Record the first `[ExplosiveCasualties][dry-run] compiled enum:` line along with the callback/decision lines. Inspect any `UNKNOWN` state against the installed source rather than guessing a number.

The previous diagnostic printed state names, raw values, `entity=...`, `isJIP`, `latched`, `replayCasualty`, and a separate decision. Entity IDs identify characters within this local test, not across machines. The Phase A2 results below concern **the previous revision without an authority gate**, not proof of a server-approved trigger. The current revision adds hit-zone gating and `damageRole`; it requires the separate Phase A3 tests below.

| Test | Required dry-run result | Status |
| --- | --- | --- |
| Fresh soldier: unconscious, recovers, then killed | Same entity ID; one `would detonate` at incapacitation; recovery skips with latch retained; death says `skip: already latched` | Passed locally, 2026-09-30 screenshot; entity ending `023A` |
| Different fresh soldier: killed without incapacitation first | New entity ID; one `would detonate` on named `DEAD` entry | Passed for displayed direct-death sequence, 2026-09-30 screenshot; entity ending `01EB` |
| Fresh soldier: unconscious, then killed without recovery | One `would detonate`; later death says `skip: already latched` | Not run |
| Same soldier recovers and becomes unconscious again | No new acceptance; `skip: already latched` | Not run |
| Duplicate/unchanged notification, if observed or reproducibly generated | `skip: unchanged state`; no new acceptance | Not run |
| JIP/replay notification, if observed in later multiplayer testing | `skip: JIP/replay notification`; no acceptance | Not run |
| Casualty replay then later transition on same entity | `replayCasualty=1`; subsequent casualty entry excluded, even after recovery, under provisional conservative policy | Not run |
| Healthy character replay then real casualty | Replay skips; later real casualty can accept once | Not run |

Initialization with `isJIP=false` is not covered by these provisional filters. Spawned/pre-existing casualties and the installed callback lifecycle remain separate verification work. Hot reload, persistence, component replacement, authority migration, and cross-client once-only semantics are not validated by this local latch.

## Phase A3: historical damage-authority-gated logging-only dry run (local paths passed)

This is the PREVIOUS revision. Current source produces gameplay damage; do not describe it as diagnostic-only. Network checks below will be exercised with the explosion build.

Source rationale: [installed character source notes](source-notes/2026-10-01-character-lifecycle.md). Stop Play, reload externally changed source from disk, compile with Shift+F7, and start a fresh session with fresh characters. Do not reload midway through a latch sequence.

The new diagnostic logs `damageRole=NON_PROXY`, `PROXY`, or `NO_DEFAULT_HITZONE`. A non-proxy default hit zone is required for acceptance. Missing/proxy hit zones must not set the trigger latch. `NON_PROXY` is a hit-zone role, not a separately established server-mode assertion. The provisional JIP replay-exclusion flag remains separate from acceptance. **No damage, explosion, queued task, or RPC is present.**

| Test | Required result | Status |
| --- | --- | --- |
| Current revision compiles/reloads | No addon compiler errors; record any warnings and installed versions | Current runtime output proves loading/execution; full new compiler transcript not supplied |
| Fresh local soldier: direct death | `NON_PROXY`; one `would detonate`; `latched=1` | Passed locally 2026-10-01, entity ending `026E` |
| Fresh local soldier: unconscious, recovered, then killed | `NON_PROXY`; one acceptance; recovery preserves latch; later death skips | Passed locally 2026-10-01, entity ending `0366`; repeat incapacitation occurred before death |
| Fresh local soldier: unconscious then killed without recovery | One acceptance; later death skips | Final incapacitated -> dead leg passed on `0366`; fresh uninterrupted sequence not separately recorded |
| Recovered soldier becomes unconscious again | No second acceptance | Passed locally 2026-10-01, entity ending `0366` |
| Networked AI casualty, one controlled character at a time | Appropriate authority accepts once; observed proxy callback skips with acceptance latch untouched | Not run |
| Host and remote player casualties | Damage-authority instance accepts once regardless of local possession; proxy observations do not accept | Not run |
| Missing default hit zone, if reproducible | Fail closed; no acceptance or invalid-object access | Not run |
| JIP with existing casualties and healthy characters | Replay never accepts; record roles and exclusion flags; later healthy-character real casualty eligible only on authority | Not run |

Label evidence by process/session role (authority/client, standalone/listen/dedicated) and record transitions, damage roles, and decisions. Local entity IDs cannot correlate characters across processes: use a single controlled casualty and explicit test labels instead. Absence of a client callback is not evidence of an observed proxy rejection; record it as absence rather than manufacture a pass. If a network client unexpectedly accepts or spawns a blast, or authority lacks a usable default hit zone, stop that experimental run and correct the authority model. The gameplay revision is intentionally implemented before this network verification, as approved by Jake.

Passing a local run does not validate networking, dedicated-server/JIP, persistence, or authority migration. Save a new test record rather than retroactively treating Phase A2 evidence as a pass for this revision.

## Phase B: local detonation acceptance (initial gameplay cases passed)

Latest evidence: [2026-10-01 local explosions](test-runs/2026-10-01-local-explosions.md). Jake's report confirms actual explosions, and spawn diagnostics show one damage container and a 0.05 s untriggered timer at configuration. Do not add a manual trigger: automatic activation works in this tested build. Exact damage causality, sound, cleanup and performance are not separately established. The steps below remain reproduction/regression instructions, not an assertion that every case passed.

### First isolated blast

1. Stop Play; reload both source files from disk. Compile with Shift+F7. Record full compiler errors involving our files.
2. Start fresh Play in GM_Eden; use fresh soldiers, far from other soldiers/vehicles. Stay clear of the casualty. Do not hot-reload midway through a sequence.
3. Cause a direct death. Expect one `[trigger] accepted: deferred TNT blast requested`, followed by one `[blast] spawned explosion` line and an actual explosion. Capture timerLeft/alreadyTriggered as well as effects, sound and damage observations.
4. Resource: `{72BEEF40AF179763}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Large.et`. Timer owns activation; no manual OnUserTrigger or SetLive. If a spawn produces no blast, inspect activation rather than blindly add a second trigger path. alreadyTriggered=1 at configuration means attribution/ignore-list timing needs investigation.
5. Test nearby damage with a small group and explicit spacing. Ignore list contains only the source character; other casualties can intentionally chain. The screenshot's single damage-distance field is not a complete safe-distance specification.
6. Check explosion entity lifetime: Delete On Trigger appeared unchecked. Do not claim cleanup until observed. Add verified cleanup if entities persist after effects; do not prematurely delete a replicated effect.
7. Regression: make a fresh isolated source unconscious, heal/recover it, then kill it. Its own damage-container ignore list should allow recovery; impulse/gear/other effects are unverified. Expect no second request/blast for that character.
8. Deletion test: delete source immediately after acceptance. Initial policy is cancellation if source is gone before execution; no retries or invalid access. Queue/reference lifetime needs testing.

| Test | Required result | Status |
| --- | --- | --- |
| Healthy soldier becomes unconscious | One damaging explosion with expected effects | Local blast reported; one damage-container spawn per accepted source shown; exact health delta not quantified |
| Healthy soldier dies outright | One explosion without requiring unconsciousness first | Passed locally; direct DEAD entry and one spawn shown, including entity ending 0227 |
| Unconscious soldier subsequently dies | No second explosion from the same entity | Passed locally; multiple already-latched skips, no second source spawn shown |
| Same entity revived then knocked out again | No new explosion under the once-per-entity rule | Not run |
| Duplicate/unchanged state notification | No additional blast | Not run |
| New player character after respawn | New entity can detonate once | Not run |
| Casualty already present at scenario initialization | No spontaneous blast from initialization | Not run |
| Spawned pre-unconscious/dead entity | No initialization-triggered blast | Not run |
| Nearby soldiers injured by first explosion | Each newly unconscious/dead entity explodes at most once | Multiple squad casualties accept/spawn once, consistent with chains; per-casualty damage attribution not captured |
| Spaced-out control group | No blast without a triggering casualty | Not run |
| Soldier inside a vehicle | Location, occupant/vehicle damage, and effects documented | Not run |
| Soldier on a slope/in a building | Blast placement and occlusion behavior inspected | Not run |
| Character removed immediately after transition | No invalid-object access; deletion policy honored | Not run |
| Missing/invalid explosion resource | Clear failure diagnostic, no repeated retry storm | Not run |
| Small then larger casualty chain | No runaway recursion/duplicates; load and timing recorded | Not run |

Test casualty entry from gunfire, blast damage, falls, and bleeding where feasible. These may reach different parts of damage handling.

## Phase C: networking

First use Jake's empty dedicated server and his normal game client. Choose a license and distribute an experimental build, then add that exact build to the test server. Record server/client versions and logs. No second Steam account needed. Listen-server/two-client coverage can be added later but is not established by this run.

- [ ] Dedicated authority creates one damage-producing blast per accepted casualty.
- [ ] Client receives appropriate effects/sound; actual damage is not duplicated.
- [ ] AI and connected player's casualties behave correctly, regardless of player possession.
- [ ] Observed client proxy callbacks reject acceptance; no client `[blast] spawned` from our dispatcher. If callbacks are absent, record absence, not an observed proxy-rejection pass.
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
