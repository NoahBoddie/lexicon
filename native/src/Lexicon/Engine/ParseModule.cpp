#include "Lexicon/Engine/ParseModule.h"
#include "Lexicon/Engine/Parser.h"

namespace LEX
{

	Record ParseModule::ExecuteModule(ParseModule* mdl, ParsingStream* parser, Record* target)
	{
		if (!mdl) {
			parser->croak("something about static handle token.");
		}

		Record result{};

		//Doing it like this deals with process chains.
		parser->ExecuteModule(result, target, mdl);

		return result;

		//Might have something about process chains right here.
		//Use a different version plz.
		//return mdl->HandleToken(parser, target);
	}


	bool ParseModule::QueryModule(ParseModule* mdl, ParsingStream* parser, Record* target, ParseFlag flag)
	{
		if (!mdl) {
			parser->croak("something about static handle token.");
		}

		return parser->QueryModule(target, mdl, flag);
	}

	//Need to snuff out the wrong try being used rq
	bool ParseModule::TryModule(ParseModule* mdl, ParsingStream* parser, Record& out, Record* target)
	{
		if (!mdl) {
			parser->croak("something about static handle token.");
		}

		return parser->TryModule(out, target, mdl, ParseFlag::Direct);
	}

	Record ParseModule::UseModule(ParseModule* mdl, ParsingStream* parser, Record* target)
	{

		if (!mdl) {
			parser->croak("something about static handle token.");
		}


		Record result{};

		//Try module will try to use use try module, and if it's unsuccessful, it will croak.
		// Basically a checked ParseAtomic for specific modules.
		if (TryModule(mdl, parser, result, target) == false)
			parser->croak("Cant.");

		return result;
	}

	//Doing this here because templates have to be inline and I can't provide that.
	ParseModule* ParseModule::GetBuiltModule(ParsingStream* parser, const std::type_info& info)
	{
		return parser->GetBuiltModule(info);
	}
}