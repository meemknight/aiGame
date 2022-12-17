#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include <iostream>
#include <sstream>
#include <gl2d/gl2d.h>

gl2d::Renderer2D renderer;
gl2d::Font font;


const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int SPACESHIP_WIDTH = 50;
const int SPACESHIP_HEIGHT = 50;

const int ASTEROID_WIDTH = 50;
const int ASTEROID_HEIGHT = 50;

const int ASTEROID_SPAWN_RATE = 1000; // In milliseconds

class Asteroid
{
public:
	int x, y;
	int velocity;

	Asteroid(int x, int y, int velocity): x(x), y(y), velocity(velocity) {}

	void update()
	{
		// Update the asteroid's position
		y += velocity;
	}

	void draw(gl2d::Renderer2D *renderer)
	{
		// Draw the asteroid as a rectangle
		glm::vec4 rect = {x, y, ASTEROID_WIDTH, ASTEROID_HEIGHT};
		renderer->renderRectangle(rect, Colors_Gray);
	}
};

class Spaceship
{
public:
	int x, y;

	Spaceship(int x, int y): x(x), y(y) {}

	void update(int direction)
	{
		// Update the spaceship's position based on the input direction
		x += direction * 5;

		// Clamp the spaceship's position to the screen
		x = std::max(0, std::min(x, SCREEN_WIDTH - SPACESHIP_WIDTH));
	}

	void draw(gl2d::Renderer2D *renderer)
	{
		// Draw the spaceship as a rectangle
		glm::vec4 rect = {x, y, SPACESHIP_WIDTH, SPACESHIP_HEIGHT};
		renderer->renderRectangle(rect, Colors_Blue);
	}
};



class AvoidTheAsteroidsGame
{
public:
	Spaceship spaceship;
	std::vector<Asteroid> asteroids;
	bool gameover;

	AvoidTheAsteroidsGame(): spaceship(SPACESHIP_WIDTH, SCREEN_HEIGHT - SPACESHIP_HEIGHT), gameover(false)
	{
		// Generate the first asteroid
		asteroids.push_back(Asteroid(rand() % SCREEN_WIDTH, 0, rand() % 5 + 5));
	}

	void update()
	{
		// Update the spaceship's position based on the input direction
		if (platform::isKeyHeld(platform::Button::D))
		{
			spaceship.update(1);
		}
		else if (platform::isKeyHeld(platform::Button::A))
		{
			spaceship.update(-1);
		}

		// Update the asteroids' positions
		for (auto &asteroid : asteroids)
		{
			asteroid.update();
		}

		// Check if the spaceship has collided with an asteroid
		for (auto &asteroid : asteroids)
		{
			if (spaceship.x + SPACESHIP_WIDTH > asteroid.x && spaceship.x < asteroid.x + ASTEROID_WIDTH && spaceship.y + SPACESHIP_HEIGHT > asteroid.y && spaceship.y < asteroid.y + ASTEROID_HEIGHT)
			{
				gameover = true;
			}
		}

		// Remove off-screen asteroids
		asteroids.erase(std::remove_if(asteroids.begin(), asteroids.end(), [](const Asteroid &asteroid)
		{
			return asteroid.y > SCREEN_HEIGHT;
		}), asteroids.end());

		// Generate new asteroids
		static int last_spawn_time = 0;
		int current_time = clock();
		if (current_time - last_spawn_time > ASTEROID_SPAWN_RATE)
		{
			asteroids.push_back(Asteroid(rand() % SCREEN_WIDTH, 0, rand() % 5 + 5));
			last_spawn_time = current_time;
		}
	}

	void draw(gl2d::Renderer2D *renderer)
	{
		// Draw the spaceship
		spaceship.draw(renderer);

		// Draw the asteroids
		for (auto &asteroid : asteroids)
		{
			asteroid.draw(renderer);
		}

		// Draw the gameover message
		if (gameover)
		{
			renderer->renderText({SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2}, "Game Over", font, Colors_White);
		}
	}

	void reset()
	{
		// Reset the game to its initial state
		spaceship = Spaceship(SPACESHIP_WIDTH, SCREEN_HEIGHT - SPACESHIP_HEIGHT);
		asteroids.clear();
		asteroids.push_back(Asteroid(rand() % SCREEN_WIDTH, 0, rand() % 5 + 5));
		gameover = false;
	}
};

AvoidTheAsteroidsGame game;

bool initGame()
{
	gl2d::init();

	renderer.create();

	// Load the font
	font.createFromFile(RESOURCES_PATH "roboto_black.ttf");

	return true;
}



bool gameLogic(float deltaTime)
{
	int w = 0; int h = 0;
	w = platform::getWindowSizeX();
	h = platform::getWindowSizeY();

	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT);

	renderer.updateWindowMetrics(w, h);


	// Update the game
	game.update();

	// Clear the screen
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw the game
	game.draw(&renderer);

	// Swap the buffers
	renderer.flush();
	
	return 1;
}


