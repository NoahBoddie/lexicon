#pragma once

#include "Lexicon/ITemplateBody.h"

namespace LEX
{

	struct ITemplateBodyPart : public ITemplateBody
	{
		enum State : int8_t
		{
			kUnknown = -1,
			kPart,
			kBody,
		};

		virtual bool IsResolved() const = 0;

		State GetState() const
		{
			if (_state == kUnknown)
			{
				if (IsResolved() == true)
					_state = kBody;
				else
					_state = kPart;
			}

			return _state;
		}

		void ResetState() const
		{
			_state = kUnknown;
			GetState();
		}

		ITemplateBody* TryPromoteTemplate() override
		{
			return GetState() ? this : nullptr;
		}

	private:
		mutable State _state = State::kUnknown;
	};

}