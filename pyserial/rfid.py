import serial
import time
import requests

COM_PORT = "COM10"  # 指定通訊埠名稱
BAUD_RATES = 115200  # 設定傳輸速率
ser = serial.Serial(COM_PORT, BAUD_RATES)  # 初始化序列通訊埠

print(ser.is_open)

data = []
timing = time.time()
last_data = [
    "0x43",
    "0x54",
    "0x0",
    "0x1a",
    "0x0",
    "0x45",
    "0x1",
    "0xd3",
    "0xc",
    "0x22",
    "0x12",
    "0x21",
    "0x4e",
    "0x44",
    "0x1",
    "0xd",
    "0x1",
    "0x1",
    "0x69",
    "0x95",
    "0x20",
    "0x0",
    "0x50",
    "0x12",
    "0xc9",
    "0xb5",
    "0x6e",
    "0x93",
    "0x0",
    "0x34",
]
time_del = 0
time_counter = 0
in_shelf = []
booklist = [
    "AABB00112233445522222222",
    "AABB00112233445511111111",
    "AABB00112233445533333333",
    "AABB00112233445544444444",
    "AABB00112233445555555555",
]

try:
    ser.write([0x53, 0x57, 0x00, 0x05, 0xFF, 0x24, 0x05, 0x0B, 0x1E])
    time.sleep(1)
    while ser.inWaiting():
        data.append(ser.read())
    time.sleep(0.2)
    time_count = 0
    while True:
        data = []
        ser.write([0x53, 0x57, 0x00, 0x03, 0xFF, 0x45, 0x0F])
        time.sleep(0.2)
        while ser.inWaiting():
            data.append(ser.read())  # 讀取設定後的回傳資料

        if data != []:
            time_del = time.time() - timing
            for i in range(0, len(data)):
                data[i] = hex(int.from_bytes(data[i], byteorder="little"))
            print(data == last_data)
            print(time_del <= 3)
            if data[27] == last_data[27] and time_del <= 3:
                time_counter = time.time() - time_count
                time_count = timing
            else:
                last_data = data
                timing = time.time()
                time_count = timing
                # tag_count = int(data[14], 0)
                # for i in range(0,tag_count-1):
                #     tag_long = int(data[15], 0)
                #     tag_buf = ""
                #     for y in range():
                #         tag_buf += data[14+tag_count]
                booknum = int(data[27], 0) - 1
                book = booklist[booknum]
                requests.post(
                    "https://zhuan-ti-hou-duan.onrender.com//bookTouchShelf",
                    data={"book_id": book, "touchedShelf": "A1"},
                )
                print(book)
        else:
            if time_counter > 3 and time_count != 0:
                time_count = 0
                booknum = int(last_data[27], 0) - 1
                book = booklist[booknum]
                requests.post(
                    "https://zhuan-ti-hou-duan.onrender.com//bookTouchShelf",
                    data={"book_id": book, "touchedShelf": "A1"},
                )
                print(book, "timeout")
except KeyboardInterrupt:
    ser.close()
