
#include "ConfigLoader.hpp"

#include <fstream>
#include <functional>
#include <stdexcept>
#include <string>
#include <cassert>
#include <iostream>
#include <cstdio>
#include <filesystem>


namespace fs = std::filesystem;



bool expectException(
    const std::function<void()>& operation
)
{
    try
    {
        operation();
    }
    catch (const std::exception&)
    {
        return true;
    }

    return false;
}



bool expectExceptionMessage(
    const std::function<void()>& operation,
    const std::string& expectedMessage
)
{
    try
    {
        operation();
    }
    catch (const std::runtime_error& error)
    {
        const std::string actualMessage = error.what();

        return actualMessage.find(expectedMessage)
            != std::string::npos;
    }
    catch (const std::exception&)
    {
        return false;
    }

    return false;
}




void writeTestFile(
    const std::string& path,
    const std::string& content
)
{
    std::ofstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Impossible de creer le fichier de test"
        );
    }

    file << content;

    if (!file)
    {
        throw std::runtime_error(
            "Impossible d'ecrire le fichier de test"
        );
    }
}



class TemporaryTestDirectory
{
private:
    std::filesystem::path path;

public:
    explicit TemporaryTestDirectory(
        const std::filesystem::path& directoryPath
    )
        : path(directoryPath)
    {
        std::filesystem::create_directories(path);
    }

    ~TemporaryTestDirectory()
    {
        std::error_code error;

        std::filesystem::remove_all(path, error);
    }

    TemporaryTestDirectory(
        const TemporaryTestDirectory&
    ) = delete;

    TemporaryTestDirectory& operator=(
        const TemporaryTestDirectory&
    ) = delete;
};





