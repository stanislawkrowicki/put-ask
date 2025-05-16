import subprocess
import shutil
from pathlib import Path

TARGET_DEVICE = 'esp32doit-devkit-v1'

source_root = Path(__file__).resolve().parent.parent / 'software' / 'games'
destination_root = Path(__file__).resolve().parent / 'games'

for game_dir in source_root.iterdir():
    if not game_dir.is_dir():
        continue

    print(f"\n\n\n *** Building game: {game_dir.name}\n")

    try:
        subprocess.run(['pio', 'run'], cwd=game_dir, check=True)

        firmware_path = game_dir / '.pio' / 'build' / TARGET_DEVICE / 'firmware.bin'

        if not firmware_path.exists():
            print(f"Firmware not found for {game_dir.name}, skipping.")
            continue

        dest_dir = destination_root / game_dir.name
        dest_dir.mkdir(parents=True, exist_ok=True)
        dest_path = dest_dir / 'firmware.bin'

        shutil.copy2(firmware_path, dest_path)
        print(f"Firmware copied to {dest_path}")

    except subprocess.CalledProcessError:
        print(f"ERROR: Build failed for {game_dir.name}\n\n\n")
