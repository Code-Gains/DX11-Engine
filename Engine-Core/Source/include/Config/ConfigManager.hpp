#pragma once
#include <iostream>
#include <fstream>
#include <d3d11_2.h>
#include <unordered_map>
#include "nlohmannjson/json.hpp"

class ConfigManager
{
    nlohmann::json _configData;
    std::unordered_map<std::string, int> _vsBufferSlots;
    std::unordered_map<std::string, int> _psBufferSlots;

    ConfigManager(const std::string& fileName);

public:
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    static ConfigManager& GetInstance(const std::string& fileName = "../../../../Assets/config.json")
    {
        static ConfigManager instance(fileName);
        return instance;
    }

    int GetVSConstantBufferSlot(const std::string& bufferName);
    int GetPSConstantBufferSlot(const std::string& bufferName);
};