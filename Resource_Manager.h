#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <iostream>

class Resource_Manager {
private:
    std::map<std::string, sf::Texture> textures;
    std::map<std::string, sf::Font> fonts;

    Resource_Manager() = default; // Singleton

public:
    static Resource_Manager& Instance() {
        static Resource_Manager instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    Resource_Manager(const Resource_Manager&) = delete;
    Resource_Manager& operator=(const Resource_Manager&) = delete;
    Resource_Manager(Resource_Manager&&) = delete;
    Resource_Manager& operator=(Resource_Manager&&) = delete;

    sf::Texture& getTexture(const std::string& filePath) {
        if (textures.find(filePath) == textures.end()) {
            if (!textures[filePath].loadFromFile(filePath)) {
                std::cerr << "Error loading texture: " << filePath << std::endl;
                // Handle error: maybe return a default texture or throw exception
            }
        }
        return textures[filePath];
    }

    sf::Font& getFont(const std::string& filePath) {
        if (fonts.find(filePath) == fonts.end()) {
            if (!fonts[filePath].openFromFile(filePath)) {
                std::cerr << "Error loading font: " << filePath << std::endl;
                // Handle error: maybe return a default font or throw exception
            }
        }
        return fonts[filePath];
    }
};

#endif // RESOURCE_MANAGER_H