#pragma once

namespace LEX
{


	ENUM(KeywordType)
	{
		TypeQual,
		DeclSpec,
		TypeSpec,

		Total,

		None = KeywordType::Total,
	};

	ENUM(HeaderFlag, int16_t)
	{
		None		= 0 << 0,
		Signable	= 1 << 0,
		Sizable		= 1 << 1,
		Reference	= 1 << 2,
		Mutable		= 1 << 3,
		Constness	= 1 << 4,
		DeclConst	= 1 << 5,
		DeclMute	= 1 << 6,
		Storage		= 1 << 7,
		Primary		= 1 << 8,
		Access1st	= 1 << 9,
		Access2nd	= 1 << 10,
		All			= -1,
		MostlyAll	= HeaderFlag::All & ~HeaderFlag::Storage,
	};


	//make all this a class maybe.

	struct HeaderEntry
	{
		HeaderFlag includeFlags;
		HeaderFlag excludeFlags;
		bool postOnly = false;
	};

	inline std::array<std::map<std::string, HeaderEntry>, KeywordType::Total> headerGuide;

	INITIALIZE()
	{
		//TODO: Keywords should probably be made incrementally, with this contributing to the list of keywords. Just to make it easy.

		//In of itself, reference might basically be redundant if the below are used.
		headerGuide[KeywordType::TypeQual]["ref"] = { HeaderFlag::Reference, HeaderFlag::None };
		headerGuide[KeywordType::TypeQual]["local"] = { HeaderFlag::Reference, HeaderFlag::None };
		headerGuide[KeywordType::TypeQual]["global"] = { HeaderFlag::Reference, HeaderFlag::None };
		headerGuide[KeywordType::TypeQual]["maybe"] = { HeaderFlag::Reference, HeaderFlag::None };
		headerGuide[KeywordType::TypeQual]["scoped"] = { HeaderFlag::Reference, HeaderFlag::None };
	
		headerGuide[KeywordType::TypeQual]["mutable"] = { HeaderFlag::Mutable, HeaderFlag::None };
		headerGuide[KeywordType::TypeQual]["const"] = { HeaderFlag::Constness, HeaderFlag::Mutable };
		headerGuide[KeywordType::TypeQual]["readonly"] = { HeaderFlag::Constness, HeaderFlag::Mutable };

		headerGuide[KeywordType::DeclSpec]["const"] = { HeaderFlag::DeclConst, HeaderFlag::None, true };
		headerGuide[KeywordType::DeclSpec]["static"] = { HeaderFlag::Storage, HeaderFlag::None };
		headerGuide[KeywordType::DeclSpec]["abstract"] = { HeaderFlag::Storage, HeaderFlag::None };
		//headerGuide[KeywordType::DeclSpec]["virtual"] = { HeaderFlag::Storage, HeaderFlag::None };
		headerGuide[KeywordType::DeclSpec]["external"] = { HeaderFlag::None, HeaderFlag::None };//Extern doesn't really have any enemies other than maybe abstract?
		headerGuide[KeywordType::DeclSpec]["mutable"] = { HeaderFlag::DeclMute, HeaderFlag::None, true };
		headerGuide[KeywordType::DeclSpec]["readonly"] = { HeaderFlag::DeclConst, HeaderFlag::None, true };
		headerGuide[KeywordType::DeclSpec]["public"] = { HeaderFlag::Access1st, HeaderFlag::None };
		headerGuide[KeywordType::DeclSpec]["private"] = { HeaderFlag::Access1st, HeaderFlag::None };
		headerGuide[KeywordType::DeclSpec]["protected"] = { HeaderFlag::Access1st, HeaderFlag::None };
		headerGuide[KeywordType::DeclSpec]["internal"] = { HeaderFlag::Access2nd, HeaderFlag::None };
		
		

		headerGuide[KeywordType::TypeSpec]["typename"] = { HeaderFlag::Primary, HeaderFlag::MostlyAll };
		headerGuide[KeywordType::TypeSpec]["signed"] = { HeaderFlag::Signable, HeaderFlag::None };
		headerGuide[KeywordType::TypeSpec]["unsigned"] = { HeaderFlag::Signable, HeaderFlag::None };
		headerGuide[KeywordType::TypeSpec]["short"] = { HeaderFlag::Sizable, HeaderFlag::None };
		headerGuide[KeywordType::TypeSpec]["long"] = { HeaderFlag::Sizable, HeaderFlag::None };
		headerGuide[KeywordType::TypeSpec]["bool"] = { HeaderFlag::Primary, HeaderFlag::Sizable };//booleans can be signed to represent a maybe value.
		headerGuide[KeywordType::TypeSpec]["string"] = { HeaderFlag::Primary, HeaderFlag::Sizable };
		headerGuide[KeywordType::TypeSpec]["int"] = { HeaderFlag::Primary, HeaderFlag::None };
		headerGuide[KeywordType::TypeSpec]["double"] = { HeaderFlag::Primary, HeaderFlag::Sizable };
		headerGuide[KeywordType::TypeSpec]["float"] = { HeaderFlag::Primary, HeaderFlag::Sizable };
		headerGuide[KeywordType::TypeSpec]["void"] = { HeaderFlag::Primary, HeaderFlag::MostlyAll };
		headerGuide[KeywordType::TypeSpec]["voidable"] = { HeaderFlag::Primary, HeaderFlag::MostlyAll };
		headerGuide[KeywordType::TypeSpec]["string"] = { HeaderFlag::Primary, HeaderFlag::None };
		headerGuide[KeywordType::TypeSpec]["char"] = { HeaderFlag::Primary, HeaderFlag::None };//char doesn't exist yet.
		headerGuide[KeywordType::TypeSpec]["object"] = { HeaderFlag::Primary, HeaderFlag::MostlyAll };//char doesn't exist yet.

		//These are searched in perfect order, among keywords submitted to it.

	};


	inline KeywordType GetKeywordType(std::string& name, std::optional<bool> post, const HeaderEntry** out = nullptr)
	{
		//If post is selected, the expected keywords must match the post boolean.
		KeywordType type = KeywordType::TypeQual;
		for (auto& map : headerGuide)
		{
			for (auto& [key, entry] : map)
			{
				if (key == name && (!post.has_value() || entry.postOnly == post.value())) {
					if (out) {
						*out = &entry;
					}

					return type;
				}
			}
			
			type = static_cast<decltype(type)>(type + 1);
		}

		return KeywordType::None;
	}


	inline KeywordType GetKeywordType(std::string& name, std::optional<bool> post, const HeaderEntry*& out)
	{
		return GetKeywordType(name, post, &out);
	}

	inline bool IsKeyword(std::string& name, std::optional<bool> post = std::nullopt)  //, const HeaderEntry** out = nullptr)
	{
		//If post is selected, the expected keywords must match the post boolean.

		return GetKeywordType(name, post) != KeywordType::None;
	}


	inline bool IsTypename(std::string& name)
	{
		auto& map = headerGuide[KeywordType::TypeSpec];

		auto it = map.find(name);

		return map.end() != it && it->second.includeFlags & HeaderFlag::Primary;
	}

	inline bool IsTypename(std::string&& name)
	{
		return IsTypename(name);
	}

}