window.addEventListener('load', () => {
    loadGames().then()
    loadDevicesList().then()
})

const loadGames = async () => {
    const games = await eel.get_games()()

    games.forEach((game) => {
        const a = document.createElement('a');
        a.href = `game.html?game=${game}`;
        a.className = 'game';
        
        const img = document.createElement('img');
        img.src = `img/games/${game}.png`;
        img.alt = game;
        img.className = 'game-img';

        const imgContainer = document.createElement('div');
        imgContainer.className = 'game-img-container';
        
        const overlayText = document.createElement('span');
        overlayText.className = 'game-overlay-text';
        overlayText.textContent = game;
        
        imgContainer.appendChild(img);
        imgContainer.appendChild(overlayText);
        a.appendChild(imgContainer);
        document.querySelector('#games').appendChild(a);
    })
}

const loadDevicesList = async() => {
    const devices = getLocalDevices()

    if (!devices || devices.length === 0) return 

    const devicesDiv = document.querySelector('#devices')

    devicesDiv.innerHTML = ''

    devices.forEach((deviceName) => {
        const div = document.createElement('div')
        div.className = 'device'

        const img = document.createElement('img')
        img.src = 'img/phone.svg'
        img.alt = deviceName

        const p = document.createElement('p')
        p.className = 'device-name'
        p.innerHTML = deviceName
        
        div.appendChild(img)
        div.appendChild(p)
        
        devicesDiv.appendChild(div)
    })
}