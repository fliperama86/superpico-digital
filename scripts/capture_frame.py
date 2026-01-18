#!/usr/bin/env python3
"""
Capture a single frame from SuperPico Digital via serial
"""
import serial
import serial.tools.list_ports
import time
import sys
from PIL import Image

def find_pico():
    for p in serial.tools.list_ports.comports():
        if 'usbmodem' in p.device.lower():
            return p.device
    return None

def capture_frame(port=None, output="frame.png"):
    if port is None:
        port = find_pico()
        if port is None:
            print("No Pico found")
            return False

    print(f"Connecting to {port}...")
    ser = serial.Serial(port, 115200, timeout=5)
    time.sleep(0.5)

    # Drain any pending data
    if ser.in_waiting:
        ser.read(ser.in_waiting)

    print("Sending capture command...")
    ser.write(b'C')
    ser.flush()

    # Wait for and read text responses until FRAME_START
    width, height = 0, 0
    print("Waiting for frame header...")

    while True:
        line = ser.readline().decode('utf-8', errors='replace').strip()
        print(f"  < {line}")

        if line.startswith("FRAME_START:"):
            parts = line.split(":")
            width = int(parts[1])
            height = int(parts[2])
            print(f"Frame dimensions: {width}x{height}")
            break
        elif "ERROR" in line:
            print("Capture failed!")
            ser.close()
            return False

    # Calculate expected bytes (1 bit per pixel, packed)
    bytes_per_line = width // 8
    total_bytes = bytes_per_line * height
    print(f"Expecting {total_bytes} bytes of image data...")

    # Read binary data
    data = b''
    start_time = time.time()
    while len(data) < total_bytes:
        if time.time() - start_time > 10:
            print(f"Timeout! Only got {len(data)}/{total_bytes} bytes")
            break
        chunk = ser.read(min(256, total_bytes - len(data)))
        if chunk:
            data += chunk
            print(f"  Received {len(data)}/{total_bytes} bytes", end='\r')

    print(f"\nReceived {len(data)} bytes")

    # Wait for FRAME_END
    time.sleep(0.2)
    while ser.in_waiting:
        line = ser.readline().decode('utf-8', errors='replace').strip()
        print(f"  < {line}")

    ser.close()

    if len(data) < total_bytes:
        print("Incomplete frame data!")
        return False

    # Convert to image (1-bit packed -> grayscale)
    print("Converting to image...")
    img = Image.new('L', (width, height))
    pixels = img.load()

    for y in range(height):
        for bx in range(bytes_per_line):
            byte = data[y * bytes_per_line + bx]
            for bit in range(8):
                x = bx * 8 + bit
                if x < width:
                    pixel_on = (byte >> (7 - bit)) & 1
                    pixels[x, y] = 255 if pixel_on else 0

    img.save(output)
    print(f"Saved to {output}")

    # Also show
    try:
        img.show()
    except:
        pass

    return True

if __name__ == "__main__":
    output = sys.argv[1] if len(sys.argv) > 1 else "frame.png"
    capture_frame(output=output)
