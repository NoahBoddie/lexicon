//This header file is used to copy the version namespace where included, as such it comes without a pragma. 
// Simply include it within the scope of a namespace to use it in that location.

//TODO: Some how make it only able to be included once per namespace. Include guards and pragmas are out.



#pragma region Declaring Version Namespaces

#define DECL_VERSION_SPACE(mc_number, mc_prev) \
			namespace _##mc_number { constexpr uintptr_t version = mc_number; namespace Previous = _##mc_prev; }



namespace Version
{
	//Undeclare this, should just set up a version namespace everyone should be using.

//This newer version will actually create a new struct so I can store the newest version within it. This will actually remove the need to declare get interface version everywhere.

		//These aren't important anymore, the current version thing solves this.

	DECL_VERSION_SPACE(1, 1);
	DECL_VERSION_SPACE(2, 1);
	DECL_VERSION_SPACE(3, 2);
	DECL_VERSION_SPACE(4, 3);
	DECL_VERSION_SPACE(5, 4);
	DECL_VERSION_SPACE(6, 5);
	DECL_VERSION_SPACE(7, 6);
	DECL_VERSION_SPACE(8, 7);
	DECL_VERSION_SPACE(9, 8);
	DECL_VERSION_SPACE(10, 9);



	//And so on and so on.
	//namespace _...
	//{}


}

#undef DECL_VERSION_SPACE

#pragma endregion
