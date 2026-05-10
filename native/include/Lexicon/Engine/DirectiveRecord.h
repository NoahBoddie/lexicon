#pragma once


#include "Lexicon/Engine/Directive.h"


#include "Lexicon/Report.h"
#include "Lexicon/Interfaces/ReportManager.h"

namespace LEX
{
	class Script;
	struct Element;

	struct DirectiveBody;

	struct DirectiveBody
	{
		using Self = BasicRecord<Directive, DirectiveBody>;

	private:
		Self* GetSelf();

	public:

		Directive& GetDirective();





		void PrintSyntax(std::string indent)
		{
			constexpr std::string_view __dent = "| ";

			auto self = GetSelf();

			std::string log = self->Print();

			logger::debug("{}{}", indent, log);
			indent += __dent;

			for (auto& child_rec : self->children())
			{
				child_rec->PrintSyntax(indent);
			}
		}


		void PrintSyntax()
		{
			PrintSyntax("");

			logger::debug("Record uses {} Kilobytes", GetSelf()->GetMemoryUsage() / 1000.f);
		}


	};

#define DIRECTIVE() GetEnumFromRecord<LEX::Directive>()


	using DirectiveRecord = BasicRecord<Directive, DirectiveBody>;

	//TODO: Make DirectiveRecord inherit properly from the top layer like SyntaxRecord does.
}