#pragma once



namespace LEX
{
    class Object;


    struct ObjectHandle//Rename object handle
    {
        struct _deleter
        {
            constexpr _deleter() noexcept = default;

         constexpr void operator()(Object* _Ptr) const noexcept {
                delete _Ptr;
            }

            //*/
        };


        //the main pointer to an object. Is the main thing that handles interaction with the object in question, making safe calls to all of its vfunctions.
        
        
        //I have the idea that what I want to do here is have the actual object pointers stored elsewhere.
        // And if something is a struct, it will basically be made with a zero ref count, something a thing should never have.
        
        //std::unique_ptr<Object, _deleter> _data{};//This dumb shit's copy assignment is deleted. Fuck.
        Object* _data{};


        constexpr std::strong_ordering operator <=> (const ObjectHandle& rhs) const
        {
            //This will need to decipher if it's a value type or not, but ultimately the actual object is supposed
            // to handle that. Non null is always greater than null unless they're the same.
            return _data <=> rhs._data;
        }

        constexpr bool operator==(const ObjectHandle& a_rhs) const
        {
            return operator<=>(a_rhs) == std::strong_ordering::equal;
        }

        constexpr ObjectHandle() = default;

        constexpr ObjectHandle(std::nullptr_t) {}
    
    
        //On the pointer itself, I'm considering using
        //Considering using a shared pointer
        // also
    };

}