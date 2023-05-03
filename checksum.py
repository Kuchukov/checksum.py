import sys
import struct
import binascii

if len(sys.argv) != 2:
    print("Ошибка: укажите файл")
    sys.exit(1)

filename = sys.argv[1]

ranges = [(0x0000, 0xFEFF), (0x10000, 0x1FEFF)]

for start, end in ranges:
    with open(filename, "rb") as f:
        print(f"Диапазон: 0x{start:04X}-0x{end:04X}:")

        f.seek(start)
        crc = 0
        for byte in bytearray(f.read(end - start + 1)):
            crc += byte
            byte_str = struct.pack('>H', crc & 0xFFFF)

        current_crc = binascii.hexlify(byte_str).decode('utf-8')
        print(f"  CRC: {current_crc}")

    with open(filename, "rb") as f:
        f.seek(0xFF00 if start == 0x0000 else 0x1FF00)
        saved_crc = f.read(2)

    if saved_crc == byte_str:
        print("  Текущее значение CRC совпадает с сохраненным значением.")
    else:
        current_crc = binascii.hexlify(saved_crc).decode('utf-8')
        print(f"  Текущее значение CRC: {current_crc}")
        answer = input("  Записать CRC? (y/n): ")
        if answer == "y":
            with open(filename, "r+b") as f:
                f.seek(0xFF00 if start == 0x0000 else 0x1FF00)
                f.write(byte_str)
                print("  Успешно записано новое значение CRC.")
        else:
            print("  Не записано новое значение CRC.")
