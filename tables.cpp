#include "raylib.h"
#include <vector>
#include <cmath>
#include <chrono>
#include <random>

std::mt19937 ran(time(0));

int irand(int a, int b){
	return a+(ran()%(b-a+1));
}

struct wall{
	Vector2 first;
	Vector2 second;
	float charge;
};

struct table{
	Vector2 position;
	float r;
	float R;
	float charge;
	Vector2 forceVector;
	Vector2 Size;
};

void DrawWalls(std::vector<wall>& walls){
	for(int i=0;i<walls.size();i++){
		DrawLineV(walls[i].first, walls[i].second, DARKGRAY);
	}
}

void DrawTableCircles(std::vector<table>& tables){
	for(int i=0;i<tables.size();i++){
		DrawCircleLines(tables[i].position.x,tables[i].position.y, tables[i].R, {0, 158, 47, 150});
		DrawCircleV(tables[i].position, tables[i].r, {0, 228, 48, 150});
		int a = irand(0, 1);
		DrawRectangle(tables[i].position.x - tables[i].Size.x, tables[i].position.y - tables[i].Size.y, 2*tables[i].Size.x, 2*tables[i].Size.y, {255, 161, 0, 180});
	}
}

void CreateBox(std::vector<wall>& walls){
	walls.resize(8);
	
	walls[0].first.x = 160;
	walls[0].first.y = 100;
	walls[0].second.x = 960;
	walls[0].second.y = 100;
	
	walls[1].first.x = 960;
	walls[1].first.y = 100;
	walls[1].second.x = 960;
	walls[1].second.y = 400;
	
	walls[2].first.x = 960;
	walls[2].first.y = 400;
	walls[2].second.x = 760;
	walls[2].second.y = 400;
	
	walls[3].first.x = 760;
	walls[3].first.y = 400;
	walls[3].second.x = 760;
	walls[3].second.y = 600;
	
	walls[4].first.x = 760;
	walls[4].first.y = 600;
	walls[4].second.x = 100;
	walls[4].second.y = 600;
	
	walls[5].first.x = 100;
	walls[5].first.y = 600;
	walls[5].second.x = 100;
	walls[5].second.y = 440;
	
	walls[6].first.x = 100;
	walls[6].first.y = 440;
	walls[6].second.x = 160;
	walls[6].second.y = 440;
	
	walls[7].first.x = 160;
	walls[7].first.y = 440;
	walls[7].second.x = 160;
	walls[7].second.y = 100;
	
	walls[0].charge = walls[1].charge = walls[2].charge = walls[3].charge = walls[4].charge = walls[5].charge = walls[6].charge = walls[7].charge = 50;
}

float GetLength(Vector2 input){
	return sqrt(input.x*input.x+input.y*input.y);
}

Vector2 GetDirection(Vector2 input){
	float len = GetLength(input);
	input.x = input.x/len;
	input.y = input.y/len;
	return input;
}

Vector2 GetForceVector(Vector2 pos1/*current table*/, Vector2 pos2/*from where force is*/, float charge1, float charge2){ // pos1 is current processing table
	Vector2 direction;												// pos2 is an object, which influence we
	direction.x = pos1.x - pos2.x;									// are calculating
	direction.y = pos1.y - pos2.y;
	float distance = GetLength(direction);
	direction = GetDirection(direction);
	float force = charge1*charge2/(distance*distance);
	Vector2 forceVector;
	forceVector.x = direction.x*force/3; 
	forceVector.y = direction.y*force/3;
	return forceVector;
}

