import shutil
from pathlib import Path

SOURCE_PATH = Path("./games")
DESTINATION_PATH = Path("./web/img/games")

DESTINATION_PATH.mkdir(parents=True, exist_ok=True)


def copy_to_dist():
    for img_path in SOURCE_PATH.rglob('*/thumbnail.png'):
        parent_dir_name = img_path.parent.name
        new_filename = f'{parent_dir_name}.png'
        target_path = DESTINATION_PATH / new_filename

        shutil.copy(str(img_path), str(target_path))


if __name__ == '__main__':
    copy_to_dist()