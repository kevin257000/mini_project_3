#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <set>
#include <cstdlib>
#include <ctime>
using namespace std;


struct Point {
    int x, y;
    Point() : Point(0, 0) {}
    Point(float x, float y) : x(x), y(y) {}
    bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    bool operator!=(const Point& rhs) const {
        return !operator==(rhs);
    }
    Point operator+(const Point& rhs) const {
        return Point(x + rhs.x, y + rhs.y);
    }
    Point operator-(const Point& rhs) const {
        return Point(x - rhs.x, y - rhs.y);
    }
};
array<Point, 8> directions{{
    Point(-1, -1), Point(-1, 0), Point(-1, 1),
    Point(0, -1), /*{0, 0}, */Point(0, 1),
    Point(1, -1), Point(1, 0), Point(1, 1)
}};
const int SIZE = 8;
#define Board std::array<std::array<int, SIZE>, SIZE>
enum SPOT_STATE {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};
int player;
Board board;

struct Node {
    int value, child_num;
    
    Board state;
    Node *childs[64];
};
Node *head;
std::vector<Point> next_valid_spots;

int get_disc(Board b, Point p) { return b[p.x][p.y];}
bool is_spot_on_board(Point p) { return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;}
bool is_disc_at(Board b, Point p, int disc) {
    if (!is_spot_on_board(p))
        return false;
    if (get_disc(b,p) != disc)
        return false;
    return true;
}
bool is_spot_valid(Board b, Point center, int cur_player) {
    if (get_disc(b,center) != EMPTY)
        return false;
    for (Point dir: directions) {
        // Move along the direction while testing.
        Point p = center + dir;
        if (!is_disc_at(b, p, 3-cur_player))
            continue;
        p = p + dir;
        while (is_spot_on_board(p) && get_disc(b,p) != EMPTY) {
            if (is_disc_at(b, p, cur_player))
                return true;
            p = p + dir;
        }
    }
    return false;
}
vector<Point> get_valid_spots(Board b, int cur_player) {
    std::vector<Point> valid_spots;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            Point p = Point(i, j);
            if (b[i][j] != EMPTY)
                continue;
            if (is_spot_valid(b, p, cur_player))
                valid_spots.push_back(p);
        }
    }
    return valid_spots;
}

int get_value(Board b) {
    
    return 0;
}

Node *build_tree() {
    Node *node = new Node;
    
    
    return node;
}

Point maxmin_search(Board b) {
    Point p;
    return p;
}

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        Point p(x,y);
        next_valid_spots.push_back(p);
    }
}

void write_valid_spot(std::ofstream& fout) {
    int n_valid_spots = next_valid_spots.size();
    srand(time(NULL));
    // Choose random spot. (Not random uniform here)
    int index = (rand() % n_valid_spots);
    Point p = next_valid_spots[index];
    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
