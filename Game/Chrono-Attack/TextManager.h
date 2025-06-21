#pragma once

#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <stdexcept>

class TextManager {
public:
    // Singleton instance access
    static TextManager& getInstance();

    // Load texts from a file
    void loadTexts(const std::string& filename);

    // Get text by key
    std::string getText(const std::string& key) const;

    // Delete copy constructor and assignment operator to prevent copying
    TextManager(const TextManager&) = delete;
    TextManager& operator=(const TextManager&) = delete;

private:
    // Private constructor for singleton pattern
    TextManager() = default;

    std::map<std::string, std::string> texts_;
};