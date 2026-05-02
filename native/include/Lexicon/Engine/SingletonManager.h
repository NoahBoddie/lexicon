#pragma once

namespace LEX
{
    //TODO: move this to rogues gallery, pretty much 0 reason I can't have this in other places.


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


#ifdef TEHTEGASG
    template <typename... Args>
    struct auto_singleton_
    {
        std::tuple<Args...> _args;

        template<typename T>
        operator T& ()
        {
            return std::apply(make_singleton<T>, std::forward<std::tuple<Args...>>(_args));
        }

        //Should want to move these probs
        auto_singleton_(Args&&... args) : _args{ std::make_tuple(std::forward<Args>(args)...) }
        {

        }

    };

    template <typename... Args>
    auto auto_singleton(Args&&... args)
    {
        return auto_singleton_(std::forward<Args>(args)...);
    }
#endif


    namespace detail
    {




        template<typename T, typename... Args>
        inline auto& make_singleton_impl(Args&&... args)
        {
           

            constexpr bool k_can_inherit = !std::is_final_v<T> && std::is_class_v<T>;

            //struct _unprotect : public std::conditional_t<k_can_inherit, T, detail::not_implemented> {
            //    using Base = std::conditional_t<k_can_inherit, T, detail::not_implemented>;
            //    _unprotect(Args...args) requires(do_the_thing) : Base(std::forward<Args>(args)...) {}
            //};


            std::unique_ptr<SingletonManager::IEntry> entry;

            T* result = nullptr;

            if constexpr (!std::is_final_v<T> && std::is_class_v<T>)
            {
                struct _unprotect : public T {
                    _unprotect(Args...args) : T(std::forward<Args>(args)...) {}
                };

                using Entry = SingletonManager::BasicEntry<_unprotect>;

                std::unique_ptr<Entry> dtor = std::make_unique<Entry>(std::forward<Args>(args)...);


                result = std::addressof(dtor->value);
                entry = std::move(dtor);
            }
            else
            {
                using Entry = SingletonManager::BasicEntry<T>;

                std::unique_ptr<Entry> dtor = std::make_unique<Entry>(std::forward<Args>(args)...);

                result = std::addressof(dtor->value);
                entry = std::move(dtor);
            }


            SingletonManager::singleton->Place(std::move(entry));

            return *result;
        }


        template <typename... Args>
        struct singleton_deducer
        {
            std::tuple<Args...> _args;

            //Need some kind of requires here to prevent possible construction error.
            template<typename T>
            operator T& ()
            {
                return std::apply(make_singleton_impl<T>, std::forward<std::tuple<Args...>>(_args));
            }

            //Should want to move these probs
            singleton_deducer(Args&&... args) : _args{ std::make_tuple(std::forward<Args>(args)...) }
            {

            }

        };

    }

    template<typename... Args>
    auto make_singleton(Args&&... args)
    {
        return detail::singleton_deducer(std::forward<Args>(args)...);
    }

}