import os
import time
from threading import Thread
import select

class DroneControllerService:

    def __init__(self):
        self.available_ports = []
        self.serial_port = None
        self.fd = None
        self.hw_log_bytes = b""
        self.hw_logs = []
        self.listeners = []
        Thread(target=self.__task_read_hw_logs).start()
        Thread(target=self.__task_update_serial_ports).start()

    def __notify(self):
        for listener in self.listeners:
            listener()

    def __del__(self):
        self.disconnect()

    def __task_read_hw_logs(self):
        while True:
            if self.fd is None:
                time.sleep(0.1)
                continue
            
            try:
                ready, _, _ = select.select([self.fd], [], [], 1.0)  # 1-second timeout
                if not ready:
                    continue

                data = os.read(self.fd, 32)
                self.hw_log_bytes += data

                while b'\n' in self.hw_log_bytes:
                    log, self.hw_log_bytes = self.hw_log_bytes.split(b'\n', 1)
                    self.hw_logs.append(log.decode("utf-8"))

                while len(self.hw_logs) > 32:
                    self.hw_logs.pop(0)

                self.__notify()
            except Exception as e:
                print(f"Error reading hardware logs: {e}")

    def __task_update_serial_ports(self):
        while True:
            devices = os.listdir("/dev")
            serial_ports = []
            for device in devices:
                if (
                    device.startswith("ttyUSB")
                    or device.startswith("ttyACM")
                    or device.startswith("cu.usb")
                ):
                    serial_ports.append(f"/dev/{device}")
            serial_ports.sort()
            if serial_ports != self.available_ports:
                self.available_ports = serial_ports
                self.__notify()
            time.sleep(1.0)

    def __write(self, data):
        if self.fd is None:
            raise ValueError("Serial port is not connected")

        # Check input data length
        if len(data) > 31:
            raise ValueError("Data length must be less than 31 bytes")

        # Add padding
        if len(data) < 31:
            data += b"\x00" * (31 - len(data))
        assert len(data) == 31

        # Calculate checksum
        checksum = 0
        for byte in data:
            checksum ^= byte
        checksum = bytes([checksum])

        magic_bytes = b"\x55\xAA"
        data = magic_bytes + data + checksum
        os.write(self.fd, data)

    def list_ports(self):
        return self.available_ports

    def get_port(self):
        return self.serial_port
    
    def get_hw_logs(self):
        return self.hw_logs

    def connect(self, serial_port):
        self.disconnect()
        self.serial_port = serial_port
        self.fd = os.open(serial_port, os.O_RDWR | os.O_NOCTTY)
        self.set_baud_rate(9600)
        self.__notify()

    def disconnect(self):
        if self.fd is not None:
            os.close(self.fd)
            self.fd = None
            self.serial_port = None
            self.__notify()

    def set_baud_rate(self, baud_rate):
        os.system(f"stty -F {self.serial_port} {baud_rate}")

    def read_state(self):
        '''
        Reads the state of the controller.
        '''
        command_byte = 0x01
        timestamp_bytes = (0).to_bytes(4, byteorder="little")
        data = bytes([command_byte]) + timestamp_bytes
        self.__write(data)

    def set_throttle(self, throttle: float):
        '''
        Sets the throttle.

        available throttle range: 0.0 to 1.0
        '''
        command_byte = 0x02
        throttle = int(throttle * 2**16)
        if throttle >= 2**16:
            throttle = 2**16 - 1
        if throttle < 0:
            throttle = 0
        throttle_bytes = throttle.to_bytes(2, byteorder="little")
        data = bytes([command_byte]) + throttle_bytes
        self.__write(data)

    def set_pitch(self, pitch: float):
        '''
        Sets the pitch.

        available pitch range: -1.0 to 1.0
        '''
        command_byte = 0x03
        pitch = int((pitch + 1) / 2 * 2**16)
        if pitch >= 2**16:
            pitch = 2**16 - 1
        if pitch < 0:
            pitch = 0
        pitch_bytes = pitch.to_bytes(2, byteorder="little")
        data = bytes([command_byte]) + pitch_bytes
        self.__write(data)

    def set_roll(self, roll: float):
        '''
        Sets the roll.

        available roll range: -1.0 to 1.0
        '''
        command_byte = 0x04
        roll = int((roll + 1) / 2 * 2**16)
        if roll >= 2**16:
            roll = 2**16 - 1
        if roll < 0:
            roll = 0
        roll_bytes = roll.to_bytes(2, byteorder="little")
        data = bytes([command_byte]) + roll_bytes
        self.__write(data)