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
};

void DrawWalls(std::vector<wall>& walls){
	for(int i=0;i<walls.size();i++){
		DrawLineV(walls[i].first, walls[i].second, DARKGRAY);
	}
}

void DrawTableCircles(std::vector<table>& tables){
	for(int i=0;i<tables.size();i++){
		DrawCircleLines(tables[i].position.x,tables[i].position.y, tables[i].R, LIME);
		DrawCircleV(tables[i].position, tables[i].r, GREEN);
	}
}

void CreateBox(std::vector<wall>& walls){
	walls.resize(4);
	
	walls[0].first.x = 50;
	walls[0].first.y = 100;
	walls[0].second.x = 50;
	walls[0].second.y = 300;
	
	walls[1].first.x = 50;
	walls[1].first.y = 100;
	walls[1].second.x = 250;
	walls[1].second.y = 100;
	
	walls[2].first.x = 250;
	walls[2].first.y = 100;
	walls[2].second.x = 250;
	walls[2].second.y = 300;
	
	walls[3].first.x = 50;
	walls[3].first.y = 300;
	walls[3].second.x = 250;
	walls[3].second.y = 300;
	
	walls[0].charge = walls[1].charge = walls[2].charge = walls[3].charge = 10;
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

Vector2 GetForceVector(Vector2 pos1, Vector2 pos2, float charge1, float charge2){ // pos1 is current processing table
	Vector2 direction;															 // pos2 is an object, which influence we
	direction.x = pos1.x - pos2.x;												 // are calculating
	direction.y = pos1.y - pos2.y;
	float distance = GetLength(direction);
	direction = GetDirection(direction);
	float force = charge1*charge2/(distance*distance);
	Vector2 forceVector;
	forceVector.x = direction.x*force/10; 
	forceVector.y = direction.y*force/10;
	return forceVector;
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "f3cking t@bles");
    
    std::vector<wall> walls;
    std::vector<table> tables;
    CreateBox(walls);
    
    const float tableWidth = 10;
    const float tableLength = 30;
    
    bool stable = true; // checks if the sistem is stable? if new table added its unstable
    
    int curNum = 0;
    int tableNum = 2;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
		if(curNum < tableNum){
			table Da;
			Da.charge = 10;
			Da.r = tableWidth;
			Da.R = sqrt(tableWidth*tableWidth+tableLength*tableLength);
			if(curNum == 0){
				float x = 0,y = 0;
				for(int i=0;i<walls.size();i++){
					x += walls[i].first.x;
					y += walls[i].first.y;
					x += walls[i].second.x;
					y += walls[i].second.y;
				}
				x = x/walls.size()/2;
				y = y/walls.size()/2;
				Da.position.x = x;
				Da.position.y = y;
			} else {
				Da.position.x = tables[tables.size()-1].position.x + irand(-10, 10);
				Da.position.y = tables[tables.size()-1].position.y + irand(-10, 10);
			}
			tables.push_back(Da);
			stable = false;
			curNum++;
		}
		while(!stable){
			//------------------------------------------------------------------------------
			//calculating forces
			//------------------------------------------------------------------------------
			for(int i=0;i<tables.size();i++){
				Vector2 forceVector;
				forceVector.x = 0;
				forceVector.y = 0;
				for(int g=0;g<walls.size();g++){
					Vector2 center;
					center.x = (walls[g].first.x + walls[g].second.x)/2;
					center.y = (walls[g].first.y + walls[g].second.y)/2;
					Vector2 v = GetForceVector(tables[i].position, center, tables[i].charge, walls[g].charge);
					forceVector.x += v.x;
					forceVector.y += v.y;
				}
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
				if(GetLength(tables[i].forceVector)>0.001){
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