import eel
import thumbnails
from pathlib import Path
import esp_client
import asyncio

GAMES_PATH = './games/'

eel.init('web')

@eel.expose
def get_games():
    games_directory = Path(GAMES_PATH)
    games = [directory.name for directory in games_directory.iterdir() if directory.is_dir()]
    return games

@eel.expose
def discover_devices():
    return esp_client.mock_discover_devices()

@eel.expose
def start_game(game: str):
    device_ip = ''
    firmware_path = Path(GAMES_PATH) / game / 'firmware.bin'
    esp_client.mock_upload_firmware(device_ip, firmware_path)
    print('Game started!')

eel.start('index.html')

if __name__ == '__main__':
    thumbnails.copy_to_dist()
