#pragma once

namespace LEX
{
	struct ITypeInfo;

	struct GenericBase;
	struct ITemplateBody;

	class GenericArray;

	struct ITemplatePart
	{
		virtual size_t GetSize() const = 0;

		virtual ITypeInfo* GetPartArgument(size_t i) const = 0;

		//TODO: Instead of all this, why not make a hash of each entry?

		//Always try to promote before using. 
		virtual ITemplateBody* TryResolve() { return nullptr; }

		//This should be pure
		virtual GenericBase* GetClient() const { return nullptr; }

		bool IsSpecialized() { return TryResolve(); }




		static GenericArray SpecializeTemplate(ITemplatePart* args);
	};

}