#include "AdaptiveHuffmanAlg.h"
#include <iostream>
#include <fstream>   
#include <sstream>   
#include <string>    
#include <iostream>  
#include <chrono>
#include <filesystem>

bool createDirectoryIfNotExists(const std::string& folderPath) {
    if (!std::filesystem::exists(folderPath)) {
        return  std::filesystem::create_directory(folderPath);
    }
    return true; // Directory already exists
}

void writeStringToFile(const std::string& str, const std::string& filePath) {
    std::ofstream outFile(filePath);
    if (outFile) {
        outFile << str;
        std::cout << "String has been successfully written to " << filePath << std::endl;
    }
    else {
        std::cerr << "An error occurred while writing to the file." << std::endl;
    }
}

// Function to open a file and read its contents into a string
std::string open_file(const std::string& file_path) {
    std::ifstream file(file_path);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file " << file_path << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

void first_log(const std::string& logFilename) {
    std::ofstream logFile(logFilename, std::ios::out);
    if (logFile.is_open()) {
        logFile << "Iteration,EncodedTextLen\n";
        logFile.close();
    }
    else {
        std::cerr << "Unable to open log file: " << logFilename << std::endl;
    }
}

void logConfiguration(const std::string& logFilename, const std::string& data) {
    std::ofstream logFile(logFilename, std::ios::app); // Open in append mode
    if (logFile.is_open()) {
        logFile << data << "\n";
        logFile.close();
    }
    else {
        std::cerr << "Unable to open log file: " << logFilename << std::endl;
    }
}

void executeCommand(const std::string& command) {
    int result = system(command.c_str());
    if (result != 0) {
        std::cout << "Command failed: " << command << std::endl;
    }
}

void create_final_tree_png(int i = 0) {
    std::string command = "dot -Tpng results/tree" + std::to_string(i - 1) + ".dot -o final_tree" + std::to_string(i - 1) + ".png ";
    executeCommand(command.c_str());
}

void createVideoFromImages() {
    std::string command = "ffmpeg -start_number 0 -framerate 5 -i results/tree%d.png -c:v libx264 -r 30 -pix_fmt yuv420p tree_video_final.mp4";
    executeCommand(command.c_str());
}

void create_png(int number_of_dots_files) {
    for (int i = 0; i < number_of_dots_files; i++) {
        std::string command = "dot -Tpng results/tree" + std::to_string(i) + ".dot -o results/tree" + std::to_string(i) + ".png -Gsize=8,8\! -Gratio=fill";
        executeCommand(command.c_str());
    }
}

void create_video_func(int number_of_dots_files) {
    create_png(number_of_dots_files);
    createVideoFromImages();
}

void start(int inertion, const std::string& text, bool printTree = false, bool printEncodedTextLen = false, bool printEncodedText = false, bool writeToFile = false, const std::string& outputFile = "", bool create_video = false) {
    //enum save_state { save, save_final_img_only, dont_save };
    save_state is_it_saving = save_final_img_only;
    if (create_video) { is_it_saving = save; }
    createDirectoryIfNotExists("results");
    AdaptiveHuffmanAlg tree(0, 51, inertion, is_it_saving);
    if (inertion == 0) { 
        std::cout << "Start encodeText without inertion" << std::endl;
    }
    else {
        std::cout << "Start encodeText with inertion: " << inertion << std::endl;
    }
    auto start = std::chrono::high_resolution_clock::now();

    std::string encodedTextOut = tree.encodeText(text);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> exactSearchElapsedTime = end - start;
    std::cout << "encodeText time: " << exactSearchElapsedTime.count() << " seconds." << std::endl;

    if (writeToFile) { 
        writeStringToFile(encodedTextOut, outputFile); 
        //std::cout << "encoded text has been written to file: " << outputFile << std::endl;
    }
    if (printTree) {
        std::cout << "\nTree Structure:\n";
        tree.printTree();
    }
    if (printEncodedText) { std::cout << "\nEncoded text: " << encodedTextOut << "\n\n";}
    if (printEncodedTextLen) {std::cout << "Encoded text length: " << encodedTextOut.length() << std::endl;}

    int step = tree.get_max_index_img();
    std::cout << "how many frames: " << step << "\n";
    
    create_final_tree_png(step);
    std::cout << "created final image tree in project dir" << "\n";

    if (create_video) {
        auto start = std::chrono::high_resolution_clock::now();
        std::cout << "start creating video\n" << std::endl;
        create_video_func(step);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> exactSearchElapsedTime = end - start;
        std::cout << "create_video time: " << exactSearchElapsedTime.count() << " seconds." << std::endl;
    }
}

int main() {
    int INERTION =              96; // if you dont want to use insertion, set it to 0
    std::string text =          open_file("path..to..file/text_input.txt");
    std::string outputFilePath= "path..to..file/encoded_output.txt"; // encoded text will be written to this file // not have to output file
    bool writeToFile =          true;// if you want to write encoded text to a file, set it to true
    bool printTree =            false; //for terminal print // debug purposes
    bool printEncodedTextLen =  true;
    bool printEncodedText =     false;
    bool create_video =         false; //note this will take from the run time (beacuse it creating an img for every step)!! it will need space of disk
    
    start(INERTION, text, printTree, printEncodedTextLen, printEncodedText, writeToFile, outputFilePath, create_video);
    return 0;
}
