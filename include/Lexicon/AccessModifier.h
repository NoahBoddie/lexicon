#pragma once

#include "DeclareSpecifier.h"

namespace LEX
{
	//Is access specifier.

	ENUM(Access, uint8_t)
	{//The base struct that handles this might be later named "ClassicInfos". Maybe. Will decide if it sounds dumb.
		None,		//No access is given at this point.
		Private,	//Only a given scope can observe it
		Protected,	//Only the scope and derived scopes can observe it (derived data types or required/imported/included scripts). Means different things between data type and 
		//				script block usage.
		//ProtectedInternal,//extends internal to derived scripts. In a class functions much like internal.
		Public,

		Internal = 1 << 7,	//Only the script can observe it. Is a flag rather than another thing
		

		NoneInternal = Access::Internal,
		PrivateInternal = Access::Private | Access::Internal,
		ProtectedInternal = Access::Protected | Access::Internal,
		PublicInternal = Access::Public | Access::Internal,
			
			
		//Protected		Only the scope and derived scopes can observe it (derived data types or required/imported/included scripts). Means different things between data type and 
		
	};


	namespace Misc
	{
		inline Access DeclareToAccess(DeclareSpecifier spec)
		{
			spec &= DeclareSpecifier::Access;

			switch (spec)
			{
			case DeclareSpecifier::Private:
				return Access::Private;

			case DeclareSpecifier::Protected:
				return Access::Protected;

			case DeclareSpecifier::Public:
				return Access::Public;

			case DeclareSpecifier::InternalPrivate:
				return Access::PrivateInternal;

			case DeclareSpecifier::InteralProtected:
				return Access::ProtectedInternal;

			case DeclareSpecifier::InternalPublic:
				return Access::PublicInternal;

			default:
				return Access::None;
			}
		}
	}
}
