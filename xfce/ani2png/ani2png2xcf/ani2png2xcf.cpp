// Inspired by https://github.com/Mastermindzh/Scripts/blob/master/c%2B%2B/ani2png.c
// build with: g++ ani2png.cpp -o ani2png

// How is a .ico file formatted:
// TODO

// How is a .png file formatted:
// TODO

// How is a .xcf file formatted:
// TODO

#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <memory>
#include <vector>
#include <cstring>

const auto usageInfo = "$ ani2png FILE.ani [PNG_FILE_OUTPUT_DIR] [PNG_FILE_PREFIX]";

/**
 * @brief Read binary file to vector
 * @param filePath The filepath of the .ani file to be read
 * @return Binary data of ani file
 */
std::shared_ptr<std::vector<uint8_t>> readBinaryFile(const std::filesystem::path& filePath);
void writeBinaryFile(const std::filesystem::path& filePath, const std::shared_ptr<std::vector<uint8_t>>& data);

struct AniFileHeader {
    uint32_t cbSizeOf; // Num bytes in AniHeader (36 bytes)
    uint32_t cFrames; // Number of unique Icons in this cursor
    uint32_t cSteps; // Number of Blits before the animation cycles
    uint32_t cx, cy; // reserved, must be zero.
    uint32_t cBitCount, cPlanes; // reserved, must be zero.
    uint32_t JifRate; // Default Jiffies (1/60th of a second) if rate chunk not present.
    uint32_t flags; // Animation Flag (see AF_ constants)
};

struct AniFileInformation {
    std::vector<std::shared_ptr<std::vector<uint8_t>>> icons = {};
    std::string art = {};
    std::string name = {};
    AniFileHeader header = {};
};

AniFileInformation parseAniFileData(const std::shared_ptr<std::vector<uint8_t>>& data);

void printIcoDataHeader(const std::shared_ptr<std::vector<uint8_t>>& data);


int main(int argc, char **argv)
{
    std::string aniFileName;
    std::string pngFileOutputDir = "./";
    std::string pngFilePrefix = "out";

    if (argc <= 1 || argc > 4) {
        std::cout << usageInfo << std::endl;
        return -1;
    }
    if (argc == 4) {
        aniFileName = argv[1];
        pngFileOutputDir = argv[2];
        pngFilePrefix = argv[3];
    } else if (argc == 3) {
        aniFileName = argv[1];
        pngFileOutputDir = argv[2];
    } else if (argc == 2) {
        aniFileName = argv[1];
    }
    const auto dataBytes = readBinaryFile(aniFileName);
    const auto aniFileInformation = parseAniFileData(dataBytes);

    for (std::size_t iconCounter = 0; iconCounter < aniFileInformation.icons.size(); iconCounter++) {
        printIcoDataHeader(aniFileInformation.icons.at(iconCounter));
        writeBinaryFile(aniFileName + "_" + std::to_string(iconCounter) + ".ico", aniFileInformation.icons.at(iconCounter));
    }
    writeBinaryFile(aniFileName + ".ani", dataBytes);
    return 0;
}

std::shared_ptr<std::vector<uint8_t>> readBinaryFile(const std::filesystem::path& filePath)
{
    if (!std::filesystem::exists(filePath)) {
        throw std::runtime_error("The file '" + filePath.string() + "' was not found");
    }
    // Load the file into memory
    const auto fileSize = std::filesystem::file_size(filePath);
    std::cout << "The file size of " << filePath << " is " << fileSize << std::endl;
    char* buffer = new char[fileSize];
    std::ifstream aniInputFile(filePath, std::ios::in | std::ios::binary);
    if (!aniInputFile.is_open()) {
        throw std::runtime_error("The file '" + filePath.string() + "' could not be opened");
    }
    aniInputFile.read(buffer, fileSize);
    aniInputFile.close();
    auto sharedBufferVec = std::make_shared<std::vector<uint8_t>>(fileSize);
    sharedBufferVec->reserve(fileSize);
    for (unsigned long i = 0; i < fileSize; i++) {
        sharedBufferVec->at(i) = static_cast<uint8_t>(buffer[i]);
    }
    delete[] buffer;
    return sharedBufferVec;
}

