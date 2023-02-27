#include "Serialization.h"
#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Serialization {
	
    void generateYamlFile(const std::string& path, const std::string& filename, const int model_id) {

        // Combine the path and filename to form the full file path
        std::string fullpath = path + "/" + filename + ".entity";

        // Create a YAML::Emitter object to generate YAML output
        YAML::Emitter emitter;

        // Start the YAML document
        emitter << YAML::BeginMap;

        // Add the model_id property to the document
        emitter << YAML::Key << "model_id";
        emitter << YAML::Value << model_id;

        // End the YAML document
        emitter << YAML::EndMap;

        // Write the YAML output to a file
        std::ofstream fout(fullpath);
        fout << emitter.c_str();
        fout.close();

        // Print a message to confirm that the file was generated
        std::cout << "Generated YAML file: " << fullpath << "\n";
    }
	

}
