#pragma once

#include "Lexicon/Engine/ProcessContext.h"

namespace LEX::Impl
{
	struct Parser;

	ENUM(ModulePriority, uint32_t)
	{
		None	= 0x00000000,
		Minimal = 0x00000001,
		Low		= 0x000000FF,
		Medium	= 0x0000FFFF,
		High	= 0x00FFFFFF,
		Max		= 0xFFFFFFFF
	};


	ENUM(ParseKeyword, uint8_t)
	{
		None = 0,
		Statement = 1 << 0,

	};

	ENUM(ParseFlag, uint32_t)//This is just passed around so it can be as big as it wants to be.
	{
		None = 0,
		Atomic =  1 << 0,
		Direct =  1 << 1,//This parser is being used from another parser, different rules might apply
	};


	struct ParseModule : public ProcessContext
	{
	private:
		//Change Question to Query, 
		//Query to Try,
		// and try to Expect or smth
		
		static Record _ExecuteModule(ParseModule* mdl, Parser* parser, Record* target);

		
		static bool _QuestionModule(ParseModule* mdl, Parser* parser, Record* target, ParseFlag flag);

		static bool _QueryModule(ParseModule* mdl, Parser* parser, Record& out, Record* target);


		static Record _TryModule(ParseModule* mdl, Parser* parser, Record* target);

		//Doing this here because templates have to be inline and I can't provide that.
		static ParseModule* _GetBuiltModule(Parser* parser, const std::type_info& info);
	public:


		//TODO:Make ParseModule::GetPriority a constexpr function, this way I don't have to worry about anyone changing the values at runtime.
		virtual uint32_t GetPriority() const;

		
		virtual bool CanHandle(Parser* parser, Record* target, ParseFlag flag) const = 0;

		virtual Record HandleToken(Parser* a_this, Record* target) = 0;


		//Here I'll place a line of common context keywords that will be enabled by using one of these.
		virtual ParseKeyword GetKeywords();

		bool HasKeyword(std::string_view str) override;


		//Controls what can be percieved as being part of a single statement when just trying to encapsulate the next valid statement
		virtual bool IsAtomic() const { return false; }


		//This is what should be used if one wishes to access another parse module
		// also instead of requiring it derives from this, require that it derive from a singleton class for parse modules.
		template <std::derived_from<ParseModule> Module>
		static Record ExecuteModule(Parser* parser, Record* target)
		{
			//Static HandleToken (to be ExecuteModule) will not accept constraint of being from "PivotSingleton<ParseModule>". Needs investigation.
			// note, it will if it derives from ParseModule

			ParseModule* mdl = _GetBuiltModule(parser, typeid(Module));

			std::unique_ptr<ParseModule> new_mod;

			if (!mdl) {
				new_mod = std::make_unique<Module>();
				mdl = new_mod.get();
			}

			return _ExecuteModule(mdl, parser, target);

			//Doing it like this deals with process chains.
			//parser->_ExecuteModule(result, target, mdl);
			//return result;
			
			//Might have something about process chains right here.
			//Use a different version plz.
			//return mdl->HandleToken(parser, target);
		}

		template <std::derived_from<ParseModule> Module>
		static bool QueryModule(Parser* parser, Record& out, Record* target)
		{
			//Does what TryModule does, but will return in the case that it fails instead of croaking..
			//Try module will try to use use try module, and if it's unsuccessful, it will croak.
			// Basically a checked ParseAtomic

			ParseModule* mdl = _GetBuiltModule(parser, typeid(Module));

			std::unique_ptr<ParseModule> new_mod;

			if (!mdl) {
				new_mod = std::make_unique<Module>();
				mdl = new_mod.get();
			}

			return _QueryModule(mdl, parser, out, target);
			//return parser->_TryModule(out, target, mdl);
		}

		template <std::derived_from<ParseModule> Module>
		static Record TryModule(Parser* parser, Record* target)
		{
			ParseModule* mdl = _GetBuiltModule(parser, typeid(Module));

			std::unique_ptr<ParseModule> new_mod;

			if (!mdl) {
				new_mod = std::make_unique<Module>();
				mdl = new_mod.get();
			}

			return _TryModule(mdl, parser, target);
		}
		template <std::derived_from<ParseModule> Module>
		static bool QuestionModule(Parser* parser, Record* target, ParseFlag flag)
		{
			ParseModule* mdl = _GetBuiltModule(parser, typeid(Module));

			std::unique_ptr<ParseModule> new_mod;

			if (!mdl) {
				new_mod = std::make_unique<Module>();
				mdl = new_mod.get();
			}

			return _QuestionModule(mdl, parser, target, flag);
		}
		
		virtual std::string FailureMessage() const
		{
			return "Module failed to parse";
		}

		//Should actually be pure but it's defined for ease of use right now.
		std::string_view GetContext() override
		{
			return "";
		}

		virtual std::string_view GetModuleName() = 0;

		virtual ParseMode GetParseMode() const
		{
			return ParseMode::kBasic;
		}
	};
}