void writeBinaryFile(const std::filesystem::path& filePath, const std::shared_ptr<std::vector<uint8_t>>& data)
{
    std::ofstream pngOutputFile(filePath, std::ios::out | std::ios::trunc | std::ios::binary);
    if (!pngOutputFile.is_open()) {
        throw std::runtime_error("The file '" + filePath.string() + "' could not be opened");
    }
    for (const auto& entry: *data) {
        pngOutputFile.write(reinterpret_cast<const char*>(&entry), sizeof(char));
    }
    pngOutputFile.close();

    // Debugging:
    const std::filesystem::path filePathText = filePath.string() + ".txt";
    std::ofstream pngOutputFileText(filePathText, std::ios::out | std::ios::trunc);
    if (!pngOutputFileText.is_open()) {
        throw std::runtime_error("The file '" + filePathText.string() + "' could not be opened");
    }
    std::string charNumber;
    for (const auto& entry: *data) {
        pngOutputFile.write(reinterpret_cast<const char*>(&entry), sizeof(char));
        charNumber = "=" + std::to_string(std::stoi(std::to_string(entry))) + "\n";
        pngOutputFileText.write(charNumber.c_str(), charNumber.size());
    }
    pngOutputFileText.close();
}

uint32_t parseDwordToUnsignedInt(const std::shared_ptr<std::vector<uint8_t>>& data, const size_t start)
{
    uint32_t dwordValue;
    uint8_t bytes[4]{ data->at(start), data->at(start +1), data->at(start + 2), data->at(start + 3) };
    std::memcpy(&dwordValue, bytes, sizeof(uint32_t));
    return dwordValue;
}

int16_t parseWordToUnsignedInt(const std::shared_ptr<std::vector<uint8_t>>& data, const size_t start)
{
    int16_t wordValue;
    uint8_t bytes[2]{ data->at(start), data->at(start +1) };
    std::memcpy(&wordValue, bytes, sizeof(int16_t));
    return wordValue;
}

std::string parseCharToString(const std::shared_ptr<std::vector<uint8_t>>& data, const std::size_t start, const uint32_t length)
{
    std::string out = "";
    for (std::size_t i = start; i < start + length; i++) {
        out += static_cast<char>(data->at(i));
    }
    return out;
}

/**
 * RIFF/.ani file format:
 *
 * Start of the file should be this to be a valid .ani file:
 * "RIFF" {4 Bytes=DWORD=length of file} (container type and length)
 * "ACON" (what does the generic RIFF container represent)
 * Then in no particular order:
 * "LIST" {4 Bytes=DWORD=length of list} (TODO - not important?)
 * "INAM" {4 Bytes=DWORD=length of title} {title data in chars} (the title of the icon)
 * "IART" {4 Bytes=DWORD=length of author} {author data in chars} (the author of the icon)
 * "fram" (TODO - not important?)
 * "icon" {4 Bytes=DWORD=length of icon} {icon data} (TODO - IMPORTANT - WHAT IS THE DATA FORMAT???)
 * "anih" {4 Bytes=DWORD=length of ANI header (36 bytes)} {Data}
 *   > {4 Bytes=DWORD=cbSizeOf} (Number of unique Icons in this cursor)
 *   > {4 Bytes=DWORD=cFrames} (Num bytes in AniHeader)
 *   > {4 Bytes=DWORD=cSteps} (Number of Blits before the animation cycles)
 *   > {4 Bytes=DWORD=cx} (reserved, must be zero)
 *   > {4 Bytes=DWORD=cy} (reserved, must be zero)
 *   > {4 Bytes=DWORD=cBitCount} (reserved, must be zero)
 *   > {4 Bytes=DWORD=cPlanes} (reserved, must be zero)
 *   > {4 Bytes=DWORD=JifRate} (Default Jiffies (1/60th of a second) if rate chunk not present)
 *   > {4 Bytes=DWORD=flags} (Animation Flag - TODO?)
 * "rate" {4 Bytes=DWORD=length of rate block} {Data} (TODO - not important?)
 * "seq " {4 Bytes=DWORD=length of sequence block} {Data} (TODO - not important?)
 *
 * Sources are https://www.gdgsoft.com/anituner/help/aniformat.htm which cites a post by R. James Houghtaling
 * and the website www.wotsit.org by Paul Oliver which is not accessible any more.
 *
 * This parser is not a good one.
 * Without having any information about the actual .ani file format and what the differences are between
 * animated and not animated icons it just checks if it is an animated icon and finds all icon blocks.
 * If there are multiple INAM/IART blocks or something similar this parser WILL FAIL!!!!!
 *
 * @brief Parse .ani file information
 * @param data the loaded .ani file information buffer
 */
