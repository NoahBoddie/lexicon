#pragma once

#include "Lexicon/Engine/ProcessContext.h"
#include "Lexicon/Engine/Tokenizer.h"
namespace LEX
{
	struct ParsingStream;
	struct ModuleChain;
	ENUM(ModulePriority, uint32_t)
	{
		None	= 0x00000000,
		Minimal = 0x00000001,
		Low		= 0x000000FF,
		Medium	= 0x0000FFFF,
		High	= 0x00FFFFFF,
		Max		= 0xFFFFFFFF
	};

	//Delete
	ENUM(ParseKeyword, uint8_t)
	{
		None = 0,
		Statement = 1 << 0,

	};


	ENUM(ModuleFlag, uint32_t)//This is just passed around so it can be as big as it wants to be.
	{
		None = 0,
		Atomic = 1 << 0,
		Eof = 1 << 1,//Parser can be used at EOF
	};

	ENUM(ParseFlag, uint32_t)//This is just passed around so it can be as big as it wants to be.
	{
		None = 0,
		Atomic =  1 << 0,
		Direct =  1 << 1,//This parser is being used from another parser, different rules might apply
	};

	struct ParseModule;

	using ModuleBuilder = std::unique_ptr<ParseModule>(*)();



	struct ParseModule
	{
	protected:
		struct SetBuilder
		{
			SetBuilder(ModuleBuilder ctor, const std::string_view& name)
			{
				//move ctor to some place where it can be used.
				ParseModule::AddBuilder(ctor, name);
			}
		};


		static void AddBuilder(ModuleBuilder build, const std::string_view& category)
		{
			auto pair = std::make_pair(category, build);
			buildList.insert(std::upper_bound(buildList.begin(), buildList.end(), pair, [](auto& lhs, auto& rhs)
				{return lhs.second()->GetPriority() > rhs.second()->GetPriority(); }),
				pair);
		}

	public:
		static std::vector<std::unique_ptr<ParseModule>> BuildModules(const std::string_view& name)
		{
			std::vector<std::unique_ptr<ParseModule>> result{};
			result.reserve(buildList.size());

			for (auto& [category, builder] : buildList)
			{
				if (category.empty() || category.starts_with(name) == true) {
					result.push_back(builder());
				}
			}

			return result;
		}


	private:
		inline static std::vector<std::pair<std::string_view, ModuleBuilder>> buildList;


	public:
		virtual ~ParseModule() = default;

	private:


		static Record ExecuteModule(ParseModule* mdl, ParsingStream* parser, Record* target);


		static bool QueryModule(ParseModule* mdl, ParsingStream* parser, Record* target, ParseFlag flag);

		//Need to snuff out the wrong try being used rq
		static bool TryModule(ParseModule* mdl, ParsingStream* parser, Record& out, Record* target);

		static Record UseModule(ParseModule* mdl, ParsingStream* parser, Record* target);

		//Doing this here because templates have to be inline and I can't provide that.
		static ParseModule* GetBuiltModule(ParsingStream* parser, const std::type_info& info);




	public:

		//rename to get keyword state so HasKeyword can be free again.
		virtual std::optional<bool> GetKeywordState(const std::string_view& type)
		{
			return false;
		}

		bool HasKeyword(const std::string_view& type)
		{
			return GetKeywordState(type).value_or(false);
		}

		//Idea here is I can have a context respond to if it wants to be able to have a certain thing handle it. if it's damning enough,
		// I can send an error.
		virtual bool ContextAllowed(ParseModule* context, ModuleChain* link)
		{
			return true;
		}


		virtual std::string_view GetCategory() const { return ""; }




		//Change Question to Query, 
		//Query to Try,
		// and try to Expect or smth









	public:


		//TODO:Make ParseModule::GetPriority a constexpr function, this way I don't have to worry about anyone changing the values at runtime.
		virtual uint32_t GetPriority() const
		{
			return ModulePriority::Minimal;
		}


		virtual bool CanHandle(ParsingStream* parser, Record* target, ParseFlag flag) const = 0;

		virtual Record HandleToken(ParsingStream* a_this, Record* target) = 0;




		//Controls what can be percieved as being part of a single statement when just trying to encapsulate the next valid statement


		//This is what should be used if one wishes to access another parse module
		// also instead of requiring it derives from this, require that it derive from a singleton class for parse modules.
		template <std::derived_from<ParseModule> Module>
		static Record ExecuteModule(ParsingStream* stream, Record* target)
		{
			//Static HandleToken (to be ExecuteModule) will not accept constraint of being from "PivotSingleton<ParseModule>". Needs investigation.
			// note, it will if it derives from ParseModule

			ParseModule* mdl = GetBuiltModule(stream, typeid(Module));

			std::unique_ptr<ParseModule> new_mod;

			if (!mdl) {
				new_mod = std::make_unique<Module>();
				mdl = new_mod.get();
			}

			return ExecuteModule(mdl, stream, target);

			//Doing it like this deals with process chains.
			//parser->_ExecuteModule(result, target, mdl);
			//return result;

			//Might have something about process chains right here.
			//Use a different version plz.
			//return mdl->HandleToken(parser, target);
		}

		template <std::derived_from<ParseModule> Module>
		static bool TryModule(ParsingStream* stream, Record& out, Record* target)
		{
			//Does what TryModule does, but will return in the case that it fails instead of croaking..
			//Try module will try to use use try module, and if it's unsuccessful, it will croak.
			// Basically a checked ParseAtomic

			ParseModule* mdl = GetBuiltModule(stream, typeid(Module));

			std::unique_ptr<ParseModule> new_mod;

			if (!mdl) {
				new_mod = std::make_unique<Module>();
				mdl = new_mod.get();
			}

			return TryModule(mdl, stream, out, target);
			//return parser->_TryModule(out, target, mdl);
		}

		template <std::derived_from<ParseModule> Module>
		static Record UseModule(ParsingStream* parser, Record* target)
		{
			ParseModule* mdl = GetBuiltModule(parser, typeid(Module));

			std::unique_ptr<ParseModule> new_mod;

			if (!mdl) {
				new_mod = std::make_unique<Module>();
				mdl = new_mod.get();
			}

			return UseModule(mdl, parser, target);
		}
		template <std::derived_from<ParseModule> Module>
		static bool QueryModule(ParsingStream* parser, Record* target, ParseFlag flag)
		{
			ParseModule* mdl = _GetBuiltModule(parser, typeid(Module));

			std::unique_ptr<ParseModule> new_mod;

			if (!mdl) {
				new_mod = std::make_unique<Module>();
				mdl = new_mod.get();
			}

			return QueryModule(mdl, parser, target, flag);
		}


		virtual Tokenizer* GetTokenizer() const
		{
			return std::addressof(scriptTokens);
		}

		virtual bool IsAtomic() const { return false; }

		virtual bool CanParseEOF() const { return false; }

		//New stuff


		//If it fails to be used manually this is what will display
		virtual std::string FailureMessage() const
		{
			return "Attempt to use module has failed.";
		}


		virtual bool HasConditional(const std::string_view& name) const
		{
			return false;
		}


	};


}
