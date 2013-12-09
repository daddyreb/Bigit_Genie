#ifndef ISCHEDULINGRULE_H
#define ISCHEDULINGRULE_H
class ISchedulingRule
{
public:
	/**
	* Returns whether this scheduling rule completely contains another scheduling
	* rule.  Rules can only be nested within a thread if the inner rule is completely 
	* contained within the outer rule.
	* <p>
	* Implementations of this method must obey the rules of a partial order relation
	* on the set of all scheduling rules.  In particular, implementations must be reflexive
	* (a.contains(a) is always true), antisymmetric (a.contains(b) and b.contains(a) iff a.equals(b), 
	* and transitive (if a.contains(b) and b.contains(c), then a.contains(c)).  Implementations
	* of this method must return <code>false</code> when compared to a rule they
	* know nothing about.
	* 
	* @param rule the rule to check for containment
	* @return <code>true</code> if this rule contains the given rule, and 
	* <code>false</code> otherwise.
	*/
	virtual bool contains(ISchedulingRule* rule)=0;

	/**
	* Returns whether this scheduling rule is compatible with another scheduling rule.
	* If <code>true</code> is returned, then no job with this rule will be run at the 
	* same time as a job with the conflicting rule.  If <code>false</code> is returned, 
	* then the job manager is free to run jobs with these rules at the same time.
	* <p>
	* Implementations of this method must be reflexive, symmetric, and consistent,
	* and must return <code>false</code> when compared  to a rule they know 
	* nothing about.
	* <p>
	* This method must return true if calling {@link #contains(ISchedulingRule)} on
	* the same rule also returns true. This is required because it would otherwise
	* allow two threads to be running concurrently with the same rule.
	*
	* @param rule the rule to check for conflicts
	* @return <code>true</code> if the rule is conflicting, and <code>false</code>
	* 	otherwise.
	*/
	virtual bool isConflicting(ISchedulingRule* rule)=0;
};
#endif