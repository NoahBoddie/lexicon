#pragma once

#include "Lexicon/EnumPolicy.h"

#include "Lexicon/Engine/EnumInfo.h"
#include "Lexicon/Engine/ConcreteType.h"

//*src
#include "Lexicon/Formula.h"

namespace LEX
{

    struct EnumType : public ConcreteType, public EnumPolicy
    {
        ELEM_ENUM
        {
            ELEM_FLAG(Scoped, 0),
            ELEM_FLAG(OutputNumber, 1), //Can it implicitly cast into underlying
            ELEM_FLAG(InputNumber, 2),//Can have arithmetic done to it
            ELEM_FLAG(HasFlags, 3),
            //I think this will be specifically allow arithmetic with numbers.

            ELEM_NEXT,
        };

        //Parts of this need to get saved for when deriving from this.
        struct ConstructData
        {

            std::bitset<64> activeFlags = 0;
            std::bitset<64> replaceFlags = 0;

            union
            {
                intptr_t nextValue = 0;
                uintptr_t unextValue;
            };
            uint8_t nextFlag = 1;
            EntryType currentMode = EntryType::Value;



            void AddActiveFlags(uintptr_t flags)
            {
                if ((activeFlags.to_ullong() & flags) == flags) {
                    return;
                }

                activeFlags |= flags;

                replaceFlags.reset();




                bool last_active = false;

                for (int i = 63; i >= 0; i--)
                {
                    if (activeFlags[i] == true) {
                        if (!last_active) {
                            replaceFlags[i + 1] = true;
                        }

                        last_active = true;
                    }
                    else {
                        last_active = false;
                    }
                }


            }

            intptr_t SanitizeNextValue(intptr_t value)
            {
                if (value &= activeFlags.to_ullong()) {
                    value |= replaceFlags.to_ullong();
                }

                return value;
            }

        };

        using ConcreteType::ConcreteType;


        EnumFlag GetEnumFlags() const override
        {
            EnumFlag result{};

            if (GetFlags() & Flag::Scoped)
                result |= EnumFlag::Scoped;

            return result;
        }

        std::vector<IHierarchyTree*> GetPostAffixedTypes() const override 
        { 
            return { IdentityManager::instance->GetBaseByOffset("ENUM", 0)};
        }



        
        //Note, an unscoped enum makes a relationship with its script as imported. If I need a different
        // thing I'll make a custom one.

        

        std::optional<EnumEntry> GetEntryFromName(const std::string_view& name) const override
        {
            for (auto it = entries.rbegin(); it != entries.rend(); ++it) {
                auto& entry = *it;

                if (entry.name.view() == name) {
                    return entry.GetEntry();
                }
            }

            return std::nullopt;
        }

        std::optional<EnumEntry> GetEntryFromValue(intptr_t val) const override
        {
            for (auto it = entries.rbegin(); it != entries.rend(); ++it) {
                auto& entry = *it;

                bool result;
                switch (entry.type)
                {
                case EntryType::Flag:
                    result = entry.value & val;
                    break;

                case EntryType::Value:
                    result = entry.value == val;
                    break;

                }

                if (result) {
                    return entry.GetEntry();
                }
            }

            return std::nullopt;
        }

        //This should be non virtual
        //virtual String EnumToString(const Enumeration& enm) = 0;

        TypeInfo* GetType() const override
        {
            return unconst(this);
        }


        EnumInfo* FindEnumInfo(const std::string_view& name)
        {
            auto end = entries.end();

            auto it = std::find_if(entries.begin(), end, [&](EnumInfo& search) { return search.GetName() == name; });

            if (it != end) {
                return &*it;
            }

            return nullptr;
        }

        void AddEnumInfo(EnumInfo&& info)
        {
            auto it = std::upper_bound(entries.begin(), entries.end(), info, [](auto& lhs, auto& rhs) {return lhs < rhs; });
            entries.insert(it, std::move(info));
        }


