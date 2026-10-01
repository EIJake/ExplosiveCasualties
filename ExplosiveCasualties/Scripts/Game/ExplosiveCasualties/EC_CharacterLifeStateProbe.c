// ExplosiveCasualties: authority-gated, repeatable casualty explosions.
// Every real transition into INCAPACITATED or DEAD can queue its own blast.
// No lifetime latch; authority, JIP and unchanged-state checks are retained.
// Previous large-TNT/latch build passed local tests and Jake's Linux server run.
// Small TNT / repeat transitions / two-second delay revision needs testing.
// Entity IDs are session-local diagnostics, not cross-client network IDs.

modded class SCR_CharacterDamageManagerComponent
{
	// Diagnostic counter only: never used to suppress an eligible transition.
	protected int m_iEC_BlastRequestCount;
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
		Instigator capturedInstigator;
		string requestLabel;
		string decision;
		if (isJIP)
			decision = "skip: JIP/replay notification";
		else if (previousLifeState == newLifeState)
			decision = "skip: unchanged state";
		else if (!EC_IsCasualty(newLifeState))
			decision = "skip: not a casualty entry (pending requests unchanged)";
		else if (!owner)
			decision = "skip: missing owner";
		else if (!defaultHitZone)
			decision = "skip: missing default hit zone (authority unknown)";
		else if (damageProxy)
			decision = "skip: damage proxy";
		else if (!GetGame() || owner.GetWorld() != GetGame().GetWorld())
			decision = "skip: source outside current game world";
		else
		{
			// Capture attribution before vanilla/listener work can alter it.
			// Position is sampled by the dispatcher after the two-second delay.
			capturedInstigator = GetInstigator();
			if (!capturedInstigator)
				capturedInstigator = Instigator.CreateInstigator(owner);

			if (!capturedInstigator)
				decision = "skip: cannot capture/create instigator";
			else
			{
				// Each eligible transition creates an independent queue entry.
				// Never reset/coalesce earlier requests on recovery or later death.
				m_iEC_BlastRequestCount++;
				requestLabel = string.Format("%1; request=%2", characterLabel, m_iEC_BlastRequestCount);
				queueBlast = true;
				decision = string.Format("accepted: small TNT blast requested; request=%1; delayMs=%2", m_iEC_BlastRequestCount, EC_CasualtyExplosion.DEFER_MS);
			}
		}

		// Always preserve vanilla casualty audio/particle behavior.
		super.OnLifeStateChanged(previousLifeState, newLifeState, isJIP);

		if (!s_bEC_EnumLogged)
		{
			s_bEC_EnumLogged = true;
			Print(string.Format("[ExplosiveCasualties][trigger] compiled enum: ALIVE=%1; INCAPACITATED=%2; DEAD=%3", ECharacterLifeState.ALIVE, ECharacterLifeState.INCAPACITATED, ECharacterLifeState.DEAD));
		}

		Print(string.Format("[ExplosiveCasualties][trigger] %1; life-state %2(%3) -> %4(%5); isJIP=%6; requests=%7", characterLabel, EC_StateName(previousLifeState), previousLifeState, EC_StateName(newLifeState), newLifeState, isJIP, m_iEC_BlastRequestCount));
		Print(string.Format("[ExplosiveCasualties][trigger] %1; damageRole=%2", characterLabel, damageRole));
		Print(string.Format("[ExplosiveCasualties][trigger] %1; %2", characterLabel, decision));

		if (queueBlast)
		{
			// Do not Remove() earlier calls: unconsciousness then death queues TWO.
			// Dispatcher rechecks existence/world/damage authority. No RPC here.
			GetGame().GetCallqueue().CallLater(EC_CasualtyExplosion.Spawn, EC_CasualtyExplosion.DEFER_MS, false, owner, capturedInstigator, requestLabel);
		}
	}
}
