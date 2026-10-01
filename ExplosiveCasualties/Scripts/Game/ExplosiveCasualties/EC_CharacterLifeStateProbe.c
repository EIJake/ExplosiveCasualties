// ExplosiveCasualties: experimental authority-gated casualty explosions.
// Previous logging-only gate/latch passed local NON_PROXY paths in 1.8.0.13.
// This gameplay revision has NOT been compiled or runtime-tested yet.
// Vanilla handler is preserved; activation uses a verified resource reference.
// Networking/JIP, timer activation, queue lifetime, and cleanup remain unverified.
// Entity IDs are session-local diagnostics, not cross-client network IDs.

modded class SCR_CharacterDamageManagerComponent
{
	// One request per character's damage-manager instance; never reset on revival
	// or a failed/cancelled spawn. Not replicated or persisted.
	protected bool m_bEC_BlastRequested;

	// Conservative exclusion for a character observed as a replayed casualty.
	// This flag is separate from acceptance and never queues a blast.
	protected bool m_bEC_ReplayCasualty;
	protected static bool s_bEC_EnumLogged;

	protected string EC_StateName(ECharacterLifeState state)
	{
		switch (state)
		{
			case ECharacterLifeState.ALIVE:
				return "ALIVE";
			case ECharacterLifeState.INCAPACITATED:
				return "INCAPACITATED";
			case ECharacterLifeState.DEAD:
				return "DEAD";
		}

		return "UNKNOWN";
	}

	protected bool EC_IsCasualty(ECharacterLifeState state)
	{
		return state == ECharacterLifeState.INCAPACITATED || state == ECharacterLifeState.DEAD;
	}

	override void OnLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState, bool isJIP)
	{
		IEntity owner = GetOwner();
		string characterLabel = "entity=<no owner>";
		if (owner)
			characterLabel = string.Format("entity=%1", owner.GetID());

		HitZone defaultHitZone;
		string damageRole = "NO_DEFAULT_HITZONE";
		bool damageProxy;
		if (owner)
		{
			defaultHitZone = GetDefaultHitZone();
			if (defaultHitZone)
			{
				damageProxy = defaultHitZone.IsProxy();
				if (damageProxy)
					damageRole = "PROXY";
				else
					damageRole = "NON_PROXY";
			}
		}

		bool queueBlast;
		vector capturedPosition;
		Instigator capturedInstigator;
		string decision;
		if (isJIP)
		{
			if (EC_IsCasualty(newLifeState))
				m_bEC_ReplayCasualty = true;

			decision = "skip: JIP/replay notification";
		}
		else if (previousLifeState == newLifeState)
			decision = "skip: unchanged state";
		else if (!EC_IsCasualty(newLifeState))
			decision = "skip: not a casualty entry (latch preserved)";
		else if (!owner)
			decision = "skip: missing owner";
		else if (!defaultHitZone)
			decision = "skip: missing default hit zone (authority unknown)";
		else if (damageProxy)
			decision = "skip: damage proxy (trigger latch untouched)";
		else if (m_bEC_ReplayCasualty)
			decision = "skip: entity previously observed as a replayed casualty";
		else if (m_bEC_BlastRequested)
			decision = "skip: already latched";
		else if (!GetGame() || owner.GetWorld() != GetGame().GetWorld())
			decision = "skip: source outside current game world";
		else
		{
			// Capture before vanilla/listener work can alter attribution or pose.
			// Initial placement: entity origin + 0.3 m world-up; tune via testing.
			capturedPosition = owner.GetOrigin() + Vector(0, 0.3, 0);
			capturedInstigator = GetInstigator();
			// Initial credit policy: existing attacker; if absent, casualty itself.
			if (!capturedInstigator)
				capturedInstigator = Instigator.CreateInstigator(owner);

			if (!capturedInstigator)
				decision = "skip: cannot capture/create instigator";
			else
			{
				// Reserve BEFORE super and queueing; protects against reentrancy.
				m_bEC_BlastRequested = true;
				queueBlast = true;
				decision = "accepted: deferred TNT blast requested";
			}
		}

		// Always preserve vanilla casualty audio/particle behavior.
		super.OnLifeStateChanged(previousLifeState, newLifeState, isJIP);

		if (!s_bEC_EnumLogged)
		{
			s_bEC_EnumLogged = true;
			Print(string.Format("[ExplosiveCasualties][trigger] compiled enum: ALIVE=%1; INCAPACITATED=%2; DEAD=%3", ECharacterLifeState.ALIVE, ECharacterLifeState.INCAPACITATED, ECharacterLifeState.DEAD));
		}

		Print(string.Format("[ExplosiveCasualties][trigger] %1; life-state %2(%3) -> %4(%5); isJIP=%6; latched=%7; replayCasualty=%8", characterLabel, EC_StateName(previousLifeState), previousLifeState, EC_StateName(newLifeState), newLifeState, isJIP, m_bEC_BlastRequested, m_bEC_ReplayCasualty));
		Print(string.Format("[ExplosiveCasualties][trigger] %1; damageRole=%2", characterLabel, damageRole));
		Print(string.Format("[ExplosiveCasualties][trigger] %1; %2", characterLabel, decision));

		if (queueBlast)
		{
			// One-shot deferred call, never spawn inside this damage callback.
			// Dispatcher rechecks existence/world/damage authority. No RPC here.
			GetGame().GetCallqueue().CallLater(EC_CasualtyExplosion.Spawn, EC_CasualtyExplosion.DEFER_MS, false, owner, capturedPosition, capturedInstigator, characterLabel);
		}
	}
}
