import struct
import binascii

filename = input("Введите имя файла: ")

ranges = [(0x0000, 0xFEFF), (0x10000, 0x1FEFF)]

for start, end in ranges:
    with open(filename, "rb") as f:
        print(f"Диапазон: 0x{start:04X}-0x{end:04X}:")

        f.seek(start)
        crc = 0
        for byte in bytearray(f.read(end - start + 1)):
            crc += byte
            byte_str = struct.pack('>H', crc & 0xFFFF)

        print(f"  CRC: {binascii.hexlify(byte_str).decode('utf-8')}")

    answer = input("Записать CRC? (y/n): ")
    if answer == "y":
        with open(filename, "r+b") as f:
            f.seek(0xFF00 if start == 0x0000 else 0x1FF00)
            f.write(byte_str)
            print("Успешно")
    else:
        print("Неудачно")