int main()
{
    const std::filesystem::path testDirectory = std::filesystem::current_path() / "test_data";

    TemporaryTestDirectory cleanup(testDirectory);


    const std::filesystem::path configPath =
        testDirectory / "client_test.json";

    std::filesystem::create_directories(testDirectory);

    {
        std::ofstream file(configPath);

        if (!file.is_open())
        {
            throw std::runtime_error(
                "Impossible de creer le JSON de test"
            );
        }

        file << R"({
            "server": {
                "host": "127.0.0.1",
                "port": 5555
            }
        })";
    }

    ConnectionConfig config = ConfigLoader::loadFromFile(configPath.string());

    assert(config.host == "127.0.0.1");
    assert(config.port == 5555);
    assert(config.isValid());


    std::cout
        << "Configuration chargee : "
        << config.host
        << ":"
        << config.port
        << std::endl;


    // Test : fichier inexistant


    const std::string missingFilePath = (testDirectory / "fichier_inexistant.json").string();

    bool missingFileRejected = expectExceptionMessage(
        [&missingFilePath]()
        {
            ConfigLoader::loadFromFile(missingFilePath);
        },
        "Impossible d'ouvrir la configuration :"
    );

    assert(missingFileRejected);

    std::cout
        << "Fichier inexistant : rejet confirme"
        << std::endl;



    // Test : fichier JSON malforme

    const std::string invalidJsonPath = (testDirectory / "test_invalid.json").string();

    writeTestFile(
        invalidJsonPath,
        R"({"server": {"host": "127.0.0.1", "port": 5555)"
    );


    bool invalidJsonRejected = expectExceptionMessage(
        [&invalidJsonPath]()
        {
            ConfigLoader::loadFromFile(
                invalidJsonPath
            );
        },
        "JSON invalide :"
    );


    assert(invalidJsonRejected);

    std::cout
        << "JSON malforme : rejet confirme"
        << std::endl;




    // Test : port superieur a 65535

    const std::string invalidPortPath = (testDirectory / "test_invalid_port.json").string();

    writeTestFile(
        invalidPortPath,
        R"({"server": {"host": "127.0.0.1", "port": 70000}})"
    );


    bool invalidPortRejected = expectExceptionMessage(
        [&invalidPortPath]()
        {
            ConfigLoader::loadFromFile(
                invalidPortPath
            );
        },
        "Port invalide : valeur attendue entre 1 et 65535"
    );


    assert(invalidPortRejected);

    std::cout
        << "Port superieur a 65535 : rejet confirme"
        << std::endl;


    // Test : port negatif

    const std::string negativePortPath = (testDirectory / "test_negative_port.json").string();

    writeTestFile(
        negativePortPath,
        R"({"server": {"host": "127.0.0.1", "port": -1}})"
    );


    bool negativePortRejected = expectExceptionMessage(
        [&negativePortPath]()
        {
            ConfigLoader::loadFromFile(
                negativePortPath
            );
        },
        "Port invalide : valeur attendue entre 1 et 65535"
    );


    assert(negativePortRejected);

    std::cout
        << "Port negatif : rejet confirme"
        << std::endl;

    // Test : port sous forme de chaine de caracteres

    const std::string stringPortPath = (testDirectory / "test_string_port.json").string();

    writeTestFile(
        stringPortPath,
        R"({"server": {"host": "127.0.0.1", "port": "5555"}})"
    );


    bool stringPortRejected = expectExceptionMessage(
        [&stringPortPath]()
        {
            ConfigLoader::loadFromFile(
                stringPortPath
            );
        },
        "Port invalide : un entier est attendu"
    );


    assert(stringPortRejected);

    std::cout
        << "Port sous forme de texte : rejet confirme"
        << std::endl;



    // Test : port numerique extremement grand

    const std::string hugePortPath = (testDirectory / "test_huge_port.json").string();

    writeTestFile(
        hugePortPath,
        R"({"server": {"host": "127.0.0.1", "port": 4294967296}})"
    );


    bool hugePortRejected = expectExceptionMessage(
        [&hugePortPath]()
        {
            ConfigLoader::loadFromFile(
                hugePortPath
            );
        },
        "Port invalide : valeur attendue entre 1 et 65535"
    );


    assert(hugePortRejected);

    std::cout
        << "Port numerique extremement grand : rejet confirme"
        << std::endl;


    // Test : champ server manquant

    const std::string missingServerPath = (testDirectory / "test_missing_server.json").string();

    writeTestFile(
        missingServerPath,
        R"({"application": "PrivateMessage"})"
    );


    bool missingServerRejected = expectExceptionMessage(
        [&missingServerPath]()
        {
            ConfigLoader::loadFromFile(
                missingServerPath
            );
        },
        "Configuration invalide :"
    );


    assert(missingServerRejected);

    std::cout
        << "Champ server manquant : rejet confirme"
        << std::endl;




    // Test : champ host manquant

    const std::string missingHostPath = (testDirectory / "test_missing_host.json").string();

    writeTestFile(
        missingHostPath,
        R"({"server": {"port": 5555}})"
    );


    bool missingHostRejected = expectExceptionMessage(
        [&missingHostPath]()
        {
            ConfigLoader::loadFromFile(
                missingHostPath
            );
        },
        "Configuration invalide :"
    );


    assert(missingHostRejected);

    std::cout
        << "Champ host manquant : rejet confirme"
        << std::endl;


    // Test : adresse du serveur vide

    const std::string emptyHostPath = (testDirectory / "test_empty_host.json").string();

    writeTestFile(
        emptyHostPath,
        R"({"server": {"host": "", "port": 5555}})"
    );


    bool emptyHostRejected = expectExceptionMessage(
        [&emptyHostPath]()
        {
            ConfigLoader::loadFromFile(
                emptyHostPath
            );
        },
        "Configuration invalide : hote ou port manquant"
    );


    assert(emptyHostRejected);

    std::cout
        << "Adresse host vide : rejet confirme"
        << std::endl;



    // Test : port egal a zero

    const std::string zeroPortPath = (testDirectory / "test_zero_port.json").string();

    writeTestFile(
        zeroPortPath,
        R"({"server": {"host": "127.0.0.1", "port": 0}})"
    );


    bool zeroPortRejected = expectExceptionMessage(
        [&zeroPortPath]()
        {
            ConfigLoader::loadFromFile(zeroPortPath);
        },
        "Port invalide : valeur attendue entre 1 et 65535"
    );


    assert(zeroPortRejected);

    std::cout
        << "Port egal a zero : rejet confirme"
        << std::endl;




    return 0;
}
