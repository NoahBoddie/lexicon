#include "Lexicon/Engine/AttributeHandler.h"

#include "Lexicon/Engine/Script.h"
#include "Lexicon/Engine/Directory.h"
#include "Lexicon/Engine/Component.h"
#include "Lexicon/Engine/SyntaxRecord.h"
#include "Lexicon/Engine/ConcreteType.h"

namespace LEX
{
    void AttributeHandler::LoadAttributes(AttributeOwner* a_this, Directory* parent, SyntaxRecord& record)
    {
        if (IsInitialized() == true) {
            return;
        }

        assert_if(record.GetView() != parse_strings::attributes) {
            //Expected attributes
            return;
        }

        _length = 0;


        std::vector<AttributePtr> temps{ };

        temps.reserve(record.size());

        for (auto& attr_name : record.children())
        {
            ITypeInfo* base_type = parent->SearchTypePath(attr_name).info;

            assert_if(!base_type) {

            }

            assert_if(base_type->IsAttribute() == false) {

            }

            ConcreteType* type = base_type->As<ConcreteType>();

            assert_if(!type) {

            }

            type->HandleAttributes();


            std::unique_ptr<Attribute> attribute = type->CreateAttribute();

            assert_if(!attribute) {

            }
            //What we want to do here is make a formula with the syntax record. We'll copy it and make it's a constructor
            // both in name and designation.

            if (attribute->Initialize(attr_name, a_this, parent->As<Script>()) == false) {
                continue;
            }

            report::trace("Adding '{}' attribute.", attribute->GetName());

            temps.push_back(std::move(attribute));
        }

        _length = temps.size();

        if (_length) {
            _data = std::unique_ptr<AttributePtr[]>{ new AttributePtr[_length] };

            std::span span{ _data.get(), _length };

            std::transform(temps.begin(), temps.end(), span.begin(), [&](AttributePtr& it)
                {
                    return std::move(it);
                });

        }

    }

}