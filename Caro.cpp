/* Project 1: Programming
- title: Caro Game C++ on Linux
- @author: Duong Quang Minh
*/
#include <iostream>
#include <stdlib.h> 
#include <termios.h> // getch
#include <unistd.h> 
#include <term.h> //ClearScreen
#include <stdio.h>
#include <signal.h> // EXIT_PROCESS
/* Severing for getch() and clear screen
-note: add - lncurse (terminal) */ 

using namespace std;

/*add lib*/
void    gotoxy(int x,int y)
{
	/* comments*/
    printf("%c[%d;%df",0x1B,y,x);
}
/* Clear Screen */
void ClearScreen()
{
	if (!cur_term)
    {
    	int result;
    	setupterm( NULL, STDOUT_FILENO, &result );
    if (result <= 0) return;
    }

  	putp( tigetstr( "clear" ) );
 }
 /* reads from keypress, echoes */
int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}
 
/* reads from keypress, echoes */
int getche(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}


/*global variable*/
const int   WIDTH = 80;
const int 	HIDTH = 24;
const int 	OFFSET = 20;
char    a[100][100];
long 	attackscore[7] = {0,3,24,193,1536,8288,72288};
long 	defendscore[7] = {0,1,9,81,729,6561,66561};
char    key;
int 	turn_i = 0, n, m;
int     pos_x, pos_y; // -note: x - collumn y - row

/* List Function in Program*/
void    StartGame();
void    MakeFrame();
/* Make table to play*/
void    Guide();
/* Gide players how to play*/
void    Controller(char key,char mode);
void    GetXY(int x,int y);
void    MatchInfo(char mode);
/* Display Infomation of Match*/
void    drawOX(int x,int y);
bool 	CheckWinner();
void	Judge(char mode);
/* Annouce winner*/
void    TwoPlayers();
void    AIChallenge();
/*Play with AI*/
void    Replay();

void    EXIT_PROCESS();
/*exit game and close terminal*/
void	FindMove();
/*Searching move*/

/*Funtions computing score (Heuristic)*/
void	GenerateMove();
long	Att_Collumn(int x,int y);
long	Att_Row(int x,int y);
long	Att_Cross1(int x,int y);
long	Att_Cross2(int x,int y);
long	Def_Collumn(int x,int y);
long	Def_Row(int x,int y);
long	Def_Cross1(int x,int y);
long	Def_Cross2(int x,int y);

/*main */
int main(int argc, char const *argv[])
{
	/* code */
	ClearScreen();
	StartGame();
	return 0;
}

void    MakeFrame() 
{
	/* comments*/
	ClearScreen();
    for (int i=0; i< WIDTH; i++) // 80
        for (int j=0; j< HIDTH; j++) // 24
            a[i][j]=' ';
	gotoxy(OFFSET,0);
	for (int i = 0; i < WIDTH-OFFSET; i++) // 80-OFF
		cout << "*";
	for (int j = 0; j < HIDTH-1; j++) // 24-1
	{ 
		gotoxy(OFFSET,j+1);
		cout << "*";
	}
	gotoxy(OFFSET,HIDTH-1);
	for (int i = 0; i < WIDTH-OFFSET; i++) //80-OFF
		cout << "*";
	for (int j = 0; j < HIDTH-1; j++) //24-1
	{
		gotoxy(WIDTH,j+1);
		cout << "*";
	}
}

void    Guide() 
{
	/*comments*/
	ClearScreen();
	gotoxy(10,2);
	cout << "How to play ";
	gotoxy(10,3);
	cout << "Press A,D,S,W to move";
	gotoxy(10,4);
	cout << "Use E or Enter to tick OX";
	gotoxy(10,5);
	cout << "Press any key to  back StartGame";
	getch();
	StartGame();
}

