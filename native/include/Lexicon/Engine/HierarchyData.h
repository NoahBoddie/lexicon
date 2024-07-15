#pragma once

#include "InheritData.h"


namespace LEX
{

	struct ITypePolicy;
	struct QualifiedType;
	struct OverloadCode;


	struct HierarchyData
	{
		//FakeType will become a part of something called Hierarchy data. TypeBases have this, but so do Specializations of generic types.

		//Id like it if in a test of ambiguity this always wins, but that might not be possible in this set up.
		ITypePolicy* _extends;//Deprecated* extends = nullptr;

		std::vector<InheritData> inheritance;

		//This should actually be a kind of field
		std::vector<ITypePolicy*> members;


		uint32_t hashRange = 0;//Range is equal to zero to this number.

		uint32_t memberCount = 0;//Should bind classes increase this value any? Nah, probably handle in post.
		

		bool hasInternal = false;



		virtual ITypePolicy* GetHierarchyType() = 0;
		//TODO: This should come back at a later point I think.
		virtual void HandleInheritance() {}

		
		InheritData* _InheritData(InheritData& data);

		const InheritData* GetInheritData(const ITypePolicy* type) const;

		InheritData* GetInheritData(ITypePolicy* type);

		void FinalizeAndSort();

		std::vector<InheritData> GetInheritFrom(uint32_t hashMin, uint32_t idxInc);

		//One of these is for inheriting proper, the other is for manual setting. I would like to order these proper.
		void SetInheritFrom(ITypePolicy* other, Access a_access = Access::Public);
		
		void SetDerivesTo(ITypePolicy* other, Access a_access = Access::Public);
		
		


		OverloadCode CreateCode(ITypePolicy* target);

		void PrintInheritance();

		//This should be moved to qualified type, and the main thing of desire here should be the overload code.
		int CompareType(OverloadCode& left, OverloadCode& right, QualifiedType&& left_type, QualifiedType&& right_type);

		int CompareType(ITypePolicy* a_lhs, ITypePolicy* a_rhs);


		bool _IsInternalParent(const InheritData* intern) const;

	};


}