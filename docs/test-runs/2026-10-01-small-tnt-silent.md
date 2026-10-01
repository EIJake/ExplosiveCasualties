# Small TNT / repeat-transition / delayed build: silent explosions

Recorded by **Element0**, 2026-10-01, from Jake's test report.

**Current outcome: local audio fixed and verified in World Editor/Game Master.** The inherited prefab's saved `SoundComponent Enabled 1` override is verified on disk. Corrected dedicated-server/client audio awaits retesting. The sections below preserve the chronological investigation; see [Resolution](#resolution-persisted-native-sound-override-local-audio-passes) for the final local result.

## User-reported results

Jake tested the revised build in Workbench and on his dedicated server. Explosions trigger properly but have no sound in either environment. The server is the previously reported local Linux dedicated server; exact published revision, commit and current mod list were not supplied.

Current resource:
`{2F690C7C59FB4DBF}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Small.et`

This updates the previous "uncompiled/untested" checkpoint: the current revision has run in both environments. No new logs or timing measurements were provided; independent repeat-transition, exact delay, cleanup, attribution and JIP/proxy edge-case coverage is not individually established by this report.

## Audio investigation

- Silence occurs locally as well as on the server. Do not start by adding a sound RPC or changing the damage-authority gate.
- Check the SMALL prefab's components, trigger projectile effects (especially any `ExplosionEffect`), and audio configuration against the previously working LARGE prefab. Missing effect/configuration is a hypothesis, not a confirmed cause.
- Earlier reports confirmed LARGE explosions but did not explicitly establish their sound. Ask whether LARGE was audible; a prefab-only A/B test with the revised delay/repeat logic can isolate the asset change if necessary.
- Do not invent sound-event names, audio-resource GUIDs or engine methods. Prefer fixing/reusing the vanilla asset audio path once identified, rather than creating a second independently broadcast sound.
- Request SMALL prefab/parent text if readily available, or a screenshot of its TimerTriggerComponent Projectile Effects and entity component list. No base-game asset edits.

No gameplay change or audio fix has been made as part of this investigation record.

## Inspector evidence and pending local diagnostic

Subsequent SMALL inspector screenshots showed TimerTriggerComponent timer 0.050 s and `Sound Event = SOUND_EXPLOSION` under SpawnParticleEffect. SoundComponent was present with Enabled unchecked and one Filenames entry. The exact audio resource was not supplied. Disabled sound is a hypothesis, not a demonstrated cause. Jake could not start Play in the temporary inspection world.

The dispatcher was then patched to log native SoundComponent.IsActive before/after and call Activate(explosion) when inactive. This is an uncompiled/untested LOCAL diagnostic, not a confirmed fix or replicated configuration change. No manual event is played. Compile and test a casualty in the known-working GM_Eden scenario; inspect `[ExplosiveCasualties][audio]` output and listen. Server-side activation alone is not assumed to activate a client's component. A successful local test may lead to an addon-owned inherited small-explosion prefab with audio enabled, preserving one native audio path on all peers.

