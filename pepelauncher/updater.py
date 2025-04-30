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

sock.sendto(DISCOVERY_MESSAGE, (BROADCAST_IP, UDP_PORT))

def discover_devices():
    DISCOVERY_TIME = 3
    start = time.time()
    devices = []

    while time.time() - start < DISCOVERY_TIME:
        try:
            data, addr = sock.recvfrom(1024)
            devices.append((data.decode(), addr[0]))
        except socket.timeout:
            break

    print(devices)

def upload_firmware(device_ip: str, firmware_path: str):
    with open(firmware_path, 'rb') as f:
        response = requests.post(f'http://{device_ip}/update', files={'file': f})

    print(response.text)

if __name__ == '__main__':
    # discover_devices()
    upload_firmware('192.168.1.138', './games/example.bin')
    sock.close()