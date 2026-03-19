#pragma once

namespace LEX
{

    //Shared singleton is init'd both by meyers singleton, and also static initialization, attempting to
    // be first in either.

    //A singleton that manages other singletons, maintaining their lifetime until they're no longer required
    struct SingletonManager
    {
        struct IEntry
        {
            virtual ~IEntry() noexcept = default;
        };



        template <typename T>
        struct BasicEntry : public IEntry
        {
            T value;

            template <typename...Args>requires(requires (Args... args) { T{ std::forward<Args>(args)... }; })
                BasicEntry(Args... args) : value{ std::forward<Args>(args)... }
            {
            }

        };

        struct DestructEntry : public IEntry
        {
            ~DestructEntry() override
            {
                if (dtor) {
                    dtor();
                }
            }


            void(*dtor)() = nullptr;

        };


        static void ModRefCount(bool inc)
        {
            GetSingleton()->ModRefCount(inc, false);
        }

        static SingletonManager* GetSingleton()
        {
            auto test = GetSingleton;
            static SingletonManager singleton{};
            return &singleton;

        }




        inline static SingletonManager* singleton = GetSingleton();



        void Place(std::unique_ptr<IEntry>&& dtor)
        {
            entries->push_back(std::move(dtor));
        }


        void TryDestroy()
        {
            if (!refCount) {
                if (destructed) {
                    //logger::debug("successfully destructed ObjectPolicyList");
                }

                for (size_t i = entries->size(); i; i--)
                {
                    auto& it = entries->at(i - 1);
                    it.reset();
                }

                delete entries;
            }

        }

        void ModRefCount(bool inc, bool destructing)
        {
            refCount += inc ? 1 : -1;

            //logger::trace("temp, {} to {}", inc ? "increment" : "decrement", refCount.load());

            assert(refCount >= 0);

            if (refCount && destructed) {
                //logger::debug("{} detected, {} refs remaining", inc ? "increment" : "decrement", refCount.load());
            }
            else assert_if(!refCount && !destructed && !destructing)
            {
                //report::fault::critical("ObjectPolicyManager ran out of uses outside of destruction, this should not happen");
            }

            TryDestroy();
        }

        ~SingletonManager()
        {
            ModRefCount(false, true);
            destructed = true;

            if (refCount) {
                //logger::debug("Delaying destruction of ObjectPolicyList, {} refs remaining", refCount.load());
            }
        }

        std::vector<std::unique_ptr<IEntry>>* entries = new std::vector<std::unique_ptr<IEntry>>;

        std::atomic<ptrdiff_t>  refCount = 1;
        bool destructed = false;
    };


    template<typename T, typename... Args>
    auto&& make_singleton(Args&&... args)
    {
        struct token
        {
        private:
            ~token() = default;

        public:
            token() = default;
            token(T& t) : result{ &t } {}

            constexpr operator T& () noexcept
            {
                return *result;
            }


        private:
            T* result = nullptr;
        };

        constexpr bool do_the_thing = !std::is_final_v<T> && std::is_class_v<T>;

        struct _unprotect : public std::conditional_t<do_the_thing, T, detail::not_implemented> {
            using Base = std::conditional_t<do_the_thing, T, detail::not_implemented>;
            
            _unprotect(Args...args) requires(do_the_thing) : Base(std::forward<Args>(args)...) {}
        };




        using Entry = SingletonManager::BasicEntry<std::conditional_t<do_the_thing, _unprotect, T>>;


        token result;

        std::unique_ptr<SingletonManager::IEntry> entry;

        if constexpr (!std::is_final_v<T> && std::is_class_v<T>)
        {
            struct _unprotect : public T {
                _unprotect(Args...args) : T(std::forward<Args>(args)...) {}
            };

            using Entry = SingletonManager::BasicEntry<std::conditional_t<do_the_thing, _unprotect, T>>;

            std::unique_ptr<Entry> dtor = std::make_unique<Entry>(std::forward<Args>(args)...);


            result = token{ dtor->value };
            entry = std::move(dtor);
        }
        else
        {
            using Entry = SingletonManager::BasicEntry<T>;

            std::unique_ptr<Entry> dtor = std::make_unique<Entry>(std::forward<Args>(args)...);

            result = token{ dtor->value };

            entry = std::move(dtor);
        }


        SingletonManager::singleton->Place(std::move(entry));

        return std::move(result);
    }

}