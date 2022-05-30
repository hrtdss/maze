#include <iostream>
#include <stack>
#include <cstring>
#include <ctime>
#include <algorithm>
#include "graphics.h"

using namespace std;

const int WIDTH = 800, HEIGHT = 600; // ������  � ������  ���� ����������
const int SIDE = 40;  //���������� ����� � ���������

IMAGE *mainMenu, *gameWindow, *rulesWindow, *informationWindow, *fightWindow, *victoryScreen, *defeatScreen, *omtScreen;
IMAGE *wallTexture, *floorTexture, *playerPathTexture, *solutionPathCoin, *solutionPathEnemy, *exitTexture, *playerTexture, *enemyTexture, *coinTexture, *abilities[3];

struct cellstruct
{
   bool visited = 0; // �������� �� ������ ���������� ��������� ���������
   bool isWall = 1; // ������ ��� ������� �������� ������ ����� ������ ��������� ��������� ���������
   bool visitedByPlayer = 0; // �������� �� ������ ������� �� ����� ����������� ���������
   bool isCoin = 0; // ���� �� ������ � ������
   bool isEnemy = 0; // ���� �� ���� � ������
};

cellstruct c[SIDE][SIDE]; // ���� ����� �������� SIDE*SIDE
stack <pair<int, int> > cellStack; // ������ ���������, �������� ���������� ������ � �������� ��������� ���������

int eX = SIDE - 2, eY = SIDE - 2; //  ������� ������-������
int currX, currY; // ��������� ������ � �������� ����
int computerAbility; // ����������� ���������� � ����-���� (���������� ��������� �������)
int menuStatus; // � ����� ������ ���� ��������� ������������
int collectedCoins, defeatedEnemies; // ���������� ��������� �����; ���������� ����������� ������
int initialPosX, initialPosY, finalPosX, finalPosY; // ���������� ������ �������� � ������� ������� ����� ���� ���������
int dx[5] = {0, 0, 0, -1, 1}, dy[5] = {0, 1, -1, 0, 0}; // ������� �������
char buffer[1000]; // ������, ������������ ��� ������ ����� �� �����
bool wasHere[SIDE][SIDE]; // ���� �����, ���������� ���������� ������ �������
bool correctPath[SIDE][SIDE]; // ���� �����, ������������ �������(���� � ������)
bool isNewGame = 1, isGameGoingOn; // �������� �� ����� ����; ���� �� ������� ����

void loadFiles() // �������� ����������� �����������
{
   mainMenu = loadBMP("bmpImages/MainMenu.bmp");
   gameWindow = loadBMP("bmpImages/GameWindow.bmp");
   rulesWindow = loadBMP("bmpImages/Rules.bmp");
   informationWindow = loadBMP("bmpImages/AboutTheProgram.bmp");
   fightWindow = loadBMP("bmpImages/fight.bmp");
   victoryScreen = loadBMP("bmpImages/victoryScreen.bmp");
   defeatScreen = loadBMP("bmpImages/defeatScreen.bmp");
   omtScreen = loadBMP("bmpImages/omtScreen.bmp");
   wallTexture = loadBMP("bmpImages/wall.bmp");
   floorTexture = loadBMP("bmpImages/floor.bmp");
   playerPathTexture = loadBMP("bmpImages/playerPath.bmp");
   playerTexture = loadBMP("bmpImages/player.bmp");
   enemyTexture = loadBMP("bmpImages/enemy.bmp");
   coinTexture = loadBMP("bmpImages/coin.bmp");
   exitTexture = loadBMP("bmpImages/exit.bmp");
   solutionPathCoin = loadBMP("bmpImages/solutionWayCoin.bmp");
   solutionPathEnemy = loadBMP("bmpImages/solutionWayEnemy.bmp");
   abilities[0] = loadBMP("bmpImages/fireBall.bmp");
   abilities[1] = loadBMP("bmpImages/waterWave.bmp");
   abilities[2] = loadBMP("bmpImages/magicShield.bmp");
}

void createLevel() // �������� ������
{
   initialPosX = 14; initialPosY = 27; // ���������� ������ �������� ���� ���������
   finalPosX = (initialPosX + SIDE * 14), finalPosY = (initialPosX + SIDE * 14); // ���������� ������� ������� ���� ���������
   
   c[0][0].visitedByPlayer = 1;
   c[0][0].visited = 1;
   c[0][0].isWall = 0;
   cellStack.push(make_pair(0, 0));

   currX = 0, currY = 0;
   isGameGoingOn = 1;
}

