const getName = () => {
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
})

const play = () => {
    eel.start_game(getName())().then()
}