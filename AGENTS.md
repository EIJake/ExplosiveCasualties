# ExplosiveCasualties development guidance

## Goal

Arma Reforger addon: one large explosion per character entity on unconsciousness or direct death, with intentional casualty-driven chain reactions. See `docs/DESIGN.md` for the contract.

## Environment and honesty

- The workspace root is the Git repository root.
- Inspect existing files before changing them. Preserve user-generated Workbench project files and resource identifiers.
- Do not claim to compile, run, package, publish, or commit unless the action actually occurred.
- The initial `.c` file is a logging-only, uncompiled probe. No explosion implementation is present.
- Verify engine APIs and vanilla resource paths against the installed game source/assets. Never invent a resource GUID, project descriptor, enum member, or replication guarantee.
- If execution tools are unavailable, ask the user to compile/test in Workbench and provide exact logs.

## Implementation constraints

- Preserve vanilla behavior by calling the original life-state handler.
- Handle unconsciousness and instant death, but latch once per entity before queuing any blast.
- Ignore JIP/initialization replay. Verify callback semantics; do not assume a single flag covers every initialization path.
- Gameplay detonation must be authoritative. Client observations are useful diagnostics but are not permission to deal damage.
- Defer explosion execution outside the current damage callback. Capture necessary data and define behavior if the character is deleted.
- Reuse a verified vanilla explosion implementation and its networking behavior; do not substitute particles for damage or fire duplicate explosions via RPC.
- Intentional chain reactions are allowed. Duplicate callbacks and recursive damage-handler execution are not.
- Do not claim compatibility with medical overhauls or dedicated servers until tested.

## Maintenance

- Keep README status accurate and record outstanding decisions in `docs/DESIGN.md`.
- Use `docs/TESTING.md` for reproducible tests and actual outcomes.
- Do not put base-game archives, private configuration, or tokens in Git.
- Keep required project/resource metadata in Git; refine ignore patterns from observed generated output.
- No license has been selected. Do not add a license or publish on the user's behalf without agreement.
