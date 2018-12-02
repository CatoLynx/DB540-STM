import time
import datetime

from display import Display
from graphics import Graphics
from _hw_config import *

from googleapiclient.discovery import build
from httplib2 import Http
from oauth2client import file, client, tools


def main():
    display = Display(CONTROL_PORT, spi_channel=SPI_CHANNEL, spi_clock=SPI_CLOCK)
    graphics = Graphics(NUM_COLS, NUM_ROWS, NUM_LINES, 3000)
    static_graphics = Graphics(NUM_COLS, NUM_ROWS, NUM_LINES, NUM_COLS)
    static_mask = Graphics(NUM_COLS, NUM_ROWS, NUM_LINES, NUM_COLS)
    
    # Set up the Google Calendar API
    # The file token.json stores the user's access and refresh tokens, and is
    # created automatically when the authorization flow completes for the first
    # time.
    store = file.Storage('gapi_token.json')
    creds = store.get()
    if not creds or creds.invalid:
        flow = client.flow_from_clientsecrets('gapi_credentials.json', SCOPES)
        creds = tools.run_flow(flow, store)
    service = build('calendar', 'v3', http=creds.authorize(Http()))
    
    for line in range(5):
        display.set_scroll_step_x(line, 1)
        display.set_scroll_interval_x(line, 1)
        display.set_scroll_width(line, 30, 1)
        display.set_blink_interval_on(line, 0)
        display.set_blink_interval_off(line, 0)
        display.set_scroll_position_x(line, 0)
        display.set_scroll_enabled_x(line, 0)
        display.set_auto_scroll_enabled_x(line, 1)
        display.set_auto_scroll_reset_enabled_x(line, 1)
    
    last_display_update = 0 # Last display update timestamp
    last_calendar_update = 0 # Last calendar update timestamp
    last_calendar_entry_cycle = 0 # Timestamp when the displayed calendar entry last changed
    current_calendar_entry = 0 # Which of the upcoming calendar entries is currently being displayed
    while True:
        now = time.time()
        
        if now - last_calendar_update > 600:
            last_calendar_update = now
            # Call the Google Calendar API
            dt_now = datetime.datetime.utcnow().isoformat() + 'Z' # 'Z' indicates UTC time
            events_result = service.events().list(calendarId='primary', timeMin=dt_now,
                                                maxResults=5, singleEvents=True,
                                                orderBy='startTime').execute()
            calendar_events = events_result.get('items', [])
    
        if now - last_calendar_entry_cycle > 5:
            last_calendar_entry_cycle = now
            current_calendar_entry %= len(calendar_events)
            if calendar_events:
                event = calendar_events[current_calendar_entry]
                
                event_start = event.get('start')
                has_start_time = 'dateTime' in event_start
                if has_start_time:
                    start_dt = datetime.datetime.strptime(event_start.get('dateTime')[:19] + event_start.get('dateTime')[19:].replace(":", ""), "%Y-%m-%dT%H:%M:%S%z")
                else:
                    start_dt = datetime.datetime.strptime(event_start.get('date'), "%Y-%m-%d")
                
                event_end = event.get('end')
                has_end_time = 'dateTime' in event_end
                if has_end_time:
                    end_dt = datetime.datetime.strptime(event_end.get('dateTime')[:19] + event_end.get('dateTime')[19:].replace(":", ""), "%Y-%m-%dT%H:%M:%S%z")
                else:
                    # Substract one day because the API reports the end as one day later than it actually is...
                    end_dt = datetime.datetime.strptime(event_end.get('date'), "%Y-%m-%d") - datetime.timedelta(days = 1)
                
                event_text = ""
                if has_start_time:
                    event_text += start_dt.strftime("%d.%m. %H:%M")
                else:
                    event_text += start_dt.strftime("%d.%m.")
                
                if has_end_time:
                    if end_dt.date() == start_dt.date():
                        event_text += "-" + end_dt.strftime("%H:%M")
                    else:
                        event_text += "-" + end_dt.strftime("%d.%m. %H:%M")
                else:
                    if end_dt.date() != start_dt.date():
                        event_text += "-" + end_dt.strftime("%d.%m.")
                event_text += " " + event.get('summary').upper()
            else:
                event_text = "KEINE ANSTEHENDEN EREIGNISSE"
            current_calendar_entry += 1
        
        if now - last_display_update > 1:
            last_display_update = now
            graphics.clear()
            graphics.text("%H:%M:%S", halign='left', valign='top', font="Luminator7_Bold", timestring=True)
            graphics.text("INFOSCREEN", halign='center', valign='top', font="Luminator7_Bold")
            graphics.text("%d.%m.%Y", halign='right', valign='top', font="Luminator7_Bold", timestring=True)
            graphics.text(event_text, halign='left', top=7, font="Luminator7_Bold")
            display.send_graphics(graphics)


if __name__ == "__main__":
    main()