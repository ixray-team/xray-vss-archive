////////////////////////////////////////////////////////////////////////////
//	Module 		: seniority_hierarchy_space.h
//	Created 	: 12.11.2001
//  Modified 	: 03.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Seniority hierarchy space
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace SeniorityHierarchy {
	IC	ref_str to_string		(u32 number)
	{
		string16	S;
		sprintf		(S,"%d",number);
		return		(ref_str(S));
	}

	template <typename T1>
	IC	void	assign_svector	(T1 &container, u32 count, const typename T1::value_type &value)
	{
		container.resize		(count);
		typename T1::iterator	I = container.begin();
		typename T1::iterator	E = container.end();
		for ( ; I != E; ++I)
			*I					= value;
	}
}