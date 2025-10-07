#Modified from https://github.com/official-stockfish/books/
import chess, chess.pgn, sys

def split(bookname):
    pgn = open(bookname)
    games = {}
    while True:
        game = chess.pgn.read_game(pgn)
        if game is None:
            break
        count = 0
        for _ in game.mainline_moves():
            count += 1
        if count not in games:
            games[count] = []
        games[count].append(game)

    for key, val in games.items():
        with open(bookname.replace(".pgn", "_" + str(key) + "_moves.pgn"), "w") as f:
            for game in val:
                f.write(str(game) + "\n\n")
            f.close()

    print(f"Wrote the parsed books to {bookname.replace(".pgn", "_<i>_moves.pgn")}")

if __name__ == "__main__":
    if len(sys.argv) == 2 and sys.argv[1].endswith(".pgn"):
        split(sys.argv[1])
    else:
        print(f"Usage: python {sys.argv[0]} <book.pgn>")
        print("\nGenerates separate files for 0.5, 1.0, 1.5, ... move games")
