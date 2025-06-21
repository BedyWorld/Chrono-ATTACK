#include "TextManager.h"
#include <sstream>

TextManager& TextManager::getInstance() {
    static TextManager instance; // Guaranteed to be destroyed, instantiated on first use.
    return instance;
}

void TextManager::loadTexts(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open text file: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        size_t delimiterPos = line.find('=');
        if (delimiterPos != std::string::npos) {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);            texts_[key] = value;
        }
    }
    file.close();
}

std::string TextManager::getText(const std::string& key) const {
    auto it = texts_.find(key);
    if (it != texts_.end()) {
        return it->second;
    }
    // Fallback if key not found (can be replaced with error logging or throwing)
    std::cerr << "Warning: Text key '" << key << "' not found!" << std::endl;
    return "MISSING_TEXT_" + key;
}