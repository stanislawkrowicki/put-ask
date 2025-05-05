const getName = () => {
    return new URLSearchParams(window.location.search).get('game')
}

window.addEventListener('load', () => {
    document.getElementById('name').innerHTML = getName()
})