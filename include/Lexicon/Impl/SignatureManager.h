#pragma once

#include "GenericTypePolicy.h"
#include "GenericFunction.h"

namespace LEX
{
	//Puttig under wraps until it's time for execution.

	/*

	class Function;
	struct ITypePolicy;
	struct AbstractTypePolicy;
	struct IGenericParameter;

	//The generic type signature might not be needed, function only has it because it's not a type, and doesn't have
	// a type in of itself. Variables won't have this problem though.

	struct FuncSignatureHandler
	{
		//Do I want this to return an abstractor ITypePolicy? Because I may need to make special clients right?
		// If I do, I need the actual target.
		//<!> This has exposed a major difference in how this gets used. There's one where it's like partial
		// where it only needs the idea of the type, and then there's the situation where an unspecified type
		// is giving unspecialized parameter expecting it to create a generic function signature.

		//So it needs to be specified, is one tried to get a 100% real signature, or are they trying to
		// get the signatures interface?

		ITypePolicy* ObtainGenericSignature(IGenericParameter* gen_params)
		{
			//This obtains a signature that must be specialized at runtime.
		}

		AbstractTypePolicy* ObtainSpecialSignature(Function*, IGenericParameter* func_params, IGenericParameter* gen_params)
		{
			//Func params are a requirement. Gen Params are if it actually has any.

			//This obtains a signature that is constant during runtime.
			//EX: void()
		}

		

	};




	//A few notes. Make the below mirror this a bit
	// struct SpecialFunction : public AbstractFunction, public SpecServer
	//Instead of these looking like this, I'll make a singleton control these, the manager if you will.
	// They'll be simple as fuck, and wont even be an abstractTypePolicy. Just ISpecializable (which might need to consider
	// being a real interface again).
	// lastly, I need a value to represent non-accessible types again, that are basically treated like void (which I'll need)
	// to check for.
	//That ID is likely going to be all bits on everywhere. This id essentially shouldn't make it into code.

	struct GenericSignature : public AbstractTypePolicy, public SpecServer
	{
		//if a function signature is generic, this is the first thing that goes within it's type.
		// this is inaccessible in about every sense of the word, it shouldn't be counted as anything even remotely valid.
		// so much so, I don't think I need to treat it like a real type policy, more like an GenericParamPolicy, but abstract

		//that may be confusing but let me word it like this. Abstract policies can stand on their own, they don't need arguments
		// to answer quests about themselves, interface policies do. Despite this not having any extends, or an actual proper
		// id, it won't change the fact that it can answer any question levied at it.

		//TLDR, I need an AbstractTypePolicy type that has virtually nothing in it, but still functions as an abstract
		// policy. To save space sure, but so I can specify which functions currently do and will always do nothing.

		
		//Major problem with that though,


		ISpecializable* GetSpecializable() override
		{
			return GetTarget();
		}

		GenericSignature(ISpecializable* tar, IGenericArgument& args) : SpecServer{ tar, args } {}
	};

	struct FunctionSignature : public AbstractTypePolicy, public SpecServer
	{
		//The size of the function signature is the amount of parameters it has. If these match between function
		// signatures, regardless of return types further queries will need to be made into their ambiguity.
		size_t _size;

		//If it's generic, the first type is the a GenericSignature that houses all of the generic parameter types.
		bool _generic;

		bool IsMethod()
		{
			//I completely forgot about this element, so I'm just going to have this like this for now. Fucking help me god.
			return false;
		}


		bool IsGeneric()
		{
			return _generic;
		}

		GenericSignature* GetGenericSignature()
		{
			return nullptr;
		}

		size_t GetSize()
		{
			//if it's not generic remove 1 for return type, if not remove
			return _types.size() - GetFirstIndex();
		}
		
		size_t GetFirstIndex()
		{
			return _generic ? 2 : 1;
		}

		ISpecializable* GetSpecializable() override
		{
			return GetTarget();
		}

		bool CompareSignature(FunctionSignature* a_rhs)
		{
			//should the right hand side or "this" should allowed to be null?


			//if all else breaks down, we test if one is generic and the other isn't
			//<!>This is wrong actually, generic should ALWAYS be checked, to make sure they don't have the same arg count.
			bool is_generic = IsGeneric();

			if (a_rhs->IsGeneric() == is_generic && is_generic){
				//If they are both generic
				return false;
			}
			else if (is_generic)
			{
				//if it is a method, ask if the target types are both the same. If so, ambiguity is possible and we continue, but if not
				// that's merely considered overloading.
				if (_types[l_index]->IsRelated(a_rhs->_types[r_index]) == false)
				{
					return false;
				}


				//And if not the above, push the indices forward the first index gets pushed forward by one.
				l_index += 1;
				r_index += 1;

				//Also reduce by one to prevent out of bounds.
				l_size -= 1;
			}
			//Above is generic area, still unsure how to deal with.



			//we should start from the first parameter, that should be either 1 if not generic, or 2 if it is

			size_t l_index = GetFirstIndex();
			size_t r_index = a_rhs->GetFirstIndex();
			
			size_t l_size = GetSize();
			size_t r_size = a_rhs->GetSize();
			
			
			if (l_size != r_size)
			{
				//The functions aren't ambiguous, but that doesn't always mean their use always is. See params/defaults.
				return false;
			}

			bool method = IsMethod();

			if (a_rhs->IsMethod() != method)
			{
				//If they aren't both methods, it's not possible for them to 
				return false;
			}
			else if (method)
			{
				//if it is a method, ask if the target types are both the same. If so, ambiguity is possible and we continue, but if not
				// that's merely considered overloading.
				if (_types[l_index]->IsRelated(a_rhs->_types[r_index]) == false)
				{
					return false;
				}

				
				//And if not the above, push the indices forward the first index gets pushed forward by one.
				l_index += 1;
				r_index += 1;

				//Also reduce by one to prevent out of bounds.
				l_size -= 1;
			}

			//TODO: The first member matters, if 
			for (int i = 0; i < l_size; i++)
			{
				auto l_type = _types[i + l_index];
				auto r_type = a_rhs->_types[i + r_index];

				//If one of the types aren't related, there's no way that it can be ambiguous (once again, arguments could beg to differ this.
				if (l_type->IsRelated(r_type) == false){
					//This is incorrect, even if there's a difference here, generic could pose an issue
					return false;
				}
			}


			
		}

		FunctionSignature(ISpecializable* tar, IGenericArgument& args) : SpecServer{ tar, args } {}
	};
	//The above can be the exact same fucking thing btw. Probably about to make it so.
	//If so, Signature is their names, and SignatureManager is the below. Wait, parameter signature.
	// Even so, these both function so differently in what they hold, I'm unsure how I'll handle that.
	// I think I'll override the <=> operator, and have one direction mean "these aren't supposed to be compared".

	struct Signature : public ISpecializable
	{
		//The idea of this is that this controls both versions of signatures. it merely has a setting to control
		// which it makes

		bool _isFunc;

		//The size is the argument count but for func sig a value with the sign bit (below 0) means it's generic.
		//Could I just have these without the need of making them?
		inline static std::map<size_t, Signature*> genSigTable;
		inline static std::map<int64_t, Signature*> funcSigTable;

		inline static Signature* funcSignature = nullptr;
		inline static Signature* genSignature = nullptr;


		

		GenericSignature* ObtainGenericSignature(ISpecializable* target)
		{
			//This obtains a signature that must be specialized at runtime.
			Signature*& sig = genSigTable[target->size()];

			if (!sig) {
				sig = new Signature();
				sig->_isFunc = false;
				//What else gets assigned here?
			}

			GenericArgumentArray args = target->GetGenericParamsAsArgument();

			SpecServer* result = sig->ObtainSpecialized(args);

			//Could static cast this probably right?
			return dynamic_cast<GenericSignature*>(result);
		}

		//In making the generic parameters a compile time signature, I can use abstract Type.
		FunctionSignature* ObtainGenericFunctionSignature(GenericFunction* func, IGenericParameter* func_params)
		{
			///Should be func_params->AsArgument()
			std::vector<AbstractTypePolicy*> param_types = func->GetParameters();

			//Could make a common function to handle this maybe???
			size_t base_count = func->GetParameterCount();
			size_t count = base_count | sign_value<size_t>;

			//This obtains a signature that must be specialized at runtime.
			Signature*& sig = funcSigTable[count];

			if (!sig) {
				sig = new Signature();
				sig->_isFunc = true;
				//What else gets assigned here?
			}

			//Now how we do this.
			//ObtainGenericSignature(ISpecializable * target)


			GenericArgumentArray args{ base_count + 1 };

			args[0] = ObtainGenericSignature(func);


			std::transform(param_types.begin(), param_types.end(), args.begin() + 1, [](AbstractTypePolicy* it) {return it; });

			//Could static cast this probably right?
			FunctionSignature* result = dynamic_cast<FunctionSignature*>(sig->ObtainSpecialized(args));

			result->_generic = true;

			return result;
		}



	


		ITypePolicy* ObtainFunctionSignature(AbstractFunction* func, IGenericParameter* func_params)
		{
			//This should not be able to be created via a generic function. Rather, abstract should only be either a specialization of a generic function or a regular
			// function.

			//The main reason is because this is used to obtain the runtime of a perhaps unspecialized

			//Func params are a requirement. Gen Params are if it actually has any. Actually, the actual function being generic or not should cover that,
			// and generic function signatures arent handled here.


			//This obtains a signature that is constant during runtime.
			//EX: void()


			//This is going to assume this Isn't a generic function, and thus can be made partial.
			// later, if it's is a generic I could set it manually.
			
			//if (IsGenericFunction)
			//	return nullptr;// Cannot use?

			
			std::vector<AbstractTypePolicy*> param_types = func->GetParameters();

			
			size_t count = func->GetParameterCount();

			//This obtains a signature that must be specialized at runtime.
			Signature*& sig = funcSigTable[count];

			if (!sig) {
				sig = new Signature();
				sig->_isFunc = false;
				//What else gets assigned here?
			}

			//Now how we do this.
			//ObtainGenericSignature(ISpecializable * target)

			//I can't use this like this. I need the actual arguments. The ITypePolicy.
			//GenericArgumentArray args{ count };

			//std::transform(param_types.begin(), param_types.end(), args.begin(), [](AbstractTypePolicy* it) {return it; });


			//Since this is getting things only from the specialization of a function, it wouldn't have to do it like this right?
			// So I think I've made a mistake somewhere along the way. The targeting func needs to change.
			//GenericFunction* gen_func = dynamic_cast<GenericFunction*>(func);//?
			//if (gen_func){}else{}
			
			//Means the type is genuin with no need for partializing.
			auto* def_params = func_params->AsArgument();
			if (def_params)
			{

			}
			else

			//TODO: has no accounting for methods. ObtainFunctionSignature needs to be split.
			ITypePolicy* result = dynamic_cast<ITypePolicy*>(func->GetSpecializable()->ObtainSpecializer(sig, *func_params));
			

			return result;
		}

		
		SpecServer* MakeServer(IGenericArgument& args) override
		{
			SpecServer* result = nullptr;
			
			//Not actually proper, this is just what it sends.
			if (_isFunc){
				FunctionSignature* sig = new FunctionSignature(this, args);
				result = sig;
				//Use the target of the server instead.
				sig->_generic = args.size() && dynamic_cast<GenericSignature*>(args.GetArgAtIndex(0));

				//Gotta set the generic status here, because server might be created from partial.
			}
			else{
				result = new GenericSignature(this, args);
			}

			return result;
		}
	};

	//*/
}