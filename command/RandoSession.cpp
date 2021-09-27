
#include "RandoSession.hpp"
#include <fstream>
#include "../filetypes/yaz0.hpp"
#include "../filetypes/sarc.hpp"


RandoSession::RandoSession(const fspath& gameBaseDir, 
             const fspath& randoWorkingDir, 
             const fspath& outputDir) : 
    gameBaseDirectory(gameBaseDir), workingDir(randoWorkingDir), outputDir(outputDir)
{
    
}

std::string splitPath(std::string& tail, char delim)
{
    std::string head;
    auto sepIndex = tail.find_first_of(delim);
    if (sepIndex == std::string::npos)
    {
        return "";
    }
    head = tail.substr(0, sepIndex);
    tail = tail.substr(sepIndex + 1);
    return head;
}

std::string splitPathRL(std::string& tail, char delim)
{
    std::string head;
    auto sepIndex = tail.find_last_of(delim);
    if (sepIndex == std::string::npos)
    {
        head = tail;
        tail = "";
        return head;
    }
    head = tail.substr(0, sepIndex);
    tail = tail.substr(sepIndex + 1);
    return head;
}

RandoSession::fspath RandoSession::relToGameAbsolute(const RandoSession::fspath& relPath)
{
    return gameBaseDirectory / relPath;
}

RandoSession::fspath RandoSession::absToGameRelative(const RandoSession::fspath& absPath)
{
    return std::filesystem::relative(absPath, gameBaseDirectory);
}

RandoSession::fspath RandoSession::extractFile(const std::vector<std::string>& fileSpec)
{
    // ["content/Common/Stage/example.szs", "YAZ0", "SARC", "data.bfres"]
    // first part is an extant game file
    std::string cacheKey{""};
    std::string resultKey{""};
    for (const auto& element : fileSpec)
    {
        if (element.empty()) continue;

        if (element.compare("YAZ0") == 0)
        {
            resultKey = cacheKey + ".dec";
        }
        else if(element.compare("SARC") == 0)
        {
            // TODO: do we need to make / here platform agnostic?
            resultKey = cacheKey + ".unpack/";
        }
        else
        {
            resultKey = cacheKey + element;
        }
        // if we've already cached this
        if (fileCache.count(resultKey) > 0)
        {
            cacheKey = resultKey;
            continue;
        }
        
        if (element.compare("YAZ0") == 0)
        {
            std::ifstream inputFile(workingDir / cacheKey, std::ios::binary);
            if (!inputFile.is_open()) return "";
            std::ofstream outputFile(workingDir / resultKey, std::ios::binary);
            if (!inputFile.is_open()) return "";
            if(FileTypes::yaz0Decode(inputFile, outputFile) == 0)
            {
                return "";
            }
        }
        else if(element.compare("SARC") == 0)
        {
            FileTypes::SARCFile sarc;
            auto err = sarc.loadFromFile((workingDir / cacheKey).string());
            if (err != SARCError::NONE) return "";
            fspath extractTo = workingDir / resultKey;
            if (!std::filesystem::is_directory(extractTo))
            {
                std::filesystem::create_directory(extractTo);
            }
            if((err = sarc.extractToDir(extractTo.string())) != SARCError::NONE) return "";
        }
        else
        {
            if (!std::filesystem::is_regular_file(workingDir / resultKey))
            {
                // attempt to copy from game directory
                fspath gameFilePath = relToGameAbsolute(resultKey);
                if (!std::filesystem::is_regular_file(gameFilePath))
                {
                    return "";
                }
                fspath outputPath = workingDir / resultKey;
                if (!std::filesystem::is_directory(outputPath.parent_path()))
                {
                    if(!std::filesystem::create_directories(outputPath.parent_path())) return "";
                }
                auto copyOptions = std::filesystem::copy_options::overwrite_existing;
                if (!std::filesystem::copy_file(gameFilePath, outputPath, copyOptions))
                {
                    return "";
                }
            }
        }
        fileCache.emplace(resultKey);
        cacheKey = resultKey;
    }
    return workingDir / resultKey;
}

std::ifstream RandoSession::openGameFile(const std::vector<std::string>& gameFilePath, RandoSession::fspath& relPath)
{
    auto workingPath = extractFile(gameFilePath);
    relPath = std::filesystem::relative(workingPath, workingDir);
    return std::ifstream(workingPath, std::ios::binary);
}

void RandoSession::repackGameFile(const std::vector<std::string>& gameFilePath)
{

}

