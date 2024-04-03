#include <iostream>
#include <cmath>
#include <vector>
#include <windows.h>

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
int targetx, targety;
int startx, starty;

class tile {
	public:
		int x, y;
		int pathx, pathy;
		int gcost = 0; // distance to the start point
		int hcost = 0; // distance to the target
		int fcost = 0; // sum of these two
		int timesused = 0;
		
		void costcalc(int oldy, int oldx) {
			// gcost calc
			int xcost = abs(oldx - x);
			int ycost = abs(oldy - y);
			while(true) {
				if (xcost > 0 && ycost > 0) {
					xcost--;
					ycost--;
					gcost += 14;
				}
				else if (xcost > 0) {
					xcost--;
					gcost +=10;
				}
				else if (ycost > 0) {
					ycost--;
					gcost +=10;
				}
				else break;
			}
			
			// hcost calc
			xcost = abs(targetx - x);
			ycost = abs(targety - y);
			while(true) {
				if (xcost > 0 && ycost > 0) {
					xcost--;
					ycost--;
					hcost += 14;
				}
				else if (xcost > 0) {
					xcost--;
					hcost += 10;
				}
				else if (ycost > 0) {
					ycost--;
					hcost += 10;
				}
				else break;
			}
			
			// fcost calc
			fcost = gcost + hcost;
			
			// for determinating the final path
			timesused++;
			pathx = oldx;
			pathy = oldy;
		}
};

void deletelastlines(int count) {
	Sleep(200);
	for (int i = 0; i < count; i++) {
		std::cout << "\r";
		CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
		GetConsoleScreenBufferInfo(hConsole, &bufferInfo);
		COORD coord = bufferInfo.dwCursorPosition;
		coord.Y--;
		SetConsoleCursorPosition(hConsole, coord);
	}
}

