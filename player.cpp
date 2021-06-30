#include <iostream>
#include <algorithm>
#include <fstream>
#include <array>
#include <vector>
#include <set>
#include <cstdlib>
#include <ctime>
using namespace std;

const int SIZE = 8;
#define Board std::array<std::array<int, SIZE>, SIZE>
#define MAX 2147483647

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
enum SPOT_STATE {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};
int player;
Board board;

struct Node {
    int value, child_num;
    int x, y;
    Board state;
    Node *childs[640];
    Node(Board s): child_num(0) {state = s;};
    Node(Board s, Point p): child_num(0), x(p.x), y(p.y){state = s;};
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
            if (board[i][j] != EMPTY)
                continue;
            if (is_spot_valid(b, p, cur_player))
                valid_spots.push_back(p);
        }
    }
    return valid_spots;
}

int get_value(Board b, int x, int y, int cur_player) {
    int value = 0, m = cur_player;
    for(int i=0;i<SIZE;i++)for(int j=0;j<SIZE;j++) {
        if((i == 0 || i == SIZE-1) && (j == 0 || j == SIZE-1)) {//角落
            if(b[i][j] == player) value += 50;
            if(b[i][j] == 3-player) value -= 50;
        } else if(i == 0 || j == 0 || i == SIZE-1 || j == SIZE-1) {//外圍
            if(b[i][j] == player) value += 10;
            if(b[i][j] == 3-player) value -= 10;
        } else if((i == 1 || i == SIZE-2) && (j == 1 || j == SIZE-2)) {//次角落
            if(b[i][j] == player) value -= 10;
            if(b[i][j] == 3-player) value += 10;
        } else if(i == 1 || j == 1 || i == SIZE-2 || j == SIZE-2) {//次外圍
            if(b[i][j] == player) value -= 2;
            if(b[i][j] == 3-player) value += 2;
        } else {
            if(b[i][j] == player) value += 1;
            if(b[i][j] == 3-player) value -= 1;
            if(x == i && y == j) m = 1;
        }
    }
    /*
    const int score[8][8] = {
        {50,10,10,10,10,10,10,50},
        {10,-10,1,1,1,1,-10,10},
        {10,1,1,1,1,1,1,10},
        {10,1,1,1,1,1,1,10},
        {10,1,1,1,1,1,1,10},
        {10,1,1,1,1,1,1,10},
        {10,-10,1,1,1,1,-10,10},
        {50,10,10,10,10,10,10,50},
    };
    value = 0;
    for(int i=0;i<SIZE;i++)for(int j=0;j<SIZE;j++) {
        if(b[i][j] == player) value += score[i][j];
        if(b[i][j] == 3-player) value -= score[i][j];}
     */
    return value;
}
Board flip(Board b, Point p, int cur_player) {
    b[p.x][p.y] = cur_player;
    Point t = p;
    for (Point dir: directions) {//flip
        t = p;
        while (is_spot_on_board(t) && get_disc(b,t) != EMPTY) {
            if (is_disc_at(b, t, cur_player)) {
                t = p;
                while (is_spot_on_board(t) && get_disc(b,t) != EMPTY && !is_disc_at(b, t, cur_player)) {
                    b[t.x][t.y] = cur_player;
                    t = t + dir;
                }
                break;
            }
            t = t + dir;
        }
    }
    return b;
}
int build_tree(Node *node, int depth, int A, int B, int cur_player) {
    // with Alpha-Beta Pruning
    node->value = get_value(node->state, node->x, node->y, cur_player);
    if(depth == 0) return node->value;
    vector<Point> valid_spots = get_valid_spots(node->state, cur_player);
    if(cur_player == player) {
        int value = -MAX;
        int spot_num = valid_spots.size();
        for(int i=0;i<spot_num;i++) {
            Node *child = new Node(flip(node->state, valid_spots[i], cur_player), valid_spots[i]);
            node->childs[node->child_num++] = child;
            value = max(value, build_tree(child, depth-1, A, B, 3-player));
            A = max(A, value);
            if(A >= B) break;
        }
        return value;
    }else {
        int value = MAX;
        int spot_num = valid_spots.size();
        for(int i=0;i<spot_num;i++) {
            Node *child = new Node(flip(node->state, valid_spots[i], cur_player), valid_spots[i]);
            node->childs[node->child_num++] = child;
            value = min(value, build_tree(child, depth-1, A, B, player));
            B = min(B, value);
            if(B <= A) break;
        }
        return value;
    }
}

int minimax_search(Node *node, int depth, int cur_player) {
    if(depth == 0 || node->child_num == 0) return node->value;
    if(cur_player == player) {
        int value = -MAX;
        for(int i=0;i<node->child_num;i++)
            value = max(value, minimax_search(node->childs[i], depth-1, 3-cur_player));
        return value;
    } else {
        int value = MAX;
        for(int i=0;i<node->child_num;i++)
            value = min(value, minimax_search(node->childs[i], depth-1, 3-cur_player));
        return value;
    }
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
    ofstream temp("test");
    
    fout << next_valid_spots[0].x << " " << next_valid_spots[0].y << endl;
    fout.flush();
    
    int value = -MAX;
    int n_valid_spots = next_valid_spots.size();
    temp<< "next_valid_spots : "<<endl;temp.flush();
    for(int i=0;i<n_valid_spots;i++) {
        Point p = next_valid_spots[i];
        temp << p.x << " " << p.y << endl;temp.flush();
    }temp<< "end "<<endl;temp.flush();
    for(int i=0;i<head->child_num;i++) {
        temp<<i<<endl;
        int t = minimax_search(head->childs[i], 5, player);
        Point p(head->childs[i]->x,head->childs[i]->y);
        if((p.x == 0 || p.x == SIZE-1) && (p.y == 0 || p.y == SIZE-1))
            t *= 50;
        if(t >= value) {
            value = t;
            for(int i=0;i<n_valid_spots;i++) {
                Point valid_p = next_valid_spots[i];
                if(valid_p == p) {
                    fout << p.x << " " << p.y << endl;
                    fout.flush();
                    temp << p.x << " " << p.y << endl;
                    temp.flush();
                }
            }
        }
    }
    temp.close();
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    head = new Node(board);
    build_tree(head, 5, -MAX, MAX, player);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
