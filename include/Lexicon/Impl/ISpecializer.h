#pragma once



namespace LEX
{
	struct ISpecializable;

	struct ISpecializer
	{
		ISpecializer(ISpecializable* tar) : _target{ tar } {}

		//I don't wanna do this but there's no dynamic cast without polymorphism so +8
		virtual ~ISpecializer() = default;

		//This should derive from IGenericParameter, and have a way to compare perhaps?

		//SpecialType GetSpecialType()
		//{
		//	return _target->GetSpecialType();
		//}

		ISpecializable* GetTarget()
		{
			return _target;
		}

	private:
		ISpecializable* _target = nullptr;

	};

}