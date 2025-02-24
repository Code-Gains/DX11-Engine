#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>
#include <fstream>

// 1) Cereal headers
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/archives/json.hpp>

// -------------------------------------------------------
// A basic base class for components
// -------------------------------------------------------
struct Component
{
    virtual ~Component() = default;

    // If you had data here, you'd serialize it
    template <class Archive>
    void serialize(Archive&)
    {
        // base component stuff
    }
};

// -------------------------------------------------------
// An interface for storing multiple components
// -------------------------------------------------------
class IComponentVector
{
public:
    virtual ~IComponentVector() = default;

    // Typically empty for the interface unless
    // you have data at the IComponentVector level
    template <class Archive>
    void save(Archive&) const { }

    template <class Archive>
    void load(Archive&) { }
};

// -------------------------------------------------------
// A templated vector class that implements IComponentVector
// -------------------------------------------------------
template <typename T>
class ComponentVector : public IComponentVector
{
public:
    int dummy = 123;

    // Example data you want to serialize:
    // std::vector<T> components;

    template <class Archive>
    void save(Archive& ar) const
    {
        ar(dummy);
        // ar(components);
    }

    template <class Archive>
    void load(Archive& ar)
    {
        ar(dummy);
        // ar(components);
    }
};

// -------------------------------------------------------
// Our derived component that also derives from "Component"
// -------------------------------------------------------
struct CameraComponent : public Component
{
    float myValue = 42.f;

    // Additional data or methods

    template <class Archive>
    void serialize(Archive& ar)
    {
        // First, optionally serialize the base class
        ar(cereal::base_class<Component>(this));
        // Then serialize derived data
        ar(myValue);
    }
};

// -------------------------------------------------------
// Cereal Registration for polymorphic IComponentVector
// -------------------------------------------------------
CEREAL_REGISTER_TYPE(ComponentVector<CameraComponent>);
CEREAL_REGISTER_POLYMORPHIC_RELATION(IComponentVector, ComponentVector<CameraComponent>);

// -------------------------------------------------------
// Demonstration main
// -------------------------------------------------------
int main()
{
    // We store a pointer to the base of IComponentVector
    // but the actual object is ComponentVector<CameraComponent>
    std::unique_ptr<IComponentVector> vec = std::make_unique<ComponentVector<CameraComponent>>();

    // Prove it's really a ComponentVector<CameraComponent> at runtime
    auto* actual = dynamic_cast<ComponentVector<CameraComponent> *>(vec.get());
    if (!actual)
    {
        std::cerr << "dynamic_cast failed! Something's off.\n";
        return 1;
    }
    actual->dummy = 999;

    // We store multiple IComponentVector pointers in a map
    std::unordered_map<std::string, std::unique_ptr<IComponentVector>> typeMap;
    typeMap["camera"] = std::move(vec);

    // Serialize out (to out.json)
    {
        std::ofstream os("out.json");
        cereal::JSONOutputArchive archive(os);
        archive(CEREAL_NVP(typeMap));
    }

    // Deserialize back in
    {
        std::ifstream is("out.json");
        cereal::JSONInputArchive archive(is);
        std::unordered_map<std::string, std::unique_ptr<IComponentVector>> newMap;
        archive(newMap);  // Polymorphic load

        // Check we got "camera" entry
        auto it = newMap.find("camera");
        if (it == newMap.end())
        {
            std::cerr << "Missing 'camera' entry after load.\n";
            return 1;
        }

        // Check the actual runtime type
        auto* loadedActual = dynamic_cast<ComponentVector<CameraComponent> *>(it->second.get());
        if (!loadedActual)
        {
            std::cerr << "Polymorphic relation failed.\n";
            return 1;
        }

        std::cout << "Success! dummy = " << loadedActual->dummy << std::endl;
    }

    return 0;
}
