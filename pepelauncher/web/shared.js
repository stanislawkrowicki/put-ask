const FETCH_DEVICES_INTERVAL = 5 * 1000

let lastFetchedDevicesJSON = ''

window.addEventListener('load', async () => {
    await fetchDevices()
    setInterval(fetchDevices, FETCH_DEVICES_INTERVAL);
})

const fetchDevices = async () => {
    const deviceNames = await eel.get_devices()()
    const devicesJSON = JSON.stringify(deviceNames)

    if (lastFetchedDevicesJSON === devicesJSON)
        return

    lastFetchedDevicesJSON = devicesJSON
    localStorage.setItem('devices', devicesJSON)

    if (loadDevicesList)
        loadDevicesList()
}

const getLocalDevices = () => {
    return JSON.parse(localStorage.getItem('devices'))
}
