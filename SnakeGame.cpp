#include <iostream>
#include <ncurses.h>
#include <curses.h>
//////////////
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>


bool kbhit()                //function kbhit()
{
    termios term;
    tcgetattr(0, &term);
    
    termios term2 = term;
    term2.c_lflag &= ~ICANON;
    tcsetattr(0, TCSANOW, &term2);
    
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    
    tcsetattr(0, TCSANOW, &term);
    
    return byteswaiting > 0;
}


////////////
using namespace std;
bool gameOver;
class Snake{
protected:
    const int width=20;
    const int height=20;
    int x,y,fruitX,fruitY,score;
public:
    enum eDirection {STOP=0, LEFT, RIGHT, UP, DOWN};
    eDirection dir;

};
class Body : public Snake{
protected:
    int tailX[100], tailY[100];
    int nTail;
public:
    void Draw();
    void Setup();
    void Logic();
    void Input();
};

char gecz(){                //function getch()
    char buf=0;
    struct termios old={0};
    fflush(stdout);
    if(tcgetattr(0, &old)<0)
        perror("tcsetattr()");
    old.c_lflag&=~ICANON;
    old.c_lflag&=~ECHO;
    old.c_cc[VMIN]=1;
    old.c_cc[VTIME]=0;
    if(tcsetattr(0, TCSANOW, &old)<0)
        perror("tcsetattr ICANON");
    if(read(0,&buf,1)<0)
        perror("read()");
    old.c_lflag|=ICANON;
    old.c_lflag|=ECHO;
    if(tcsetattr(0, TCSADRAIN, &old)<0)
        perror ("tcsetattr ~ICANON");
   cout<<buf;
    return buf;
}

void Body::Setup(){
    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    srand(time(NULL));
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
}
void Body::Draw(){
    system("clear");
    for (int i = 0; i < width+2; i++)       //top of the frame
        cout << "#";
    cout << endl;
    
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (j == 0)                     //left frame
                cout << "#";
            if (i == y && j == x)
                cout << "O";
            else if (i == fruitY && j == fruitX)
                cout << "F";
            else
            {
                bool print = false;
                for (int k = 0; k < nTail; k++)
                {
                    if (tailX[k] == j && tailY[k] == i)
                    {
                        cout << "o";
                       print = true;
                    }
                }
                if (!print)
                    cout << " ";
            }
            
            
            if (j == width - 1)             //right frame
                cout << "#";
        }
        cout << endl;
    }
    
    for (int i = 0; i < width+2; i++)       //bottom frame
        cout << "#";
    cout << endl;
    cout << "Score:" << score << endl;
    

}
void Body::Input()
{
   
    if(kbhit()){
    
    int a=gecz();
    switch (a)
    {
        case 'a':
            dir = LEFT;
            break;
        case 'd':
            dir = RIGHT;
            break;
        case 'w':
            dir = UP;
            break;
        case 's':
            dir = DOWN;
            break;
        case 'q':
            gameOver = true;
            break;
            
    }
    
    
    }
    
}
void Body::Logic()
{
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;       //following the head
    tailY[0] = y;
    for (int i = 1; i < nTail; i++)
    {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    switch (dir)
    {
        case LEFT:
            x--;
            break;
        case RIGHT:
            x++;
            break;
        case UP:
            y--;
            break;
        case DOWN:
            y++;
            break;
        default:
            break;
            
    }
    //if (x > width || x < 0 || y > height || y < 0)
    //    gameOver = true;
    if (x >= width) x = 0; else if (x < 0) x = width - 1;
    if (y >= height) y = 0; else if (y < 0) y = height - 1;
    
    for (int i = 0; i < nTail; i++)          //condition for bite a tail
        if (tailX[i] == x && tailY[i] == y)
            gameOver = true;
    
    if (x == fruitX && y == fruitY)         //condition for printing a new fruit after eating
    {
        score += 10;
        fruitX = rand() % width;            //new random position of fruit
        fruitY = rand() % height;
        nTail++;                            //after eating the fruit snake shoulc become bigger
    }
}
int main() {
    Snake wonsz;
    Body body;
    body.Setup();
    body.Draw();
    while (!gameOver)                       //while loop which check if we lose
    {
        body.Input();
        body.Logic();
        body.Draw();
        sleep(1);
    }
    return 0;
}
