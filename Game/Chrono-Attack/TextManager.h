#ifndef TEXTMANAGER_H
#define TEXTMANAGER_H

#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <stdexcept>

class TextManager {
public:
    static TextManager& getInstance();

    void loadTexts(const std::string& filename);

    std::string getText(const std::string& key) const;

    TextManager(const TextManager&) = delete;
    TextManager& operator=(const TextManager&) = delete;

private:
    TextManager() = default;

    std::map<std::string, std::string> texts_;
};

#endif