int getNeighbours(int x, int y) // ����� ����������� ���������� ���������
{
   vector <int> var;
   
   if (y + 2 < SIDE && !c[x][y + 2].visited)
      var.push_back(1);
   
   if (y - 2 >= 0 && !c[x][y - 2].visited)
      var.push_back(2);
   
   if (x - 2 >= 0 && !c[x - 2][y].visited)
      var.push_back(3);
   
   if (x + 2 < SIDE && !c[x + 2][y].visited)
      var.push_back(4);
   
   if (var.size() == 0)
      return 0;
   
   return var[rand() % var.size()];
}

void generateMaze() // ��������� ��������� ����� � �������������� �����
{
   int currCellX = 0, currCellY = 0;
   int coinsQuantity = 5 + rand() % 15, enemyQuantity = 5 + rand() % 10; // ���������� ����� � ������ � ���������
   bool flag = 0;

   while (1)
   {
      int neighbours = getNeighbours(currCellX,currCellY);
      int coinX = 1 + rand() % 38, coinY = 1 + rand() % 38, enemyX = 1 + rand() % 38, enemyY = 1 + rand() % 38;

      if (flag && currCellX == 0 && currCellY == 0)
         break;

      flag = 1;
      
      if (neighbours == 0)
      {
         cellStack.pop();
         currCellX = cellStack.top().first;
         currCellY = cellStack.top().second;

         continue;
      }
      
      c[currCellX + dx[neighbours]][currCellY + dy[neighbours]].isWall = 0;

      currCellY += 2 * dy[neighbours];
      currCellX += 2 * dx[neighbours];
      
      c[currCellX][currCellY].isWall = 0;
      c[currCellX][currCellY].visited = 1;
      
      cellStack.push(make_pair(currCellX, currCellY)); 
      
      if (coinsQuantity > 0 && !c[coinX][coinY].isWall)
      {
         coinsQuantity--;
         c[coinX][coinY].isCoin = 1;
      }

      if (enemyQuantity > 0 && !c[enemyX][enemyY].isWall && !c[enemyX][enemyY].isCoin)
      {
         enemyQuantity--;
         c[enemyX][enemyY].isEnemy = 1;
      }
   }
}

void drawMaze() // ��������� ���������
{
   putimage(0, 13, wallTexture, COPY_PUT);
   
   for (int i = 0; i < SIDE ;i++)
      for (int j = 0; j < SIDE ;j++)
      {
         int x = initialPosX + i * 14, y = initialPosY + j * 14;
         
         putimage(x, 13, wallTexture, COPY_PUT);
         putimage(0, y, wallTexture, COPY_PUT);
         
         if (c[i][j].isWall)
            putimage(x, y, wallTexture, COPY_PUT);
         else
            putimage(x, y, floorTexture, COPY_PUT);

         if (c[i][j].isCoin)
            putimage(x, y, coinTexture, COPY_PUT);

         if (c[i][j].isEnemy)
            putimage(x, y, enemyTexture, COPY_PUT);
         
         if (x == finalPosX - 28 && y == finalPosY - 15)
            putimage(x, y, exitTexture, COPY_PUT);

      }
}

void restorePath() // ��������� ����������� ����
{
   for (int i = 0; i < SIDE ;i++)
      for (int j = 0; j < SIDE ;j++)
         if (i == 0 && j == 0 || c[i][j].visitedByPlayer)
         {
            int x = initialPosX + i * 14, y = initialPosY + j * 14;
            putimage(x, y, playerPathTexture, COPY_PUT);
         }
}

bool recursiveSolve(int x, int y) // ����������� ����� ������ �� ���������
{
   if (x == eX && y == eY)
      return 1;

   if (c[x][y].isWall == 1 || wasHere[x][y])
      return 0;

   wasHere[x][y] = 1;

   if (y != 0 && recursiveSolve(x, y - 1))
   {
      correctPath[x][y] = 1;
      return 1;
   }

   if (y != SIDE - 1 && recursiveSolve(x, y + 1))
   {
      correctPath[x][y] = 1;
      return 1;
   }

   if (x != 0 && recursiveSolve(x - 1, y))
   {
      correctPath[x][y] = 1;
      return 1;
   }

   if (x != SIDE - 1 && recursiveSolve(x + 1, y))
   {
      correctPath[x][y] = 1;
      return 1;
   }

   return 0;
}

void clearMaze() // ������� ���� �������� � ���������� ��� ����������� ��������� ������ ���������
{
   for (int i = 0; i < SIDE ;i++)
      for (int j = 0; j < SIDE ;j++)
      {
         c[i][j].visited = 0;
         c[i][j].isWall = 1;
         c[i][j].visitedByPlayer = 0;
         c[i][j].isCoin = 0;
         c[i][j].isEnemy = 0;
         wasHere[i][j] = 0;
         correctPath[i][j] = 0;
      }

   isNewGame = 1;
}

