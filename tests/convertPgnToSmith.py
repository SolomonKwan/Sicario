import chess.pgn
import sys

def main(input, output):
    pgnFile = open(input, "r")
    moves = ""
    while True:
        game = chess.pgn.read_game(pgnFile)
        if game is None: break

        for move in game.mainline_moves():
            moves += str(move) + " "
        moves += "\n"
    output = open(output, "w")
    output.write(moves)
    output.close()

if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])