Vector2 GetForceFromWalls(table Table, std::vector<wall>& walls){
	Vector2 upper,lower,left,right;
	float miny = 720,maxy = 0,minx = 1280,maxx = 0;
	for(int i=0;i<walls.size();i++){
		if((Table.position.x > walls[i].first.x && Table.position.x < walls[i].second.x)||(Table.position.x < walls[i].first.x && Table.position.x > walls[i].second.x)){
			float Yw,Yv,Xw,Xv;
			if(walls[i].first.y >= walls[i].second.y){
				Yw = walls[i].first.y;
				Yv = walls[i].second.y;
				Xw = walls[i].first.x;
				Xv = walls[i].second.x;
			} else {
				Yv = walls[i].first.y;
				Yw = walls[i].second.y;
				Xv = walls[i].first.x;
				Xw = walls[i].second.x;
			}
			float X = Table.position.x,Y = Table.position.y;
			float XwXv = fabs(Xw - Xv);
			float YwYv = Yw - Yv;
			float XvX = fabs(Xv - X);
			float YvY0 = (YwYv*XvX)/XwXv;
			float Y0 = Yv+YvY0;
			if(Y0 > Y){
				if(Y0 < miny){
					miny = Y0;
				}
			} else {
				if(Y0 > maxy){
					maxy = Y0;
				}
			}
		} 
		if((Table.position.y > walls[i].first.y && Table.position.y < walls[i].second.y)||(Table.position.y < walls[i].first.y && Table.position.y > walls[i].second.y)){
			float Yw,Yv,Xw,Xv;
			if(walls[i].first.x >= walls[i].second.x){
				Yw = walls[i].first.y;
				Yv = walls[i].second.y;
				Xw = walls[i].first.x;
				Xv = walls[i].second.x;
			} else {
				Yv = walls[i].first.y;
				Yw = walls[i].second.y;
				Xv = walls[i].first.x;
				Xw = walls[i].second.x;
			}
			float X = Table.position.x,Y = Table.position.y;
			float YwYv = fabs(Yw - Yv);
			float XwXv = Xw - Xv;
			float YvY = fabs(Yv - Y);
			float XvX0 = (XwXv*YvY)/YwYv;
			float X0 = Xv + XvX0;
			if(X0 > X){
				if(X0 < minx){
					minx = X0;
				}
			} else {
				if(X0 > maxx){
					maxx = X0;
				}
			}
		}
	}
	upper.x = Table.position.x;
	upper.y = maxy;
	lower.x = Table.position.x;
	lower.y = miny;
	left.x = maxx;
	left.y = Table.position.y;
	right.x = minx;
	right.y = Table.position.y;
	upper = GetForceVector(Table.position, upper, Table.charge, 10);
	lower = GetForceVector(Table.position, lower, Table.charge, 10);
	left = GetForceVector(Table.position, left, Table.charge, 10);
	right = GetForceVector(Table.position, right, Table.charge, 10);
	Vector2 forceVector;
	forceVector.x = upper.x+lower.x+left.x+right.x;
	forceVector.y = upper.y+lower.y+left.y+right.y;
	return forceVector;
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "f3cking t@bles");
    
    std::vector<wall> walls;
    std::vector<table> tables;
    CreateBox(walls);
    
    const float tableWidth = 20;
    const float tableLength = 60;
    
    bool stable = true; // checks if the sistem is stable? if new table added its unstable
    
    int curNum = 0;
    int tableNum = 8;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
		if(curNum < tableNum){
			table Da;
			Da.charge = 30;
			Da.r = tableWidth;
			Da.R = sqrt(tableWidth*tableWidth+tableLength*tableLength);
			int a = irand(0,1);
			if(a == 1){
				Da.Size.x = tableWidth;
				Da.Size.y = tableLength;
			} else {
				Da.Size.x = tableLength;
				Da.Size.y = tableWidth;
			}
			float x = 0,y = 0;
			for(int i=0;i<walls.size();i++){
				x += walls[i].first.x;
				y += walls[i].first.y;
				x += walls[i].second.x;
				y += walls[i].second.y;
			}
			x = x/walls.size()/2;
			y = y/walls.size()/2;
			Da.position.x = x+irand(-10, 10);
			Da.position.y = y+irand(-10, 10);
			tables.push_back(Da);
			stable = false;
			curNum++;
		}
		while(!stable){
			//------------------------------------------------------------------------------
			//calculating forces
			//------------------------------------------------------------------------------
			for(int i=0;i<tables.size();i++){
				Vector2 forceVector = GetForceFromWalls(tables[i], walls);
				for(int g=0;g<tables.size();g++){
					if(g!=i){
						Vector2 v = GetForceVector(tables[i].position, tables[g].position, tables[i].charge, tables[g].charge);
						forceVector.x += v.x;
						forceVector.y += v.y;
					}
				}
				tables[i].forceVector = forceVector;
			}
			//------------------------------------------------------------------------------
			//checking if the sistem is stable
			//------------------------------------------------------------------------------
			bool flag = true;
			for(int i=0;i<tables.size();i++){
				if(GetLength(tables[i].forceVector)>0.005){
					flag = false;
					break;
				}
			}
			//------------------------------------------------------------------------------
			//if the sistem is unstable, move objects, else end cycle
			//------------------------------------------------------------------------------
			if(flag){
				stable = true;
				break;
			} else {
				for(int i=0;i<tables.size();i++){
					tables[i].position.x += tables[i].forceVector.x;
					tables[i].position.y += tables[i].forceVector.y;
					tables[i].forceVector.x = 0;
					tables[i].forceVector.y = 0;
				}
			}
		}
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText("fuck", 10, 10, 20, DARKGRAY);

			DrawWalls(walls);
			
			DrawTableCircles(tables);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}