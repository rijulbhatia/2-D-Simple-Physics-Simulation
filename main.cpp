#include <iostream>
#include <SFML/Graphics.hpp>
#include <random>

// 2-D n body simulation

struct Body {
	float pos_x;
	float pos_y;
	float vel_x;
	float vel_y;
	float acc_x;
	float acc_y;
	float mass;
	float radius;
};

// physics constants
const float G = 100.0f;
const float dt = 0.016f;
const float dampening = 0.95f;
const float minDistance = 5.0f; //???

// camera constants
const float cameraSpeed = 300.0f;
const float zoomSpeed = 0.1f;

// boundary
const float boundaryWidth = 4000.0f;
const float boundaryheight = 3000.f;

// gloabal variables
std::vector<Body*> bodies;
sf::RenderWindow window;
sf::View camera;
sf::RectangleShape boundary;

void initializer(float posX, float posY, float rad, float m, float velX, float velY) {
	Body* nbody = new Body;
	nbody->pos_x = posX;
	nbody->pos_y = posY;
	nbody->radius = rad;
	nbody->mass = m;
	nbody->vel_x = velX;
	nbody->vel_y = velY;
	nbody->acc_x = 0.0f;
	nbody->acc_y = 9.0f;
	bodies.push_back(nbody);
}

void generateBodies(int n) {
	// gen 6 random numbers for pos(x,y) vel(x,y), mass, radius
	while (n) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> mass(10, 100);
		std::uniform_real_distribution<float> posX(100, boundaryWidth - 100);
		std::uniform_real_distribution<float> posY(100, boundaryheight - 100);
		std::uniform_real_distribution<float> velX(-100, 100);
		std::uniform_real_distribution<float> velY(-100, 100);
		std::uniform_real_distribution<float> halfDia(10, 50);
		float randMass = mass(gen);
		float randPosX = posX(gen);
		float randPosY = posY(gen);
		float randRad = halfDia(gen);
		float randVelX = velX(gen);
		float randVelY = velY(gen);
		bool flag = true;
		for (int i = 0; i < bodies.size(); i++) {
			float x = bodies[i]->pos_x;
			float y = bodies[i]->pos_y;
			float r = bodies[i]->radius;
			if (abs(x - randPosX) <= r + randRad || abs(y - randPosY) <= r + randRad) {
				flag = false;
				break;
			}
		}
		if (flag) {
			n--;
			initializer(randPosX, randPosY, randRad, randMass, randVelX, randVelY);
		}
	}
}

// lets try without gravity

void handlecollision() {
	// simpler collisions
	//std::cout << "handlecollision(): ";
	for (int i = 0; i < bodies.size(); i++) {
		for (int j = i + 1; j < bodies.size(); j++) {
			float dist = sqrt((bodies[i]->pos_x - bodies[j]->pos_x) * (bodies[i]->pos_x - bodies[j]->pos_x) + (bodies[i]->pos_y - bodies[j]->pos_y) * (bodies[i]->pos_y - bodies[j]->pos_y));
			//std::cout << "dist: " << dist << " ";
			if (dist <= bodies[i]->radius + bodies[j]->radius) {
				// collision detected
				float m1 = bodies[i]->mass;
				float v1x = bodies[i]->vel_x;
				float v1y = bodies[i]->vel_y;
				float m2 = bodies[j]->mass;
				float v2x = bodies[j]->vel_x;
				float v2y = bodies[j]->vel_y;


				float VcomX = (m1 * v1x + m2 * v2x) / (m1 + m2);
				float VcomY = (m1 * v1y + m2 * v2y) / (m1 + m2);
				bodies[i]->vel_x = VcomX - (m2 * dampening * (v1x - v2x)) / (m1 + m2);
				bodies[i]->vel_y = VcomY - (m2 * dampening * (v2x - v1x)) / (m1 + m2);
				bodies[j]->vel_x = VcomX - (m1 * dampening * (v1y - v2y)) / (m1 + m2);
				bodies[j]->vel_y = VcomY - (m1 * dampening * (v2y - v1y)) / (m1 + m2);
			}
		}
	}
}


