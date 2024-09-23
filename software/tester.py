import struct
import time

import serial
from serial.tools import list_ports


def unpack_data(data):
    fmt = "="
    for i in range(len(data)//2):
        fmt += "H"

    return struct.unpack(fmt, data)


class DeviceNotFound(Exception):
    """Cannot find device"""

class SerialManager:
    """Wrapper class for pyserial"""

    def __init__(self, device, baud, device_number=0, print_devices=False) -> None:
        self.baud = baud
        self.device = None
        ports = list_ports.comports()
        for port in ports:
            if print_devices:
                print(f"Device: {port.device} : {port.description}")

            is_device = port.description == device

            if is_device:
                if device_number == 0:
                    self.device = port.device
                    if not print_devices:
                        break

                device_number -= 1

        if self.device:
            self.ser = serial.Serial(self.device, self.baud)
        else:
            raise DeviceNotFound(f"{device} not found")

    def read_bytes(self, length):
        """Waits for length bytes recieved"""
        return self.ser.read(length)



manager = SerialManager("soft-hand", baud=115200)
while True:
    data = manager.read_bytes(6)
    values = unpack_data(data)
    print(values)

    time.sleep(0.001)