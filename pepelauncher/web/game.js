const getName = () => {
    return new URLSearchParams(window.location.search).get('game')
}

window.addEventListener('load', async () => {
    const games = await eel.get_games()()

    games.forEach((game) => {
        const container = document.querySelector('#container')

        const heading = document.createElement('h1')
        heading.id = 'name'
        heading.innerHTML = game.name

        const img = document.createElement('img')
        img.src = `./img/games/${game.thumbnail}`
        img.alt = game.name

        container.appendChild(heading)
        container.appendChild(img)

    })
})