#include <string>
#include <vector>

#define BOARDSIZE 220
#define WHITE 0
#define BLACK 1


 struct Point {
    int32_t x = 0, y = 0;

    Point() = default;
    constexpr Point(int32_t x, int32_t y) : x(x), y(y) {}

    inline Point& operator-= (const Point &a) { x -= a.x; y -= a.y; return *this; }
    inline Point& operator+= (const Point &a) { x += a.x; y += a.y; return *this; }
    inline Point& operator*= (const float a) { x = static_cast<int32_t>(x * a); y = static_cast<int32_t>(y * a); return *this; }
    inline Point& operator/= (const int32_t a) { x /= a;   y /= a;   return *this; }
  };

  inline bool operator== (const Point &lhs, const Point &rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
  inline bool operator!= (const Point &lhs, const Point &rhs) { return !(lhs == rhs); }
  inline Point operator-  (Point lhs, const Point &rhs) { lhs -= rhs; return lhs; }
  inline Point operator-  (const Point &rhs) { return Point(-rhs.x, -rhs.y); }
  inline Point operator+  (Point lhs, const Point &rhs) { lhs += rhs; return lhs; }
  inline Point operator*  (Point lhs, const float a) { lhs *= a; return lhs; }
  inline Point operator/  (Point lhs, const int32_t a) { lhs /= a; return lhs; }

std::string status;

typedef struct piece_t {
	Point pos;
	char type;
	int selected;

} piece_t;
std::vector <piece_t> pieces;

typedef struct move_t { Point from; Point to; char piecetaken;} move_t;
std::vector <move_t> moves, movehistory;

// place pieces according to a line of notation 
void set_boardline(std::string boardline, int y) {
    piece_t piece;
    for (int x=0;x < 8; x++){
	    char p = boardline[x];
	    piece.pos = Point(x,y);
	    piece.type = p;
	    piece.selected = 0;
    	    pieces.push_back(piece);
    }
}

// Chess functions

bool vacant(Point pos) {
    for (auto piece: pieces) 
	    if (piece.pos == pos) return false;
    return true;
}

bool onboard(Point pos) { return (pos.x >= 0 && pos.y >= 0 && pos.x < 8 && pos.y < 8); }

bool takeenemy(Point from, Point dest) {
	// check if move is taking an enemy piece
	int myside = 0;
	int destside = -1;
	for (auto piece : pieces) {
		if (piece.pos == from) myside = isupper(piece.type);
		if (piece.pos == dest) destside = isupper(piece.type);
	}
	if (destside != -1 && destside != myside) return true;
	else return false;
}

void pawnaddmove (Point from,Point to) {
Point dest = from + to ;
   if (onboard(dest) && vacant(dest))
	    moves.push_back({from,dest});
}

bool addmove (Point from,Point to) {
Point dest = from + to ;
   if (onboard(dest) && vacant(dest)) {
	    moves.push_back({from,dest});
   	    return true;
	    }
   if (takeenemy(from,dest)){
	    moves.push_back({from,dest});
   	    return false;
   }
   return false;
}

void kingmoves(Point pos) {
	addmove (pos, Point(1,0));
	addmove (pos, Point(1,1));
	addmove (pos, Point(1,-1));
	addmove (pos, Point(0,1));
	addmove (pos, Point(-1,1));
	addmove (pos, Point(-1,0));
	addmove (pos, Point(-1,-1));
	addmove (pos, Point(0,-1));
}
void knightmoves(Point pos) {
	addmove (pos, Point(1,2));
	addmove (pos, Point(1,-2));
	addmove (pos, Point(-1,2));
	addmove (pos, Point(-1,-2));
	addmove (pos, Point(2,1));
	addmove (pos, Point(2,-1));
	addmove (pos, Point(-2,1));
	addmove (pos, Point(-2,-1));
}

void rookmoves(Point pos) {
	for (int n=1; n < 8 && addmove(pos,Point( 0, n)); n++);	// right
	for (int n=1; n < 8 && addmove(pos,Point( 0,-n)); n++);	// left
	for (int n=1; n < 8 && addmove(pos,Point( n, 0)); n++);	// up
	for (int n=1; n < 8 && addmove(pos,Point(-n, 0)); n++);	// down
}
void bishopmoves(Point pos) {
	for (int n=1; n < 8 && addmove(pos,Point( n,  n)); n++);// up-right
	for (int n=1; n < 8 && addmove(pos,Point( n, -n)); n++);// up-left
	for (int n=1; n < 8 && addmove(pos,Point( -n, n)); n++); //down-right
	for (int n=1; n < 8 && addmove(pos,Point( -n,-n)); n++); //down-left
}

bool valid_move(Point from, Point to) {
    for (auto move : moves) 
		if (move.from == from && move.to == to)  return true;
    return false;
}
void remove_piece(Point p){
	// find & remove piece at p
	int num;
    	for (num = 0; num < pieces.size(); num++)
	    	if (pieces[num].pos == p) break;
	pieces.erase(pieces.begin() + num);
}

void get_black_moves() {
    moves.clear();
    for (auto piece: pieces) {
	    if (piece.type == 'P') {
		    pawnaddmove (piece.pos, Point(0,1));
		    if (piece.pos.y == 1 && vacant(piece.pos + Point(0,1))) pawnaddmove(piece.pos,Point(0,2));
		    if (takeenemy(piece.pos,piece.pos+Point(1,1))) addmove(piece.pos,Point(1,1));
		    if (takeenemy(piece.pos,piece.pos+Point(-1,1))) addmove(piece.pos,Point(-1,1));
	    }
	    if (piece.type == 'K') kingmoves(piece.pos);
	    if (piece.type == 'R') rookmoves(piece.pos);
	    if (piece.type == 'N') knightmoves(piece.pos);
	    if (piece.type == 'B') bishopmoves(piece.pos);
	    if (piece.type == 'Q') {
		    rookmoves(piece.pos);
		    bishopmoves(piece.pos);
	    }
    }
}

void get_white_moves() {
    moves.clear();
    for (auto piece: pieces) {
	    if (piece.type == 'p') {
		    pawnaddmove (piece.pos, Point(0,-1));
		    if (piece.pos.y == 6 && vacant(piece.pos + Point(0,-1))) pawnaddmove(piece.pos,Point(0,-2));
		    if (takeenemy(piece.pos,piece.pos+Point(1,-1))) addmove(piece.pos,Point(1,-1));
		    if (takeenemy(piece.pos,piece.pos+Point(-1,-1))) addmove(piece.pos,Point(-1,-1));
	    }
	    if (piece.type == 'k') kingmoves(piece.pos);
	    if (piece.type == 'r') rookmoves(piece.pos);
	    if (piece.type == 'n') knightmoves(piece.pos);
	    if (piece.type == 'b') bishopmoves(piece.pos);
	    if (piece.type == 'q') {
		    rookmoves(piece.pos);
		    bishopmoves(piece.pos);
	    }
    }
}

bool inCheck(int side) { 
	char king;
	Point kingpos;
	//get opposing side moves 
	if (side == BLACK) { king = 'K'; get_white_moves(); } 
	else 		   { king = 'k'; get_black_moves(); }
	// get King position
	for (auto piece : pieces) 
		if (piece.type == king) kingpos = piece.pos;
	// if any valid moves would take the king, then you're in check
	for (auto move : moves) 
		if (move.to == kingpos) return true;
	return false;
}

move_t pick_computer_move(int side){
    if (side == WHITE) get_white_moves();
    else get_black_moves();

    //favour taking a piece if possible
    for (auto m: moves)  { if (takeenemy(m.from,m.to) && (rand() % 10 > 3)) return (m); }
    //or just pick any random valid move
    return (moves[rand() % moves.size()]);
}

void do_move(move_t move) {
    //remove enemy piece if taking move
    for (auto piece: pieces) 
	    if (piece.pos == move.to) {
		    move.piecetaken = piece.type;
		    remove_piece(move.to);
		    break;
	    	    }
    //move the piece
    for (auto &piece: pieces) 
	  if (piece.pos == move.from) {
		  piece.pos = move.to;
		  //pawn promotion if pawn has reached other side
		  if (piece.type == 'P' && piece.pos.y == 7) piece.type = 'Q';
		  if (piece.type == 'p' && piece.pos.y == 0) piece.type = 'q';
	  }
    movehistory.push_back(move);
}

void undo_last_move() {
	        if (!movehistory.size()) return;
		move_t lastmove = movehistory.back(); 
		//update piece positions
		for (auto &p : pieces) 
			if (p.pos == lastmove.to) p.pos = lastmove.from;
		// add taken piece back to board
		if (lastmove.piecetaken != 0) {
			piece_t p;
			p.type = lastmove.piecetaken;
			p.pos = lastmove.to;
			p.selected = 0;
			pieces.push_back(p);
		}
		movehistory.pop_back(); 
}

int do_computer_move(int side) {
int count = 0;
    while (1) {
	    	do_move(pick_computer_move(side));
    		if (inCheck(side)) undo_last_move(); else break;
		// checkmate if no more moves
		if (++count > 500) { status = "checkmate"; break; }
		}
    return !side;
}

void reset_game() {
    pieces.clear();
    // setup board
    set_boardline ( "RNBQKBNR",0);
    set_boardline ( "PPPPPPPP",1);
    set_boardline ( "pppppppp",6);
    set_boardline ( "rnbqkbnr",7);
}


int main() {
static int turn;

  reset_game();
  while (1) {
    if (turn == BLACK) { turn = do_computer_move(BLACK); }
    if (turn == WHITE) { turn = do_computer_move(WHITE); }

    char board[8][8];
    // clear board
    for (int x=0;x<8;x++) 
    	    for (int y=0;y<8;y++)
		board[y][x] = ' ';

    // fill in piece positions
    for (auto piece : pieces)
	    board[piece.pos.x][piece.pos.y] = piece.type;

    // print board
    for (int x=0;x<8;x++) {
	    printf("\n");
    	    for (int y=0;y<8;y++)
		printf("%c",board[y][x]);
    }
    printf("\n");

    int nextmove = scanf("%c");
}
}