void closeGame()
{

}


///flappybird

/*
#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include <iostream>
#include <sstream>
#include <gl2d/gl2d.h>

gl2d::Renderer2D renderer;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int PIPE_WIDTH = 50;
const int PIPE_GAP = 450;
const int PIPE_SPACING = 250;

const int BIRD_WIDTH = 50;
const int BIRD_HEIGHT = 50;

const int GRAVITY = 2;
const int JUMP_VELOCITY = -15;

class Pipe
{
public:
	int x, y;

	Pipe(int x, int y): x(x), y(y) {}

	void draw(gl2d::Renderer2D *renderer)
	{
		// Draw the upper pipe
		glm::vec4 rect = {x, 0, PIPE_WIDTH, y - PIPE_GAP / 2};
		renderer->renderRectangle(rect, Colors_Green);

		// Draw the lower pipe
		rect = {x, y + PIPE_GAP / 2, PIPE_WIDTH, SCREEN_HEIGHT - (y + PIPE_GAP / 2)};

		renderer->renderRectangle(rect, Colors_Green);
	}
};

class Bird
{
public:
	int x, y;
	int velocity;

	Bird(int x, int y): x(x), y(y), velocity(0) {}

	void update()
	{
		// Apply gravity to the bird's velocity
		velocity += GRAVITY;

		// Update the bird's position
		y += velocity;

		// Clamp the bird's position to the screen
		y = std::max(0, std::min(y, SCREEN_HEIGHT - BIRD_HEIGHT));
	}

	void jump()
	{
		// Set the bird's velocity to the jump velocity
		velocity = JUMP_VELOCITY;
	}

	void draw(gl2d::Renderer2D *renderer)
	{
		// Draw the bird as a rectangle
		glm::vec4 rect = {x, y, BIRD_WIDTH, BIRD_HEIGHT};
		renderer->renderRectangle(rect, Colors_Yellow);
	}
};

gl2d::Font font;

class FlappyBirdGame
{
public:
	std::vector<Pipe> pipes;
	Bird bird;
	int score;
	bool gameover;

	FlappyBirdGame(): bird(BIRD_WIDTH, SCREEN_HEIGHT / 2), score(0), gameover(false)
	{
		// Generate the first set of pipes
		pipes.push_back(Pipe(SCREEN_WIDTH, rand() % (SCREEN_HEIGHT - PIPE_GAP)));
	}

	void update()
	{
		// Update the bird's position
		bird.update();

		// Check if the bird has collided with a pipe
		for (auto &pipe : pipes)
		{
			if (bird.x + BIRD_WIDTH > pipe.x && bird.x < pipe.x + PIPE_WIDTH && (bird.y < pipe.y - PIPE_GAP / 2 || bird.y + BIRD_HEIGHT > pipe.y + PIPE_GAP / 2))
			{
				gameover = true;
			}
		}

		// Check if the bird has collided with the ground or ceiling
		if (bird.y == 0 || bird.y + BIRD_HEIGHT == SCREEN_HEIGHT)
		{
			gameover = true;
		}

		// Update the pipes
		for (auto &pipe : pipes)
		{
			pipe.x -= 2;
		}

		// Check if the leftmost pipe is off the screen
		if (pipes.front().x + PIPE_WIDTH < 0)
		{
			pipes.erase(pipes.begin());
			score++;
		}

		// Generate a new pipe if the rightmost pipe is past the spacing threshold
		if (pipes.back().x < SCREEN_WIDTH - PIPE_SPACING)
		{
			pipes.push_back(Pipe(SCREEN_WIDTH, rand() % (SCREEN_HEIGHT - PIPE_GAP)));
		}
	}

	void draw(gl2d::Renderer2D *renderer)
	{
		// Draw the pipes
		for (auto &pipe : pipes)
		{
			pipe.draw(renderer);
		}

		// Draw the bird
		bird.draw(renderer);

		// Draw the score
		std::stringstream ss;
		ss << "Score: " << score;
		renderer->renderText({renderer->windowW/2.f,renderer->windowH / 2.f}, ss.str().c_str(), font, Colors_White);
	}
};

FlappyBirdGame game;

bool initGame()
{
	gl2d::init();

	renderer.create();

	font.createFromFile(RESOURCES_PATH "roboto_black.ttf");

	return true;
}


void drawGame()
{
	int w = 0; int h = 0;
	w = platform::getWindowSizeX();
	h = platform::getWindowSizeY();

	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT);

	renderer.updateWindowMetrics(w, h);

	game.draw(&renderer);

	renderer.flush();
}


bool gameLogic(float deltaTime)
{
	if (!game.gameover)
	{
		game.update();
	}

	// Handle input
	if (platform::isKeyPressedOn(platform::Button::Space) && !game.gameover)
	{
		game.bird.jump();
	}
	if (platform::isKeyPressedOn(platform::Button::Escape))
	{
		exit(0);
	}

	drawGame();

	return 1;
}


void closeGame()
{

}


*/