void    StartGame() 
{
	/* Menu Game and chose GameMode*/
	ClearScreen();
	gotoxy(10,5);
	cout << "1. TwoPlayers";
	gotoxy(10,6);
	cout << "2. AIChallenge (Buiding)";
	gotoxy(10,7);
	cout << "3. Guide: How to play";
	gotoxy(10,8);
	cout << "4. Press Q to exit";
	gotoxy(10,9);
	char chosen = getch();
	if (chosen == '1') 
		TwoPlayers();
	else if (chosen == '2')
		AIChallenge();
	else if (chosen == '3') 
		Guide();
	else if (chosen == 'q' || chosen == 'Q') 
		EXIT_PROCESS();
}
long 	Att_Collumn(int x,int y) 
{
	int quandich = 0,quanta = 0;
	for (int off = 1; off < 6 && y + off < HIDTH; off++) 
	{
		if (a[x][y+off] == 'O') 
			quanta++;
		else if (a[x][y+off] == 'X')
		{
			quandich++;
			break;
		}
		else 
			break;
	}
	for (int off = 1; off < 6 && y - off > 2; off++) 
	{
		if (a[x][y-off] == 'O') 
			quanta++;
		else if (a[x][y-off] == 'X')
		{
			quandich++;
			break;
		}
		else 
			break;
	}
	if (quandich == 2)
		return 0;
	long score = attackscore[quanta] - defendscore[quandich+1];
	if (quanta == 3 && quandich == 0) 
		score = attackscore[5];
	if (quanta == 4)
		score = attackscore[6];
	return score;
}
long 	Att_Row(int x,int y) 
{
	int quandich = 0,quanta = 0;
	for (int off = 1; off < 6 && x + off < WIDTH; off++) 
	{
		if (a[x+off][y] == 'O') 
			quanta++;
		else if (a[x+off][y] == 'X')
		{
			quandich++;
			break;
		}
		else 
			break;
	}
	for (int off = 1; off < 6 && x - off > OFFSET; off++) 
	{
		if (a[x-off][y] == 'O') 
			quanta++;
		else if (a[x-off][y] == 'X')
		{
			quandich++;
			break;
		}
		else 
			break;
	}
	if (quandich == 2)
		return 0;
	long score = attackscore[quanta] - defendscore[quandich+1];
	if (quanta >= 3 && quandich == 0) 
		score = attackscore[5];
	if (quanta == 4)
		score = attackscore[6];
	return score;
}
long 	Att_Cross1(int x,int y) 
{
	int quandich = 0,quanta = 0;
	for (int off = 1; off < 6 && y - off > 2 && x + off < WIDTH ; off++) 
	{
		if (a[x+off][y-off] == 'O') 
			quanta++;
		else if (a[x+off][y+off] == 'X')
		{
			quandich++;
			break;
		}
		else 
			break;
	}
	for (int off = 1; off < 6 && y + off < HIDTH && x - off > OFFSET; off++) 
	{
		if (a[x-off][y+off] == 'O') 
			quanta++;
		else if (a[x-off][y+off] == 'X')
		{
			quandich++;
			break;
		}
		else 
			break;
	}
	if (quandich == 2)
		return 0;
	long score = attackscore[quanta] - defendscore[quandich+1];
	if (quanta >= 3 && quandich == 0) 
		score = attackscore[5];
	if (quanta == 4)
		score = attackscore[6];
	return score;
}
long 	Att_Cross2(int x,int y) 
{
	int quandich = 0,quanta = 0;
	for (int off = 1; off < 6 && y + off < HIDTH && x + off < WIDTH; off++) 
	{
		if (a[x+off][y+off] == 'O') 
			quanta++;
		else if (a[x+off][y+off] == 'X')
		{
			quandich++;
			break;
		}
		else 
			break;
	}
	for (int off = 1; off < 6 && y - off > 2 && x-off > OFFSET; off++) 
	{
		if (a[x-off][y-off] == 'O') 
			quanta++;
		else if (a[x-off][y-off] == 'X')
		{
			quandich++;
			break;
		}
		else 
			break;
	}
	if (quandich == 2)
		return 0;
	long score = attackscore[quanta] - defendscore[quandich+1];
	if (quanta >= 3 && quandich == 0) 
		score = attackscore[5];
	if (quanta == 4)
		score = attackscore[6];
	return score;
}


