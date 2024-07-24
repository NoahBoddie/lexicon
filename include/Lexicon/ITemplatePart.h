#pragma once

namespace LEX
{
	struct GenericBase;
	struct ITypePolicy;
	struct ITemplateBody;


	struct ITemplatePart
	{
		virtual size_t GetSize() const = 0;

		virtual ITypePolicy* GetPartArgument(size_t i) const = 0;

		//TODO: Instead of all this, why not make a hash of each entry?

		//Always try to promote before using. 
		virtual ITemplateBody* TryPromoteTemplate() { return nullptr; }

		bool IsSpecialized() { return TryPromoteTemplate(); }





		virtual std::unique_ptr<ITemplatePart> MakeGenericArray(GenericBase* ask, ITemplatePart* args);

		virtual std::unique_ptr<ITemplateBody> MakeGenericArray(ITemplateBody* args);



	};

}