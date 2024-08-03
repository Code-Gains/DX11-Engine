#include "ConfigManager.hpp"

ConfigManager::ConfigManager(const std::string& fileName)
{
    std::ifstream file(fileName);
    if (file.is_open())
    {
        std::cout << "[Config] File found\n";
        file >> _configData;

        if (_configData.contains("VSConstantBuffers"))
        {
            for (auto& [key, value] : _configData["VSConstantBuffers"].items())
            {
                _vsBufferSlots[key] = value;
            }
            std::cout << "[Config] VS CBs loaded\n";
        }
        if (_configData.contains("PSConstantBuffers"))
        {
            for (auto& [key, value] : _configData["PSConstantBuffers"].items())
            {
                _psBufferSlots[key] = value;
            }
            std::cout << "[Config] PS CBs loaded\n";
        }
    }
    else
    {
        throw std::runtime_error("Unable to open config file");
    }
}

int ConfigManager::GetVSConstantBufferSlot(const std::string& bufferName)
{
    return _vsBufferSlots.at(bufferName);
}

int ConfigManager::GetPSConstantBufferSlot(const std::string& bufferName)
{
    return _psBufferSlots.at(bufferName);
}
