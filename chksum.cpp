#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <sstream>

std::string calculateCRC(const std::string& filename, size_t start, size_t end) {
    std::ifstream file(filename, std::ios::binary);
    file.seekg(start);

    uint16_t crc = 0;

    for (size_t i = start; i <= end; i++) {
        uint8_t byte;
        file.read(reinterpret_cast<char*>(&byte), sizeof(byte));
        crc += byte;
        crc &= 0xFFFF;
    }

    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << std::nouppercase << std::hex << crc;
    return oss.str();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Ошибка: укажите файл" << std::endl;
        return 1;
    }

    std::string filename = argv[1];

    std::vector<std::pair<size_t, size_t>> ranges = {{0x0000, 0xFEFF}, {0x10000, 0x1FEFF}};

    for (const auto& range : ranges) {
        size_t start = range.first;
        size_t end = range.second;

        std::cout << "Диапазон: 0x" << std::hex << std::setw(4) << std::setfill('0') << start << "-0x"
                  << std::hex << std::setw(4) << std::setfill('0') << end << ":" << std::endl;

        std::string current_crc = calculateCRC(filename, start, end);
        std::cout << "  Текущая: " << current_crc << std::endl;

        std::ifstream file(filename, std::ios::binary);
        file.seekg(start == 0x0000 ? 0xFF00 : 0x1FF00);
        uint16_t saved_crc;
        file.read(reinterpret_cast<char*>(&saved_crc), sizeof(saved_crc));

        uint16_t saved_crc_little_endian = (saved_crc >> 8) | (saved_crc << 8);

        if (saved_crc_little_endian == std::stoul(current_crc, nullptr, 16)) {
            std::cout << "  Контрольная сумма верна." << std::endl;
        } else {
            std::cout << "  Сохраненная в файле: " << std::hex << std::setw(4) << std::setfill('0')
                      << std::uppercase << std::noshowbase << saved_crc_little_endian << std::endl;
            std::cout << "  Исправить? (y/n): ";
            std::string answer;
            std::cin >> answer;
            if (answer == "y" || answer == "Y") {
                uint16_t crc_to_write = std::stoul(current_crc, nullptr, 16);
                crc_to_write = (crc_to_write >> 8) | (crc_to_write << 8);

                std::ofstream file(filename, std::ios::binary | std::ios::in | std::ios::out);
                file.seekp(start == 0x0000 ? 0xFF00 : 0x1FF00);
                file.write(reinterpret_cast<const char*>(&crc_to_write), sizeof(crc_to_write));
                file.close();

                std::cout << "  Успех" << std::endl;
            } else {
                std::cout << "  Неудача" << std::endl;
            }
        }
    }

    return 0;
}
