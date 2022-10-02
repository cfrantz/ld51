# Ludum Dare 51

This is my submission for Ludum Dare 51: _Lil' Digger 3: It's a Trilogy!_.

Like many of my previous Ludum Dare submissions, this game is a game for
the 8-bit Nintendo Entertainment System (aka the NES).

In Lil' Digger 3, you must guide _Dignan_ through various mazes.  In
accordance with the theme of LD51 ("Every 10 seconds"), if you don't
make it to the exit in time, the world resets and a new maze is
generated.

This game uses the VRC7 mapper and includes music composed for the sound
expansion chip in the VRC7 mapper.  To play, you'll need either an
emulator or a flash-cart and audio-modded NES.

# Uncompleted Work

- I had hoped to add an item that _Dignan_ could acquire which would extend
  the 10 second timer and prevent regeneration of the maze.
- I also wanted to add enemies and some basic weapons.

Unfortunately, neither of these are in place because:

1. I'm rusty.  I haven't written any NES code in several months and I haven't
   written a complete NES game since LD43.
2. I prioritized adding music since non of my previous NES games had music.
3. I refactored my `empty-nes` project for writing NES games while waiting for
   inspiration regarding the theme to strike.

# Technical Details

- This game is written in a combination of C and 6502 assembly language.
- Mazes are generated randomly using the [Growing Tree](https://weblog.jamisbuck.org/2011/1/27/maze-generation-growing-tree-algorithm.html)
  algorithm.
- The random number generator is a Galois LFSR.
- The VRC7 audio expansion is a cost-reduced version of the Yamaha OPL chip
  used in classic Adlib and Sound Blaster cards.

The Galois generator has a long period, but a lot of the mazes look the
same.  I'd hoped to investigate a different construction for generating
random numbers.

I've implemented the most basic version of the Growing Tree algorithm.  It
would be interesting to implement some of the selection options disussed in
the linked article to mazes with different characteristics.
