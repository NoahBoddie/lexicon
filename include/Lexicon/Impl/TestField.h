#pragma once

//#include "Object.h"
//#include "ObjectID.h"
//#include "ObjectIDMapper.h"

#include "Lexicon/Impl/Operand.h"

#include "Lexicon/Impl/Prototype.h"
#include "Lexicon/Impl/Construct.h"
#include "Lexicon/Impl/Hierarchy.h"
#include "Lexicon/Impl/Script.h"


#include "Lexicon/Impl/Exception.h"
//#include "Parser.h"
#include "Lexicon/Impl/ParserTest.h"


#include "Compiler.h"



//This is a test space for this. I'd like to move it to a more fitting location.

namespace LEX
{
//#include "API_Lexicon.h"




	//What a parse visitor used to take.
	// Things I agree with.
	// Send the parser with it, this way it can get new information, or send the token stream
	// Send the record its going to maybe? Unsure on this one. Actually, the more I think about it, doesn't
	//  really
	//ParseQueryResult(*)(Parser*, Record*, DataCollection);


	enum struct ParseType
	{
		Atomic, 
		Nested
	};

	//This is how simple it needs to be. Parsers don't need to know where something is coming from, it only needs to know if something makes no fucking
	// since, like having a keyword after an identifier. Because that would be an expectation. Identifier + identifer is a type declaration. If the next
	// is a keyword then you know.
	//using ParseFunc = Record(Parser*, Record*);
	//Try this one instead

	//ParseModule is the name of the class that's used to parse shit. But should it be used for more than just parsing? It seems like it should fit with compiling
	// but I think I'll handle such concepts later.
	
	
	/*
	struct ParseModule : public ProcessContext
	{
		//I don't think I'll actually need a parse module, maybe just some manually arranged functions
		virtual ModuleType GetType() = 0;

		virtual uint32_t GetDepth() const { return 0; }
		
		bool IsAtomic() const { return GetDepth() == 0; }

		virtual bool CanHandle(Parser* parser, Record* target) const = 0;

		virtual Record HandleToken(Parser* parser, Record* target) = 0;
		
		//This is what should be used if one wishes to access another parse module
		// also instead of requiring it derives from this, require that it derive from a singleton class for parse modules.
		template <std::derived_from<ParseModule> Module>
		static Record HandleToken(Parser* parser, Record* target)
		{
			ParseModule* mdl = Module::GetSingleton();

			if (!mdl) {
				parser->GetInput()->croak("something about static handle token.");
			}

			//Might have something about process chains right here.
			//Needs to use the specific version of execute module
			return mdl->HandleToken(parser, target);
		}
	};
	//*/






	//This is an example of what is to be put in registers, parameter arguments and such.
	// The idea is basically there are 3 types of variables.
	//Variables that are captured from one runtime to another,
	// or Variables that are from another function runtime
	struct Variable_;
	
	using VarRuntime = std::variant<
		Variable_,
		Variable_&//,
		//std::unique_ptr<Variable_>
	>;
	



	

	enum struct ReserveType
	{
		None,
		Function,
		Property,
		Other,		//with other you can make your own format string as to why, otherwise there's a prebuilt reason for each.

	};
	//You use the above along with a reservation type to reserve the use of a name at a particular place. This will be attached to the environment.
	// basically it will prevent you from 


	struct ReserveNode{};

	//I reckon to dare this is where 
	enum struct Statement
	{
		TopLevel,	//The top level, where most things can go
		Function,	//the declaration of a function obvs
		IfBounds,	//Within the bounds of an if statements paras, if that's ever relevant to me.
		//Switch,	//Never going to do these, but stands as the point of some statement that would require extra noting.
		Line,		//The moment a non-one of the above happens, it's a line.
		Assign,		//THIS is what subroutines most often are. Simple single line evaluations that go to one place.

		//YOu should also be able to start writing from one of these statements, like within a line or function, both of things lexicon may be needed for.
	};

	//Contexts
	// Script-top level
	//  struct
	//   member
	//   constructor
	//   method
	//  function
	//   line


	//Notes for parsing some things
	// There's a parser function for each and every thing to parse of the [ExpressionType?], however there's specific compiling for each operator.
	// how the above should work would likely be very similar to that of how Expression works (pulling all classes via type).