void erasePlayer(int x, int y) // �������� ������� ������ ����������
{
   int xx = initialPosX + x * 14, yy = initialPosY + y * 14;
   putimage(xx, yy, playerPathTexture, COPY_PUT);
}

void drawPlayer(int x, int y) // ���������� ������ � ������� ������
{
   int xx = initialPosX + x * 14, yy = initialPosY + y * 14;
   putimage(xx, yy, playerTexture, COPY_PUT);
}

void initializeGame() // ������������� ����
{
   setbkcolor(COLOR(35,35,35));
   putimage(0, 0, gameWindow, COPY_PUT);

   createLevel();
   generateMaze();
   drawMaze();
   drawPlayer(currX, currY);

   if (isNewGame)
   {
      setcolor(WHITE);
      settextstyle(10, 0, 4);

      outtextxy(680, 190, "0");
      outtextxy(680, 271, "0");

      isNewGame = 0;
   }
}

void drawSolution() // ��������� ���� � ������ �� ���������
{
   for (int i = 0; i < SIDE ;i++)
      for (int j = 0; j < SIDE ;j++)
         if (correctPath[i][j] && !c[i][j].visitedByPlayer)
         {
            int x = initialPosX + i * 14, y = initialPosY + j * 14;

            setfillstyle(SOLID_FILL, RED);
            bar(x, y, x + 13, y + 13);
            
            if (c[i][j].isCoin)
               putimage(x, y, solutionPathCoin, COPY_PUT);

            if (c[i][j].isEnemy)
               putimage(x, y, solutionPathEnemy, COPY_PUT);
         }
         
   clearMaze();
   collectedCoins = defeatedEnemies = 0;

   char ch = getch();
   if (ch == 110 || ch == -14) // N
      initializeGame();
   else
      isGameGoingOn = 0;
}

void displayCounters() // ��������� ���������� ��������� ����� � ����������� ������
{
   setbkcolor(COLOR(35,35,35));
   setcolor(WHITE);

   int coins = snprintf(buffer, 1000, "%d", collectedCoins);
   if (collectedCoins <= 9)
      outtextxy(679, 190, buffer);
   else
      outtextxy(669, 190, buffer);

   int enemies = snprintf(buffer, 1000, "%d", defeatedEnemies);
   if (defeatedEnemies <= 9)
      outtextxy(679, 271, buffer);
   else
      outtextxy(669, 271, buffer);
}

void fight(int playerAbility, int computerAbility); // ����-���� �� ������ ���� "������-�������-������"

void wonInFight() // �������� � ������ ������ � ����-����
{
   c[currX][currY].isEnemy = 0;
   defeatedEnemies++;
   
   putimage(0, 0, victoryScreen, COPY_PUT);
   setactivepage(1);
   setvisualpage(0);
   delay(1500);
   putimage(0, 0, gameWindow, COPY_PUT);

   drawMaze();
   restorePath();

   displayCounters();
   
   setvisualpage(1);
}

void lostInFight() // �������� � ������ ��������� � ����-����
{
   putimage(0, 0, defeatScreen, COPY_PUT);
   delay(2000);

   clearMaze();
   collectedCoins = defeatedEnemies = 0;
   isGameGoingOn = 0;
}

void foundAnEnemy() // �������� � ������ ����������� �����
{
   setbkcolor(BLACK);
   setcolor(WHITE);
   putimage(0, 0, fightWindow, COPY_PUT);

   computerAbility = 1 + rand() % 3;
   putimage(230, 165, abilities[computerAbility - 1], COPY_PUT);

   bool isPlayerAbilitySelected = 0;
   int selectedAbility;
   clock_t end_time = clock() + 5 * CLOCKS_PER_SEC; // ������ �� 5 ������

   while (clock() < end_time)
   {
      int x, y;

      if (mousebuttons() == 1)
         x = mousex(), y = mousey();

      delay(100);

      if (x >= 62 && x <= 161 && y >= 388 && y <= 487) // fireball
         selectedAbility = 1, isPlayerAbilitySelected = 1;

      if (x >= 230 && x <= 329 && y >= 388 && y <= 487) // water wave
         selectedAbility = 2, isPlayerAbilitySelected = 1;

      if (x >= 398 && x <= 497 && y >= 388 && y <= 487) // magic shield
         selectedAbility = 3, isPlayerAbilitySelected = 1;

      if (isPlayerAbilitySelected)
      {
         fight(selectedAbility, computerAbility);
         break;
      }
   }

   if (!isPlayerAbilitySelected)
      lostInFight();
}

void drawInFight() // �������� � ������ ���������� ���������� � ����-����
{
   putimage(0, 0, omtScreen, COPY_PUT);
   delay(1500);

   foundAnEnemy();
}

