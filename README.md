# ENGG1340 Game Project Group 11

## Group Members
1. Alvin Alexander Widjaja&nbsp;&nbsp;&nbsp;&nbsp;@alvinalexanderw
2. Bradley Wilson&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;@bradleywilss
3. Deandro Elbert Brasali&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;@debrasali
4. Dong Jiajun&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;@dong060111
5. Hu Xiaoxuan&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;@Hu06xuan

## A Memory Card Game
This game is a memorization based game derived from the Concentration card game, where players will flip over two cards each turn. If the cards have matching images, those cards will be flipped over until the rest of the game, turns will continue until the end of the game when all of the cards are paired.

## Our Interpretation
Our game is very similar to the initial concept, however our game uses modernized emojis as images for the cards. Each pair of cards that are correct will grant the player coins, which can be exchanged for various powerups to assist the user in completing the game! 

## How to run the game
1. **Downloading the game**

     As this game is on github you can download it using this commmand below :
    ``` 
    git clone https://github.com/debrasali/ENGG1340-Group-11
    ```
    This will create a folder called ENGG1340-Group-11 with all of the files the game needed to run.

2. **Checking the game files**

    After downloading the game, you can check it with the command ``` ls ``` or ```./ls ``` on the terminal
    This should display the folder ``` ENGG1340-Group-11 ``` in the list of directories :open_file_folder:.


4. **Changing directory to the game folder**

    In order to run the game, you need to change the terminal directory through this following command :
    ```
    cd ENGG1340-Group-11
    ```
    To check if you are in the correct directory, the command ``` ls ``` will show the files of the game such as ``` memory_main.cpp ``` , ``` animation.cpp ``` and      others.
  
5. **Building and running the game**

     The game has a special file called a Makefile to make it easier to compile and run the game.

   To build and run the game you only need to type in this command :
     ```
     make run
     ```
     This will automatically take you into the main menu of the game and you can start playing!

## Game Features

1. **Generation of random events** : The game will generate a board of cards of a fixed size, with each card having randomly matched with another within the board

2. **Data structures for storing data**: Vectors and arrays are used in the logic of matching the cards with each other. 

3. **Dynamic memory management**:The game uses vector as a dynamic memory management system to store the cards in a grid, allowing for easy space manipulation. 

4. **File input/output**: File I/O is handled in loading the game and saving highscores on to a ``` .txt ``` file, saves are also saved in a file. 

5. **Program codes in multiple files**: Animations and memory utility for the game is stored in separate files to not clutter the main ``` memory-main.cpp ``` file 

6. **Multiple Difficulty Levels**: The game has multiple difficulty levels, with each level having a different board size.

   - Easy **(3x4)**
   - Medium **(5x6)**
   - Hard **(7x8)**
   - Extreme **(9x10)**
     
8. **Powerups**: After completing the game, the player will receive coins that can be exchanged for powerups for their next playthrough. Currently there are 2 powerups:

   - Quick Peek (50 Coins) : Allows the player to see all of the cards for a brief moment
   - Count manipulator (100 Coins) : The next step taken by the player doesn't count, allowing the player to achieve a higher score with lesser move count.

## How To Play The Game

1. **Starting the Game** : After running the game with make run, you will be greeted with the main menu.
   
   - Select 1 to start a **New Game**
   - Select 2 to **Load Game** (if you have a saved progress)
   - Select 3 to **View High Score**
   - Select 4 to **Exit the game**
  
2. **Selecting Difficulty**
   The game offers four difficulty levels with different board sizes:
   - **Easy (3x4)** - Perfect for beginners
   - **Medium (5x6)** - A balanced challenge
   - **Hard (7x8)** - For experienced players
   - **Extreme (9x10)** - Only for the bravest souls
   
   Each difficulty affects:
   - Number of cards to match
   - Coin rewards (more difficult = more coins)
   - High score potential


