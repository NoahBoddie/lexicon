#pragma once

namespace LEX
{
	


	struct Overload
	{
		using _Self = Overload;
		static constexpr size_t incompatible = -1;
		static constexpr size_t match = 0;

		enum ResultEnum
		{
			Self,		//Self par is closer to zero
			Other,		//Other par is closer to zero
			Ambiguous,	//Ambiguious overload with other, by hash or par

		};


		void Hash(size_t h)
		{
			hash = h;
		}

		Overload& Unmatch()
		{
			par = incompatible;
			return *this;
		}

		bool IsCompatible() const
		{
			return par != incompatible;
		}


		_Self& operator+= (const _Self& rhs)
		{
			
			if (IsCompatible() == true) {
			
				hash += rhs.hash;//TODO: This handling of overload::hash is just place holder, as I can't be bothered rn and inheritance doesn't exist.
				par += rhs.par;
				
			}

			return *this;
		}


		_Self& operator+= (size_t rhs)
		{
			if (IsCompatible() == true) {
				par += rhs;
			}

			return *this;
		}

		_Self& operator= (size_t rhs)
		{
			if (IsCompatible() == true) {
				par = rhs;
			}

			return *this;
		}

		operator size_t() const
		{
			return par;
		}


		operator bool() const
		{
			return IsCompatible();
		}

		

		static Overload Failure(int = 0)
		{
			//TODO: Vary the types of failure by using the hash so I can tell what kind of failure it is, similar to that of  NaN.
			//To do this, I can make the hash alternatively carry a code to an error within itself.

			//For each failure and where it came from, there will be a list presented if no overload matches, telling all why each overload failed.
			return Overload{}.Unmatch();
		}

		constexpr ResultEnum Compare(_Self rhs) const
		{
			if (hash == rhs.hash)
			{
				if (par < rhs.par)
					return ResultEnum::Self;

				if (par > rhs.par)
					return ResultEnum::Other;
			}

			return ResultEnum::Ambiguous;
		}

		

		//For easy manipulation, I'd like it to be that you can increment this with numbers. No negatives, 
		// no decreasing

		size_t hash = 0;
		size_t par = 0;



	};
}
