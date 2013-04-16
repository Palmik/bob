This describes the protocol for Connect 6 in a nutshell.
All outgoing and incoming messages should end with `\n`.

  1. _Send_ `<string>` -- your name.
  2. _Read_ `<number> <number>` -- size of the board.
  2. _Read_ `"white"` or `"black"` -- your color.
  3. Take turns as you would usually do in Connect 6 based on your color.
This is how turn should proceed:
    1. _Read_ `<over>` or `"play"` -- either it is the end or the game goes on. 
    2. In case it is your turn: _Send_ `<number> <number>` -- your move.
    3. In case it is your opponent's turn: _Read_ `<number> <number>` -- your opponent's move.

The `<over>` has the following format `"over [d, w, l]; *"` where
`'d'` stands for _draw_, `'l'` stands for _loss_ and `'w'` stands for win.
This message may also include the reason why the game ended and the ID of the replay.

