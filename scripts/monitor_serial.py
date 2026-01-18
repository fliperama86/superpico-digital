#!/usr/bin/env python3
import serial
import serial.tools.list_ports
import sys
import time

def find_pico_port():
    """Auto-detect Pico serial port"""
    ports = serial.tools.list_ports.comports()
    for p in ports:
        if "usbmodem" in p.device.lower() or "acm" in p.device.lower():
            return p.device
    return None

def monitor(port=None, baudrate=115200):
    if port is None:
        port = find_pico_port()
        if port is None:
            print("No Pico found. Available ports:")
            for p in serial.tools.list_ports.comports():
                print(f"  {p.device}: {p.description}")
            return

    print(f"Connecting to {port}...")
    try:
        ser = serial.Serial()
        ser.port = port
        ser.baudrate = baudrate
        ser.timeout = 0.1
        ser.dtr = False  # Disable DTR to prevent reset
        ser.rts = False  # Disable RTS too
        ser.open()
        print(f"Connected (DTR disabled)! Monitoring... (Ctrl+C to stop)\n")

        while True:
            try:
                # Read any available data
                if ser.in_waiting:
                    data = ser.read(ser.in_waiting)
                    text = data.decode('utf-8', errors='replace')
                    print(text, end='', flush=True)
                else:
                    time.sleep(0.01)
            except serial.SerialException as e:
                print(f"\nSerial error: {e}")
                print("Reconnecting...")
                time.sleep(1)
                try:
                    ser.close()
                    ser = serial.Serial(port, baudrate, timeout=0.1)
                    print("Reconnected!")
                except:
                    pass
            except KeyboardInterrupt:
                print("\nStopped.")
                break
    except Exception as e:
        print(f"Failed to open {port}: {e}")

if __name__ == "__main__":
    port = sys.argv[1] if len(sys.argv) > 1 else None
    monitor(port)