#pragma once

struct FInnerResult
{
protected:
	bool bIsCancelled = false;
	uint8 CurrentPriority = 0;
	
public:
	bool ApplyModifier(const bool Cancelled, const uint8 Priority)
	{
		if (Priority > CurrentPriority)
		{
			bIsCancelled = Cancelled;
			CurrentPriority = Priority;
			return true;
		}
		return false;
	}
	
	bool IsCancelled() const { return bIsCancelled; }

	FInnerResult();

	FInnerResult(const bool InIsCancelled, const uint8 InPriority)
		: bIsCancelled(InIsCancelled)
		, CurrentPriority(InPriority)
	{
	}
};
