# mahjong-solitaire-algorithm
engine independent game logic in C++

mahjong solitaire games usually provide players a shuffle button, besides, from a developer's perspective, a shuffle function can also be used to generate the initial game board tiles.
there're 2 conditions for a shuffle function:
1. arrange the board as random as you can
2. make it solvable without further shuffle if there's one solution


here I use a depth first tree search algorithm.
1. the current board is the root state.
2. list all currently exposed tiles in an array, shuffle it.
3. detach a pair of tiles array[0] & array[1], current board is a child state.
4. repeat 2~3, if all tiles are detached, you get the solution sequence, assign the tiles accordingly. otherwise if you meet a dead end(only one exposed tile left), take one step back, try detaching array[0] & array[2] ...... to array[n-1] & array[n], until you get the solution.