	//To begin compiling the information, I should be addressing the Expression, maybe some of the RoutineCompiler
	//So I'm thinking like, the compil




	//So for pointer specification this is the set up I'm going to take, but the major difference is and will be that
	// the test base is the thing that will be specialized, and not the upper part. This will allow me to throw
	// errors about what's expected where better using concepts (I hope). 
	namespace MoreTesting
	{
		struct test_base {};

		template <class T>
		class testing : public test_base
		{

		};

		template <>
		class testing<int>
		{

		};
	}
	//How this sort of thing will then likely work is akin to a type trait, with a singular type alias
	// handling all of the grunt work.

	//A note also is that I can use this kind of specialization on functions too, so long as they share a return type,
	// so I can just use that rather than an out and out class getting the object code of something


	

}

//TODO: Most if not all todos past this point are invalid



//This shouldn't be deleted, has some valuable things within.
/*
namespace TestField__Types
{

	//Just a note about parsing now, I think I want to move away from toml, for a bit, but have an area specifically for toml, like once it's done, it's done.
	//I additionally want some stuff that can signify special roles, like

	//#use DEBUG
	//or
	//#use CASE SENSETIVE
	//Another one,
	//#use return <Type>, this will set the default type for functions in the project. So making something just 
	// function(){} in someting like AVG will automatically be a float. In other projects that are more about doing, it will likely be void.
	//#use this <Type> could be another idea, setting the default call from function, which is normally determined via other methods.

	//Lastly, a thing that I could do, is have certain sections of code only loaded if another file is detected


	struct Void {};

	//Just a fucking example
	using ObjectVar = std::variant<Void, void*, int*, char*>;

	//TODO:struct is called Construct
	struct Struct;


	//This is generally the type of setup I'm looking to have.
#define TYPES_MACRO void*
#define HEAD_TYPES_MACRO 1
	//This might not work though, because here's something possibly stupid. What if someone has 2 different APIs like ToolUI set up, 
	// and actor value generator both.
	using TruerObjectVar = std::variant<
		Void,
		TYPES_MACRO,
		Struct			//No matter where this index is, the outputted region should always be 7F if regular, and FF ir array.
	>;


	//I think it would possibly be for the best that the variant is exposed as something that can be mallible as a template, but the hard cast type when working
	// in it from this side. 
	//The idea would be to allow me to have the API version of API to apply different types (which you'd juse like AVG::Object or TUI::Object).

	//I can probably do something like this, and have which one makes it as the "defacto" type macro. So it will check if it's own source is enabled
	// and if not, it doesn't set a "core". Most importantly, if no LEX source is used, it's not relevant.

	//So something like this

	template <class... Ts>
	using APIObject = int;


	//This goes in lexicon, given it's  macros and the idea of types macro is made there first (designed to be overwritten or perhaps pushed)
#define TYPES() Void, TYPES_MACRO, Struct
#define HEAD_TYPES() Void, HEAD_TYPES_MACRO, Struct
	//This goes in XXX
#define TYPES_MACRO std::string, int, void*, float
	//This too goes in XXX, which looks for it's own source value.
#ifndef IS_XXX_SOURCE
	//HEAD_TYPES is what the actual LEX::Object class uses to populate it's values.
#define HEAD_TYPES_MACRO TYPES_MACRO
#endif

	namespace XXX
	{

		//Using this version of object is vastly different than using the actual version of object. What this version will do is send you
		// an index and a void pointer. The point being that depending on what API version you're requesting from type you're requesting from will cause it to be different.
		// additionally, to tell the difference between which API is supposed to be gotten, I think it would possibly be best to seperate uses of the API source by
		// templates or something. Because only one API use wins, and getting and storing the location of them all might be a pain.
		//So make it so
		using Object = APIObject<TYPES()>;
	}


	//The more and more I see this, the more I think it might be prudent to make lexicon redistributables. These redistributables would store the main "OBJECT"
	// and a lot of the virtual machine related behaviour.
	//Then the individual plugins can have their own implementations of Object

	//Alright, so that idea is fucking insane, instead, include the API inside of a namespace. DO NOT USE RE namespace, that shit will fuck some stuff up,
	// but other than that, this will allow us to include the same API twice. As long as you gate it with a namespace. The large part is, don't use the API directly
	// I'll need to convey that.

	//Also, important note. No pragma once, so I can include it in the namespace. To take up some less space, I can make some common API headers
	// that contain my ABIContainer and such.

	//If someone to use it directly, should make something for that.
	
#undef TYPES_MACRO



	






	//A specialization like this is required when
	// LookingUp an object to which a lookup can be invalid
	// when getting the LEX::ObjectID of a given object
	//
	template<uint8_t N> struct speck
	{
		virtual LEX::ObjectID GetType(ObjectVar& object)
		{
			return {};
		}
	};




	template<> struct speck<0> {
		//Speck zero is invalid

		virtual LEX::ObjectID GetType(ObjectVar& object)
		{
			//Report that there's no valid specialization of the type here.
			return {};
		}
	};

	//needs a class above it.
	//Also, may make a macro for this, just so all the classes conform properly.
	//Main reason for the macro, is it would be nice to just use the type to get the number. I guess all this is fine. For now.
	template<> struct speck<1>
	{
		//using DefinedClass = T;

		//Should return only the 7 bits that matter.
		LEX::ObjectID operator()(ObjectVar& object)
		{
			return GetType(object);
		}

		LEX::ObjectID GetType(ObjectVar& object)
		{
			//Report that there's no valid specialization of the type here.
			return {};
		}
	};
S
	template <uint8_t I>
	struct speck_user : public speck<I>
	{
		//Should return only the 7 bits that matter.
		static LEX::ObjectID TESTType(ObjectVar& object)
		{
			speck<I> speck;

			return speck(object);

			return GetType(object);
		}
	};
	



	namespace RE
	{
		using TESForm = int;
	}



	template <int I, int Max>
	LEX::ObjectID _GetType(ObjectVar& obj, uint8_t index)
	{
		if (index != I)
		{

			if constexpr (I != Max)
				return _GetType<I + 1, Max>(obj, index);

			return LEX::ObjectID::Invalid();
		}

		using ClassType = speck<I>;

		speck<uint8_t(I)> speck;

		return speck.GetType(object);
	}

	LEX::ObjectID _GetType(ObjectVar obj)
	{
		uint8_t index = obj.index();

		if (!index)
			return LEX::ObjectID::Invalid();

		if (index == structIndex)
		{
			//Get obj as a struct
			//Get prototype
			//Get Type ID
			return LEX::ObjectID::Invalid();
		}

		
		//Depending on what the index is, I can give it certain starting points. Kinda shortens the searching I guess.

		if (index < 128)
			return _GetType<0, 127>(obj, index);
		else
			return _GetType<128, 255>(obj, index);
	}

	//So the idea with object type ids

	//Having conversion functions might be interesting.
	// Like say for forms and strings. Explicitly used when 

	
	
}
//*/

