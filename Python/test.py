import serial
import time

from display import Display
from graphics import Graphics
from _hw_config import *
from wiringpi import *


def send_bitmap_spi(bitmap):
    def _chunks(l, n):
        """Yield successive n-sized chunks from l."""
        for i in range(0, len(l), n):
            yield l[i:i + n]
    
    for chunk in _chunks(bitmap, 4000):
        wiringPiSPIDataRW(SPI_CHANNEL, bytes(chunk))


def main():
    display = Display("/dev/ttyAMA0")
    graphics = Graphics(NUM_COLS, NUM_ROWS, NUM_LINES, 3000)
    static_graphics = Graphics(NUM_COLS, NUM_ROWS, NUM_LINES, NUM_COLS)
    static_mask = Graphics(NUM_COLS, NUM_ROWS, NUM_LINES, NUM_COLS)

    wiringPiSetupGpio()
    wiringPiSPISetup(SPI_CHANNEL, SPI_CLOCK)

    frame_index = 0
    last_update = 0
    
    graphics.text("DIESER TEXT WIRD DURCH DEN STATISCHEN TEXT UEBERDECKT", font="Luminator7_Bold", left=0, top=0)
    graphics.text("ES GIBT ALLERDINGS NOCH PROBLEME MIT DEM RAM", font="Luminator7_Bold", left=0, top=7)
    graphics.text("ABER PRINZIPIELL FUNKTIONIERT DAS SCHON GUT", font="Luminator7_Bold", left=0, top=14)
    graphics.text("MAL SCHAUEN, WIE ICH DAS BEHEBE", font="Luminator7_Bold", left=0, top=21)
    graphics.text("LANGER SCROLLTEXT ZUM TESTEN DER AUTOMATISCHEN LAENGENERKENNUNG DER ZEILE ABER DIE FRAGE IST DOCH OB DAS AUCH GEHT WENN DER TEXT ARSCHLANG IST", font="Luminator7_Bold", left=0, top=28)
    """
    static_graphics.text("STATIC TEXT", font="Luminator7_Bold", left=0, top=0)
    static_graphics.text("03.10.2018", font="Luminator7_Bold", halign='right', top=0)
    static_graphics.text("MITTENDRIN!", font="Luminator7_Bold", top=14)
    static_graphics.text(":D", font="Comic Sans MS Bold", size=20, halign='left', top=20)
    
    static_mask.rectangle([0, 0, 70, 6], fill=True)
    static_mask.line([70, 0, 71, 0])
    static_mask.line([70, 1, 72, 1])
    static_mask.line([70, 2, 73, 2])
    static_mask.line([70, 3, 74, 3])
    static_mask.line([70, 4, 75, 4])
    static_mask.line([70, 5, 76, 5])
    static_mask.line([70, 6, 77, 6])
    static_mask.rectangle([182, 0, 239, 6], fill=True)
    static_mask.rectangle([80, 14, 160, 20], fill=True)
    static_mask.line([75, 14, 80, 14])
    static_mask.line([75, 16, 80, 16])
    static_mask.line([75, 18, 80, 18])
    static_mask.line([75, 20, 80, 20])
    static_mask.line([160, 15, 165, 15])
    static_mask.line([160, 17, 165, 17])
    static_mask.line([160, 19, 165, 19])
    static_mask.rectangle([0, 21, 20, 34], fill=True)
    """
    fb_scroll = graphics.get_framebuffer_new()
    #fb_static = static_graphics.get_framebuffer()
    #fb_mask = static_mask.get_framebuffer()
    
    graphics.img.save("out_scroll.png")
    static_graphics.img.save("out_static.png")
    static_mask.img.save("out_mask.png")
    
    send_bitmap_spi(fb_scroll)
    
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