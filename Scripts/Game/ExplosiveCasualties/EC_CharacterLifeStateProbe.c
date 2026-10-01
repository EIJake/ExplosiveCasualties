// ExplosiveCasualties: diagnostic-only first milestone.
// NOT compiled or tested in Workbench yet. No detonation is implemented.
// This intentionally logs callbacks on every instance, including clients and
// JIP notifications, so we can observe callback behavior before filtering it.
// A client log is NOT a request to apply gameplay damage.

modded class SCR_CharacterDamageManagerComponent
{
	override void OnLifeStateChanged(ECharacterLifeState previousLifeState, ECharacterLifeState newLifeState, bool isJIP)
	{
		super.OnLifeStateChanged(previousLifeState, newLifeState, isJIP);

		Print(string.Format("[ExplosiveCasualties][probe] life-state %1 -> %2; isJIP=%3", previousLifeState, newLifeState, isJIP));
	}
}