AniFileInformation parseAniFileData(const std::shared_ptr<std::vector<uint8_t>>& data)
{
    AniFileInformation aniFileInformation;
    if (8 <= data->size() && static_cast<char>(data->at(0)) == 'R' && static_cast<char>(data->at(1)) == 'I' && static_cast<char>(data->at(2)) == 'F' && static_cast<char>(data->at(3)) == 'F') {
        auto riffDataLength = parseDwordToUnsignedInt(data, 4);
        std::cout << "> Found RIFF identifier at the start with a data length of " << riffDataLength << std::endl;
    } else {
        throw std::runtime_error("Data did not start with RIFF container name");
    }
    if (12 <= data->size() && static_cast<char>(data->at(8)) == 'A' && static_cast<char>(data->at(9)) == 'C' && static_cast<char>(data->at(10)) == 'O' && static_cast<char>(data->at(11)) == 'N') {
        std::cout << "> Found ACON identifier after the RIFF identifier" << std::endl;
    } else {
        throw std::runtime_error("The RIFF container did not specify that it is an ACON");
    }

    for (std::size_t i = 12; i < data->size(); i ++) {
        if (i + 8 <= data->size() && static_cast<char>(data->at(i)) == 'I' && static_cast<char>(data->at(i + 1)) == 'N' && static_cast<char>(data->at(i + 2)) == 'A' && static_cast<char>(data->at(i + 3)) == 'M') {
            std::cout << "> Found 'INAM' at " << i;
            i += 4;
            auto length = parseDwordToUnsignedInt(data, i);
            std::cout << " [length=" << length << "]" << std::endl;
            i += 4;
            if (i + length <= data->size()) {
                auto content = parseCharToString(data, i, length);
                std::cout << "  > '" << content << "'" << std::endl;
                i += length;
                aniFileInformation.name = content;
            } else {
                throw std::runtime_error("Unexpected end of file while reading 'INAM' data");
            }
            i -= 1;
            continue;
        }
        if (i + 8 <= data->size() && static_cast<char>(data->at(i)) == 'I' && static_cast<char>(data->at(i + 1)) == 'A' && static_cast<char>(data->at(i + 2)) == 'R' && static_cast<char>(data->at(i + 3)) == 'T') {
            std::cout << "> Found 'IART' at " << i;
            i += 4;
            auto length = parseDwordToUnsignedInt(data, i);
            std::cout << " [length=" << length << "]" << std::endl;
            i += 4;
            if (i + length <= data->size()) {
                auto content = parseCharToString(data, i, length);
                std::cout << "  > '" << content << "'" << std::endl;
                i += length;
                aniFileInformation.art = content;
            } else {
                throw std::runtime_error("Unexpected end of file while reading 'IART' data");
            }
            i -= 1;
            continue;
        }
        if (i + 8 <= data->size() && static_cast<char>(data->at(i)) == 'i' && static_cast<char>(data->at(i + 1)) == 'c' && static_cast<char>(data->at(i + 2)) == 'o' && static_cast<char>(data->at(i + 3)) == 'n') {
            std::cout << "> Found 'icon' at " << i;
            i += 4;
            auto length = parseDwordToUnsignedInt(data, i);
            std::cout << " [length=" << length << "]" << std::endl;
            i += 4;
            auto iconData = std::make_shared<std::vector<uint8_t>>(length);
            iconData->reserve(length);
            for (std::size_t j = 0; j < length; j++) {
                iconData->at(j) = data->at(i + j);
            }
            aniFileInformation.icons.push_back(iconData);
            i += length;
            i -= 1;
            continue;
        }
        if (i + 8 <= data->size() && static_cast<char>(data->at(i)) == 's' && static_cast<char>(data->at(i + 1)) == 'e' && static_cast<char>(data->at(i + 2)) == 'q' && static_cast<char>(data->at(i + 3)) == ' ') {
            std::cout << "> Found 'seq ' at " << i;
            i += 4;
            auto length = parseDwordToUnsignedInt(data, i);
            std::cout << " [length=" << length << "]" << std::endl;
            i += 4;
            if (i + length <= data->size()) {
                auto content = parseCharToString(data, i, length);
                std::cout << "  Content: " << content << std::endl;
                i += length;
            } else {
                throw std::runtime_error("Unexpected end of file while reading 'seq ' data");
            }
            i -= 1;
            continue;
        }
        if (i + 8 <= data->size() && static_cast<char>(data->at(i)) == 'r' && static_cast<char>(data->at(i + 1)) == 'a' && static_cast<char>(data->at(i + 2)) == 't' && static_cast<char>(data->at(i + 3)) == 'e') {
            std::cout << "> Found 'rate' at " << i;
            i += 4;
            auto length = parseDwordToUnsignedInt(data, i);
            std::cout << " [length=" << length << "]" << std::endl;
            i += 4;
            if (i + length <= data->size()) {
                auto content = parseCharToString(data, i, length);
                std::cout << "  Content: " << content << std::endl;
                i += length;
            } else {
                throw std::runtime_error("Unexpected end of file while reading 'rate' data");
            }
            i -= 1;
            continue;
        }
        if (i + 8 <= data->size() && static_cast<char>(data->at(i)) == 'a' && static_cast<char>(data->at(i + 1)) == 'n' && static_cast<char>(data->at(i + 2)) == 'i' && static_cast<char>(data->at(i + 3)) == 'h') {
            std::cout << "> Found 'anih' at " << i;
            i += 4;
            auto length = parseDwordToUnsignedInt(data, i);
            std::cout << " [length=" << length << "]" << std::endl;
            i += 4;
            if (i + length <= data->size()) {
                aniFileInformation.header.cbSizeOf = parseDwordToUnsignedInt(data, i);
                i += 4;
                aniFileInformation.header.cFrames = parseDwordToUnsignedInt(data, i);
                i += 4;
                aniFileInformation.header.cSteps = parseDwordToUnsignedInt(data, i);
                i += 4;
                aniFileInformation.header.cx = parseDwordToUnsignedInt(data, i);
                i += 4;
                aniFileInformation.header.cy = parseDwordToUnsignedInt(data, i);
                i += 4;
                aniFileInformation.header.cBitCount = parseDwordToUnsignedInt(data, i);
                i += 4;
                aniFileInformation.header.cPlanes = parseDwordToUnsignedInt(data, i);
                i += 4;
                aniFileInformation.header.JifRate = parseDwordToUnsignedInt(data, i);
                i += 4;
                aniFileInformation.header.flags = parseDwordToUnsignedInt(data, i);
                i += 4;
            } else {
                throw std::runtime_error("Unexpected end of file while reading 'anih' data");
            }
            i -= 1;
            continue;
        }
        if (i + 8 <= data->size() && static_cast<char>(data->at(i)) == 'L' && static_cast<char>(data->at(i + 1)) == 'I' && static_cast<char>(data->at(i + 2)) == 'S' && static_cast<char>(data->at(i + 3)) == 'T') {
            std::cout << "> Found 'LIST' at " << i;
            i += 4;
            auto length = parseDwordToUnsignedInt(data, i);
            std::cout << " [length=" << length << "]" << std::endl;
            i += 4;
            i -= 1;
            continue;
        }
        if (i + 4 <= data->size() && static_cast<char>(data->at(i)) == 'f' && static_cast<char>(data->at(i + 1)) == 'r' && static_cast<char>(data->at(i + 2)) == 'a' && static_cast<char>(data->at(i + 3)) == 'm') {
            std::cout << "> Found 'fram' at " << i << std::endl;
            i += 4;
            i -= 1;
            continue;
        }
        throw std::runtime_error("PARSE PROBLEM: Unexpected input detected at pos " + std::to_string(i) + " ('" + static_cast<char>(data->at(i)) + "')");
    }
    return aniFileInformation;
}

