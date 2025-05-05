import eel
import thumbnails
from pathlib import Path

GAMES_PATH = './games/'

eel.init('web')


@eel.expose
def get_games():
    games_directory = Path(GAMES_PATH)
    games = [directory.name for directory in games_directory.iterdir() if directory.is_dir()]
    return games


eel.start('index.html')


if __name__ == '__main__':
    thumbnails.copy_to_dist()