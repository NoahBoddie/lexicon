#pragma once 


namespace LEX
{
	struct ITypePolicy;

	enum struct FieldType
	{
		Invalid,
		Global,
		Local,
		Param,
		Variable,
		Member,
		Function,//Doesn't differentiate between method or function
	};

	
	struct Field
	{
	public:
		~Field() = default;

		virtual FieldType GetFieldType() const = 0;
		
		virtual size_t GetFieldIndex() const = 0;
		
		virtual ITypePolicy* GetTypePolicy() const { return nullptr; }

	};

}