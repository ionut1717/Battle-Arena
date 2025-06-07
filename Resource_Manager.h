#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <memory>
#include <stdexcept>
#include <type_traits>

template <typename T>
class Resource_Manager {
    std::map<std::string, std::unique_ptr<T>> resources;

    Resource_Manager() = default;

public:
    Resource_Manager(const Resource_Manager&) = delete;
    Resource_Manager& operator=(const Resource_Manager&) = delete;

    Resource_Manager(Resource_Manager&&) = delete;
    Resource_Manager& operator=(Resource_Manager&&) = delete;

    static Resource_Manager& Instance() {
        static Resource_Manager instance;
        return instance;
    }
    T& getResource(const std::string& filePath) {
        if (resources.find(filePath) == resources.end()) {
            std::unique_ptr<T> newResource = std::make_unique<T>();
            bool loaded = false;
            if constexpr (std::is_same_v<T, sf::Texture>) {
                loaded = newResource->loadFromFile(filePath);
            }
            else if constexpr (std::is_same_v<T, sf::Font>) {
                loaded = newResource->openFromFile(filePath);
            }
            else {
                throw std::runtime_error("Unknown function to load this asset");
            }
            if (!loaded) {
                throw std::runtime_error("Failed to load resource from file " + filePath);
            }
            resources[filePath] = std::move(newResource);
        }
        return *resources[filePath];
    }
};

#endif // RESOURCE_MANAGER_H