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

struct PhysicsBody
{
	Vector2 position = Vector2Zeros; // 0 by default (Zeroes for constant)
	Vector2 velocity = Vector2Zeros;
	float drag = 1.0f; // No dampening by default
	float mass; 
};

class PhysicisSimulation
{
	float dt = 1.0f / TARGET_FPS; //seconds/frame
	float time = 0;

	Vector2 gravity = { 0, 9.81f }; // Gravity acceleration

public:
	std::vector<PhysicsBody> objects;

	void updateTime()
	{
		time += dt;
	}
};

const unsigned int TARGET_FPS = 50; //frames/second

Vector2 launchPosition = { 200, 200 };
float launchAngle = 300.0f;
float launchSpeed = 200.0f;

//Display world state
void draw()
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

	EndDrawing();

}

int main()
{
	InitWindow(InitialWidth, InitialHeight, "Angry Birds");
	SetTargetFPS(TARGET_FPS);

	PhysicisSimulation sim;

	while (!WindowShouldClose()) // Loops TARGET_FPS times per second
	{
		if (IsKeyPressed(KEY_SPACE))
		{
			PhysicsBody b;
			b.position = launchPosition;

			DrawCircleV(b.position, 10, RED);
			
			sim.objects.push_back(b);
		}

		sim.updateTime();
		draw();
	}

	CloseWindow();
	return 0;
}