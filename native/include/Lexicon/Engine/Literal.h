#pragma once

#include "Lexicon/Variable.h"
#include "Lexicon/ObjectInfo.h"

namespace LEX
{
	class Variable;


	//If I can, I'd like to find a way to remove hash and type info.
	struct Literal
	{
		Literal(bool object)
		{
			if (object) {
				value = Object{};
				data = std::make_unique<ObjectLiteralData>();
			}
			
		}
		struct ObjectLiteralData
		{
			std::string code;
			ObjLitCtor ctor;
		};

		mutable Variable value;

		union
		{
			size_t _hash{};
			TypeInfo* _info;
		};
		mutable std::unique_ptr<ObjectLiteralData> data;

		const Literal* ptr() const
		{
			return this;
		}

		size_t hash() const
		{
			return value.IsObject() ? _hash : 0;
		}

		TypeInfo* info() const
		{
			return !value.IsObject() ? _info : nullptr;
		}

		Variable& GetVariable() const
		{
			if (data) {
				value = data->ctor(data->code);
				data.reset();
			}

			return value;
		}


		//Here's a question, what do I do if the type is generic? We can load whatever core type it is, but having some method that'll allow
		// for the actual declared type would be good. Maybe a conversion?
		//I think what I'll do there is just do a conversion into whatever register is demanded of it.
		//Actually, check that, I'll return a solution. A type it's supo


		auto operator <=>(const Literal& other) const
		{
			//I'm actually kinda sure sets can't handle what I intend to do here. I think hash should serve as a complete override
			// and data should be unaccounted for entirely. That if hash is present, even the value is completely irrelevant.

			auto self_hash = hash();
			auto other_hash = other.hash();

			if (!!self_hash != !!other_hash || self_hash) {
				return self_hash <=> other_hash;
			}

			auto self_type = info();
			auto other_type = other.info();

			if (self_type != other_type) {
				return self_type <=> other_type;
			}

			return value <=> other.value;
		}
	};

	using LiteralPtr = const Literal*;

}