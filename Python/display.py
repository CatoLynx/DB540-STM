import serial


class Display:
    def __init__(self, port_name, baudrate = 115200, id = 1):
        self.port_name = port_name
        self.baudrate = baudrate
        self.id = id
        self.port = serial.Serial(port_name, baudrate=baudrate)
    
    def send_command(self, command, parameters = None):
        if parameters is None:
            parameters = []
        self.port.write(bytes([0xFF, self.id, len(parameters) + 1, command] + parameters))
    
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
    
    def set_destination_buffer(self, buf_index):
        return self.send_command(0x20, [buf_index])