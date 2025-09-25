/*
This project uses the Raylib framework to provide us functionality for math, graphics, GUI, input etc.
See documentation here: https://www.raylib.com/, and examples here: https://www.raylib.com/examples.html
*/

#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "game.h"
#include <vector>
#include <iostream>

using namespace std;

struct PhysicsBody
{
	Vector2 position = Vector2Zeros; // 0 by default (Zeroes for constant)
	Vector2 velocity = Vector2Zeros;
	float drag = 1.0f; // No dampening by default
	// float mass; 
};

class PhysicsSimulation
{
	float dt = 1.0f / TARGET_FPS; //seconds/frame
	float time = 0;

	Vector2 gravity = { 0, 9.81f }; // Gravity acceleration

public:
	const unsigned int TARGET_FPS = 50; //frames/second
	std::vector<PhysicsBody> objects;

	void updateTime()
	{
		time += dt;
	}

	void UpdateObjectPositions()
	{
		for (int i = 0; i < objects.size(); ++i)
		{
			PhysicsBody& o = objects[i];
			o.velocity += gravity * dt;
			o.position += o.velocity * dt;
		}
	}
};


Vector2 launchPosition = { 200, 200 };
float launchAngle = 300.0f;
float launchSpeed = 200.0f;

//Display world state
void draw(PhysicsSimulation& sim)
{
	BeginDrawing();
	ClearBackground(WHITE);

	// Slider variables
	float rectangleWidth = 400;
	GuiSliderBar(Rectangle{ InitialWidth - rectangleWidth - 60, 15, rectangleWidth, 20 }, "Launch Angle", 
		TextFormat("%.2f", launchAngle), &launchAngle, 0, 360.0f);
	GuiSliderBar(Rectangle{ InitialWidth - rectangleWidth - 60, 45, rectangleWidth, 20 }, "Launch Speed", 
		TextFormat("%.2f", launchSpeed), &launchSpeed, 50, 500);
	GuiSliderBar(Rectangle{ InitialWidth - rectangleWidth - 60, 75, rectangleWidth, 20 }, "Launch X", 
		TextFormat("%.2f", launchPosition.x), &launchPosition.x, 0, GetScreenWidth());
	GuiSliderBar(Rectangle{ InitialWidth - rectangleWidth - 60, 105, rectangleWidth, 20 }, "Launch Y", 
		TextFormat("%.2f", launchPosition.y), &launchPosition.y, 0, GetScreenHeight());

	// Text for slider variables
	DrawText(TextFormat("Angle: %.2f", launchAngle), 10, 15, 20, BLACK);
	DrawText(TextFormat("Speed: %.2f", launchSpeed), 10, 45, 20, BLACK);
	DrawText(TextFormat("Launch Position: (%.2f, %.2f)", launchPosition.x, launchPosition.y), 10, 75, 20, BLACK);

	// Circle representing the launch position
	DrawCircleV(launchPosition, 10, ORANGE);

	// Line representing the launch angle and speed
	Vector2 velocityVector = Vector2Rotate(Vector2UnitX, DEG2RAD * launchAngle) * launchSpeed;
	DrawLineV(launchPosition, launchPosition + velocityVector, RED);

	for (const PhysicsBody& o : sim.objects)
		DrawCircleV(o.position, 10, RED);

	EndDrawing();
}

int main()
{
	PhysicsSimulation sim;

	InitWindow(InitialWidth, InitialHeight, "Angry Birds");
	SetTargetFPS(sim.TARGET_FPS);

	while (!WindowShouldClose()) // Loops TARGET_FPS times per second
	{
		if (IsKeyPressed(KEY_SPACE))
		{
			PhysicsBody b;
			b.position = launchPosition;
			b.velocity = Vector2Rotate(Vector2UnitX, DEG2RAD * launchAngle) * launchSpeed;
			
			sim.objects.push_back(b);
		}

		if (IsKeyPressed(KEY_U))
			launchAngle = 0;
		else if (IsKeyPressed(KEY_I))
			launchAngle = 45;
		else if (IsKeyPressed(KEY_O))
			launchAngle = 60;
		else if (IsKeyPressed(KEY_P))
			launchAngle = 90;
		else if (IsKeyPressed(KEY_J))
			launchAngle = 315;
		else if (IsKeyPressed(KEY_K))
			launchAngle = 300;
		else if (IsKeyPressed(KEY_L))
			launchAngle = 270;

		sim.updateTime();
		sim.UpdateObjectPositions();
		draw(sim);
	}

	CloseWindow();
	return 0;
}