#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    const std::string filename = argv[1];
    const uint32_t RANGE_1_START = 0x0000;
    const uint32_t RANGE_1_END = 0xFEFF;
    const uint32_t RANGE_2_START = 0x10000;
    const uint32_t RANGE_2_END = 0x1FEFF;

    uint32_t checksum1 = 0;
    for (uint32_t i = RANGE_1_START; i <= RANGE_1_END; ++i) {
        std::ifstream file(filename, std::ios::binary);
        file.seekg(i);
        char byte;
        file.get(byte);
        checksum1 += static_cast<uint32_t>(byte);
        file.close();
    }

    uint32_t checksum2 = 0;
    for (uint32_t i = RANGE_2_START; i <= RANGE_2_END; ++i) {
        std::ifstream file(filename, std::ios::binary);
        file.seekg(i);
        char byte;
        file.get(byte);
        checksum2 += static_cast<uint32_t>(byte);
        file.close();
    }

    std::fstream file(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return 1;
    }

    file.seekp(0xFF00);
    file.write(reinterpret_cast<char*>(&checksum1), sizeof(checksum1));

    file.seekp(0x1FF00);
    file.write(reinterpret_cast<char*>(&checksum2), sizeof(checksum2));

    file.close();

    return 0;
}