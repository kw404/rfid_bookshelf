import serial
import requests

COM_PORT = "COM6"  # 指定通訊埠名稱
BAUD_RATES = 9600  # 設定傳輸速率
ser = serial.Serial(COM_PORT, BAUD_RATES)  # 初始化序列通訊埠

print(ser.is_open)

data = []

try:
    while True:
        data = []
        while ser.inWaiting():
            data.append(ser.readline())  # 讀取設定後的回傳資料

        if data != []:
            for i in range(len(data), 0):
                data[i] = hex(int.from_bytes(data[i], byteorder="little"))
            data_decode = data[0].decode("utf-8").strip()
            print(data_decode)
            requests.get(
                "https://zhuan-ti-hou-duan.onrender.com//bookTouchShelf",
                params={"rfid": data_decode, "touchedShelf": "A"},
            )
except KeyboardInterrupt:
    ser.close()
