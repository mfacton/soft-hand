import struct

import FingerMeasure
import rospy
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


def micro_reader():
    manager = SerialManager("soft-hand", baud=115200)
    rospy.init_node('micro_reader', anonymous=True)

    finger_index = rospy.Publisher('finger_index/measure', FingerMeasure, queue_size=10)
    finger_middle = rospy.Publisher('finger_middle/measure', FingerMeasure, queue_size=10)
    finger_ring = rospy.Publisher('finger_ring/measure', FingerMeasure, queue_size=10)
    finger_little = rospy.Publisher('finger_little/measure', FingerMeasure, queue_size=10)
    finger_thumb = rospy.Publisher('finger_thumb/measure', FingerMeasure, queue_size=10)

    rospy.loginfo("Started publishing angles")

    while True:
        data = manager.read_bytes(30)
        values = unpack_data(data)
        # print(values)

        angles_index = FingerMeasure()
        angles_index.angles = [values[0], values[1], values[2]]
        finger_index.publish(angles_index)

        angles_middle = FingerMeasure()
        angles_middle.angles = [values[3], values[4], values[5]]
        finger_middle.publish(angles_middle)

        angles_ring = FingerMeasure()
        angles_ring.angles = [values[6], values[7], values[8]]
        finger_ring.publish(angles_ring)

        angles_little = FingerMeasure()
        angles_little.angles = [values[9], values[10], values[11]]
        finger_little.publish(angles_little)

        angles_thumb = FingerMeasure()
        angles_thumb.angles = [values[12], values[13], values[14]]
        finger_thumb.publish(angles_thumb)


if __name__ == '__main__':
    try:
        micro_reader()
    except rospy.ROSInterruptException:
        pass