#include <iostream>
#include <iomanip>
#include <fstream>

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
    oss << std::hex << std::setw(4) << std::setfill('0') << crc;
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
        std::cout << "  CRC: " << current_crc << std::endl;

        std::ifstream file(filename, std::ios::binary);
        file.seekg(start == 0x0000 ? 0xFF00 : 0x1FF00);
        uint16_t saved_crc;
        file.read(reinterpret_cast<char*>(&saved_crc), sizeof(saved_crc));

        if (saved_crc == std::stoul(current_crc, nullptr, 16)) {
            std::cout << "  Текущее значение CRC совпадает с сохраненным" << std::endl;
        } else {
            std::cout << "  Текущее значение CRC: " << std::hex << std::setw(4) << std::setfill('0')
                      << saved_crc << std::endl;
            std::cout << "  Записать CRC? (y/n): ";
            std::string answer;
            std::cin >> answer;
            if (answer == "y") {
                std::ofstream outfile(filename, std::ios::binary | std::ios::in | std::ios::out);
                outfile.seekp(start == 0x0000 ? 0xFF00 : 0x1FF00);
                outfile.write(reinterpret_cast<const char*>(&crc), sizeof(crc));
                std::cout << "  Успешно" << std::endl;
            } else {
                std::cout << "  Не записано" << std::endl;
            }
        }
    }

    return 0;
}
