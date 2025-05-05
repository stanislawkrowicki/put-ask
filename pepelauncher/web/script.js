window.addEventListener('load', async () => {
    const games = await eel.get_games()()

    games.forEach((game) => {
        const a = document.createElement('a')
        a.href = `game.html?game=${game}`
        a.className = 'game'

        const img = document.createElement('img')
        img.src = `img/games/${game}.png`
        img.alt = game
        img.className = 'game-img'

        a.appendChild(img)
        document.querySelector('#games').appendChild(a)
    })
})