void foundCoin() // �������� � ������ ����������� ������
{
   c[currX][currY].isCoin = 0;
   collectedCoins++;

   displayCounters();
}

void traceCell(int dir) // ����������� ������ � ��������� ����������� � �������� �������(�����) ������ �� ������� ������ ��� �����
{
   erasePlayer(currX, currY);

   switch (dir) // 1 - up, 2 - down, 3 - left, 4 - right
   {
   case 1:
      {
         if (!c[currX][currY - 1].isWall && currY)
            currY--;

         break;
      }
   case 2:
      {
         if (!c[currX][currY + 1].isWall)
            currY++;

         break;
      }
   case 3:
      {
         if (!c[currX - 1][currY].isWall && currX)
            currX--;

         break;
      }
   case 4:
      {
         if (!c[currX + 1][currY].isWall)
            currX++;

         break;
      }
   }

   c[currX][currY].visitedByPlayer = 1;

   if (c[currX][currY].isCoin)
      foundCoin();
   if (c[currX][currY].isEnemy)
      foundAnEnemy();

   drawPlayer(currX, currY);
}

void gameProcess() // ������� ���������� �� ������� ���� (����������� ������. ������ ���: ������, ������ ����� ����, ������������ �������)
{   
   while (isGameGoingOn)
   {
      if (c[eX][eY].visitedByPlayer) // ���� ����� �� �����(������), ���������� ���� � ����������� ������ ��������� � ����������� �����
      {
         clearMaze();
         initializeGame();
         displayCounters();
      }

      char ch = getch();
      delay(50);

      if (ch == 27) // ESC
      {
         delay(100);

         clearMaze();
         collectedCoins = defeatedEnemies = 0;
         isGameGoingOn = 0;
      }

      if (ch == 72 || ch == 119 || ch == -10) // up
         traceCell(1);

      if (ch == 78 || ch == 115 || ch == -5) // down
         traceCell(2);

      if (ch == 74 || ch == 97 || ch == -12) // left
         traceCell(3);

      if (ch == 76 || ch == 100 || ch == -30) // right
         traceCell(4);

      if (ch == 110 || ch == -14) // N
      {
         clearMaze();
         collectedCoins = defeatedEnemies = 0;
         initializeGame();
      }

      if (ch == 103 || ch == -17) // G
      {
         recursiveSolve(0, 0);
         drawSolution();
      }
   }
}

int menuButtons(int currentStatus) // ����� ������ ����
{
   int x, y;

   putimage(0, 0, mainMenu, COPY_PUT);

   while (1)
   {
      do
      {
         x = mousex();
         y = mousey();
      } while (mousebuttons() != 1);

      delay(100);

      if (x >= 140 && x <= 312 && y >= 243 && y <= 268) // new game button
         return 1;

      if (x >= 120 && x <= 332 && y >= 292 && y <= 318) //rules button
         return 2;

      if (x >= 124 && x <= 329 && y >= 344 && y <= 369) // about the programm button
         return 3;

      if (x >= 175 && x <= 278 && y >= 393 && y <= 422) // exit button
         return 4;
   }
}

void showRules() // ������������ �� ���� � ��������� ����
{
   int x, y;
   
   putimage(0, 0, rulesWindow, COPY_PUT);

   while (1)
   {
      do
      {
         x = mousex();
         y = mousey();
      } while (mousebuttons() != 1);

      delay(100);

      if (x >= 684 && x <= 766 && y >= 561 && y <= 582) // return button
         return;
   }
}

void showAbout() // ������������ �� ���� � ����������� � ����
{
   int x, y;
   
   putimage(0, 0, informationWindow, COPY_PUT);

   while (1)
   {
      do
      {
         x = mousex();
         y = mousey();
      } while (mousebuttons() != 1);

      delay(100);

      if (x >= 684 && x <= 766 && y >= 561 && y <= 582) // return button
         return;
   }
}

int main()
{
   srand(time(0));
   initwindow(WIDTH, HEIGHT, "SD", 560, 240);

   loadFiles();

   do
   {
      menuStatus = menuButtons(menuStatus);

      switch (menuStatus)
      {
      case 1:
         {
            initializeGame();
            gameProcess();
            break;
         }
      case 2:
         {
            showRules();
            break;
         }
      case 3:
         {
            showAbout();
            break;
         }
      }
   } while (menuStatus != 4);

   closegraph();

   return 0;
}

void fight(int playerAbility, int computerAbility)
{
   // 1 - fireball, 2 - water wave, 3 - magic shield
   if (playerAbility == computerAbility)
      drawInFight();

   if (computerAbility % 3 + 1 == playerAbility)
      wonInFight();

   if (computerAbility == playerAbility % 3 + 1)
      lostInFight();
}