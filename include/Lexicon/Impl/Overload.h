#pragma once

namespace LEX
{
	


	struct Overload
	{
		using _Self = Overload;
		static constexpr size_t incompatible = max_value<size_t>;
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

		void Unmatch()
		{
			par = incompatible;
		}

		bool IsCompatible() const
		{
			return par != incompatible;
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