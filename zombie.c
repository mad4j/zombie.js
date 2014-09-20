/* Made by Stupebrett aka Strupe321 */
/* http://www.youtube.com/user/Strupe321 */

#include <curses.h>
#include <time.h>
#include <stdlib.h>

//external definition
void emscripten_sleep(int ms);

struct bullet
{
	int posY, posX;
	bool is_moving;
};

struct zombie
{
	int posY, posX, balancer;
	bool is_moving, start;
};

int main()
{	
	/* Initialize curses */
	initscr();
	start_color();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	srand(time(0));

	char map[25][80]; //the game map
	int posY = 11, posX = 5; //the game characters coordinates
	struct bullet bullets[10]; //the bullets
	struct zombie zombies[100]; //the zombies
	int zombiespeed = 3; //how fast the zombies move
	int fences = 3; //amount of fences left (3 at startup)
	int zombie_index = 0;
	int zombie_spawnrate = 100; //how fast the zombies spawn
	int zombie_spawnbalancer = 0;
	int zombies_killed = 0;

	for(int i=0; i < 10; ++i)
	{
		bullets[i].is_moving = FALSE;
		bullets[i].posX = 0;
		bullets[i].posY = 0;
	}

	for(int i=0; i < 100; ++i)
	{
		zombies[i].is_moving = FALSE;
		zombies[i].balancer = 0;
		zombies[i].posX = -1;
	}

	//color initalizations
	init_pair(1, COLOR_GREEN, COLOR_GREEN);
	init_pair(2, COLOR_WHITE, COLOR_GREEN);
	init_pair(3, COLOR_YELLOW, COLOR_GREEN);
	init_pair(4, COLOR_BLACK, COLOR_GREEN);
	init_pair(5, COLOR_RED, COLOR_BLACK);
	init_pair(6, COLOR_RED, COLOR_GREEN);
	init_pair(7, COLOR_RED, COLOR_RED);

	for(int i=0; i < 80; ++i)
		map[0][i] = ' ';

	//loads map with top and bottom borders
	for(int i=0; i < 80; ++i)
	{
		map[1][i] = '#';
		map[23][i] = '#';
	}
	//loads map with the inner game field
	for(int a=2; a < 23; ++a)
	{
		for(int b=0; b < 80; ++b)
		{
			map[a][b] = ' ';
		}
		map[a][10] = '|';
		map[a][11] = '|';
		map[a][12] = '|';
	}
	for(int i=0; i < 80; ++i)
		map[24][i] = ' ';

	map[posY][posX] = 'P';
	map[posY][posX+1] = '=';

	//print a startup message
	char text1[] = "Zombie survival!";
	char text2[] = "Made by Stupebrett aka Strupe321";
	char text3[] = "Press any key to start";
	int maxX, maxY;
	getmaxyx(stdscr, maxY, maxX);
	attron(COLOR_PAIR(5));
	mvprintw((maxY / 2) - 2, (maxX - sizeof(text1)) / 2, "%s", text1);
	mvprintw(maxY / 2, (maxX - sizeof(text2)) / 2, "%s", text2);
	attroff(COLOR_PAIR(5));

	attron(A_BOLD);
	mvprintw((maxY / 2) + 4, (maxX - sizeof(text3)) / 2, "%s", text3);
	attroff(A_BOLD);

	getch();
	
	curs_set(0);

	int ch, bullet_stabilizer = 10;
	int bulletIndex = 0;
	bool quit = FALSE;

	//the main game loop
	while(quit == FALSE)
	{
		emscripten_sleep(30);
		++bullet_stabilizer;
		++zombie_spawnbalancer;

		timeout(0);
		ch = getch();
		switch(ch)
		{
		case KEY_UP:
			if(posY - 1 > 1)
			{
				map[posY][posX] = ' ';
				map[posY][posX+1] = ' ';
				--posY;
			}
			break;
		case KEY_DOWN:
			if(posY + 1 < 23)
			{
				map[posY][posX] = ' ';
				map[posY][posX+1] = ' ';
				++posY;
			}
			break;
		case ' ':
			if(bullet_stabilizer >= 10)
			{
				bullets[bulletIndex].is_moving = TRUE;
				bullets[bulletIndex].posX = posX + 2;
				bullets[bulletIndex].posY = posY;
			
				if(bulletIndex == 9) bulletIndex = 0;
				else ++bulletIndex;

				bullet_stabilizer = 0;
			}
			break;
		case 27:
			return 0;
			break;
		default:
			break;
		}
		//moves the player to the new location
		map[posY][posX] = 'P';
		map[posY][posX+1] = '=';
	
		//moves the bullet(s) & handles bullet related stuff
		for(int i=0; i < 10; ++i)
		{
			if(bullets[i].is_moving == TRUE)
			{
				map[bullets[i].posY][bullets[i].posX] = ' ';
				++bullets[i].posX;
				map[bullets[i].posY][bullets[i].posX] = '*';
			}
			if(bullets[i].posX == maxX-1)
			{
				bullets[i].is_moving = FALSE;
				map[bullets[i].posY][bullets[i].posX] = ' ';
			}

			if(bullets[i].posX == 11 || bullets[i].posX == 12 || bullets[i].posX == 13)
			{
				if(fences == 3)
					map[bullets[i].posY][bullets[i].posX-1] = '|';
				else if(fences == 2 && bullets[i].posX != 13)
					map[bullets[i].posY][bullets[i].posX-1] = '|';
				else if(fences == 1 && bullets[i].posX != 13 && bullets[i].posX != 12)
					map[bullets[i].posY][bullets[i].posX-1] = '|';
			}
		}
		//moves the zombies & handles zombie related stuff
		for(int i=0; i < 100; ++i)
		{
			if(zombies[i].is_moving == TRUE && zombies[i].balancer >= zombiespeed)
			{
				map[zombies[i].posY][zombies[i].posX] = ' ';
				--zombies[i].posX;
				map[zombies[i].posY][zombies[i].posX] = '@';
				zombies[i].balancer = 0;
			}
			if(zombies[i].start == TRUE)
			{
				zombies[i].posX = maxX-1;
				zombies[i].posY = rand() % 20 + 3;
				zombies[i].is_moving = TRUE;
				zombies[i].start = FALSE;
			}
			if(zombies[i].posX == 9 + fences && fences > 0)
			{
				zombies[i].is_moving = FALSE;
				if(fences > 0) --fences;
				else fences = 0;
			}
			if(zombies[i].posX == 0)
				quit = TRUE;

			//checks if a bullet has hit a zombie
			for(int a=0; a < 10; ++a)
			{
				if(zombies[i].posX > 0 && zombies[i].is_moving == TRUE &&
				((zombies[i].posY == bullets[a].posY && zombies[i].posX == bullets[a].posX) ||
				(zombies[i].posY == bullets[a].posY && zombies[i].posX == bullets[a].posX+1)))
				{
					bullets[a].is_moving = FALSE;
					zombies[i].is_moving = FALSE;
					map[bullets[a].posY][bullets[a].posX] = ' ';
					map[zombies[i].posY][zombies[i].posX] = '%';
					bullets[a].posY = 0;
					++zombies_killed;
				}
				if(bullets[a].posX-1 == zombies[i].posX && bullets[a].posY == zombies[i].posY)
					map[zombies[i].posY][zombies[i].posX] = '%';

				for(int b=0; b < 100; ++b)
					if(zombies[i].posX+1 == zombies[b].posX && zombies[i].posY == zombies[b].posY)
						map[zombies[b].posY][zombies[b].posX] = '%';
			}
		}
		//spawn a zombie
		if(zombie_spawnbalancer >= zombie_spawnrate)
		{
			zombie_spawnbalancer = 0;
			zombies[zombie_index].start = TRUE;
			if(zombie_index == 99) zombie_index = 0;
			else ++zombie_index;
			
			//gradually speeds up how fast the zombies spawn
			if(zombie_spawnrate >= 30) zombie_spawnrate -= 2;
			else if(zombie_spawnrate < 30 && zombie_spawnrate > 10) --zombie_spawnrate;
			else zombie_spawnrate = 10;
		}
		if(fences <= 2)
			for(int i=2; i < 23; ++i)
				map[i][10+fences] = ' ';

		for(int i=0; i < maxX - 1; ++i)
			map[0][i] = ' ';

		move(0, 0);
		//displays the map
		for(int a=0; a < 25; ++a)
			for(int b=0; b < 80; ++b)
			{
				if(map[a][b] == ' ') attron(COLOR_PAIR(1) | A_BOLD);
				else if(map[a][b] == 'P') attron(COLOR_PAIR(3) | A_BOLD);
				else if(map[a][b] == '=') attron(COLOR_PAIR(4));
				else if(map[a][b] == '*') attron(COLOR_PAIR(4));
				else if(map[a][b] == '@') attron(COLOR_PAIR(6));
				else if(map[a][b] == '%') attron(COLOR_PAIR(7) | A_BOLD);
				else attron(COLOR_PAIR(2) | A_BOLD);
				addch(map[a][b]);
				standend();
			}
			attron(COLOR_PAIR(2) | A_BOLD);
			mvprintw(maxY-1, 0, "Zombies killed: %i", zombies_killed);
			attroff(COLOR_PAIR(2) | A_BOLD);

		for(int i=0; i < 100; ++i)
			++zombies[i].balancer;
	}
	char text[] = " You lost... ";
	attron(COLOR_PAIR(7) | A_BOLD);
	mvprintw(maxY / 2, (maxX - sizeof(text)) / 2, "%s", text);
	attroff(COLOR_PAIR(7) | A_BOLD);

	//the player needs the press enter to quit (so he gets time to see how many zombies he killed)
	timeout(-1);
	do
	{
		ch = getch();
	} while(ch != 10);

	endwin();
	return 0;
}