void handleBoundary() {
	for (int i = 0; i < bodies.size(); i++) {
		float px = bodies[i]->pos_x;
		float py = bodies[i]->pos_y;
		//left boundary collision
		if (px - bodies[i]->radius <= minDistance) {
			bodies[i]->vel_x = -1 * bodies[i]->vel_x;
		}
		// right boundary collision
		if (px + bodies[i]->radius >= boundaryWidth - minDistance) {
			bodies[i]->vel_x = -1 * bodies[i]->vel_x;
		}
		// upper boundary
		if (py - bodies[i]->radius <= minDistance) {
			bodies[i]->vel_y = -1 * bodies[i]->vel_y;
		}
		// right boundary collision
		if (py + bodies[i]->radius >= boundaryheight - minDistance) {
			bodies[i]->vel_y = -1 * bodies[i]->vel_y;
		}
	}
}


void updatePhysics() {
	// for every mass calculate next psotiton and velocity
	for (int i = 0; i < bodies.size(); i++) {
		bodies[i]->vel_x += bodies[i]->acc_x * dt;
		bodies[i]->vel_y += bodies[i]->acc_y * dt;
		bodies[i]->pos_x += bodies[i]->vel_x * dt;
		bodies[i]->pos_y += bodies[i]->vel_y * dt;
	}
	// handle collision and boundary collisions
	handlecollision();
	handleBoundary();
}

void handleinp() {
	sf::Vector2f movement(0, 0);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
		movement.y -= cameraSpeed * dt;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
		movement.y += cameraSpeed * dt;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		movement.x -= cameraSpeed * dt;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		movement.x += cameraSpeed * dt;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
		camera.zoom(1.0f + zoomSpeed);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) {
		camera.zoom(1.0f - zoomSpeed);
	}
	camera.move(movement);
}

void render() {
	window.clear(sf::Color::Black);
	window.setView(camera);

	window.draw(boundary);

	for (int i = 0; i < bodies.size(); i++) {
		sf::CircleShape shape;
		shape.setRadius(bodies[i]->radius);
		shape.setFillColor(sf::Color::White);
		shape.setOrigin({ bodies[i]->radius, bodies[i]->radius });
		shape.setPosition({ bodies[i]->pos_x, bodies[i]->pos_y });
		window.draw(shape);
	}
	window.display();
}

void initializeSim() {
	window.create(sf::VideoMode({ 1920,1080 }), "N-body Sim");
	window.setFramerateLimit(60);

	camera.setSize({ 1920, 1080 });
	camera.setCenter({ boundaryWidth / 2, boundaryheight / 2 });

	boundary.setSize(sf::Vector2f(boundaryWidth, boundaryheight));
	boundary.setFillColor(sf::Color::Transparent);
	boundary.setOutlineColor(sf::Color::White);
	boundary.setOutlineThickness(5.0f);
	boundary.setPosition({ 0, 0 });

	generateBodies(32);
}

void reset() {
	generateBodies(32);
	camera.setCenter({ boundaryWidth / 2, boundaryheight / 2 });
	camera.setSize({ 1920,1080 });
}


int main() {
	try {
		initializeSim();

		while (window.isOpen()) {
			while (const std::optional event = window.pollEvent()) {
				if (event->is<sf::Event::Closed>()) {
					window.close();
				}
				else if(const auto* keypressed = event->getIf<sf::Event::KeyPressed>()){
					if (keypressed->scancode == sf::Keyboard::Scancode::Escape) {
						window.close();
					}
					if (keypressed->scancode == sf::Keyboard::Scancode::R) {
						reset();
					}
				}
			}
			handleinp();
			updatePhysics();
			render();
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return -1;
	}
	return 0;
}