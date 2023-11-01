import requests

b = 1
while b == 1:
    a = input()
    a = str(a)
    c = requests.get(
        "https://zhuan-ti-hou-duan.onrender.com/bookTouchShelf",
        params={"rfid": a, "touchShelf": "A1"},
    )
    print(c)
    print(a)
