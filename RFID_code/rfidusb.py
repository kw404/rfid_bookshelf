import time
import hid

# List available HID devices
devices = hid.enumerate()
print("devices:", devices)

# Find the specific HID device
device_info = next(
    (
        d
        for d in devices
        if d["vendor_id"] == 0x1A86
        and d["product_id"] == 0xE010
        and "kbd" not in d["path"]
    ),
    None,
)

if device_info:
    try:
        device = hid.device()
        device.open_path(device_info["path"])
        print("open success")

        # Send a feature report to the device (similar to your Node.js code)
        device.send_feature_report([0x00, 0xFF, 0xC7, 0x83, 0xCC, 0x30, 0x00])

        # Sleep for 200ms
        time.sleep(0.2)

        # Send other commands (similar to your Node.js code)
        device.write([0x00, 0x09, 0x53, 0x57, 0x00, 0x05, 0xFF, 0x24, 0x05, 0x07, 0x22])
        time.sleep(0.2)
        device.write([0x00, 0x09, 0x53, 0x57, 0x00, 0x05, 0xFF, 0x24, 0x02, 0x01, 0x2B])
        time.sleep(0.2)
        device.write([0x00, 0x09, 0x53, 0x57, 0x00, 0x05, 0xFF, 0x3F, 0x31, 0x80, 0x62])
        time.sleep(0.2)
        device.write([0x00, 0x09, 0x53, 0x57, 0x00, 0x05, 0xFF, 0x24, 0x0A, 0x01, 0x23])
        time.sleep(0.2)

        # Read data from the device (similar to your Node.js code)
        while True:
            data = device.read(64)  # Adjust the buffer size as needed
            if data:
                # Process data as needed
                print("tag:", data)

    except Exception as e:
        print("open failed:", str(e))
else:
    print("Device not found")