long 	Def_Collumn(int x,int y) 
{
	int quandich = 0,quanta = 0;
	for (int off = 1; off < 6 && y + off < HIDTH; off++) 
	{
		if (a[x][y+off] == 'O') 
		{	
			quanta++;
			break;
		}
		else if (a[x][y+off] == 'X')
		{
			quandich++;
		}
		else 
			break;
	}
	for (int off = 1; off < 6 && y - off > 2; off++) 
	{
		if (a[x][y-off] == 'O') { 
			quanta++;
			break;
		}
		else if (a[x][y-off] == 'X')
		{
			quandich++;
		}
		else 
			break;
	}
	if (quanta == 2)
		return 0;
	long score = defendscore[quandich];
	if (quandich >= 3 && quanta == 0) 
		score = defendscore[5];
	if (quandich == 4)
		score = defendscore[6];
	return score;
}
long 	Def_Row(int x,int y) 
{
	int quandich = 0,quanta = 0;
	for (int off = 1; off < 6 && x + off < WIDTH; off++) 
	{
		if (a[x+off][y] == 'O') 
		{
			quanta++;
			break;
		}
		else if (a[x+off][y] == 'X')
		{
			quandich++;
		}
		else 
			break;
	}
	for (int off = 1; off < 6 && x - off > OFFSET; off++) 
	{
		if (a[x-off][y] == 'O')
		{ 
			quanta++;
			break;
		}
		else if (a[x-off][y] == 'X')
		{
			quandich++;
		}
		else 
			break;
	}
	if (quanta == 2)
		return 0;
	long score = defendscore[quandich];
	if (quandich >= 3 && quanta == 0) 
		score = defendscore[5];
	if (quandich == 4)
		score = defendscore[6];
	return score;
}
long 	Def_Cross1(int x,int y) 
{
	int quandich = 0,quanta = 0;
	for (int off = 1; off < 6 && y - off > 1 && x + off < WIDTH ; off++) 
	{
		if (a[x+off][y-off] == 'O') {

			quanta++;
			break;
		}
		else if (a[x+off][y-off] == 'X')
		{
			quandich++;
		}
		else 
			break;
	}
	for (int off = 1; off < 6 && y + off < HIDTH && x - off > OFFSET; off++) 
	{
		if (a[x-off][y+off] == 'O') { 
			quanta++;
			break;
		}
		else if (a[x-off][y+off] == 'X')
		{
			quandich++;
		}
		else 
			break;
	}
	if (quanta == 2)
		return 0;
	long score = defendscore[quandich];
	if (quandich >= 3 && quanta == 0) 
		score = defendscore[5];	
	if (quandich == 4)
		score = defendscore[6];
	return score;
}
long 	Def_Cross2(int x,int y) 
{
	int quandich= 0,quanta= 0;
	for (int off = 1; off < 6 && y + off < HIDTH && x + off < WIDTH; off++) 
	{
		if (a[x+off][y+off] == 'O') 
		{
			quanta++;
			break;
		}
		else if (a[x+off][y+off] == 'X')
		{
			quandich++;
		}
		else 
			break;
	}
	for (int off = 1; off < 6 && y - off > 1 && x-off > OFFSET; off++) 
	{
		if (a[x-off][y-off] == 'O') 
		{ 
			quanta++;
			break;
		}
		else if (a[x-off][y-off] == 'X')
		{
			quandich++;
		}
		else 
			break;
	}
	if (quanta == 2)
		return 0;
	long score = defendscore[quandich];
	if (quandich == 3 && quanta == 0) 
		score = defendscore[5];
	if (quandich == 4)
		score = defendscore[6];
	return score;
}
void 	FindMove() 
{
	long max_score = 0;
	int x,y;
	for (int i = OFFSET+1;i < WIDTH;i++)
		for(int j = 1;j < HIDTH;j++)
		if (a[i][j] == ' ') 
		{
			long Att_score =  Att_Collumn(i,j) + Att_Row(i,j)
						   	+ Att_Cross1(i,j) + Att_Cross2(i,j);
			long Def_score =  Def_Collumn(i,j) + Def_Row(i,j)
						   	+ Def_Cross1(i,j) + Def_Cross2(i,j);
			long score = Att_score;
			if (Def_score > score)
				score = Def_score;
			if (score > max_score) 
			{
				max_score = score;
				x = i;
				y = j;
			}
		}
		pos_x = x;
		pos_y = y;
}
void 	GenerateMove() 
{
	FindMove();
	gotoxy(pos_x,pos_y);
	drawOX(pos_x,pos_y);
	if ( CheckWinner() == true) 
			Judge('2');
}

