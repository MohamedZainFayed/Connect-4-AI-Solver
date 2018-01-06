#include <bits/stdc++.h>

using namespace std;

class GameBoard{
    private:
        const static int H = 6, W = 7;
        const int Size = H * W;
        long long mask, bottom, position, fullMask;
        int moves;

    public:

        GameBoard(){
            moves = mask = position = bottom = 0;
            fullMask = (1LL << ((H + 1) * W)) - 1;

            for(int i=0; i<W; i++){
                bottom |= 1LL << ((H + 1) * i);
                fullMask ^= 1LL << ((H + 1) * (i + 1) - 1);
            }
        }

        int size(){
            return Size;
        }

        int played(){
            return moves;
        }

        int height(){
            return H;
        }

        int width(){
            return W;
        }

        bool free(int col){
            long long top = (1LL << (H - 1)) << (col * (H + 1));
            return !(mask & top);
        }

        void play(int col){
            position ^= mask;
            long long base = 1LL << (col * (H + 1));
            mask |= mask + base;
            ++moves;
        }

        bool win(long long pos){
            // Horizontal
            long long x = pos & (pos >> (H + 1));
            if(x & (x >> (2 * (H + 1))))
                return true;

            // Main Diagonal
            x = pos & (pos >> H);
            if(x & (x >> (2 * H)))
                return true;

            // Reverse Diagonal
            x = pos & (pos >> (H + 2));
            if(x & (x >> (2 * (H + 2))))
                return true;


            // Vertical
            x = pos & (pos >> 1);
            if(x & (x >> 2))
                return true;

            // Lose
            return 0;
        }

        bool WinNext(int col){
            long long pos = position;
            pos |= (mask + bottom) & (((1LL << H) - 1) << (col * (H + 1)));
            return win(pos);
        }

        long long get_Key(){
            return position + bottom + mask;
        }

        long long possible(){
            return (mask + bottom) & fullMask;
        }

        long long win_pos(long long pos){
            // Vertical
            long long msk = mask | pos;
            long long ret = (pos << 1) & (pos << 2) & (pos << 3);

            // Horizontal

            long long x = (pos << (H + 1)) & (pos << ((H + 1) * 2));

            ret |= x & (pos << (3 * (H + 1)));
            ret |= x & (pos >> (H + 1));

            x >>= 3 * (H + 1);

            ret |= x & (pos >> (3 * (H + 1)));
            ret |= x & (pos << (H + 1));

            // Diagonal 1

            x = (pos << H) & (pos << (H * 2));

            ret |= x & (pos << (3 * H));
            ret |= x & (pos >> H);

            x >>= 3 * H;

            ret |= x & (pos >> (3 * H));
            ret |= x & (pos << H);


            // Diagonal 2

            x = (pos << (H + 2)) & (pos << ((H + 2) * 2));

            ret |= x & (pos << (3 * (H + 2)));
            ret |= x & (pos >> (H + 2));

            x >>= 3 * (H + 2);

            ret |= x & (pos >> (3 * (H + 2)));
            ret |= x & (pos << (H + 2));


            return ret & (fullMask ^ msk);
        }

        long long opponent_win_pos(){
            return win_pos(mask ^ position);
        }

        long long nonLosing_pos(){
            long long pos = possible();
            long long opponent_win = opponent_win_pos();
            long long force = pos & opponent_win;
            if(force){
                if(force & (force - 1))
                    return 0;
                pos = force;
            }
            return pos & (~(opponent_win >> 1));
        }

        long long next_col_pos(int col){
            if(((((1LL << H) - 1) << (col * (H + 1))) & (mask + bottom)))
                return ((((1LL << H) - 1) << (col * (H + 1))) & (mask + bottom)) | position;
            return 0;
        }

        void print(){
            long long position_2 = position ^ mask;
            for(int i=H - 1; ~i; --i){
                for(int j=0; j<W; j++){
                    int f = 1 * (!!(((1LL << i) << (j * (H + 1))) & position));
                    f += 2 * (!!(((1LL << i) << (j * (H + 1))) & position_2));
                    if(f == 1){
                        if(moves & 1)
                            putchar('X');
                        else
                            putchar('O');
                    }
                    if(f == 2){
                        if(moves & 1)
                            putchar('O');
                        else
                            putchar('X');
                    }
                    if(!f)
                        putchar('.');
                }
            putchar('\n');
            }
            for(int i=0; i<W; i++)
                printf("%d", i);
            printf("\n\n");
        }

        bool empty(int col){
            return !((((1LL << H) - 1) << (col * (H + 1))) & mask);
        }

        bool finish(){
            return win(position ^ mask);
        }
};

class Trans_Table{

    private:

        const static int SIZE = 1e5 + 7;

        struct Entry{
            long long key;
            int val;
            int best;
        } Table[SIZE];

        int index(long long k){
            return k % SIZE;
        }

    public:

