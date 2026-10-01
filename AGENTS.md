# ExplosiveCasualties development guidance

## Goal and current contract

Arma Reforger addon: a small TNT explosion on EVERY real transition into unconsciousness or death, delayed about two seconds. Intentional casualty-driven chain reactions are allowed. See `docs/DESIGN.md` and start resumed sessions with `docs/RESUME.md`.

Jake explicitly changed the original once-per-character behavior on 2026-10-01:
- Remove the lifetime acceptance latch, not the damage-authority gate.
- Unconsciousness then death must queue two independent blasts; recovery then another unconsciousness queues another.
- Skip JIP/replay callbacks, unchanged notifications and non-casualty transitions. There is no permanent replay-casualty exclusion flag; later real transitions remain eligible.
- Keep pending requests through recovery/later transitions. Do not coalesce, replace or cancel earlier queue entries merely because another transition occurs.

## Environment and evidence

- Repository/docs root is the workspace root; addon root is `ExplosiveCasualties/`. Keep scripts under `ExplosiveCasualties/Scripts/Game/`.
- Preserve Workbench-generated `addon.gproj`, resource database, metadata and project identifiers. Do not regenerate or flatten the project layout.
- Inspect files before editing. Do not claim compile/run/package/publish/commit actions that were not performed.
- Previous large-TNT/once-per-entity revision passed local runtime tests. Jake then reported publication, successful play testing on his local Linux dedicated server alongside approximately 130 existing mods, and a commit/push. This is user-reported aggregate success, not individually instrumented proxy/JIP/cleanup/performance coverage. Exact mod list, Workshop ID, version/commit and license terms were not supplied.
- CURRENT small-TNT/repeat-transition/2000 ms revision triggered correctly in Workbench and on Jake's dedicated server, initially without audio. Jake subsequently confirmed sound working in World Editor/Game Master using our sound-enabled inherited prefab. Element0 verified the saved SoundComponent override (`Enabled 1`). Local audio is now confirmed; audio on the server's client with this corrected asset still needs retesting. See `docs/test-runs/2026-10-01-small-tnt-silent.md`. Detailed timing/edge-case coverage remains separate from aggregate success.
- Observed build: 1.8.0.13. Compiled constants ALIVE=0, INCAPACITATED=1, DEAD=10; compare named constants, not raw integers.
- Installed source confirms the damage manager already subscribes to the controller event and uses non-proxy hit zones for damage-authority work. Do not add a second subscription. See source notes.
- Jake has a Linux dedicated server and one normal client and accepts restarts during iteration. Do not require a second Steam account, a separate logging-only release or maintenance-window ceremony.

## Implementation

- Preserve vanilla life-state handler.
- Require non-proxy default hit zone at acceptance AND execution. Client control is not damage authority. No duplicate damage RPC.
- `EC_CharacterLifeStateProbe.c` is the historical filename for the gameplay trigger, not a harmless probe. Its request counter is diagnostic ONLY, not a latch.
- `EC_CasualtyExplosion.c` uses independent static deferred calls. `DEFER_MS=2000` delays spawn; actual detonation includes frame scheduling and the prefab timer. Do not claim an exact two-second fuse. LARGE runtime timer and SMALL inspector timer were 0.05 s; actual detonation timing is not precisely measured.
- Vanilla small resource supplied by Jake: `{2F690C7C59FB4DBF}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Small.et`. Current dispatcher selects Jake's generated addon-owned derivative: `{B0DC0394D3820463}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Small_Inherited.et`. Verified its `.et` and `.meta`; the saved SoundComponent override now contains `Enabled 1`, and Jake confirmed local sound. Preserve generated IDs/metadata; do not hand-invent component identifiers/schema. Do not infer effective damage/radius from filenames.
- Sample source origin + 0.3 m world-up at execution so the blast follows the delayed source. Capture instigator at transition, fallback to casualty. Ignore ONLY source character, not root hierarchy/vehicle/occupants.
- Preserve prefab timer activation; no extra OnUserTrigger/SetLive path unless testing demonstrates a need. Audio correction is the enabled native SoundComponent in OUR inherited asset, preserving SOUND_EXPLOSION and the original sound project. Previous runtime Activate diagnostic found SoundComponent missing locally and was removed. Current code has presence/IsActive diagnostics ONLY, no manual sound event/RPC. Missing audio on headless authority alone does not prove a client bug. Jake confirmed corrected local sound; corrected dedicated-server/client audio remains untested. LARGE audio was not explicitly confirmed.
- Workbench persistence quirk reported by Jake: save the disposable empty inspection world for the object's edits to persist, then apply/save the change to OUR prefab and verify its `.et` on disk. A checked box or disabled Apply button alone is insufficient verification. Do not edit/save vanilla GM_Eden or base-game prefabs. Earlier file/UI discrepancies were an editor persistence issue, not user failure; do not repeat unsuccessful Apply instructions.
- Cancel if source/component is deleted, world changes, authority is lost, or required attribution is unavailable. No retry. Recovery does not cancel. Immediate-deletion explosion guarantee is not implemented.
- Failure logs delete malformed spawned entities; successful-spawn cleanup remains unverified. A spawn log is not proof of damage/effects/cleanup.
- Entity IDs and request numbers are local diagnostics, not cross-process correlation IDs.
- Chain waves can be larger with repeat transitions. No custom load/concurrency cap exists. Do not claim medical-overhaul/persistence/migration compatibility without evidence.

## Maintenance and security

- Keep README, resume/design/setup/testing docs accurate; preserve historical test records as historical evidence.
- Use available time tool for dated records. Ask Jake to compile/test when execution tools are unavailable.
- Verify new engine APIs and resources against installed source/assets; no invented signatures or replication guarantees.
- Keep private server configuration, credentials, account identifiers and base-game archives out of Git/publication.
- Commit required descriptors/source/metadata; ignore observed disposable outputs narrowly, not all `.gproj`/`.meta` files.
- Jake published the prior build; license terms/visibility were not reported. Do not select/change a license or publish on his behalf without agreement.