void    AIChallenge()
{
	ClearScreen();
	gotoxy(5,10);
	cout << " Wellcome to AIChallenge GameMode";
	getch();
	ClearScreen();
	MakeFrame();
	pos_x = 50;
	pos_y = 13;
	turn_i = 0;
	do 
	{
		MatchInfo('2');
		gotoxy(pos_x,pos_y);
		if (turn_i % 2 == 0) 
		{
			char key = getch();
			Controller(key,'2');
		} 
		else {
			GenerateMove(); // turn of Computer
		}
	} while (1);
	EXIT_PROCESS();
}
void    TwoPlayers() 
{
	/* GameMode : TwoPlayers*/
	ClearScreen();
	MakeFrame();
	pos_x = 50;
	pos_y = 13;
	turn_i = 0;
	do 
	{
		MatchInfo('1');
		gotoxy(pos_x,pos_y);
		char key = getch(); 
		Controller(key,'1');
	} while (1);
	EXIT_PROCESS ();
} 
void    MatchInfo(char mode)
{
	/* Display info of Match*/
	gotoxy(0,3); 
	cout << "turns: " << turn_i + 1;
	gotoxy(0,5);
	if (turn_i % 2 == 0 && mode == '1')
		cout << "turn: player 1";
	else if (turn_i % 2 == 1 && mode == '1')
		cout << "turn: player 2";
	else if (turn_i % 2 == 0 && mode == '2')
		cout << "turn: Player";
	gotoxy(0,7);
	cout << "Press R to Replay";
	gotoxy(0,9);
	cout << "Press Q to exit";
}

