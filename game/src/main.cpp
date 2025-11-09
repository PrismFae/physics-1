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
#include <cassert>

//struct Circle
//{
//	Vector2 position = Vector2Zeros;
//	float radius = 0.0f;
//};

enum ColliderType
{
	COLLIDER_TYPE_INVALID,
	COLLIDER_TYPE_CIRCLE,
	COLLIDER_TYPE_HALF_SPACE
	// COLLIDER_BOX
};

// Union to hold different collider types in a single variable
union Collider
{
	struct
	{
		float radius;
	} circle;

	struct
	{
		Vector2 normal; // Direction the half-space is facing
		float distance; // Distance from 0,0
	} halfSpace;
};

struct PhysicsBody
{
	Vector2 position = Vector2Zeros; 
	Vector2 velocity = Vector2Zeros;
	float drag = 1.0f; // No dampening by default
	// float mass; 
	float gravityScale = 1.0f;
	bool collision = false; // If the body collided this frame
	Color color = MAGENTA;

	//float radius = 0.0f;

	ColliderType colliderType = COLLIDER_TYPE_INVALID;
	Collider collider{};
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

				// Ensures both have a type
				assert(a.colliderType != COLLIDER_TYPE_INVALID && b.colliderType != COLLIDER_TYPE_INVALID);
				bool collision = false;

				// mtv = minimum translation vector
				Vector2 mtv = Vector2Zeros;
				
				if (a.colliderType == COLLIDER_TYPE_CIRCLE && b.colliderType == COLLIDER_TYPE_CIRCLE)
					collision = CircleCircle(
						a.position, a.collider.circle.radius, 
						b.position, b.collider.circle.radius,
						&mtv);

				else if (a.colliderType == COLLIDER_TYPE_CIRCLE && b.colliderType == COLLIDER_TYPE_HALF_SPACE)
					collision = CircleHalfSpace(
						a.position, a.collider.circle.radius,
						b.position, b.collider.halfSpace.normal,
						&mtv);

				else if (a.colliderType == COLLIDER_TYPE_HALF_SPACE && b.colliderType == COLLIDER_TYPE_CIRCLE)
					collision = CircleHalfSpace(
						b.position, b.collider.circle.radius,
						a.position, a.collider.halfSpace.normal,
						&mtv);

				a.collision |= collision;
				b.collision |= collision; // only if single true

