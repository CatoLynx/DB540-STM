import time

from display import Display
from graphics import Graphics
from _hw_config import *


def main():
    display = Display(CONTROL_PORT, spi_channel=SPI_CHANNEL, spi_clock=SPI_CLOCK)
    graphics = Graphics(NUM_COLS, NUM_ROWS, NUM_LINES, 3000)
    static_graphics = Graphics(NUM_COLS, NUM_ROWS, NUM_LINES, NUM_COLS)
    static_mask = Graphics(NUM_COLS, NUM_ROWS, NUM_LINES, NUM_COLS)

    frame_index = 0
    last_update = 0
    
    for line in range(5):
        display.set_scroll_step_x(line, 0)
        display.set_scroll_interval_x(line, 0)
        display.set_scroll_width(line, 0, 0)
        display.set_blink_interval_on(line, 0)
        display.set_blink_interval_off(line, 0)
        display.set_scroll_position_x(line, 0)
    
    last_time = 0
    while True:
        now = time.time()
        if(now - last_time > 1):
            graphics.clear()
            graphics.text("%H:%M:%S", font="NovaMono", size=80, timestring=True)
            display.send_graphics(graphics)
            last_time = now
    
    for line in range(5):
        display.set_scroll_step_x(line, 1)
        display.set_scroll_interval_x(line, 1)
        display.set_scroll_width(line, 240, 0)
        #display.set_blink_interval_on(line, 20)
        #display.set_blink_interval_off(line, 20)
    """
    time.sleep(1)
    
    for line in range(5):
        display.set_scroll_stop_position_x(line, line*7+4, 1)
    """
    """
    display.set_destination_buffer(2)
    time.sleep(0.1)
    send_bitmap_spi(fb_mask)
    time.sleep(0.1)
    
    display.set_destination_buffer(1)
    time.sleep(0.1)
    send_bitmap_spi(fb_static)
    time.sleep(0.1)
    
    display.set_destination_buffer(0)
    """

if __name__ == "__main__":
    main()