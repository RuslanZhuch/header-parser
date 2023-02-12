#include "parser.h"
#include "handler.h"
#include "options.h"
#include <tclap/CmdLine.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <fstream>
#include <filesystem>

//----------------------------------------------------------------------------------------------------
void print_usage()
{
  std::cout << "Usage: inputFile" << std::endl;
}

//----------------------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    Options options;
    std::string inputFolder;
    std::string outputFolder;
    try
    {
        using namespace TCLAP;

        CmdLine cmd("Header Parser");

        ValueArg<std::string> enumName("e", "enum", "The name of the enum macro", false, "ENUM", "", cmd);
        ValueArg<std::string> className("c", "class", "The name of the class macro", false, "CLASS", "", cmd);
        MultiArg<std::string> functionName("f", "function", "The name of the function macro", false, "", cmd);
        ValueArg<std::string> propertyName("p", "property", "The name of the property macro", false, "PROPERTY", "", cmd);
        ValueArg<std::string> outputFolderArg("o", "output", "Output folder", false, "", "", cmd);
        MultiArg<std::string> customMacro("m", "macro", "Custom macro names to parse", false, "", cmd);
        UnlabeledValueArg<std::string> inputFolderArg("inputFolder", "A folder with header files to parce", true, "", "", cmd);

        cmd.parse(argc, argv);

        inputFolder = inputFolderArg.getValue();
        outputFolder = outputFolderArg.getValue();
        options.classNameMacro = className.getValue();
        options.enumNameMacro = enumName.getValue();
        options.functionNameMacro = functionName.getValue();
        options.customMacros = customMacro.getValue();
        options.propertyNameMacro = propertyName.getValue();
    }
    catch (TCLAP::ArgException& e)
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        return -1;
    }  

    for (auto const& dirEntry : std::filesystem::recursive_directory_iterator("sources"))
    {

        if (!dirEntry.is_regular_file())
            continue;
        const auto filePath{ dirEntry.path() };
        if (filePath.extension() != ".h")
            continue;

        std::ifstream file(filePath);
        if (!file.is_open())
            continue;

        std::cout << "Found file " << filePath << '\n';

        std::stringstream buffer;
        buffer << file.rdbuf();

        Parser parser(options);
        if (parser.Parse(buffer.str().c_str()))
        {
            const auto filename{ filePath.filename().string() };
            const auto outputFile{ outputFolder + "\\" + filename + ".json" };
            std::ofstream out(outputFile);
            out << parser.result();
        }
              
    }

	return 0;
}