//This is the old version, I'm gonna just be playing it off the guide for now.
/*
namespace LEX
{
	//The toml handler will have a function map, that handles different types.
	//Of it, it handles 2 sorts things on 2 different inputs. The name, or the member called
	// type.

	//It can probably store string hashes, as nothing about this needs to be stored specifically.
	// 

	



	enum struct ParseQueryResult
	{
		No,
		Yes,
		Cancel
	};


	struct DataCollection
	{
		RecordDataIterator& it;
		RecordDataIterator begin;
		RecordDataIterator last;
		RecordDataIterator end;


		DataCollection(RecordDataIterator& a_it, RecordDataIterator a_end) :
			it(a_it),
			begin(a_it),
			end(a_end)
		{}

		DataCollection(RecordDataIterator& a_it, RecordDataIterator a_begin, RecordDataIterator a_last,  RecordDataIterator a_end) :
			it(a_it),
			begin(a_begin),
			last(a_last),
			end(a_end)
		{}
	};



	struct Parser;

	//TODO:Extend functionality of this a bit, I'd like this to keep record of the syntax's that parse this.
	// For example what if an '=' is detected where arguments go? Syntax should be able to beget syntax, but it should know where
	// such things came from.
	using ParseVisitor = ParseQueryResult(*)(Parser*, Record*, DataCollection);

	ENUM(ParseType)
	{
		Keyword,
		Syntax,
		Error,
		Total
	};

	struct Syntax
	{
		struct Sort
		{
			constexpr bool operator()(const Syntax& lhs, const Syntax& rhs) const
			{
				if (lhs.type != rhs.type)
					return lhs.type < rhs.type;

				return lhs.GetPriority() < rhs.GetPriority();
			}

		};

		ParseVisitor _func;

		std::string_view category;

		bool invert = false;//If invert it will only work if the choice category is not within.
		
		//Invert should be the sign bit. Make getting priority a function.
		int16_t _priority = 0;

		int16_t GetPriority() const
		{
			constexpr int16_t sign_bit = ~(1 << 15);
			return _priority & sign_bit;
		}

		bool IsInverted() const
		{
			return _priority & (1 << 15);
		}

		ParseType type = ParseType::Syntax;

		//ParseQueryResult operator()(Parser* parser, Record* record, DataCollection collection)
		//{
		//	_func(parser, record, collection);
		//}
	};

	

	//Entries are sorted by type, and if equal, by priority
	//TODO: Rename Syntax. The operator handler is the actual syntax. This is just the tokenizer?
	std::set<Syntax, Syntax::Sort> syntaxList;
	

	//Known 



	struct Parser
	{
		//virtual ~Parser() = default;//Required why?

		//Note, for memory, adding a character at the front might be a good idea to differentiate.
		std::list<std::string> memory;
		
		//If the context of a syntax is not found within the context of the parser, it will not be evaluated to run.
		// A possible setting would be the inversion of this.
		std::multiset<std::string_view> context;
		//Expandable context might be a good idea. Like to say, we are in a function right now. And have it seperate so it doesn't clash or something.
		//std::multiset<std::string_view> expandingContext;
		Record* _target;


		//TODO: Make data collection struct a function instead of a parameter. Not all types will need the set up. After, just have gets for each one. Status: Easy
		// Additionally, it can be something within collection that does it, lastly, collection can be a template class.

		RecordDataIterator it;	//The current iterator
		RecordDataIterator last;//The iterator parsed at the start of the last process
		RecordDataIterator begin;//Where it's started
		RecordDataIterator end;	//Where it will no longer be valid.

		const Syntax* lastSyntax = nullptr;

		Record& GetTarget()
		{
			return *_target;
		}


		//The might replace crew
		bool SyntaxMatch(const Syntax* syntax)
		{
			if (!syntax)
				return false;

			auto end = context.end();

			return std::find(context.begin(), end, syntax->category) != end;
		}
		bool SyntaxMatch(const Syntax& syntax)
		{
			return SyntaxMatch(&syntax);
		}

		void SyntaxHandle()
		{
			//Above this, needs to be a parsing function of it's own.
			for (auto& syntax : syntaxList)
			{
				if (SyntaxMatch(syntax) == false) {
					continue;
				}
				//TODO: Address, should the collection for syntax(? Or Tokenizer?) be able to push the actual parsing iterator?
				DataCollection collection{ it, begin, last, end };

				auto result = syntax._func(this, _target, collection);

				//Check validity?

				switch (result)
				{
				case ParseQueryResult::Yes://It did meet requirements. No more parsing.
					lastSyntax = &syntax;
					return;
				case ParseQueryResult::No://Means didn't match requirements.
					continue;
				case ParseQueryResult::Cancel://It met requirements but an error occured. Going to error parsing.
					//TODO: Make parsing error from cancel bail to the error section instead.
					throw ParsingError("Replace");
				}
			}
		}

		void HandleData(Record& tar, std::list<std::string> l_mem, std::multiset<std::string_view> txt)
		{
			//I'm just gonna manually visit for now. I sorta wish that I could do this in some other way.
			//TODO: suggestion, maybe I should use the actual visitor.

			//I want something to deal with this parser
			//{ DataType::Invalid, OperatorType::Invalid };
			//_lhs.second = OperatorType::Invalid;
			while (it != end) {
				auto old_it = it;

				SyntaxHandle();

				last = old_it;

				//If it didn't move, that's kinda an issue. Should this then throw?
				if (old_it == it) it++;
			}

		}

		template<class... Ts>requires(RGL::AllTrue<std::convertible_to<Ts, std::string_view>...>::value && sizeof...(Ts) > 0)
		static void Handle(Record& tar, Ts... txts)
		{

			Parser parser{};

			parser.HandleData(tar, {}, std::multiset<std::string_view>{txts...});
			
		}

		template<class... Ts>requires(RGL::AllTrue<std::convertible_to<Ts, std::string_view>...>::value && sizeof...(Ts) > 0)
		static void Handle(Record& tar, std::list<std::string>& l_mem, Ts... txts)
		{
			Parser parser{};

			parser.HandleData(tar, l_mem, std::multiset<std::string_view>{txts...});

			l_mem = parser.memory;
		}


	};


	


	//Now, I need to figure out if I want to visit all the syntax objects, how would I exactly go about doing such a thing?


	constexpr std::string_view routine_regex = "(?<Minus>- )|(?<Object><:.+(?=:>)..)|(?<Digits>[\\d\\.]+)|(?<Quotes>'[^']+.)|(?<Identifiers>(\\w|::)+)|(?<Symbols>[^\\w\\d\\s'\\.]([^\\w\\d\\s'\\.:]){0,})";
	constexpr std::string_view seq_routine_regex = "(?<Header>^[\\w;]{0,}\\s{0,}(?=;).)|(?<Minus>- )|(?<Object><:.+(?=:>)..)|(?<Digits>[\\d\\.]+)|(?<Quotes>'[^']+.)|(?<Identifiers>(\\w|::)+)|(?<Symbols>[^\\w\\d\\s'\\.]([^\\w\\d\\s'\\.:]){0,})";
	//May need revisions due to the ; possibly being caught first.
	constexpr std::string_view property_regex = "(?<Tooltip>;.+)|(?<DefValue>(?<=:)\\s{0,}\\K.[^;\n]+)|(?<Type>\\w+\\s{0,}:)|(?<RawValue>(\\d|'|<:)[^\\/\\/\\n]+)|(?<Identifier>\\w+)";

	namespace
	{
		using namespace RGL;

		void pure_test()
		{

			std::string func = "GetActorValue('SingleWielding', Damage | Permanent) - GetActorValue('SingleWielding', Damage | Permanent)";
			//std::string func = "label_1; GetActorValue(::ending = 'SingleWielding', ActorValue::Damage | ActorValue::Permanent); return result";

			RGL::Record function("Commons");
			function.CreateData("GetWieldingWeight");
			//I think I'll use hex for the code, just in case it grows that much in size. Keeps the space usage down.
			Record& code_record = function.CreateChild("@code0000");

			code_record.CreateData(func, routine_regex);
			//code_record.CreateData(func, seq_routine_regex);//This is what would be used if it was an array instead of a string.

			Record& params_required = function.CreateChild("params_required");

			////Would that this had parameters, this is what it would do.
			////This data represents the number of regular parameters
			//params_required.CreateData("1");
			//params_required.CreateChild("param1OrWhateverTheNameIs").CreateData("", property_regex);
			//Record& params_default = function.CreateChild("params_default");
			////This data represents the number of default parameters
			//params_default.CreateData("1");
			//params_default.CreateChild("param2OrWhateverTheNameIs").CreateData("", property_regex);

		}
	}


	//Should the goal be to produce a record, or produce a thing?
	// For now, nothing.
	
	void HandleRecord_Routine(std::string name, const FileNode& node)
	{
		//TODO: Todos for the toml handling function below. Status: Undetermined
		// I think I would prefer it if the file was made before we get to the file node personally.
		
		//First things this should do, make sure the object name can go somewhere, make sure it's not reserved, then make sure that that
		// the object is actually a table, and we are a go.
		auto& table = *node.as_table();
		

		Record& routine = FileManager::EmplaceFileRecord("TempSpot", name);
		//Right here in data, the first object will be the scope name of the record.


		std::string formula_code = table["formula"].value_or("0");

		std::list <std::string> memory{};

		auto param_view = table["params"];

		
		if (auto p_test = param_view.as_array(); p_test)
		{
			Record& req_params = routine.CreateChild("parameters_required");
				
			//If def params exist, that's where things must go, and if they don't that's a parsing error.
			Record* def_params = nullptr;

			auto& param_array = *p_test;
				

			for (auto& entry : param_array)
			{
				//Right here is the place for that handle multiple entries deally, BUT, this is not where it will be handling that properly.
				if (entry.is_string() == false) {
					throw ParsingError("parameters only accept string types.");
				}

				std::string str = entry.value_or("");

				//May create a static function to do this, might get messy.
				Record parameter{ str, property_regex.data() };

				

				//Visit right here.
				Parser::Handle(parameter, memory, "Function");

				if (parameter.FindData("\\w+:") != nullptr)
				{
					if (!def_params)
						def_params = &routine.CreateChild("parameters_default");

					def_params->PushBackChild(parameter);
				}
				else
				{
					if (!def_params)
						req_params.PushBackChild(parameter);
					else
						//ROMBUR: Needs to submit to error names.
						throw ParsingError("non-default declared after default params in... something");
				}
			}

		}

		if (table["default"])
		{
			//This does different things based on what type this is, so I'm gonna put this to the side. For now.
			//formula->defaultValue = table["default"].value_or(0);
		}
			
		//Code is stored in a record called code_body. This can range from having only one entry. The group record will hold the needed
		// data to tell whether it's a single line or sequence, with such data being held in it's tag as a boolean.

		//TODO: Parser needs to force the differences between native and non-native code.
		if (auto code = table["formula"]; code)
		{
			Record& body = routine.CreateChild("code_body");
			
			std::function list_handle = [&](toml::array list)
			{
				body.CreateData("+");
				
				std::vector<std::string> code_list{ list.size() };

				std::transform(list.begin(), list.end(), code_list.begin(),
					[](toml::node& entry) 
					{ 
						if (entry.is_string() == false)
							throw ParsingError("Code body within array must only use toml strings.");
						//Could do this another way?
						return entry.as_string()->get();
					}
				);

				body.CreateChildren("code_", code_list, seq_routine_regex.data());
					
				//body.GetChildFirst()->GetTag() = "code_entry";
				body.GetChildBack()->GetTag() = "code_return";//Only return needs to be named like this.

				//visit children right here.
				//Keeping in case.
				//for (auto& entry : list)
				//{
				//	//Right here is the place for that handle multiple entries deally, BUT, this is not where it will be handling that properly.
				//	if (entry.is_string() == false) {
				//		throw ParsingError("Code body within array must only use toml strings.");
				//	}
				//	//Need a function to do this specically.
				//		
				//	body.PushBackChild(Record{ entry.value_or(""), seq_routine_regex.data() });
				//}
			};

			std::function string_handle = [&](std::string str)
			{
				body.CreateData("-");
				//Name doesn't matter here.
				body.CreateChildWithData("", str, seq_routine_regex.data());
			};

			HandleNodeView("No name currently", code,
				list_handle,
				string_handle
			);
			//Why this is like this, I have no idea. I
			auto test = [&](Record* visitee, RecordIterator& iterator, RecordIterator end)
			{
				//It's notable that right here, this is where something would use memory
				Parser::Handle(*iterator, memory, "Function");
			};

			std::function func = test;
			GenericVisitor c_body_visitor{ func, nullptr };

			body.VisitChildren(&c_body_visitor);
		}
			
		//auto* flag_array = table["flags"].as_array();
		//if (flag_array)
		//{
		//	auto result = std::find_if(flag_array->cbegin(), flag_array->cend(), [](auto& it) {return it.value_or("") == "native"; });
		//	if (flag_array->end() == result)
		//		formula = CreateFormula(parameters);
		//}


		//End of the record.

		//At this point, the toml has been processed, now what needs to happen is the creation of an object that can handle a record.
		//This type needs to be able to be given a record, and construct itself from the data given. So for example.

		//auto routine = new Coroutine();
		//routine->HandleRecords(routine);

		//And then that would be the end of it.


		
	}
}
//*/

//Disabled for now.
//#undef TOKEN
//#undef EXPRESSION