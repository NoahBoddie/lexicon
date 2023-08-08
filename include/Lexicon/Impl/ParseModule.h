#pragma once

#include "Lexicon/Impl/ProcessContext.h"

namespace LEX::Impl
{
	struct Parser;

	ENUM(ModulePriority, uint32_t)
	{
		None	= 0x00000000,
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

	struct ParseModule : public ProcessContext
	{
	private:
		static Record _ExecuteModule(ParseModule* mdl, Parser* parser, Record* target);


		static bool _QueryModule(ParseModule* mdl, Parser* parser, Record& out, Record* target);


		static Record _TryModule(ParseModule* mdl, Parser* parser, Record* target);
	public:


		//TODO:Make ParseModule::GetPriority a constexpr function, this way I don't have to worry about anyone changing the values at runtime.
		virtual uint32_t GetPriority() const;

		
		virtual bool CanHandle(Parser* parser, Record* target) const = 0;

		virtual Record HandleToken(Parser* a_this, Record* target) = 0;


		//Here I'll place a line of common context keywords that will be enabled by using one of these.
		virtual ParseKeyword GetKeywords();

		bool HasKeyword(std::string_view str) override;

	


		//This is what should be used if one wishes to access another parse module
		// also instead of requiring it derives from this, require that it derive from a singleton class for parse modules.
		template <std::derived_from<ParseModule> Module>
		static Record ExecuteModule(Parser* parser, Record* target)
		{
			//Static HandleToken (to be ExecuteModule) will not accept constraint of being from "PivotSingleton<ParseModule>". Needs investigation.
			// note, it will if it derives from ParseModule
			ParseModule* mdl = Module::GetSingleton();

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
			ParseModule* mdl = Module::GetSingleton();

			return _QueryModule(mdl, parser, out, target);
			//return parser->_TryModule(out, target, mdl);
		}

		template <std::derived_from<ParseModule> Module>
		static Record TryModule(Parser* parser, Record* target)
		{
			ParseModule* mdl = Module::GetSingleton();

			_TryModule(mdl, parser, target);

			Record result{};

			//Try module will try to use use try module, and if it's unsuccessful, it will croak.
			// Basically a checked ParseAtomic for specific modules.
			//if (QueryModule<Module>(parser, result, target) == false)
			//	parser->GetInput()->croak("Expected blank black blah blah blah");

			return result;
		}

		//Should actually be pure but it's defined for ease of use right now.
		std::string_view GetContext() override
		{
			return "";
		}

		virtual std::string_view GetModuleName() = 0;
	};
}
