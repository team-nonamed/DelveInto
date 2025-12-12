#include "HealthInstigation.h"

FHealthInstigation::FHealthInstigation(TWeakObjectPtr<AActor> InInstigator,
                                       TWeakObjectPtr<AActor> InReceiver,
                                       EHealthInstigationType InInstigationType,
                                       float InBaseRate,
                                       float InBaseKnockback)
	: Instigator(InInstigator)
	, Receiver(InReceiver)
	, InstigationType(InInstigationType)
	, BaseRate(InBaseRate)
	, BaseKnockback(InBaseKnockback)
{
}