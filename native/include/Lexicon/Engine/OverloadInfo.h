#pragma once

#include "Lexicon/Interfaces/FunctionInfo.h"
#include "Lexicon/Engine/OverloadParameter.h"

#include "Lexicon/Engine/MemberInfo.h"
namespace LEX
{

	struct FunctionNode;
	struct ITemplatePart;
	struct ISpecializable;

    struct OverloadInfo : public FunctionInfo, public OverloadParameter
    {
        virtual void Destroy() {}


		virtual FunctionNode CreateNode(ITemplatePart* part) = 0;
		virtual bool IsOverloadUsuable() const { return true; }
		//virtual BasicCallSignature* GetCallSignature() = 0;
		//virtual TemplateContainer* GetGenericSignature() = 0;
		
    };
}