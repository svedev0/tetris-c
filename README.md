# tetris-c

A Tetris game written in C for the Windows terminal.

### Introduction

There are many versions of terminal Tetris for Linux and MacOS. However, at the time of writing, there are very few for Windows users. The aim of this project is not to be a complete product, but to hopefully spark some curiosity and interest. Fork this repository, modify it, make it your own, and share what you have made!

### Overview

This repository contains two versions of the game. The first `tetris.c` main purpose is to showcase the basic logic behind the game in only 230 lines of code. This basic version makes the games as simple as possiple using only the hash symbol and whitespaces to display the tetrominoes.

The second version `tetris-ext.c` (extended) is an overall improvment of the game and is more visually appealing, albeit with more lines of code than the basic version. The purpose of this version is to be fun to play and have "graphics" that are more faithful to the original game. This version takes a different approach to drawing tetrominoes and makes use of ANSI escape codes to render DOS-compatible box-drawing characters instead of just ASCII codes.

This game has been tested on Windows 10 and 11. It is recommended to run this game in the [Windows Terminal](https://www.microsoft.com/store/productId/9N0DX20HK701).

### Build and run

Using the C compiler of your choice (e.g. GCC or Clang), compile the basic or the extended version. Then, run the game.

```Shell
clang tetris.c -o tetris.exe
.\tetris.exe
```

or

```Shell
clang tetris-ext.c -o tetris-ext.exe
.\tetris-ext.exe
```

### Demos

Basic version

<img src="https://raw.githubusercontent.com/svedev0/tetris-c/master/img/tetris.gif" width="288" height="432" />

Extended version

<img src="https://raw.githubusercontent.com/svedev0/tetris-c/master/img/tetris-ext.gif" width="288" height="432" />