int main() {
	int mapx = 12, mapy = 5;
	int map[mapy][mapx] = {
			{0, 0, 0, 0, 3, 3, 0, 3, 0, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 3, 0},
			{0, 0, 3, 0, 0, 3, 0, 0, 0, 0, 3, 0},
			{0, 1, 3, 3, 0, 3, 0, 3, 3, 0, 0, 0},
			{0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 0, 0},
			};
			
	int intmap[mapy][mapx] = {};
	tile pathmap[mapy][mapx] = {};
	
	for(int i = 0; i < mapy; i++) {
		for(int j = 0; j < mapx; j++) {
			int element = map[i][j];
			switch(element) {
				case 0:
					intmap[i][j] = 0; // empty
					break;
				case 1:
					targety = i;
					targetx = j;
					//intmap[i][j] = 1; // target
					break;
				case 2:
					starty = i;
					startx = j;
					intmap[i][j] = 2; // entity
					//pathmap[i][j].fcost = 2;
					break;
				case 3:
					intmap[i][j] = 3; // obstacle
					//pathmap[i][j].fcost = 3;
					break;
			}
			pathmap[i][j].y = i;
			pathmap[i][j].x = j;
			//std::cout <<pathmap[i][j].x << " " << pathmap[i][j].y << "\n";
		}
	}
	
	// pathing
	std::vector<int> posxvec, posyvec;
	
	posxvec.push_back(startx);
	posyvec.push_back(starty);
	
	int posx, posy, mostused = 0;
	
	while(true) {
		// moving
		for(unsigned int i = 0; i < posxvec.size(); i++) {
			for(int j = 0; j < 8; j++) {
				posx = posxvec[i];
				posy = posyvec[i];
				int oldx = posx, oldy = posy;
				switch(j) {
					case 0:
						posx--;
						break;
					case 1:
						posx++;
						break;
					case 2:
						posy--;
						break;
					case 3:
						posy++;
						break;
					case 4:
						posx--;
						posy--;
						break;
					case 5:
						posx++;
						posy--;
						break;
					case 6:
						posx--;
						posy++;
						break;
					case 7:
						posx++;
						posy++;
						break;
				}
				if (targetx == posx && targety == posy) {
					pathmap[posy][posx].pathx = oldx;
					pathmap[posy][posx].pathy = oldy;
					goto finish;
				}
				if (posx > -1 && posx < mapx && posy > -1 && posy < mapy && pathmap[posy][posx].timesused == 0 && intmap[posy][posx] == 0) {
					pathmap[posy][posx].costcalc(oldy, oldx);
				}
				pathmap[oldy][oldx].timesused++;
			}
		}
		// selecting the pos of the tile with the least fcost, can be multiple
		posxvec.clear();
		posyvec.clear();
		int lowestfcost = 10 * mapx * mapy;
		for(int i = 0; i < mapy; i++) {
			for(int j = 0; j < mapx; j++) {
				if(pathmap[i][j].fcost < lowestfcost && pathmap[i][j].fcost != 0 && pathmap[i][j].timesused == 1)
					lowestfcost = pathmap[i][j].fcost;
			}
		}
		for(int i = 0; i < mapy; i++) {
			for(int j = 0; j < mapx; j++) {
				if(pathmap[i][j].fcost == lowestfcost) {
					posxvec.push_back(pathmap[i][j].x);
					posyvec.push_back(pathmap[i][j].y);
				}
			}
		}
	}
	
	finish:
		
	for(int i = 0; i < mapy; i++) { // intmap
		for(int j = 0; j < mapx; j++) {
			if (map[i][j] == 3) SetConsoleTextAttribute(hConsole, 12);
			if (map[i][j] == 0) SetConsoleTextAttribute(hConsole, 15);
			if (map[i][j] == 1) SetConsoleTextAttribute(hConsole, 10);
			if (map[i][j] == 2) SetConsoleTextAttribute(hConsole, 9);
			std::cout << map[i][j] << " ";
		}
		std::cout << "\n";
	}
	std::cout << "-------\n";	
	
	// tracking the path
	posx = targetx;
	posy = targety;
	for(int i = 0; i < mapy; i++) { // intmap
		for(int j = 0; j < mapx; j++) {
			if (intmap[i][j] == 3) SetConsoleTextAttribute(hConsole, 12);
			if (intmap[i][j] == 0) SetConsoleTextAttribute(hConsole, 15);
			if (intmap[i][j] == 9) SetConsoleTextAttribute(hConsole, 9);
			if (intmap[i][j] == 2) SetConsoleTextAttribute(hConsole, 9);
			if (i == targety && j == targetx) SetConsoleTextAttribute(hConsole, 10);
			std::cout << intmap[i][j] << " ";
		}
		std::cout << "\n";
	}
	
	// 
	std::cin.get();
	Sleep(1000);
	deletelastlines(mapy+1);
	bool willDelete = false;
	while(posx != startx || posy != starty) {
		if (willDelete) {
			deletelastlines(mapy);
		}
		else willDelete = true;
		int newposx = pathmap[posy][posx].pathx;
		int newposy = pathmap[posy][posx].pathy;
		posx = newposx;
		posy = newposy;
		intmap[posy][posx] = 9;
		for(int i = 0; i < mapy; i++) { // intmap
			for(int j = 0; j < mapx; j++) {
				if (intmap[i][j] == 3) SetConsoleTextAttribute(hConsole, 12);
				if (intmap[i][j] == 0) SetConsoleTextAttribute(hConsole, 15);
				if (intmap[i][j] == 9) SetConsoleTextAttribute(hConsole, 9);
				if (intmap[i][j] == 2) SetConsoleTextAttribute(hConsole, 9);
				if (i == targety && j == targetx) SetConsoleTextAttribute(hConsole, 10);
				std::cout << intmap[i][j] << " ";
			}
			std::cout << "\n";
		}
	}
	
	// printing
	
	/*for(int i = 0; i < mapy; i++) { // intmap
		for(int j = 0; j < mapx; j++) {
			if (intmap[i][j] == 3) SetConsoleTextAttribute(hConsole, 12);
			if (intmap[i][j] == 0) SetConsoleTextAttribute(hConsole, 15);
			if (intmap[i][j] == 9) SetConsoleTextAttribute(hConsole, 9);
			if (i == targety && j == targetx) SetConsoleTextAttribute(hConsole, 10);
			std::cout << pathmap[i][j].fcost << "\t";
		}
		std::cout << "\n\n\n\n";
	}*/
	std::cout << "-------\n";
	
	system("pause");
}
