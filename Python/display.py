import serial
import time
from wiringpi import *


class Display:
    def __init__(self, port_name, baudrate = 115200, id = 1, spi_channel = 0, spi_clock = 2000000):
        self.port_name = port_name
        self.baudrate = baudrate
        self.id = id
        self.spi_channel = spi_channel
        self.spi_clock = spi_clock
        self.port = serial.Serial(port_name, baudrate=baudrate)
        wiringPiSetupGpio()
        wiringPiSPISetup(spi_channel, spi_clock)

    def send_bitmap_spi(self, bitmap):
        def _chunks(l, n):
            # Yield successive n-sized chunks from l
            for i in range(0, len(l), n):
                yield l[i:i + n]
        
        for chunk in _chunks(bitmap, 4000):
            wiringPiSPIDataRW(self.spi_channel, bytes(chunk))
    
    def send_graphics(self, graphics_instance):
        return self.send_bitmap_spi(graphics_instance.get_framebuffer())
    
    def send_command(self, command, parameters = None):
        if parameters is None:
            parameters = []
        self.port.write(bytes([0xFF, self.id, len(parameters) + 1, command] + parameters))
        time.sleep(0.01)
    
    def clear_screen(self):
        return self.send_command(0x01)
    
    def single_scroll_x(self, line_index, amount, direction = 0):
        return self.send_command(0x10, [line_index, amount, direction])
    
    def set_scroll_step_x(self, line_index, amount, direction = 0):
        return self.send_command(0x11, [line_index, amount, direction])
    
    def set_scroll_interval_x(self, line_index, interval):
        return self.send_command(0x12, [line_index, interval >> 8, interval & 0xFF])
    
    def set_scroll_width(self, line_index, width, relative = 0):
        return self.send_command(0x13, [line_index, width >> 8, width & 0xFF, relative])
    
    def set_scroll_position_x(self, line_index, position, negative = 0):
        return self.send_command(0x14, [line_index, position >> 8, position & 0xFF, negative])
    
    def set_scroll_stop_position_x(self, line_index, position, negative = 0):
        return self.send_command(0x15, [line_index, position >> 8, position & 0xFF, negative])
    
    def set_auto_scroll_enabled_x(self, line_index, enabled):
        return self.send_command(0x16, [line_index, enabled])
    
    def set_auto_scroll_reset_enabled_x(self, line_index, enabled):
        return self.send_command(0x17, [line_index, enabled])
    
    def set_scroll_enabled_x(self, line_index, enabled):
        return self.send_command(0x18, [line_index, enabled])
    
    def set_blink_interval(self, line_index, interval):
        return self.send_command(0x20, [line_index, interval >> 8, interval & 0xFF])
    
    def set_blink_interval_on(self, line_index, interval):
        return self.send_command(0x21, [line_index, interval >> 8, interval & 0xFF])
    
    def set_blink_interval_off(self, line_index, interval):
        return self.send_command(0x22, [line_index, interval >> 8, interval & 0xFF])
    
    def set_destination_buffer(self, buf_index):
        return self.send_command(0x80, [buf_index])