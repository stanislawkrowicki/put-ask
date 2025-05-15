const getGameName = () => {
    return new URLSearchParams(window.location.search).get('game')
}

window.addEventListener('load', async () => {
    const games = await eel.get_games()()

    games.forEach((game) => {
        document.querySelector('#name').innerHTML = game
        
        const img = document.querySelector('#thumbnail')
        img.src = `./img/games/${game}.png`
        img.alt = game
    })

    addDevicesToSelect()
})

const play = () => {
    const selectedDevice = document.querySelector('#devices').value
    eel.start_game(getGameName(), selectedDevice)().then()
}

const addDevicesToSelect = () => {
    const select = document.querySelector('#devices')
    const devices = getLocalDevices()

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