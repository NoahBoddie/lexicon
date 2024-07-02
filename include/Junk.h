
//*
namespace
{

    template <typename T>
    AbstractTypePolicy* GetConcreteType()
    {
        //the idea of this function is that it needs explicit specialization, or the existence of a certain function within the type to be used.
        // Otherwise, it will return null, or perhaps the void type.
        return nullptr;
    }

    //These likely should be objects, or something that allows for delegate to be more of a reference type.

    // Then again, arrays are in a similar boat.



}



namespace//Object experiment
{
    //The object experimentation zone. Nothing is cut in stone,  still messing around with the concepts.

    //Span might be better, it's smaller, not aimed to be resized, I think it might fit my needs well.
    template <typename T>
    using ArgContainer = std::span<T, std::dynamic_extent>;

    class Object;


    //REMEMBER TO USE THESE.


    //This is for a core type that an object derives from, in order to prevent a scenerio where an external type will have versioning that changes the name.
    // I would like there to instead be an error if the object type doesn't derive from the targeted type.
    template<typename T> concept HasObjectType = requires
    {
        requires std::derived_from<T, typename T::ObjectType>;
    }&& std::derived_from<T, Object>;



    template <std::derived_from<Object> TObj>
    static void RegisterType(std::string_view path, ObjectFactory ctor)
    {
        //How one uses this is they use register, and specify the project they're assigning this data type. One can only do it to shared, or a project they have
        // control over.
        if constexpr (std::is_final_v<TObj> || !std::is_same_v<decltype(&TObj::GetName), decltype(&Object::GetName)>) {

            return;
        }
        else {
            //Is not viable, or maybe warn
        }
    }



}



struct GameObject : public Object
{


    //Game object is a class that's used to manage objects from skyrim. Seeing as this project isn't IN Clib though, just going to have to stomach its current set up.
};


using String = std::string;
class String_
{
    using Self = String_;

    char* _ptr = nullptr;
    size_t _length = 0;
public:
    ~String_()
    {

    }


    constexpr String_() = default;

    String_(const char* str)
    {

    }

    String_(const std::string& str)
    {

    }

    template <size_t N>
    String_(const char(&str)[N])//maybe use string lit?
    {

    }

    //virtual void Destroy() const
    //{
        //Unsure if this is required
        //delete this;
    //}
};
//*/