        //I don't know what the syntax of these will look like, nor their stipulations.
        void CreateEntry(SyntaxRecord& record, ConstructData& data)
        {
            if (FindEnumInfo(record.GetView())) {
                //Report? cancel?
                return;
            }

            EnumInfo entry{};
            //I can possibly steal if this isn't marked to be incremental

            entry.name = record.GetTag();


            auto type = record.FindChild(parse_strings::enum_type);


            switch (Hash(type->GetFront().GetView()))
            {
            case "enum"_h:
                entry.type = EntryType::Value;
                break;
            case "flag"_h:
                entry.type = EntryType::Flag;
                break;
            case "auto"_h:
                entry.type = data.currentMode;
                break;
            }

            if (record.FindChild("temp") != nullptr) {
                data.currentMode = entry.type;
            }

            if (auto expression = record.FindChild(parse_strings::def_expression)) {
                entry.value = Formula<intptr_t>::Run(entry.name, expression->GetFront());
            }
            else {
                switch (entry.type)
                {
                case EntryType::Value:
                    entry.value = data.nextValue;
                    break;
                case EntryType::Flag:
                    entry.value = (unsigned long long)1 << std::bit_width(data.unextValue);
                    break;
                }
            }

            if (entry.type == EntryType::Flag) {
                GetFlags() |= Flag::HasFlags;
                data.AddActiveFlags(entry.value);
            }



            if (record.FindChild("alias") == nullptr) {
                switch (entry.type)
                {
                case EntryType::Value:
                    data.nextValue = data.SanitizeNextValue(entry.value + 1);
                    break;
                case EntryType::Flag:
                    data.unextValue = (unsigned long long)1 << std::bit_width(data.unextValue);
                    break;
                }
            }

            entry.SetConstant(this);

            AddEnumInfo(std::move(entry));
        }

        void CreateHardCodedEntries(ConstructData& data)
        {
            //We want to make default if it doesn't exist

            //All Flags if it exists.
            //And finally, total
            
            
            //I don't actually think this needs to be created
            if constexpr(0)
            if (FindEnumInfo("Default") == nullptr)
            {
                EnumInfo entry{};
                entry.name = "Default";
                entry.type = EntryType::Hardcoded;
                entry.value = 0;
                entry.SetConstant(this);
                AddEnumInfo(std::move(entry));
            }


            if (auto flags = data.activeFlags.to_ullong())
            {
                EnumInfo entry{};
                entry.name = "FlagAll";
                entry.type = EntryType::Hardcoded;
                entry.value = flags;
                entry.SetConstant(this);
                AddEnumInfo(std::move(entry));
            }

            {
                EnumInfo entry{};
                entry.name = "Total";
                entry.type = EntryType::Hardcoded;
                entry.value = data.nextValue;
                entry.SetConstant(this);
                AddEnumInfo(std::move(entry));
            }
        }


        //This is to be for the loading stage, and it will load various flags.
        LinkResult OnLink(LinkFlag flags) override
        {
            SyntaxRecord& ast = *GetSyntaxTree();

            switch (flags)
            {
            case LinkFlag::Loaded:
                if constexpr (1)
                {

                    SyntaxRecord& settings = *ast.FindChild(parse_strings::settings);


                    

                    bool output_number;
                    bool input_number;

                    if (SyntaxRecord* enum_type = settings.FindChild(parse_strings::enum_type)) {
                        switch (Hash(enum_type->GetView()))
                        {
                        case "class"_h:
                            output_number = false;
                            input_number = false;
                            break;


                        case "struct"_h:
                            output_number = true;
                            input_number = false;
                            break;
                        }
                    }
                    else {
                        output_number = true;
                        input_number = true;
                    }


                    if (output_number) {
                        GetFlags() |= Flag::OutputNumber;
                    }
                    
                    if (input_number) {
                        GetFlags() |= Flag::InputNumber;
                    }

                    if (settings.FindChild("scoped") != nullptr) {
                        GetFlags() |= Flag::Scoped;
                    }

                }
                break;

            case LinkFlag::Constant:
                if constexpr (1)
                {
                    ConstructData data;

                    if (auto entries = ast.FindChild(parse_strings::entries)) {

                        for (SyntaxRecord& node : entries->children())
                        {
                            get_switch(node.SYNTAX().type)
                            {
                            case SyntaxType::EnumEntry:
                                CreateEntry(node, data);
                                break;

                            case SyntaxType::Function:
                            default:
                                report::compile::critical("Syntax {} not valid for environment", magic_enum::enum_name(switch_value)); break;
                            }
                        }

                    }

                    CreateHardCodedEntries(data);
                }
                break;
            }


            return ConcreteType::OnLink(flags);
        }

        //Id like to store a value that can be used to find what the next value is,
        // as well as what flags are taken (in the case of active flags and replace flags.
        // I'll store them as entries that can be looked for

        //std::bitset<64> activeFlags = 0;
        //std::bitset<64> replaceFlags = 0;


        //intptr_t nextValue = 0;
        //uint8_t nextFlag = 1;


        //This needs to reserve size before hand, knowing all the entries it's going to make.
        // Flags go first
        std::vector<EnumInfo> entries;

    };


}