				if (collision)
				{
					// move only circles
					if (a.colliderType == COLLIDER_TYPE_CIRCLE && b.colliderType == COLLIDER_TYPE_HALF_SPACE)
						a.position += mtv;
					if (a.colliderType == COLLIDER_TYPE_HALF_SPACE && b.colliderType == COLLIDER_TYPE_CIRCLE)
						b.position += mtv; 

					if (a.colliderType == COLLIDER_TYPE_CIRCLE && b.colliderType == COLLIDER_TYPE_CIRCLE)
					{
						// Move both circles apart equally
						a.position += mtv * 0.5f;
						b.position -= mtv * 0.5f;
					}
				}

			}
		}
	}

	bool CircleCircle(Vector2 pos1, float rad1, Vector2 pos2, float rad2, Vector2* mtv = nullptr)
	{
		// distance calculated by pythagorean
		float distance = Vector2Distance(pos1, pos2);

		// lab 5
		float radii_sum = rad1 + rad2;
		bool collision = distance <= radii_sum;

		if (collision && mtv != nullptr)
		{
			// direction from circle 2 to circle 1
			Vector2 direction = Vector2Subtract(pos1, pos2);
			direction = Vector2Normalize(direction);
			// overlap distance
			float overlap = radii_sum - distance;
			// minimum translation vector
			*mtv = direction * overlap;
		}

		// If distance between the two radius are less than or equal to distance calculated
		return collision;
	}

	bool CircleHalfSpace(Vector2 circlePos, float rad, Vector2 posHalfSpace, Vector2 normal, Vector2* mtv = nullptr)
	{
		// Vector from half-space position to circle position (ab = b - a)
		Vector2 toCircle = circlePos - posHalfSpace;

		// Determine distance from circle to half-space by scalar projecting AB onto normal
		float proj = Vector2DotProduct(toCircle, normal);

		// The circle is colliding if its center is closer to the plane than its radius
		bool collision = proj <= rad;

		if (collision && mtv != nullptr)
		{
			// depth = how far the circle is inside the half-space
			float depth = rad - proj;

			// MTV pushes the circle out along the plane's normal
			*mtv = normal * depth;
		}

		// Collision if projection less than or equal to radius
		return collision;
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

	//// Slider variables2
	//float rectangleWidth = 400;
	//GuiSliderBar(Rectangle{ InitialWidth - rectangleWidth - 60, 15, rectangleWidth, 20 }, "Launch Angle", 
	//	TextFormat("%.2f", launchAngle), &launchAngle, 0, 360.0f);
	//GuiSliderBar(Rectangle{ InitialWidth - rectangleWidth - 60, 45, rectangleWidth, 20 }, "Launch Speed", 
	//	TextFormat("%.2f", launchSpeed), &launchSpeed, 25, 200);
	//GuiSliderBar(Rectangle{ InitialWidth - rectangleWidth - 60, 75, rectangleWidth, 20 }, "Launch X", 
	//	TextFormat("%.2f", launchPosition.x), &launchPosition.x, 0, GetScreenWidth());
	//GuiSliderBar(Rectangle{ InitialWidth - rectangleWidth - 60, 105, rectangleWidth, 20 }, "Launch Y", 
	//	TextFormat("%.2f", launchPosition.y), &launchPosition.y, 0, GetScreenHeight());
	//GuiSliderBar(Rectangle{ InitialWidth - rectangleWidth - 60, 135, rectangleWidth, 20 }, "Gravity Y",
	//	TextFormat("%.2f", sim.gravity.y), & sim.gravity.y, -20, 20);
	//GuiSliderBar(Rectangle{ InitialWidth - rectangleWidth - 60, 165, rectangleWidth, 20 }, "Gravity x",
	//	TextFormat("%.2f", sim.gravity.x), &sim.gravity.x, -15, 15);

	//// Text for slider variables
	//DrawText(TextFormat("Angle: %.2f", launchAngle), 10, 15, 20, BLACK);
	//DrawText(TextFormat("Speed: %.2f", launchSpeed), 10, 45, 20, BLACK);
	//DrawText(TextFormat("Launch Position: (%.2f, %.2f)", launchPosition.x, launchPosition.y), 10, 75, 20, BLACK);
	//DrawText(TextFormat("Gravity: (%.2f, %.2f)", sim.gravity.x, sim.gravity.y), 10, 105, 20, BLACK);

	//// Circle representing the launch position
	//DrawCircleV(launchPosition, 10, ORANGE);

	//// Line representing the launch angle and speed
	//Vector2 velocityVector = Vector2Rotate(Vector2UnitX, DEG2RAD * launchAngle) * launchSpeed;
	//DrawLineV(launchPosition, launchPosition + velocityVector, RED);

	for (const PhysicsBody& o : sim.objects)
	{
		Color colour = o.collision ? RED : o.color;
		if (o.colliderType == COLLIDER_TYPE_CIRCLE)
			DrawCircleV(o.position, o.collider.circle.radius, colour);
		else if (o.colliderType == COLLIDER_TYPE_HALF_SPACE)
		{
			// Flip the normal to determine the direction of the half space
			Vector2 direction = { -o.collider.halfSpace.normal.y, o.collider.halfSpace.normal.x };
			Vector2 p0 = o.position + direction * 1000.0f;
			Vector2 p1 = o.position - direction * 1000.0f;

			// Draw the half-space line
			DrawLineEx(p0, p1, 5.0f,colour);

			// Line to show normal
			DrawLineEx(o.position, o.position + o.collider.halfSpace.normal * 50.0f, 5.0f, GOLD);
		}
	}

	Vector2 circlePos = sim.objects[2].position;
	Vector2 halfSpacePos = sim.objects[1].position;
	Vector2 normal = sim.objects[1].collider.halfSpace.normal;

	Vector2 toCircle = circlePos - halfSpacePos;
	float proj = Vector2DotProduct(toCircle, sim.objects[1].collider.halfSpace.normal);
	//DrawLineEx(halfSpacePos, halfSpacePos + toCircle, 5.0f, BLUE);
	//DrawCircleV(halfSpacePos + normal * proj, 20.0f, PINK);

	EndDrawing();
}

int main()
{
	PhysicsSimulation sim;
	PhysicsBody* entity = nullptr;

	//// Describe the static circle first
	//PhysicsBody& circle = sim.objects.back();
	//circle.position = { 400.0f, 400.0f };
	//circle.radius = 20.0f;
	//circle.gravityScale = 0.0f;

	//sim.objects.push_back({});
	//circle = sim.objects.back();

	// Stationary
	sim.objects.push_back({});
	entity = &sim.objects.back();
	entity->position = { 350.0f, 300.0f };
	entity->collider.circle.radius = 20.0f;
	entity->gravityScale = 0.0f;
	entity->colliderType = COLLIDER_TYPE_CIRCLE;
	entity->color = GREEN;

	// Gravity affected circle
	// Stationary
	sim.objects.push_back({});
	entity = &sim.objects.back();
	entity->position = { 350.0f, 200.0f };
	entity->collider.circle.radius = 20.0f;
	entity->gravityScale = 1.0f;
	entity->colliderType = COLLIDER_TYPE_CIRCLE;
	entity->color = GREEN;

	// Stationary half-space
	sim.objects.push_back({});
	entity = &sim.objects.back();
	entity->position = { 400.0f, 400.0f };
	entity->gravityScale = 0.0f;
	entity->colliderType = COLLIDER_TYPE_HALF_SPACE;
	entity->color = PURPLE;
	entity->collider.halfSpace.normal = Vector2Rotate(Vector2UnitX, -45.0f * DEG2RAD); // Pointing down 

	// Dynamic
	sim.objects.push_back({});
	entity = &sim.objects.back();
	entity->position = { 415.0f, 415.0f };
	entity->collider.circle.radius = 20.0f;
	entity->colliderType = COLLIDER_TYPE_CIRCLE;
	entity->color = GREEN;

	PhysicsBody circleStatic, circleDynmamic;
	circleStatic.position = { 400.0f, 400.0f };

	InitWindow(InitialWidth, InitialHeight, "Angry Birds");
	SetTargetFPS(sim.TARGET_FPS);

	while (!WindowShouldClose()) // Loops TARGET_FPS times per second
	{
		entity->position = GetMousePosition();

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