void    drawOX(int x,int y) 
{
	/* tick X or O into table*/
	char img = 'X';
	if (turn_i % 2 == 1) img = 'O';
	if (a[x][y] != 'X' && a[x][y] != 'O') {
		a[x][y] = img;
		cout << a[x][y];
		turn_i ++;
	}
}
bool    CheckCross1(char w)
{
	int count = 1; 
	int x = pos_x;
	int y = pos_y;
	do
	{
		x += 1;
		y += 1;
		if (a[x][y] == w) 
			++ count;
		else break; 
	} while (1);
	x = pos_x;
	y = pos_y;
	do
	{
		x -= 1;
		y -= 1;
		if (a[x][y] == w) 
			++ count; 
		else break; 
	} while (1);
	if (count > 4) 
		return true;
	else 
		return false;
}
bool	CheckCross2(char w)
{
	int count = 1; 
	int x = pos_x;
	int y = pos_y;
	do
	{
		x += 1;
		y -= 1;
		if (a[x][y] == w) 
			++ count;
		else break; 
	} while (1); 
	x = pos_x;
	y = pos_y;
	do
	{
		x -= 1;
		y += 1;
		if (a[x][y] == w) 
			++ count;
		else break; 
	} while (1);
	if (count > 4) 
		return true;
	else 
		return false;
}
bool	CheckRow(char w) 
{
	int count = 1; 
	int x = pos_x;
	int y = pos_y;
	do
	{
		x += 1;
		if (a[x][y] == w) 
			++ count;
		else break; 
	} while (1);
	x = pos_x;
	y = pos_y;
	do
	{
		x -= 1;
		if (a[x][y] == w) 
			++ count;
		else break; 
	} while (1);
	if (count > 4) 
		return true;
	else
		return false;
}
bool    CheckCollumn(char w)
{
	int count = 1; 
	int x = pos_x;
	int y = pos_y;
	do
	{
		y += 1;
		if (a[x][y] == w) 
			++ count;
		else break; 
	} while (1);
	x = pos_x;
	y = pos_y;
	do
	{
		y -= 1;
		if (a[x][y] == w) 
			++ count;
		else break; 
	} while (1);
	if (count > 4) 
		return true;
	else
		return false;
}
bool    CheckWinner() 
{
	/*return 1 if one of two player won*/
	char winner = 'O';
	if (turn_i % 2 == 1)
		winner = 'X';
	if ( CheckCross1(winner) == true ||
		 CheckCross2(winner) == true ||
		 CheckRow(winner)    == true ||
		 CheckCollumn(winner) == true)
		return true;
	return false;
} 
void 	Judge(char mode) 
{
	/* annouce who's win and ask replay */
		ClearScreen();
		gotoxy(10,5);
		if (turn_i % 2 == 1 && mode == '1')
			cout << "Player 1(X) won";
		else if (turn_i % 2 == 0 && mode == '1')
			cout << "Player 2(O) won";
		else if (turn_i % 2 == 1 && mode == '2')
			cout << "Player won";
		else if (turn_i % 2 == 0 && mode == '2')
			cout << "Computer won";
		gotoxy(10,7);
		cout << "Press key to continue...";
		getch();
		Replay();
} 

void    Controller(char key,char mode) 
{
	/* update pos_x pos_y when moved*/
    if (key=='a' || key=='A' || 
    	int (key==27))
    		{
    			pos_x -= 1;
    			if (pos_x < OFFSET +1) pos_x = OFFSET+1;
    		}
	else if (key=='d' || key=='D' || 
		int (key==27)) 
			{
				pos_x += 1;
				if (pos_x > WIDTH-1) pos_x = WIDTH-1;
			} 
	else if (key=='w' || key=='W' || 
		int (key==27)) 
			{
				pos_y -= 1;
				if (pos_y < 2) pos_y = 2;
			}
	else if (key=='s' || key=='S' || 
		int (key==27)) 
			{
				pos_y += 1;
				if (pos_y > HIDTH-1) pos_y = HIDTH-1;
			}
	else if (key=='e' || key=='E' || 
		int (key== 10)) 
			{ 
				drawOX(pos_x,pos_y);
				if ( CheckWinner() == true) 
					Judge(mode);
			} 
	else if (key=='r' || key=='R') 
			Replay();
	else if (key=='q' || key=='Q')
			EXIT_PROCESS();
}
void    Replay()
{
	ClearScreen();
	/*goto*/
	gotoxy(10,5);
	cout << "    Choose GameMode: ";
	/**/
	gotoxy(10,6);
	cout << "Press 1 to play TwoPlayers";
	/**/
	gotoxy(10,7);
	cout << "Press 2 to Play AIChallenge";
	/**/
	gotoxy(10,8);
	cout << "Press Q to exit";
	gotoxy(10,9);
	char mode = getch();
	if (mode == '1') 
		TwoPlayers();
	else if (mode == '2') 
		AIChallenge();
	else if (mode == 'Q' || mode == 'q')
		EXIT_PROCESS(); 
}
void    EXIT_PROCESS()
{
	/*Exit Program*/
	ClearScreen();
	gotoxy(10,5);
	cout << "Thanks for playing my game...";
	gotoxy(10,6);
	cout << "Press key to exit";
	getch();

	kill(getppid(), SIGKILL);
}




