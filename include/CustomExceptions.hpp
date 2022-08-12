#ifndef SEARCH_ENGINE_CUSTOMEXCEPTIONS_HPP
#define SEARCH_ENGINE_CUSTOMEXCEPTIONS_HPP

#include <string>
#include <stdexcept>

class ConfigFileIncorrectException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Config file has incorrect format!";
    }
};

class ConfigFileEmptyException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Config file is empty!";
    }
};

class ConfigFileMissingException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Config file is missing!";
    }
};

class FileMissingException : public std::runtime_error {
public:
    explicit FileMissingException(const std::string& fileName)
    : std::runtime_error(("Warning: File \"" + fileName + "\" is missing!").c_str()) {};
};

class FileBusyException : public std::runtime_error {
public:
    explicit FileBusyException(const std::string& fileName)
    : std::runtime_error(("Warning: File \"" + fileName + "\" is occupied by another process!").c_str()) {};
};


#endif //SEARCH_ENGINE_CUSTOMEXCEPTIONS_HPP
