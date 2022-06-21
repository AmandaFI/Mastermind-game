# Cliente-Server Mastermind Game in C

The project is a client-server version of the traditional game Mastermind entirely implemented in the C programming language. This version was named LockPick.

## Server details

The server implemented in the server.c file uses the TCP/IP communication protocol and runs in the port number 5001. The fork() method was implemented, therefore the server supports multiple clients connected at the same time playing their respectives games.

## Client details

The only important feature present in the client implemented in the client.c file is the need to inform the IP adress and the port in which the server is runnnig. The IP adressmust be informed in the Ipv4 format and the port is the 5001 as mentioned above.

## Game rules

The user must guess a password constitued by a sequence of 4 digits from 0 to 9 (repetitions are allowed). To acomplish this task the user will have 12 attempts and after each attempt the server will provide a clue based on the submited guess. The clue informs how many digits of the guess where correct digits in the correct position, how many where correct digits in a incorrect position and how many digits were incorrect and are not present in the password to be guessed. To start the game the user need to type in the 'start' command and to exit the game at any time the user just need to typoe the 'quit' command.

## Getting Started

To run this project some prerequisites are necessary:

```
Linux operational system
gcc compiler
```
To clone the repository run:
```
# git clone https://github.com/AmandaFI/Mastermind-game.git
```

To play the game first you need to open a terminal inside the directory where the server.c file is stored and run the following commands:

```
# gcc server.c -o server
# ./server
```
Next it is necessary to open a terminal inside the directory where the client.c file is stored and run the following commands:

```
# gcc client.c -o client
# ./client
```
After these steps you just need to follow the instructions presented by the game and have fun!