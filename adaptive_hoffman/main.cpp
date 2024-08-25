#include "AdaptiveHuffmanAlg.h"
#include <iostream>
#include <fstream>   
#include <sstream>   
#include <string>    
#include <iostream>  
#include <chrono>


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

void start(int INERTION, const std::string& text, bool printTree = false, bool printEncodedTextLen = false, bool printEncodedText = false, bool writeToFile = false, const std::string& outputFile = "") {

    AdaptiveHuffmanAlg tree(0, 51, INERTION);
    if (INERTION == 0) {
        std::cout << "Start encodeText without inertion" << std::endl;
    }
    else {
        std::cout << "Start encodeText with inertion: " << INERTION << std::endl;
    }
    auto start = std::chrono::high_resolution_clock::now();

    std::string encodedTextOut = tree.encodeText(text);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> exactSearchElapsedTime = end - start;
    std::cout << "encodeText time: " << exactSearchElapsedTime.count() << " seconds." << std::endl;

    if (writeToFile) {
        writeStringToFile(encodedTextOut, outputFile);
    }
    if (printTree) {
        std::cout << "\nTree Structure:\n";
        tree.printTree();
    }
    if (printEncodedText) { std::cout << "\nEncoded text: " << encodedTextOut << "\n\n";}
    if (printEncodedTextLen) {std::cout << "Encoded text length: " << encodedTextOut.length() << std::endl;}
}

int main() {
    int INERTION =              96; // if you dont want to use insertion, set it to 0
    std::string text =          open_file("path..to..file/text_input.txt");
    std::string outputFilePath= "path..to..file/encoded_output.txt"; // encoded text will be written to this file // not have to output file
    bool writeToFile =          true; // if you want to write encoded text to a file, set it to true
    bool printTree =            false; //for terminal print // debug purposes
    bool printEncodedTextLen =  true; 
    bool printEncodedText =     false;

    start(INERTION, text, printTree, printEncodedTextLen, printEncodedText, writeToFile, outputFilePath);
    
    return 0;
}
