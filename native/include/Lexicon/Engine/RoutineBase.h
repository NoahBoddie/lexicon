#pragma once

#include "Operation.h"

namespace LEX
{
	//Needs to be named RoutineData, as there is no routine class to derive from this.
	struct RoutineBase
	{
		//These are the base most elements of a routine. Though, still can't do shit by itself. Needs a callable unit to organize it.
		// Think of this as C++ like instructions, with nothing else to tell you how any of it is used.


		RoutineBase() = default;


		//Doesn't actually need to take vector could be list.
		RoutineBase(std::vector<Operation>&& op, size_t var, size_t arg) :
			operations{ op },
			varCapacity{ var },
			argCapacity{ arg }
		{

		}

		//I require a constructor with (list of Ops, var capacity, arg capacity). Required, variables constant.



		//I will go with span over vector, because this will never need to change. In creation however, a list should be prefered.
		//std::span<Operation, std::dynamic_extent> operations;
		std::vector<Operation> operations;

		Operation& operator[](size_t i)
		{
			return operations[i];
		}


		size_t varCapacity = 0;		//The maximum amount of variables to allocate for
		size_t argCapacity = 0;		//The maximum amount of arguments the stack will ever need to allocate.



		size_t GetVarCapacity() const { return varCapacity; }
		size_t GetArgCapacity() const { return argCapacity; }

		size_t GetOperativeCapacity() const { return operations.size(); }



	};

}