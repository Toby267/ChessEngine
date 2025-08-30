import chess, chess.pgn, sys

def pgn_to_epd(bookname):
    epds = set()
    pgn = open(bookname)
    while True:
        game = chess.pgn.read_game(pgn)
        if game is None:
            break
        board = game.board()
        for move in game.mainline_moves():
            board.push(move)
            epd = board.epd()
            if not epd in epds:
                epds.add(epd)

    epdname = bookname.replace(".pgn", ".epd")
    with open(epdname, "w") as f:
        for epd in epds:
            f.write(epd + "\n")
    print(f"Wrote the converted book to {epdname}.")


if __name__ == "__main__":
    if len(sys.argv) == 2 and sys.argv[1].endswith(".pgn"):
        pgn_to_epd(sys.argv[1])
    else:
        print(f"Usage: python {sys.argv[0]} <book.pgn>")
        print("\nConverts a .pgn into an .epd book, keeping the order intact.")
