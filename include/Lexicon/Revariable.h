#pragma once

#include "Lexicon/ProxyGuide.h"
#include "Lexicon/RuntimeVariable.h"

namespace LEX
{

	struct RefCollection
	{
		std::map<const void*, RuntimeVariable> collection;

		void Collect(const void* entry, RuntimeVariable& var)
		{
			//Make sure var is NOT an actual variable.
			collection[entry] = var;
		}

		void Collect(const void* entry, Variable* var)
		{
			//Make sure var is NOT an actual variable.
			collection[entry] = std::ref(*var);
		}


		void AddCollection(RefCollection& other)
		{
			collection.insert_range(std::move(other.collection));
		}

		template <typename T>
		void TryToCollect(T& other)
		{

			if constexpr (std::derived_from<T, RefCollection>)
				AddCollection(other);
		}

		bool TryLoadReference(const void* ptr, RuntimeVariable& out)
		{
			auto it = collection.find(ptr);

			if (collection.end() != it) {
				out = it->second;
				return true;
			}

			return false;
		}

		//attempting to add a runtime variable that it should fail
	};

	template <typename T>
	struct Revariable
	{
		void operator()(T& entry, Variable* arg)
		{
			if constexpr (!std::is_const_v<T>) {
				//Please note, by this point it should have already been simplified to it's look type, and is merely trying to be come it's true type.
				//arg->Assign(static_cast<detail::simplify_wrapper_t<ElemArg>&>(entry));
				arg->Assign(std::move(entry));
			}
		}
	};
	/*
	template <typename T> requires(std::is_const_v<T>)
	struct Revariable<T>
	{
		void operator()(T& entry, Variable* arg)
		{
			//if const, we don't do anything at all. The rules of the script remain more important.
		}
	};
	//*/
	template <typename T> requires (requires(ProxyGuide<std::remove_cvref_t<T>> guide, T& entry, Variable* arg) { guide.Revariable(entry, arg); })
		struct Revariable<T> : public std::conditional_t<std::derived_from<ProxyGuide<std::remove_cvref_t<T>>, RefCollection>, RefCollection, std::monostate>
	{
		void operator()(T& entry, Variable* arg)
		{
			ProxyGuide<std::remove_cvref_t<T>> guide{};

			guide.Revariable(entry, arg);

			if constexpr (std::derived_from<Revariable<T>, RefCollection>) {
				this->TryToCollect(guide);
			}
		}
	};
}