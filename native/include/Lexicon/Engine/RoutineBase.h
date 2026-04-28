#pragma once

#include "Lexicon/Engine/Instruction.h"

namespace LEX
{
	using RecordList = std::vector<SyntaxRecord*>;

	//TODO: I'd like to handle routine records by just the location, and the string that it uses as the scripts name for calling.
	//additionally I'd like to use this to help with the names of formulas. Example being for ActorValues, something like
	//"GetActorValue/HitsTaken/Damage"

	//*

	//TODO: I may not actually need the entire record, it might just be fitting to carry around a string, the parent, and the syntax stuff.
	// But like this I could probably just carry it purely. Maybe something else for macro data. That something else for macro data is pretty
	// important, cause it makes it easier to relay what's going on in one, and where it's failing.

	struct SourceLocation
	{
		SourceLocation() = default;
		SourceLocation(SyntaxRecord* record) : line{ record->GetSyntax().line }, column{ record->GetSyntax().column }
		{
		}

		Line line{ 0 };
		Column column{ 0 };


	};

	struct RecordHolder
	{
		RecordHolder() = default;
		RecordHolder(const std::vector<SyntaxRecord*>& records) : locations{ records.begin(), records.end()}
		{ 
			for (int i = 0; i < records.size(); i++)
			{
				auto& syntax = records[i]->GetSyntax();
				auto& location = locations[i];

				//report::trace("{} - {} : {} vs {} : {}", i, syntax.column, syntax.line, location.column, location.line);
			}
		}

		const char* file = nullptr;
		const char* name = nullptr;
		std::vector<SourceLocation> locations;



		std::function<LogEditor> Mutator(uint32_t i)
		{
			//return [this](LogParams& params, LogState state, LogResult&) -> void { if (state == LogState::Prep) params.suffix << GetAffix(); };
			return [=](LogParams& params, LogState state, LogResult&) -> void
				{
					if (this && locations.size() > i)
					{
						auto& loc = locations[i];

						if (state == LogState::Prep)
						{
							if (name){
								params.loc.filename = name;
							}
							else {
								params.loc.filename = "<no name>";
							}
							params.loc.line = loc.line;

							params.suffix << std::format(" ({})", loc.column);
						}
					}
				};
		}

		void Log(IssueCode code, std::source_location& src, IssueLevel level, uint32_t i)
		{
			auto handle = ReportManager::instance->AddEditor(Mutator(i));

			return report::log(code, src, ReportManager::instance->GetIssueType(), level);
			//return report::log(message.prox, GetAffix(), message.src, ReportManager::instance->GetIssueType(), IssueLevel::Debug, args...);
		}

		void Log(const std::string& message, std::source_location& src, IssueLevel level, uint32_t i, IssueType type = ReportManager::instance->GetIssueType())
		{
			auto handle = ReportManager::instance->AddEditor(Mutator(i));

			return report::log(message, src, type, level);
			//return report::log(message.prox, GetAffix(), message.src, ReportManager::instance->GetIssueType(), IssueLevel::Debug, args...);
		}


		SourceLocation GetLocation(uint32_t i)
		{
			if (locations.size() <= i) {
				return {};
			}

			return locations[i];
		}


	};
	//*/
	
	ENUM(RoutineFlag, uint8_t)
	{
		None = 0,
		ForwardsVariadic = 1 << 0,//This basically unhooks the allocations. THough, this may not really be that useful in general I think
		ForwardsParameters = 1 << 1,
	};


	//Needs to be named RoutineData, as there is no routine class to derive from this.
	struct RoutineBase : public RecordHolder
	{
		//These are the base most elements of a routine. Though, still can't do shit by itself. Needs a callable unit to organize it.
		// Think of this as C++ like instructions, with nothing else to tell you how any of it is used.


		RoutineBase() = default;


		//Doesn't actually need to take vector could be list.
		RoutineBase(std::vector<Instruction>&& op, const RecordList& recs, uint32_t var, uint32_t arg, uint32_t param, RoutineFlag flag = RoutineFlag::None) :
			instructions{ std::forward<std::vector<Instruction>>(op) }
			, varCapacity{ var }
			, argCapacity{ arg }
			, paramCapacity{ param }
			, flags { flag }
			, RecordHolder{ recs }
		{
			for (int i = 0; i < instructions.size(); i++)
			{
				auto& instruct = instructions[i];

				auto location = instruct.index == -1 ? SourceLocation{} : locations[instruct.index];

				report::trace("i:{}, index:{}, ({}:{})", i, *instruct.index, location.column, location.line);
			}
		}

		//I require a constructor with (list of Ops, var capacity, arg capacity). Required, variables constant.



		//I will go with span over vector, because this will never need to change. In creation however, a list should be prefered.
		//std::span<Instruction, std::dynamic_extent> operations;
		std::vector<Instruction> instructions;

		Instruction& operator[](size_t i)
		{
			return instructions[i];
		}


		void Set(const RoutineBase& other)
		{
			auto hold1 = file;
			auto hold2 = name;
			operator=(other);
			file = hold1;
			name = hold2;

		}
		uint32_t paramCapacity = 0;
		uint32_t varCapacity = 0;		//The maximum amount of variables to allocate for
		uint32_t argCapacity = 0;		//The maximum amount of arguments the stack will ever need to allocate.
		RoutineFlag flags = RoutineFlag::None;


		bool ForwardsVaradic() const
		{
			return flags & RoutineFlag::ForwardsVariadic;
		}

		//Add numbers here
		size_t GetParamCapacity() const { return paramCapacity; }
		size_t GetVarCapacity(uint32_t vard) const { return  paramCapacity + varCapacity + vard; }
		size_t GetArgCapacity(uint32_t vard) const { return argCapacity + (ForwardsVaradic() ? vard : 0); }


		size_t GetInstructCapacity() const { return instructions.size(); }



	};

}