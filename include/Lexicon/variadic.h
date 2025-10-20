#pragma once

#include "Lexicon/VariableType.h"
#include "Lexicon/Unvariable.h"
#include "Lexicon/ProxyGuide.h"

namespace LEX
{

    //I need a version of unvariable that also gives the number of remaining entries left, so I can reduce from there.
    //Additionally, I need something that can help represent something like a vector of references. What gets stored obviously won't be
    // but what gets pulled should be something like a span

    template <typename T>
    struct variadic : public std::span<std::remove_reference_t<T>>
    {
        using element_type = std::remove_reference_t<T>;
        using container = std::vector<element_type>;
        using base = std::span<std::remove_reference_t<T>>;
        using base::span;
        using base::operator=;
    };

    template <typename T>
    struct Unvariable<variadic<T>>
    {
        variadic<T>::container operator()(Variable*& var, size_t i)
        {
            using container = variadic<T>::container;
            using element_type = variadic<T>::element_type;

            container result;

            result.reserve(i);

            Variable** list = std::addressof(var);

            for (; i; i--, list++)
            {
                result.push_back(Unvariable<element_type>{}(*list));
            }

            return result;
        }
    };



    template<typename T>
    struct ProxyGuide <variadic<T>>// : public RefCollection
    {

        TypeInfo* VariableType(const variadic<T>* vec)
        {
            return GetVariableType<T>(nullptr);
        }
        /*
        Array ObjectTranslator(const std::vector<T>& obj)
        {
            std::vector<Variable> buff;
            buff.reserve(obj.size());
            //const std::vector<void*> test;

            //void* other = test[1];


            std::transform(obj.begin(), obj.end(), std::back_inserter(buff), [&](auto it) {return it; });


            return Array{ buff };

        }






        //TODO:Unboiler plate revariable pls, k thx
        void Revariable(const std::vector<T>& arg, Variable* var)
        {
            Array& array = var->AsObject().get<Array>();

            //We are making some assumptions here, and doing no checks

            if (auto size = array._container->size(); arg.size() != size) {
                report::runtime::error("const array's size was adjusted erroneously.");
            }

            auto data = array.data();

            for (size_t i = 0; i < arg.size(); i++)
            {
                auto& entry = arg[i];
                auto& to = data[i];

                LEX::Revariable<const T> revar;

                revar(entry, to.ptr());

                Collect(std::addressof(entry), to.ptr());

                TryToCollect(revar);
            }
        }



        void Revariable(std::vector<T>& arg, Variable* var)
        {
            Array& array = var->AsObject().get<Array>();

            //We are making some assumptions here, and doing no checks

            if (auto size = array._container->size(); arg.size() != size) {
                array._container->resize(size);
            }

            auto data = array.data();

            for (size_t i = 0; i < arg.size(); i++)
            {
                auto& entry = arg[i];
                auto& to = data[i];

                LEX::Revariable<T> revar;

                revar(entry, to.ptr());

                Collect(std::addressof(entry), to.ptr());

                TryToCollect(revar);
            }
        }
        //*/

    };



}