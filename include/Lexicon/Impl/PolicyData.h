#pragma once


namespace LEX
{
	class ConcretePolicy;

	//Give their own.

	//Rename policy type.
	enum struct TypeEnum : uint8_t
	{
		Invalid,
		Primitive,
		Intrinsic,
		//External and custom both fall under interface
		Interface,
		Data,
		Bound,
		Signature,
	};

	enum struct DataType
	{
		Invalid,
		Class,
		Struct,
		Interface,
	};


	struct PolicyData
	{
	public:

		//Is an ITypePolicy for generic classes. Concrete can just submit without error, while Special needs to submit types (or it will just store it's specialized one)
		ITypePolicy* _extends;//Deprecated
		uint32_t _length;//It shouldn't ever get that fucking long anyhow, and if I do this I can fit the object type in there.
		//Something within should signal if this is a generic parameter



		TypeID _id;


		String _name;

		DataType _dataType = DataType::Invalid;

		//AbstractTypePolicy* extends() { return _extends; }
	};

}
