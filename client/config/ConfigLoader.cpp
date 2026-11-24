
#include "ConfigLoader.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <stdexcept>
#include <string>

ConnectionConfig ConfigLoader::loadFromFile(
    const std::string& path
)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Impossible d'ouvrir la configuration : " + path
        );
    }

    nlohmann::json data;

    try
    {
        file >> data;
    }
    catch (const nlohmann::json::parse_error& error)
    {
        throw std::runtime_error(
            "JSON invalide : " + std::string(error.what())
        );
    }

    ConnectionConfig config;

    try
    {
        const nlohmann::json& server = data.at("server");

        config.host = server.at("host").get<std::string>();

        // Lire un entier avec verification des bornes.

        const nlohmann::json& portValue = server.at("port");

        if (!portValue.is_number_integer())
        {
            throw std::runtime_error("Port invalide : un entier est attendu");
        }

        if (portValue.is_number_unsigned())
        {
            std::uint64_t port = portValue.get<std::uint64_t>();

            if (port == 0 || port > 65535)
            {
                throw std::runtime_error("Port invalide : valeur attendue entre 1 et 65535");
            }

            config.port = static_cast<std::uint16_t>(port);
        }
        else
        {
            std::int64_t port = portValue.get<std::int64_t>();

            if (port < 1 || port > 65535)
            {
                throw std::runtime_error("Port invalide : valeur attendue entre 1 et 65535");
            }

            config.port = static_cast<std::uint16_t>(port);
        }
    }
    catch (const nlohmann::json::exception& error)
    {
        throw std::runtime_error("Configuration invalide : " + std::string(error.what()));
    }

    if (!config.isValid())
    {
        throw std::runtime_error("Configuration invalide : hote ou port manquant");
    }

    return config;
}
