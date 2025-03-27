# Wack Attack - 2D Shooter Game with Ducks

**Wack Attack** is a 2D multiplayer shooter game featuring ducks. The game follows a server-client architecture using **Crow**, **CPR**, **nlohmann JSON**, and **SDL2**. Players fight against each other in an arena, and the last duck standing wins. The game supports up to 4 players per match and features a variety of upgrades and gameplay mechanics.

## Overview

In **Wack Attack**, players control ducks in a battle to be the last one alive. Each duck can move around the map and shoot at opponents. Players can upgrade their stats through a menu system and are rewarded with points for eliminating other ducks. The game features a login screen where user data, scores, and upgrades are saved in a database. Additionally, bombs spawn randomly on the map, destroying walls and providing life-saving floatation devices.The game also features background music and sound effects to enhance the gameplay experience.

## Key Features

✔ **2D Shooter Gameplay**: Control ducks in an arena-style battle to eliminate opponents.  
✔ **Upgrades**: Players can upgrade their bullet speed and reload speed by spending points earned through kills.  
✔ **Server-Client Architecture**: Built using **Crow** for the server, **CPR** for HTTP requests, **nlohmann JSON** for data handling, and **SDL2** for graphics.  
✔ **Max 4 Players**: Support for up to 4 players per match.  
✔ **Login Screen**: Players can log in, and their scores and upgrades are saved to a database.If an account already exists, the data is retrieved automatically.  
✔ **Movement and Shooting**: Use **WASD** to move and **Spacebar** to shoot.  
✔ **Bombs and Floatation Devices**: Bombs randomly spawn, destroying surrounding walls and providing life-saving floatation devices.  
✔ **Kill Rewards**: Players are rewarded with points for eliminating other ducks, which can be used for upgrades.  
✔ Music and Sound Effects: Immersive background music and sound effects enhance the gameplay experience.  

## Game Controls

- **WASD**: Move your duck around the map.
- **Spacebar**: Shoot at other ducks.
- **Bombs**: Bombs spawn randomly on the map, destroying walls and providing life-saving floatation devices.

## Gameplay

The goal of **Wack Attack** is to be the last duck alive. At the start of each game, players log in and their data (including score and upgrades) is saved to the database. Players can upgrade their statistics through the in-game menu. The map features bombs that explode and destroy nearby walls and floatation devices that give players extra life. Players earn points for killing other ducks, which can then be spent on upgrading their bullet speed or reload speed.

## How to Run

### Install Dependencies

1. **Crow**: A C++ micro web framework.
2. **CPR**: A C++ Requests library for HTTP requests.
3. **nlohmann JSON**: A header-only library for JSON manipulation.
4. **SDL2**: A cross-platform library used for graphics and input handling.
   - **SDL_image**: For loading and handling images.
   - **SDL_mixer**: For handling audio and sound effects.
   - **SDL_ttf**: For rendering text using TrueType fonts.
5. **SQLite**: Used for saving player data, scores, and upgrades.

### Clone or Download the Project

Clone or download the repository to your local machine.

```bash
 git clone https://github.com/Tatu-Denis-Dimitrie/Wack-Attack
```

### Build the Project

Use a C++ IDE (such as Visual Studio or Code::Blocks) to compile the project.
Make sure to link the necessary libraries for Crow, CPR, nlohmann JSON, and SDL2.

### Run the Server

Start the server on your machine to handle player connections and manage game data.

### Start the Game

Run the game client and log in with your credentials.
Press PLAY (a minimum of 2 players to be ready is required)
Use **WASD** to move and **Spacebar** to shoot at other ducks.
Play the game and try to be the last duck standing!

## Future Enhancements

- **Add more maps**: Introduce new arenas with different layouts and obstacles.
- **Custom Duck Skins**: Allow players to customize their duck appearance.
- **Game Modes**: Implement different game modes, such as team-based matches or timed challenges.
- **Leaderboard**: Display a global leaderboard with the best scores and achievements.
- **Power-ups**: Add additional power-ups such as temporary shields or enhanced weapons.

## Contributing

If you would like to contribute to **Wack Attack**, feel free to fork the repository, create a new branch, and submit a pull request. Whether it's fixing bugs, adding new features, or enhancing the game's performance, contributions are always welcome!

## Acknowledgments

Special thanks to TeamTBD, of which I am proud to be a part, for our collective hard work and dedication in developing Wack Attack!