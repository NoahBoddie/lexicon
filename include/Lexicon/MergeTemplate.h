#pragma once

#include "Lexicon/ITemplateBodyPart.h"

namespace LEX
{


	struct MergeTemplate : public ITemplateBodyPart
	{//While this is an ITemplateBody, it should always be passed around as a part.

		MergeTemplate(ITemplatePart* lhs, ITemplatePart* rhs) :
			left{ lhs },
			right{ rhs },
			leftEnd{ lhs ? lhs->GetSize() : 0 }
		{
			assert(right);
		}


		MergeTemplate(ITemplateBody* lhs, ITemplateBody* rhs) :
			left{ lhs },
			right{ rhs },
			leftEnd{ lhs ? lhs->GetSize() : 0 }
		{
			assert(right);
		}

		//The client is whoever we'd be accessing from. So this is a little less cut and dry, and more prevailent to the circumstances.
		GenericBase* GetClient() const override { return right->GetClient(); }


		size_t GetSize() const override { return leftEnd + right->GetSize(); }


		ITypeInfo* GetPartArgument(size_t i) const override
		{
			return i < leftEnd ? left->GetPartArgument(i) : right->GetPartArgument(i - leftEnd);
		}

		TypeInfo* GetBodyArgument(size_t i) const override
		{
			if (GetState()) {
				auto lhs = left ? left->TryResolve() : nullptr;
				auto rhs = right->TryResolve();
				return i < leftEnd ? lhs->GetBodyArgument(i) : rhs->GetBodyArgument(i - leftEnd);
			}

			return nullptr;
		}



		ITemplatePart* const left = nullptr;
		ITemplatePart* const right = nullptr;
		size_t leftEnd = -1;

		bool IsResolved() const
		{
			return (!left || left->TryResolve()) && right->TryResolve();
		}


	};


}