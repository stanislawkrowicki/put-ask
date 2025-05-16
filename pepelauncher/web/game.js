const getGameName = () => {
    return new URLSearchParams(window.location.search).get('game')
}

window.addEventListener('load', async () => {
    const game = getGameName()

    document.querySelector('#name').innerHTML = game
    
    const img = document.querySelector('#thumbnail')
    img.src = `./img/games/${game}.png`
    img.alt = game


    addDevicesToSelect()
    setInterval(addDevicesToSelect, FETCH_DEVICES_INTERVAL)

    eel.expose(onGameStart, 'on_game_start')
})

const play = () => {
    const selectedDevice = document.querySelector('#devices').value
    eel.start_game(getGameName(), selectedDevice)().then()
    document.querySelector('#play-button').setAttribute('disabled', true)
}

/* Exposed to EEL */
const onGameStart = () => {
    document.querySelector('#play-button').setAttribute('disabled', false)
}

let lastAddedDevicesJSON

const addDevicesToSelect = () => {
    const select = document.querySelector('#devices')
    const devices = getLocalDevices()

    const devicesJSON = JSON.stringify(devices)

    if (lastAddedDevicesJSON === devicesJSON) return

    lastAddedDevicesJSON = devicesJSON

    select.innerHTML = ''

    if (!devices || devices.length === 0) {    
        const option = document.createElement('option')
        option.value = 'No devices'
        option.textContent = 'No devices'
        select.appendChild(option)
        return
    }

    devices.forEach((deviceName) => {
        const option = document.createElement('option')
        option.value = deviceName
        option.textContent = deviceName
        select.appendChild(option)
    })
}
