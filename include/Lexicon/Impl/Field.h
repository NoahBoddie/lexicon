#pragma once 


namespace LEX
{

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

	};

}