        Trans_Table(){
            memset(Table, 0, sizeof Table);
        }

        void insert(long long key, int val, int best){
            int i = index(key);
            Table[i] = {key, val, best};
        }

        int get(long long key){
            int i = index(key);

            if(Table[i].key == key)
                return Table[i].val; // Hit;

            return 0;
        }

        int getBest(long long key){
            int i = index(key);

            return Table[i].best;
        }

};

class Game{
    private:
        GameBoard board;
        Trans_Table table;
        int shift = 200, order[7], cnt[7];

    public:
        int popcount(long long mask){
            int ret = 0;
            while(mask){
                ++ret;
                mask -= mask & -mask;
            }
            return ret;
        }

        int Min_Max(GameBoard pos, int alpha, int beta){
            if(pos.played() == pos.size())
                return 0;  // Draw
            for(int i=0; i<pos.width(); i++){
                if(pos.free(i) && pos.WinNext(i)){
                    table.insert(pos.get_Key(), ((pos.size() - pos.played() + 1) >> 1) + shift, i);
                    return (pos.size() - pos.played() + 1) >> 1;
                }
            }

            long long possible = pos.nonLosing_pos();
            if(!possible){
                for(int i=0; i<pos.width(); i++){
                    if(pos.free(i)){
                        table.insert(pos.get_Key(), (-(pos.size() - pos.played()) / 2) + shift, i);
                        break;
                    }
                }
                return -(pos.size() - pos.played()) / 2;
            }

            beta = min(beta, (pos.size() - pos.played() - 1) >> 1);

            int v = table.get(pos.get_Key());

            if(v)
                beta = min(beta, v - shift);

            if(beta <= alpha)
                return alpha;

            for(int x=0; x<pos.width(); x++){
                order[x] = x;
                cnt[x] = -300;
                long long y = pos.next_col_pos(x);
                if(y)
                    cnt[x] = popcount(pos.win_pos(y));
                for(int i=x-1; ~i; --i){
                    if(cnt[i] > cnt[i + 1]) break;
                    if(cnt[i] < cnt[i + 1] || abs(order[i] - pos.width() / 2) > abs(order[i + 1] - pos.width() / 2)){
                        swap(cnt[i], cnt[i + 1]);
                        swap(order[i], order[i + 1]);
                    }else
                        break;
                }
            }
            int best;
            for(int x=0; x<pos.width(); x++){
                int i = order[x];
                if((((1LL << pos.height()) - 1) << (i * (pos.height() + 1))) & possible){
                    GameBoard tmp = pos;
                    tmp.play(i);
                    int score = -Min_Max(tmp, -beta, -alpha);

                    if(score >= beta){
                        table.insert(pos.get_Key(), score + shift, i);
                        return score;
                    }
                    if(alpha < score){
                        best = i;
                        alpha = max(alpha, score);
                    }
                }
            }
            table.insert(pos.get_Key(), alpha + shift, best);
            return alpha;
        }

        void AI_play(){

            if(board.played() < 2){
                for(int i=0; i<board.width(); i++){
                    if(!board.empty(i)){
                        board.play(i);
                        return;
                    }
                }
                board.play(3);
                return;
            }

            if(board.played() < 6){
                for(int i=board.width() / 2; ~i ; i += (board.played() < 4) - (board.played() >= 4)){
                    if(board.empty(i)){
                        board.play(i);
                        return;
                    }
                }
                for(int i=board.width() / 2; ; i++){
                    if(board.empty(i)){
                        board.play(i);
                        return;
                    }
                }
            }

            int st = - (board.size() - board.played()) / 2;
            int en = (board.size() - board.played() + 1) / 2;

            // Binary search to limit the exploration window [Alpha, Beta]

            while(st < en){
                int mid = (st + en) >> 1;

                int x = Min_Max(board, mid, mid + 1);

                if(x > mid)
                    st = x;
                else
                    en = x;
            }

            int best = table.getBest(board.get_Key());

            board.play(best);
        }

        bool play(int col){
            if(col < 0 || col >= board.width() || !board.free(col))
                return false;
            board.play(col);
            return true;
        }

        void print(){
            board.print();
        }

        bool done(){
            if(board.finish())
                return 2;
            if(board.played() == board.size())
                return 1;
            return 0;
        }
};


int main()
{
    Game x;
    int a;
    x.print();
    while(1){
        do{
            cout << "Enter a valid column: ";
            cin >> a;
        }while(!x.play(a));
        cout << "\n";
        x.print();
        if(x.done()){
            if(x.done() == 1)
                cout << "YOU WON!!!!! :-O\n";
            else
                cout << "DRAW GAME!!!!\n";
            break;
        }
        x.AI_play();
        cout << "\n";
        x.print();
        if(x.done()){
            if(x.done() == 1)
                cout << "AI WON!!!!! :-P\n";
            else
                cout << "DRAW GAME!!!!\n";
            break;
        }
    }
    system("pause");
    return 0;
}
