import eel
from pathlib import Path
import esp_client
import asyncio
import threading
import sys
import platform

GAMES_PATH = './games/'

devices = []

### EEL ###
eel.init('web')

@eel.expose
def get_games():
    games_directory = Path(GAMES_PATH)
    games = [directory.name for directory in games_directory.iterdir() if directory.is_dir()]
    return games

@eel.expose
def get_devices():
    global devices
    # Return names only
    return [d[0] for d in devices]

@eel.expose
def start_game(game: str, device_name: str):
    device_ip = ''
    
    for device in devices:
        if device[0] == device_name:
            device_ip = device[1]
    
    if device_ip == '':
        print(f'There is no device with name {device_name}!')
        return
    
    firmware_path = Path(GAMES_PATH) / game / 'firmware.bin'

    if not firmware_path.exists:
        print(f'There is no firmware for game {game}!')
        return
    
    esp_client.upload_firmware(device_ip, firmware_path)
    eel.on_game_start()
    print('Game started!')

### UTILS ###
def discover_devices():
    global devices
    devices = esp_client.discover_devices()

async def discover_devices_loop():
    global devices
    SLEEP_TIME = 10

    while True:
        discover_devices()
        await asyncio.sleep(SLEEP_TIME)

def start_discovery_loop():
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    loop.run_until_complete(discover_devices_loop())


if __name__ == '__main__':
    discover_devices()
    threading.Thread(target=start_discovery_loop, daemon=True).start()
    try:
        # Default open on Chrome / Chromium
        eel.start('index.html')
    except EnvironmentError:
        # If Chrome isn't found, fallback to Microsoft Edge on Win10 or greater
        # and to Firefox on Linux
        if sys.platform == 'win32' and int(platform.release()) >= 10:
            eel.start('index.html', mode='edge')
        elif sys.platform == 'linux':
            eel.start('index.html', mode='firefox')
        else:
            raise