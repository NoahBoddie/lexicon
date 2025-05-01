#pragma once

#include "Lexicon/Engine/ISpecializable.h"

#include "Lexicon/ITemplateBodyPart.h"

namespace LEX
{

	struct ITypeInfo;
	struct SpecialBase;
	struct GenericBase;
	struct ITemplatePart;



	struct SpecialBase : public ISpecializable, public ITemplateBodyPart
	{
		//this bares much of the same code as a generic array, use parts of that as the base here.
		
		//I need to use bodies for this.
		SpecialBase(GenericBase* base, ITemplatePart* args) :
			_base{ base }, 
			ITemplateBodyPart{ args->TryPromoteTemplate() ? State::kBody : State::kPart}
		{
			LoadTypes(args);
		}

		SpecialBase(GenericBase* base, ITemplateBody* args) :
			_base{ base },
			ITemplateBodyPart{ State::kBody }
		{
			LoadTypes(args);
		}
	
	private:
		void LoadTypes(ITemplatePart* args)
		{
			auto size = args->GetSize();

			_types.resize(size);

			for (auto i = 0; i < size; i++)
			{
				_types[i] = args->GetPartArgument(i);
				
			}
		}
	public:
		//I don't wanna do this but there's no dynamic cast without polymorphism so +8


		GenericBase* GetGeneric() const override
		{
			return _base;
		}

		//This doesn't actually really have a client does it?
		

		template<typename T>
		T* ToResolved()//Use this over dynamic cast for resolved objects.
		{
			if (!this || IsResolved() == false) {
				return nullptr;
			}

			return dynamic_cast<T*>(this);
		}


		//Dead
		//virtual SpecialBase* AsPart() { return nullptr; }
		
		//Dead
		//virtual SpecialBase* AsBody() { return nullptr; }
		
		//Dead
		//virtual ITemplatePart* AsTemplate() = 0;


		//TODO: TemplateMatches needs to be const
		bool TemplateMatches(ITemplatePart* args) override
		{
			//Since this is both part and body now, there's no early exist.
			//if (args->TryPromoteTemplate() != nullptr)
			//	return false;

			auto size = _types.size();



			if (args->GetSize() != size)
				return false;

			for (int i = 0; i < size; i++)
			{
				auto& lhs = _types[i];

				ITypeInfo* rhs = args->GetPartArgument(i);

				if (lhs != rhs) {
					return false;
				}

			}

			return true;
		}

		//Dead
		//virtual size_t GetSize() const = 0;


		//Dead
		//Get rid of this, AsTemplate handles this.
		//virtual ITypeInfo* GetArgument(size_t i) const = 0;

		SpecialBase* ObtainSpecial(ITemplatePart* args) override;

		//virtual SpecialBase* ObtainBody(ITemplateBody* args) override
		//{
		//	return nullptr;
		//}


		virtual size_t GetSize() const { return _types.size(); }


		ITypeInfo* GetPartArgument(size_t i) const override
		{
			return _types[i];
		}

		TypeInfo* GetBodyArgument(size_t i) const override
		{
			if (GetState())
			{
				auto& type = _types[i];

				return type->FetchTypePolicy(nullptr);;
			}

			return nullptr;
		}

		virtual GenericBase* GetClient() const override { return _base; }



		static bool AllResolved(const std::vector<ITypeInfo*>& types)
		{
			for (auto type : types)
			{
				if (type->IsResolved() == false) {
					return false;
				}
			}
			return true;
		}

		bool IsResolved() const override
		{
			//This is rare to ever actually be used, it merely exists on the one situation where it is.
			if (_state == kUnknown)
			{
				return AllResolved(_types);
			}
			
			return _state == kBody;
		}


		std::span<ITypeInfo* const> types() const
		{
			return _types;
		}





	private:
		mutable GenericBase* _base = nullptr;
		std::vector<ITypeInfo*> _types;
	};
}