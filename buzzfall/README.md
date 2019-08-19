# BuzzFall
Our goal is to implement a alternative version of the action indie video game 
TowerFall Ascencion created by Matt Thorson for CPU Ricoh 2A03 (MOS 6502).

The game will be a 2 players offline combat where players can kill each other
either with arrows or head-stomps. Player will get your character picked by
answering some easy quiz questions.

The game will be develop with assembly 6502, thus will only suport 2D graphics.

# How to compile and run

1) Install cc65
- Linux
```sudo apt-get install -y cc65 ```
```yay -y cc65 ```

- MacOS
    ```
    git clone https://github.com/cc65/cc65.git
    cd cc65
    make
    ```
2) Install fceux emulator
- Linux
    ```
    sudo apt-get install fceux
    pacman -y fceux    
    ```
- MacOS
    ```
    brew install fceux
    ```
3) Run game
    ```
    git clone https://github.com/lcagustini/nosso-emulador-supimpa.git
    cd nosso-emulador-supimpa
    make
    fceux main.nes
    ```
