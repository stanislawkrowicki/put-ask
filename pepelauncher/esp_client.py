import socket
import time
import requests

DISCOVERY_MESSAGE = b"DISCOVER_PPBOYS"
BROADCAST_IP = '255.255.255.255'
UDP_PORT = 25545

DISCOVERY_TIME = 3

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
sock.settimeout(DISCOVERY_TIME)

def discover_devices() -> tuple[str, str]:
    DISCOVERY_TIME = 3
    
    sock.sendto(DISCOVERY_MESSAGE, (BROADCAST_IP, UDP_PORT))
    
    start = time.time()
    devices = []


    while time.time() - start < DISCOVERY_TIME:
        try:
            data, addr = sock.recvfrom(1024)
            devices.append((data.decode(), addr[0]))
        except socket.timeout:
            break

    return devices


def upload_firmware(device_ip: str, firmware_path: str) -> tuple[int, str]:
    with open(firmware_path, 'rb') as f:
        response = requests.post(f'http://{device_ip}/update', files={'file': f})

    return (response.status_code, response.text)


"""
Returns:
tuple: 0: device name, 1: device ip
"""
def mock_discover_devices() -> tuple[str, str]:
    return [
        ('PepeBoy 1', '192.168.1.113'),
        ('PePeBoy 2', '192.168.1.155')
    ]


def mock_upload_firmware(device_ip: str, firmware_path: str) -> tuple[int, str]:
    print(f'Uploading {firmware_path} to {device_ip}')
    time.sleep(1)
    return (200, 'Update success')
