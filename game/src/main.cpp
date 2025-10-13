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

//struct Circle
//{
//	Vector2 position = Vector2Zeros;
//	float radius = 0.0f;
//};

struct PhysicsBody
{
	Vector2 position = Vector2Zeros; 
	Vector2 velocity = Vector2Zeros;
	float drag = 1.0f; // No dampening by default
	// float mass; 
	float gravityScale = 1.0f;
	bool collision = false; // If the body collided this frame
	Color color = MAGENTA;

	float radius = 0.0f;
};

class PhysicsSimulation
{
	float dt = 1.0f / TARGET_FPS; //seconds/frame
	float time = 0;

public:
	const unsigned int TARGET_FPS = 50; //frames/second
	Vector2 gravity = { 0, 9.81f }; // Gravity acceleration
	std::vector<PhysicsBody> objects;

	void updateTime()
	{
		dt = 1.0f / TARGET_FPS;
		time += dt;
	}

	void UpdateObjectPositions()
	{
		for (int i = 0; i < objects.size(); ++i)
		{
			PhysicsBody& o = objects[i];
			Vector2 acc = gravity * o.gravityScale;

			o.velocity += acc * dt;
			o.position += o.velocity * dt;

			// Reset every loop
			o.collision = false;
		}
	}

	void CheckCollision()
	{
		for (int i = 0; i < objects.size(); ++i)
		{
			for (int j = i + 1; j < objects.size(); ++j)
			{
				PhysicsBody& a = objects[i];
				PhysicsBody& b = objects[j];
				bool collision = CircleCircle(a.position, a.radius, b.position, b.radius);
				a.collision |= collision;
				b.collision |= collision; // only if single true
			}
		}
	}

	bool CircleCircle(Vector2 pos1, float rad1, Vector2 pos2, float rad2)
	{
		// distance calculated by pythagorean
		float distance = Vector2Distance(pos1, pos2);

		// If distance between the two radius are less than or equal to distance calculated
		return distance <= (rad1 + rad2);
	}
};


Vector2 launchPosition = { 200, 200 };
float launchAngle = 300.0f;
float launchSpeed = 150.0f;

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
		TextFormat("%.2f", launchSpeed), &launchSpeed, 25, 200);
	GuiSliderBar(Rectangle{ InitialWidth - rectangleWidth - 60, 75, rectangleWidth, 20 }, "Launch X", 
		TextFormat("%.2f", launchPosition.x), &launchPosition.x, 0, GetScreenWidth());
	GuiSliderBar(Rectangle{ InitialWidth - rectangleWidth - 60, 105, rectangleWidth, 20 }, "Launch Y", 
		TextFormat("%.2f", launchPosition.y), &launchPosition.y, 0, GetScreenHeight());
	GuiSliderBar(Rectangle{ InitialWidth - rectangleWidth - 60, 135, rectangleWidth, 20 }, "Gravity Y",
		TextFormat("%.2f", sim.gravity.y), & sim.gravity.y, -20, 20);
	GuiSliderBar(Rectangle{ InitialWidth - rectangleWidth - 60, 165, rectangleWidth, 20 }, "Gravity x",
		TextFormat("%.2f", sim.gravity.x), &sim.gravity.x, -15, 15);

	// Text for slider variables
	DrawText(TextFormat("Angle: %.2f", launchAngle), 10, 15, 20, BLACK);
	DrawText(TextFormat("Speed: %.2f", launchSpeed), 10, 45, 20, BLACK);
	DrawText(TextFormat("Launch Position: (%.2f, %.2f)", launchPosition.x, launchPosition.y), 10, 75, 20, BLACK);
	DrawText(TextFormat("Gravity: (%.2f, %.2f)", sim.gravity.x, sim.gravity.y), 10, 105, 20, BLACK);

	// Circle representing the launch position
	DrawCircleV(launchPosition, 10, ORANGE);

	// Line representing the launch angle and speed
	Vector2 velocityVector = Vector2Rotate(Vector2UnitX, DEG2RAD * launchAngle) * launchSpeed;
	DrawLineV(launchPosition, launchPosition + velocityVector, RED);

	for (const PhysicsBody& o : sim.objects)
		DrawCircleV(o.position, o.radius, o.collision ? RED : o.color);

	EndDrawing();
}

int main()
{
	PhysicsSimulation sim;
	sim.objects.push_back({});

	//// Describe the static circle first
	//PhysicsBody& circle = sim.objects.back();
	//circle.position = { 400.0f, 400.0f };
	//circle.radius = 20.0f;
	//circle.gravityScale = 0.0f;

	//sim.objects.push_back({});
	//circle = sim.objects.back();

	PhysicsBody* circle = &sim.objects.back();
	circle->position = { 400.0f, 400.0f };
	circle->radius = 20.0f;
	circle->gravityScale = 0.0f;
	circle->color = GREEN;

	sim.objects.push_back({});
	circle = &sim.objects.back();
	circle->position = { 415.0f, 415.0f };
	circle->radius = 20.0f;
	circle->gravityScale = 0.0f;
	circle->color = GREEN;

	PhysicsBody circleStatic, circleDynmamic;
	circleStatic.position = { 400.0f, 400.0f };

	InitWindow(InitialWidth, InitialHeight, "Angry Birds");
	SetTargetFPS(sim.TARGET_FPS);

	while (!WindowShouldClose()) // Loops TARGET_FPS times per second
	{
		circle->position = GetMousePosition();

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
		sim.CheckCollision();
		draw(sim);
	}

	CloseWindow();
	return 0;
}