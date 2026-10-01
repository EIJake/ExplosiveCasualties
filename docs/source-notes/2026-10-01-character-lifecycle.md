# Installed character lifecycle and damage-authority source inspection

Recorded by **Element0**, 2026-10-01. Evidence: Jake pasted the installed `SCR_CharacterDamageManagerComponent.c` source into the conversation, plus the controller's life-state method earlier. Previous runtime evidence identifies the build as 1.8.0.13; no new version output was supplied with the source. This is source inspection, not a runtime test.

## Confirmed in the supplied source

- `SCR_CharacterDamageManagerComponent` extends `SCR_ExtendedDamageManagerComponent`.
- Its `OnPostInit(IEntity owner)` calls `super.OnPostInit(owner)` and, for a character with the SCR controller, registers `OnLifeStateChanged` with `controller.m_OnLifeStateChanged.Insert(...)`.
- That subscription has no explicit authority or JIP filter. It is not necessary to add a second subscription in our modded class; doing so could duplicate callbacks.
- The damage manager's own `OnLifeStateChanged(previousLifeState, newLifeState, isJIP)` exists in this file. It returns on `isJIP`; otherwise incapacitation triggers knockout audio, and death triggers death audio and deferred bleeding-particle cleanup. Our override must always preserve it with `super`.
- The separately supplied controller method invokes `m_OnLifeStateChanged` and updates vehicle occupant counts where applicable. It has no explicit authority check in that method. This does not establish which machines the engine calls it on or at what initialization stage.
- In `OnPostInit`, vanilla registers `OnWaterEnter` only when an `RplComponent` exists and the default hit zone exists and is not a proxy: `hz && !hz.IsProxy()`.
- `AddBleedingEffectOnHitZone` explicitly says the code is handled on authority only and returns when `hitZone.IsProxy()` is true. Other damage-effect methods also reject proxy hit zones.
- `UpdateConsciousness()` asks the controller to set unconsciousness, with health/settings branches that can instead kill the character. Complete engine dispatch/timing and proxy behavior have not been traced.

## Authority-gated diagnostic revision

The current modded handler uses `GetDefaultHitZone().IsProxy()` as its **damage-authority gate**, following the installed hit-zone damage pattern. It does not use local player possession or assume the callback itself is authoritative.

- No owner or no default hit zone: reject without setting the trigger latch.
- Default hit zone is a proxy: reject without setting the trigger latch.
- Default hit zone is non-proxy: existing JIP, transition, replay-exclusion, and once-per-character filters still apply before accepting.
- Log `damageRole=NON_PROXY`, `PROXY`, or `NO_DEFAULT_HITZONE` for every observed callback.
- Reserve the acceptance latch only after the gate passes and before calling `super`. Always call `super`, including rejected callbacks.
- Keep the existing provisional replay-casualty exclusion flag separate from acceptance. A JIP casualty can still set this exclusion on any observing instance; it never counts as a detonation. Authority migration remains unsupported/unverified.
- No explosion, damage, queued task, or RPC is added.

At the time of source inspection this revision had not been compiled/tested. **Subsequent update:** Jake's runtime output confirms the gated revision loaded/executed and passed local non-proxy direct-death and incapacitation/recovery/re-incapacitation/death tests; see the [test record](../test-runs/2026-10-01-local-authority-dry-run.md). A full new compiler transcript was not supplied. Proxy/network paths remain unverified. Source evidence supports the selected gate; it does not prove correct process roles, callback distribution, JIP behavior, or exactly-once networking in actual standalone/listen/dedicated sessions. `NON_PROXY` is a hit-zone role, not an independently verified server-mode assertion. If a network client unexpectedly accepts a trigger, stop before adding damage and inspect the replication/authority model further.

## Remaining investigation

1. Compile and repeat fresh-character local tests; record roles and decisions.
2. Run two networked instances and record authoritative acceptance versus proxy rejection for AI and player casualties. Local entity IDs are not cross-machine identifiers; correlate by a controlled single-character test and supplied server/client labels.
3. Trace initialization and `isJIP` dispatch, including existing/spawned casualties. Audio's JIP suppression is useful precedent, not proof that every initialization path is tagged.
4. Inspect the inherited explosion implementation and actual vanilla call sites/resources before selecting a deferred damaging operation.
5. Decide attribution, deletion/lifetime policy, and validated support scope before gameplay detonation.

Do not copy base-game archives or the entire vanilla source into the addon. Inspect originals in Workbench without editing them.