API source consulted: [GenericComponent](https://arexplorer.zeroy.com/_generic_component_8c_source.html), [SoundComponent](https://arexplorer.zeroy.com/_sound_component_8c_source.html), [SimpleSoundComponent](https://arexplorer.zeroy.com/_simple_sound_component_8c_source.html), [SndComponent](https://arexplorer.zeroy.com/_snd_component_8c_source.html). Mirror version is 1.7.0.54, older than Jake's observed installed build; compilation is the next verification.

## Runtime diagnostic result (2026-10-01)

Jake supplied the following diagnostic output after the activation patch:

```text
[ExplosiveCasualties][audio] entity=0x40000000000003A4 {}; request=2; SoundComponent missing; blast retained
```

The patched audio branch executed, so compilation/execution is no longer pending for that branch. `SoundComponent.Cast(explosion.FindComponent(SoundComponent))` returned null; the activation call did not run for this request. This does NOT prove that disabled components are omitted from runtime entities: the source of this log (Workbench versus Linux dedicated server) was not identified. A headless server may omit audio components. First establish which environment produced this line; use local GM_Eden evidence to diagnose client audio. The preceding trigger excerpt names a different source entity (`…02DF`, request 1), so do not treat those excerpts as a single request.

No sound fix demonstrated and no gameplay code changed in response to this result. If local runtime also lacks the component, investigate an addon-owned inherited SMALL prefab with SoundComponent enabled at creation, preserving the native SOUND_EXPLOSION path and existing damage/timer. Workbench should generate the prefab and its metadata; no guessed prefab serialization or GUIDs.

## Log-origin clarification

Jake confirmed the `SoundComponent missing` diagnostic above came from Workbench. He also confirmed explosions were silent while playing as a character on the dedicated server. Headless-only component omission does not explain the local warning. The diagnostic lookup returned null; this still does not establish why, or prove the unchecked Enabled property is the cause.

Next controlled test: create an addon-owned inherited SMALL prefab via Workbench, enable its native SoundComponent at creation, obtain its real generated reference, and select it in the dispatcher. Preserve the native timer/event and current gameplay contract. Do not override the base-game asset globally, invent metadata/GUIDs, or declare the sound fixed before listening locally and on the server. `Inherit in` is documented in Bohemia's [prefab configuration guide](https://community.bistudio.com/wiki/Arma_Reforger:Weapon_Creation/Prefab_Configuration).

## Inherited-prefab reference supplied; saved override still pending

Jake supplied `{B0DC0394D3820463}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Small_Inherited.et`. Element0 verified the workspace `.et` and `.meta`. The metadata matches this reference, but the saved entity template consists only of vanilla inheritance and entity ID `9D9BE68EA2672F76`: no SoundComponent override is saved yet.

The dispatcher now selects this derivative. The unsuccessful runtime Activate attempt was removed; presence/IsActive diagnostics remain without manual activation, event playback, or audio RPC. Gameplay delay, repeat-trigger policy and damage authority were not changed. This new revision is not compiled/run by Element0, and no fix is claimed. Jake still needs to enable SoundComponent on OUR inherited prefab through Workbench and apply/save that override, then test in GM_Eden and on the server. Preserve both generated `.et` and `.et.meta` in source control.

## Resolution: persisted native sound override; local audio passes

Recorded by **Element0**, 2026-10-01, after Jake's confirmation.

Jake identified the World Editor persistence issue: the disposable empty inspection world needed to be saved for the object's changes to persist. Earlier repeated Apply-to-prefab clicks did not yield the expected override on disk; this was an editor-workflow issue, not evidence of user error or a different workspace.

Element0 re-read the addon-owned prefab and verified the saved component override:

```text
components {
 SoundComponent "{D092A75ECEFAE456}" {
  Enabled 1
 }
}
```

Resource remains `{B0DC0394D3820463}Prefabs/Weapons/Warheads/Explosions/Explosion_Tnt_Small_Inherited.et`, inheriting vanilla SMALL. Generated resource/component identifiers were not changed or invented. The dispatcher already selects this inherited asset.

**Jake confirmed the explosion sound is working properly in Game Master mode in World Editor.** This establishes a successful local audio correction using the native SoundComponent configuration. No additional sound event, sound RPC, manual explosion trigger, or further gameplay-script change was required for this documentation update. It does not establish exactly why the disabled vanilla component was missing from the earlier runtime lookup.

The earlier sections are the chronological investigation record, not the current unresolved status. Current outcome:
- Small/repeat/delayed triggering: previously reported working in Workbench and on the Linux server.
- Saved sound-enabled prefab override: verified on disk by Element0.
- Corrected local audio: confirmed by Jake.
- Corrected audio from the dedicated server's client: retest pending after publishing/updating the inherited asset.
- Detailed delay/repeat-event, cleanup, JIP/proxy, attribution and performance cases: remain separate targeted checks.

Workbench reminder: save the disposable addon-owned inspection/test world, apply/save changes to OUR inherited prefab, and verify the actual `.et`. Do not apply modifications to vanilla prefabs or save modified vanilla GM_Eden. Include the inherited `.et` and generated `.et.meta` in the next commit/Workshop update. No publication or commit was performed by Element0.
