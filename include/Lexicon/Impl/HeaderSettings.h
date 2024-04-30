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

	ENUM(HeaderFlag, uint8_t)
	{
		None		= 0 << 0,
		Signable	= 1 << 0,
		Sizable		= 1 << 1,
		Reference	= 1 << 2,
		Constness	= 1 << 3,
		DeclConst	= 1 << 4,
		DeclMute	= 1 << 5,
		Storage		= 1 << 6,
		Primary		= 1 << 7,

		All			= (uint8_t)-1,
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
		headerGuide[KeywordType::TypeQual]["ref"] = { HeaderFlag::Reference, HeaderFlag::None };
		headerGuide[KeywordType::TypeQual]["const"] = { HeaderFlag::Constness, HeaderFlag::None };
		headerGuide[KeywordType::TypeQual]["mutable"] = { HeaderFlag::Constness, HeaderFlag::None };
		headerGuide[KeywordType::DeclSpec]["readonly"] = { HeaderFlag::Constness, HeaderFlag::None };

		headerGuide[KeywordType::DeclSpec]["const"] = { HeaderFlag::DeclConst, HeaderFlag::None, true };
		headerGuide[KeywordType::DeclSpec]["static"] = { HeaderFlag::Storage, HeaderFlag::None };
		headerGuide[KeywordType::DeclSpec]["mutable"] = { HeaderFlag::DeclMute, HeaderFlag::None, true };
		headerGuide[KeywordType::DeclSpec]["readonly"] = { HeaderFlag::DeclConst, HeaderFlag::None, true };

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
		headerGuide[KeywordType::TypeSpec]["void"] = { HeaderFlag::Primary, HeaderFlag::All };
		headerGuide[KeywordType::TypeSpec]["string"] = { HeaderFlag::Primary, HeaderFlag::None };
		headerGuide[KeywordType::TypeSpec]["char"] = { HeaderFlag::Primary, HeaderFlag::None };//char doesn't exist yet.

		//These are searched in perfect order, among keywords submitted to it.

	};


	inline KeywordType GetKeywordType(std::string name, std::optional<bool> post)//, const HeaderEntry** out = nullptr)
	{
		//If post is selected, the expected keywords must match the post boolean.
		KeywordType type = KeywordType::TypeQual;
		for (auto& map : headerGuide)
		{
			for (auto& [key, entry] : map)
			{
				if (key == name && (!post.has_value() || entry.postOnly == post.value())) {
					//if (out) {
					//	*out = &entry;
					//}

					return type;
				}
			}

			type = static_cast<decltype(type)>(type + 1);
		}

		return KeywordType::None;
	}

	inline bool IsKeyword(std::string name)//, const HeaderEntry** out = nullptr)
	{
		//If post is selected, the expected keywords must match the post boolean.

		return GetKeywordType(name, std::nullopt) != KeywordType::None;
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