/**
 * Source: https://en.wikipedia.org/wiki/ICO_(file_format)
 *
 * All values in ICO/CUR files are represented in little-endian byte order.
 *
 * Header
 * -------
 * ICONDIR structure:
 *
 * | Offset# | Size | Purpose
 * | 0       | 2    | Reserved. Must always be 0.
 * | 2       | 2    | Specifies image type: 1 for icon (.ICO) image, 2 for cursor (.CUR) image. Other values are invalid.
 * | 4       | 2    | Specifies number of images in the file.
 *
 * Structure of image directory:
 *
 * Image #1 	Entry for the first image
 * Image #2 	Entry for the second image
 * ...
 * Image #n 	Entry for the last image
 *
 * ICONDIRENTRY structure:
 *
 * | Offset# | Size | Purpose
 * | 0       | 1    | Specifies image width in pixels. Can be any number between 0 and 255. Value 0 means image width is 256 pixels.
 * | 1       | 1    | Specifies image height in pixels. Can be any number between 0 and 255. Value 0 means image height is 256 pixels.
 * | 2       | 1    | Specifies number of colors in the color palette. Should be 0 if the image does not use a color palette.
 * | 3       | 1    | Reserved. Should be 0.[Notes 2]
 * | 4       | 2    | In ICO format: Specifies color planes. Should be 0 or 1.[Notes 3] In CUR format: Specifies the horizontal coordinates of the hotspot in number of pixels from the left.
 * | 6       | 2    | In ICO format: Specifies bits per pixel. [Notes 4] In CUR format: Specifies the vertical coordinates of the hotspot in number of pixels from the top.
 * | 8       | 4    | Specifies the size of the image's data in bytes
 * | 12      | 4    | Specifies the offset of BMP or PNG data from the beginning of the ICO/CUR file

 * @brief printIcoDataHeader
 * @param data
 */
void printIcoDataHeader(const std::shared_ptr<std::vector<uint8_t>>& data)
{
    std::cout << "Position | Size | Purpose    | Content" << std::endl;
    std::cout << "0        | 2    | reserved   | '" << parseWordToUnsignedInt(data, 0) << "'" << std::endl;
    std::cout << "2        | 2    | image type | '" << parseWordToUnsignedInt(data, 1) << "'" << std::endl;
    std::cout << "4        | 2    | image #    | '" << parseWordToUnsignedInt(data, 2) << "'" << std::endl;
}

