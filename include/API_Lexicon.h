

namespace LEX
{

	namespace Impl
	{
		struct ArgumentType
		{
			enum EnumType
			{
				Void,			//Invalid or empty argument. Cannot get any value.		Type: None
				Other,			//A value that cannot be interpreted via API.			Type: None
				String,			//A string												Type: std::string_view/std::string
				Number,			//A float/double depending on source project			Type: float/double
				Integer,		//A 32/64 bit integer									Type: int32_t(int)/int64_t long int
				Boolean,
				Object,			//Object type designated by Lexicon's source project.	Type: LEX::Object
				Any,			//I haven't a clue, how is this represented again? A variant?
				StringArray,
				NumberArray,
				IntegerArray,
				BooleanArray,
				ObjectArray,
				AnyArray,
				Total,		//Similarly inva
			};
		};
	}

	using ArgumentType = Impl::ArgumentType::EnumType;
}


//At the end, the macro that is used to say which dll it belongs to, and who it is to search for,
// I'd like to unload it here. This way, I enforce someone specifying the macro. AND then additionally, I can have
// a value to be created to act as a sort of include guard for someone making a name already.

//Another thing that I can do is an include guard on most of the defined interface, preventing it from loading
// if the interface has already been loaded once. Or just somehow enforce what namespace one is in.