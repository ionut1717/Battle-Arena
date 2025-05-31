#include "Resource_Manager.h"
// No need to include specific SFML headers here if Resource_Manager.h already does.

// Definition of the static Instance() method for the singleton
// This is typically the only non-templated function defined outside the header
// for a singleton pattern.
Resource_Manager& Resource_Manager::Instance() {
    static Resource_Manager instance; // The single instance is created here, once.
    return instance;
}

// --- Private Helper Template Methods Implementations ---
// These need to be defined in the .cpp file, even though they are templates.
// The `template<typename T_Resource>` part must precede each definition.

template<typename T_Resource>
T_Resource& Resource_Manager::loadResource(typename ResourceManagerMapType<T_Resource>::type& resourceMap, const std::string& filename) {
    auto it = resourceMap.find(filename);
    if (it != resourceMap.end()) {
        // Resource already loaded, return existing one
        return *it->second;
    }

    // Attempt to load the resource
    auto newResource = std::make_unique<T_Resource>();
    // All SFML resources like sf::Texture, sf::Font, sf::SoundBuffer have loadFromFile()
    if (!newResource->loadFromFile(filename)) {
        std::cerr << "Error: Could not load resource '" << filename << "' of type from Resource_Manager." << std::endl;
        throw std::runtime_error("Failed to load resource: " + filename);
    }

    // Store the loaded resource and return a reference to it
    resourceMap[filename] = std::move(newResource);
    return *resourceMap[filename];
}

template<typename T_Resource>
T_Resource& Resource_Manager::getResource(typename ResourceManagerMapType<T_Resource>::type& resourceMap, const std::string& filename) {
    auto it = resourceMap.find(filename);
    if (it == resourceMap.end()) {
        std::cerr << "Warning: Resource '" << filename << "' not found. Attempting to load it on demand." << std::endl;
        // If the resource is not found, attempt to load it immediately (lazy loading)
        return loadResource<T_Resource>(resourceMap, filename);
    }
    return *it->second;
}

// --- Explicit Template Instantiations ---
// You must explicitly instantiate the template methods for each type you use.
// This tells the compiler to generate the code for these specific template types in this compilation unit.
// Without these, you will get linker errors ("undefined reference") when using the template methods.

template sf::Texture& Resource_Manager::load<sf::Texture>(const std::string& filename);
template sf::Texture& Resource_Manager::get<sf::Texture>(const std::string& filename);

template sf::Font& Resource_Manager::load<sf::Font>(const std::string& filename);
template sf::Font& Resource_Manager::get<sf::Font>(const std::string& filename);

// Add explicit instantiations for any other resource types you manage:
// template sf::SoundBuffer& Resource_Manager::load<sf::SoundBuffer>(const std::string& filename);
// template sf::SoundBuffer& Resource_Manager::get<sf::SoundBuffer>(const std::string& filename);