# BuzzFall
Our goal is to implement an alternative version of the action indie video game 
TowerFall Ascencion created by Matt Thorson for the CPU Ricoh 2A03 (MOS 6502).

The game is a 2 players offline combat where players can kill each other with arrows.

The game is developed in cc65 6502 assembly.

# How to compile and run

1) Install cc65
- Linux  
    ```
    sudo apt-get install -y cc65
    ```  
    ```
    yay -y cc65
    ```

- MacOS
    ```
    brew install cc65
    ```
2) Install fceux emulator
- Linux  
    ```
    sudo apt-get install fceux
    ```  
    ```
    pacman -y fceux
    ```
- MacOS
    ```
    brew install fceux
    ```
3) Run game
    ```
    git clone https://github.com/lcagustini/nosso-emulador-supimpa.git
    cd nosso-emulador-supimpa/buzzfall
    make
    fceux main.nes
    ```
