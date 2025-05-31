#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <string>
#include <map>
#include <memory>      // For std::unique_ptr
#include <stdexcept>   // For std::runtime_error
#include <iostream>    // For std::cerr
#include <type_traits> // For std::is_same_v, std::always_false_v (C++17)

// Include SFML types the manager will handle
#include <SFML/Graphics.hpp> // For sf::Texture, sf::Font
// #include <SFML/Audio.hpp> // Uncomment if you plan to manage sf::SoundBuffer, etc.
// Add other SFML resource headers here if you want to manage them (e.g., sf::Shader)

// Helper struct/trait to map resource types to their specific storage map types.
// This helps organize the internal maps within the ResourceManager.
template<typename T> struct ResourceManagerMapType;

// Explicit specializations for SFML resource types:
template<> struct ResourceManagerMapType<sf::Texture> { using type = std::map<std::string, std::unique_ptr<sf::Texture>>; };
template<> struct ResourceManagerMapType<sf::Font> { using type = std::map<std::string, std::unique_ptr<sf::Font>>; };
// Add more specializations for other SFML resource types you want to manage:
// template<> struct ResourceManagerMapType<sf::SoundBuffer> { using type = std::map<std::string, std::unique_ptr<sf::SoundBuffer>>; };
// template<> struct ResourceManagerMapType<sf::Shader> { using type = std::map<std::string, std::unique_ptr<sf::Shader>>; };


class Resource_Manager {
private:
    // Private constructor for Singleton pattern (prevents direct instantiation)
    Resource_Manager() = default;

    // Actual storage for each specific resource type
    typename ResourceManagerMapType<sf::Texture>::type textures;
    typename ResourceManagerMapType<sf::Font>::type fonts;
    // Add internal maps for other resource types here, using their specializations:
    // typename ResourceManagerMapType<sf::SoundBuffer>::type soundBuffers;


    // --- Private Helper Template Methods for Loading and Getting ---
    // These methods contain the core logic, separated for clarity and reusability.

    // Generic function to load a resource into a specific map.
    // T_Resource must have a .loadFromFile(std::string) method returning bool.
    template<typename T_Resource>
    T_Resource& loadResource(typename ResourceManagerMapType<T_Resource>::type& resourceMap, const std::string& filename);

    // Generic function to get a resource from a specific map.
    // If not found, it attempts to load it (lazy loading).
    template<typename T_Resource>
    T_Resource& getResource(typename ResourceManagerMapType<T_Resource>::type& resourceMap, const std::string& filename);

public:
    // --- Singleton Access ---
    // Static method to get the single instance of the Resource_Manager.
    static Resource_Manager& Instance();

    // Delete copy/move constructors and assignment operators to ensure single instance
    Resource_Manager(const Resource_Manager&) = delete;
    Resource_Manager& operator=(const Resource_Manager&) = delete;
    Resource_Manager(Resource_Manager&&) = delete;
    Resource_Manager& operator=(Resource_Manager&&) = delete;

    // --- Public API for Loading Resources ---
    // This templated method is the primary way to load resources.
    // It uses if constexpr to dispatch to the correct internal map based on T_Resource.
    template<typename T_Resource>
    T_Resource& load(const std::string& filename) {
        if constexpr (std::is_same_v<T_Resource, sf::Texture>) {
            return loadResource<sf::Texture>(textures, filename);
        } else if constexpr (std::is_same_v<T_Resource, sf::Font>) {
            return loadResource<sf::Font>(fonts, filename);
        }
        // Add more `else if constexpr` blocks for other resource types you manage:
        // else if constexpr (std::is_same_v<T_Resource, sf::SoundBuffer>) {
        //     return loadResource<sf::SoundBuffer>(soundBuffers, filename);
        // }
        else {
            // If an unsupported type is used, this will trigger a compile-time error.
            std::cout<< "Unsupported resource type for Resource_Manager::load";
        }
    }

    // --- Public API for Getting Resources ---
    // This templated method is the primary way to retrieve resources.
    // It also uses if constexpr for dispatch.
    template<typename T_Resource>
    T_Resource& get(const std::string& filename) {
        if constexpr (std::is_same_v<T_Resource, sf::Texture>) {
            return getResource<sf::Texture>(textures, filename);
        } else if constexpr (std::is_same_v<T_Resource, sf::Font>) {
            return getResource<sf::Font>(fonts, filename);
        }
        // Add more `else if constexpr` blocks for other resource types you manage:
        // else if constexpr (std::is_same_v<T_Resource, sf::SoundBuffer>) {
        //     return getResource<sf::SoundBuffer>(soundBuffers, filename);
        // }
        else {
            // Compile-time error for unsupported types.
            std::cout<< "Unsupported resource type for Resource_Manager::load";
        }
    }
};

// Helper for static_assert with `if constexpr` to ensure it's a dependent type. (C++17 feature)


#endif // RESOURCE_MANAGER_H