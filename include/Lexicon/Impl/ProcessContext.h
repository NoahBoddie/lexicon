#pragma once



namespace LEX::Impl
{

	class ProcessChain;
	struct IProcess;
	//A virtual class for Parser that helps convert a process into
	

	struct ProcessContext
	{
		
		//May seperate context from name.
		virtual std::string_view GetContext() = 0;

		virtual bool HasKeyword(std::string_view type);
		

		//Idea here is I can have a context respond to if it wants to be able to have a certain thing handle it. if it's damning enough,
		// I can send an error.
		virtual bool ContextAllowed(ProcessContext* context, ProcessChain* link);


		//Want to add a function called flags, this will be whats used for Statements and shit instead.

		//bool IsContext(std::string_view type)

		bool IsContextType(std::string_view type);
	};
	//TODO:Consider using the parsing context for compiling too.



	struct ProcessChain
	{
		ProcessChain() = default;

		ProcessChain(ProcessContext* cur, ProcessChain* prev, IProcess* pro) : current{ cur }, previous{ prev }, process{ pro } {}


		//probably wont even use this.
		bool IsContextType(std::string_view type);

		bool HasContext(ProcessContext* context);



		ProcessChain InheritChain(ProcessContext* cur, ProcessChain* prev);


		bool HasKeyword(std::string_view type);


		ProcessContext* current = nullptr;
		ProcessChain* previous = nullptr;

		IProcess* process = nullptr;

	};


	struct IProcess
	{
		template <std::derived_from<IProcess> T>
		T* As()
		{
			return dynamic_cast<T*>(this);
		}

		ProcessChain CreateChain();

		//Made virtual so dynamic cast could be used
		virtual ~IProcess() = default;

	};
}