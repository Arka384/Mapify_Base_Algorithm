#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>
#include <vector>
#include <map>

constexpr int windowX = 800;	//yeah don't foreget to change them according to image size
constexpr int windowY = 800;	//otherwise you will get an exception... duhhh..
constexpr int box_dimensions = 2;
constexpr int maxX = (int)(windowX / box_dimensions);
constexpr int maxY = (int)(windowY / box_dimensions);
constexpr int vertex = maxX * maxY;
std::string mapPath = "Maps/map6.png";

//int pred[vertex] = { -1 };
std::map<int, int> pred;	//pred is a hash map now.
//int path[vertex] = { 0 };
std::vector<int> path;	//path is a vector now.
int source = 0, des = 0;
bool sourceSet = false, destSet = false;

sf::Image map;
sf::Sprite spriteMap;
sf::Texture spriteMapTex;

float mx, my;

void init(int state);
void mouse_update(void);
void bfs(void);
int get_path(void);
void colorImgPixels(float x, float y, int size, sf::Color color);
bool compareColorValues(sf::Color color);

int main()
{
	sf::RenderWindow window(sf::VideoMode(windowX, windowY), "Optimized Maps", sf::Style::Close);
	window.setVerticalSyncEnabled(true);
	int state = 0;
	init(state);


	while (window.isOpen())
	{
		sf::Event e;
		while (window.pollEvent(e))
		{
			switch (e.type)
			{
			case sf::Event::Closed:
				window.close();
			default:
				break;
			}
		}

		///////////////////////

		mx = sf::Mouse::getPosition(window).x;
		my = sf::Mouse::getPosition(window).y;

		//states updating
		if (state == 0) {
			if (!sourceSet || !destSet)
				mouse_update();
			if (sourceSet && destSet && sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
				state = 1;
		}
		
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
			init(state);
			state = 0;
		}

		if (state == 1) {
			bfs();
			state = 2;
		}

		if (state == 2) {
			int length = get_path();

			for (int k = 0; k < length; k++) {
				int currCell = path[k];
				int i = currCell / maxX;
				int j = currCell - (i*maxX);
				float x = j * box_dimensions + (box_dimensions / 2);
				float y = i * box_dimensions + (box_dimensions / 2);
				colorImgPixels(x, y, 1, sf::Color::Red);
			}
			spriteMapTex.loadFromImage(map);
			spriteMap.setTexture(spriteMapTex);

			state = 0;
		}


		/////////////////////////

		window.clear();

		window.draw(spriteMap);

		window.display();
	}
}


void init(int state)
{
	//loading image or maybe reloading based upon when it's being called
	map.loadFromFile(mapPath);
	spriteMapTex.loadFromImage(map);
	spriteMap.setTexture(spriteMapTex);

	pred.clear();
	path.clear();
	sourceSet = false;
	destSet = false;
}


void bfs()
{
	std::list<int> queue;
	std::map<int, bool>visited;

	visited[source] = true;
	queue.push_back(source);

	while (queue.empty() != 1) {
		int x = queue.front();

		if (x == des) {
			pred[source] = -1;
			return;
		}
		queue.pop_front();

		//new implementation without hash map
		//dynamically determice the adjacents of x
		//finding adjacents of x 
		int currItem = x;
		int i = currItem / maxX;
		int j = currItem - (i*maxX);
		int halfSize = box_dimensions / 2;
		unsigned int boxPixlX = j * box_dimensions + halfSize;	//reversed here
		unsigned int boxPixlY = i * box_dimensions + halfSize;
		std::vector<int> queueTemp;
		
		//up
		int upPixlX = boxPixlX;
		int upPixlY = boxPixlY - box_dimensions;
		if (upPixlY > 0) {
			if (compareColorValues(map.getPixel(upPixlX, upPixlY))) {
				int item = currItem - maxX;
				queueTemp.push_back(item);
			}
		}
		//left
		int leftPixX = boxPixlX - box_dimensions;
		int leftPixY = boxPixlY;
		if (leftPixX > 0) {
			if (compareColorValues(map.getPixel(leftPixX, leftPixY))) {
				int item = currItem - 1;
				queueTemp.push_back(item);
			}
		}
		//right
		int rightPixX = boxPixlX + box_dimensions;
		int rightPixY = boxPixlY;
		if (rightPixX < map.getSize().x) {
			if (compareColorValues(map.getPixel(rightPixX, rightPixY))) {
				int item = currItem + 1;
				queueTemp.push_back(item);
			}
		}
		//down
		int downPixX = boxPixlX;
		int downPixY = boxPixlY + box_dimensions;
		if (downPixY < map.getSize().y) {
			if (compareColorValues(map.getPixel(downPixX, downPixY))) {
				int item = currItem + maxX;
				queueTemp.push_back(item);
			}
		}

		//now all the adjacents of x are in queueTemp
		for (auto k = queueTemp.begin(); k != queueTemp.end(); k++) {
			int vertexNum = *k;
			if (visited[vertexNum] == false) {
				visited[vertexNum] = true;
				queue.push_back(vertexNum);
				pred[vertexNum] = x;
			}
		}
	}


}


int get_path()
{
	int k = 0, i = des;
	while (pred[i] != -1) {
		//path[k++] = i;
		path.push_back(i);
		i = pred[i];
	}
	//path[k++] = i;
	path.push_back(i);
	return path.size();
}

void mouse_update()
{
	int boxJ = mx / box_dimensions;	//reveresed for a reason. looks kind of odd though :-|
	int boxI = my / box_dimensions;
	int hotCell = (boxI*maxX) + boxJ;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && sourceSet == false) {
		if (!compareColorValues(map.getPixel(mx, my)))
			return;
		source = hotCell;
		sourceSet = true;
		//colorImgPixels(mx, my, 5, sf::Color::Blue);
		spriteMapTex.loadFromImage(map);
		spriteMap.setTexture(spriteMapTex);
		return;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) && destSet == false) {
		if (!compareColorValues(map.getPixel(mx, my)))
			return;
		des = hotCell;
		destSet = true;
		//colorImgPixels(mx, my, 5, sf::Color::Green);
		spriteMapTex.loadFromImage(map);
		spriteMap.setTexture(spriteMapTex);
		return;
	}
}

void colorImgPixels(float x, float y, int size, sf::Color color)
{
	int xlow = x - size;
	int xhigh = x + size;
	int ylow = y - size;
	int yhigh = y + size;
	map.setPixel(x, y, color);//curr

	for (int i = xlow; i <= xhigh; i++) {
		for (int j = ylow; j <= yhigh; j++) {
			map.setPixel(i, j, color);
		}
	}
}

bool compareColorValues(sf::Color color) {
	int r = color.r;
	int g = color.g;
	int b = color.b;

	if (r >= 240 && g >= 240 && b >= 240)
		return true;
	
	return false;
}