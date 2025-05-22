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


		constexpr ITemplateBodyPart() = default;
		constexpr ITemplateBodyPart(State state) : _state{ state } {}

		virtual bool IsResolved() const = 0;

		virtual bool ShouldUpdateState() const
		{
			return _state == kUnknown;
		}

		State GetState() const
		{
			if (ShouldUpdateState() == true)
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
		}

		ITemplateBody* TryResolve() override
		{
			return GetState() ? this : nullptr;
		}

		ITemplatePart* AsPart()
		{
			return this;
		}

		const ITemplatePart* AsPart() const
		{
			return this;
		}


		operator ITemplatePart* ()
		{
			return this;
		}

		operator const ITemplatePart* () const
		{
			return this;
		}

	protected:
		mutable State _state = State::kUnknown;
	};

	//DEPRECATED, can't use previous stuff, so it doesn't seem worth it. Maybe if I could close off the body instead
	struct ITemplatePrivateBodyPart : protected ITemplateBodyPart
	{

		using ITemplateBodyPart::ITemplateBodyPart;

		ITemplatePart* AsPart()
		{
			return this;
		}

		const ITemplatePart* AsPart() const
		{
			return this;
		}


		operator ITemplatePart* ()
		{
			return this;
		}

		operator const ITemplatePart* () const
		{
			return this;
		}

	};

}