#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>

constexpr int windowX = 800;
constexpr int windowY = 800;
constexpr int box_dimensions = 2;
constexpr int maxX = windowX / box_dimensions;
constexpr int maxY = windowY / box_dimensions;
constexpr int vertex = maxX * maxY;
std::string mapPath = "Maps/map3.png";
using namespace sf;


//for adjacency list using a hash map of 
//integer key and vector as value
std::vector<int> temp;
std::map<int, std::vector<int>> adjacencyMap;

int pred[vertex] = { -1 };
int path[vertex] = { 0 };
int source = 0, des = 0;
bool sourceSet = false, destSet = false;

sf::Image map;
sf::Sprite spriteMap;
sf::Texture spriteMapTex;

float mx, my;

void init(int state);
void mouse_update(void);
void create_adjacencyMap(void);
void bfs(void);
int get_path(void);
void colorImgPixels(float x, float y, int size, sf::Color color);
bool compareColorValues(sf::Color color);

int main()
{
	RenderWindow window(VideoMode(windowX, windowY), "Optimized Maps", Style::Close);
	window.setVerticalSyncEnabled(true);
	//float t = 0;
	int state = 0;
	init(state);
	create_adjacencyMap();


	while (window.isOpen())
	{
		Event e;
		while (window.pollEvent(e))
		{
			switch (e.type)
			{
			case Event::Closed:
				window.close();
			default:
				break;
			}
		}

		///////////////////////

		mx = Mouse::getPosition(window).x;
		my = Mouse::getPosition(window).y;

		//states updating
		if (state == 0) {
			if (!sourceSet || !destSet)
				mouse_update();
			if (sourceSet && destSet && Keyboard::isKeyPressed(Keyboard::Enter))
				state = 1;
		}
		
		if (Keyboard::isKeyPressed(Keyboard::R)) {
			init(state);
			state = 0;
		}

		if (state == 1) {
			for (int i = 0; i < vertex; i++) {
				path[i] = 0;
				pred[i] = -1;
			}
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
				colorImgPixels(x, y, 2, sf::Color::Red);
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

	sourceSet = false;
	destSet = false;
}

void create_adjacencyMap()
{
	adjacencyMap.clear();

	for (int i = 0; i < maxX; i++) {	//full range using maxX and maxY as image size is to be considered
		for (int j = 0; j < maxY; j++) {
			temp.clear();
			int currItem = ((i*maxX) + j);

			int halfSize = box_dimensions / 2;
			unsigned int boxPixlX = j * box_dimensions + halfSize;	//reversed here
			unsigned int boxPixlY = i * box_dimensions + halfSize;

			//if (map.getPixel(boxPixlX, boxPixlY) == sf::Color::White) {
			if (compareColorValues(map.getPixel(boxPixlX, boxPixlY))) {
				
				//then sequentially up -> left -> right -> down
				//up
				int upPixlX = boxPixlX;
				int upPixlY = boxPixlY - box_dimensions;
				
				if (upPixlY > 0) {
					//if (map.getPixel(upPixlX, upPixlY) == sf::Color::White) {
					if(compareColorValues(map.getPixel(upPixlX, upPixlY))){
						//push this pixels corresponding vertexNum into temp
						int item = currItem - maxX;
						temp.push_back(item);
					}
					else
						temp.push_back(0);
				}
					
				//left
				int leftPixX = boxPixlX - box_dimensions;
				int leftPixY = boxPixlY;
				if (leftPixX > 0) {
					//if (map.getPixel(leftPixX, leftPixY) == sf::Color::White) {
					if (compareColorValues(map.getPixel(leftPixX, leftPixY))){
						int item = currItem - 1;
						temp.push_back(item);
					}
					else
						temp.push_back(0);
				}

				//right
				int rightPixX = boxPixlX + box_dimensions;
				int rightPixY = boxPixlY;
				if (rightPixX < map.getSize().x) {
					//if (map.getPixel(rightPixX, rightPixY) == sf::Color::White) {
					if (compareColorValues(map.getPixel(rightPixX, rightPixY))){
						int item = currItem + 1;
						temp.push_back(item);
					}
					else
						temp.push_back(0);
				}

				//down
				int downPixX = boxPixlX;
				int downPixY = boxPixlY + box_dimensions;
				if (downPixY < map.getSize().y) {
					//if (map.getPixel(downPixX, downPixY) == sf::Color::White) {
					if (compareColorValues(map.getPixel(downPixX, downPixY))){
						int item = currItem + maxX;
						temp.push_back(item);
					}
					else
						temp.push_back(0);
				}
				
				adjacencyMap.insert(std::make_pair(currItem, temp));
			}

		}
	}
}

void bfs()
{
	std::list<int> queue;
	bool visited[vertex] = { false };

	visited[source] = true;
	queue.push_back(source);

	while (queue.empty() != 1) {
		int x = queue.front();

		if (x == des) {
			pred[source] = -1;
			return;
		}
		queue.pop_front();

		//in this new approach adjacencyMap is a hash map of integer key and a vector of int as value
		//directly access the vector with the unique key (here x)
		for (auto k = adjacencyMap[x].begin(); k != adjacencyMap[x].end(); k++) {
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
		path[k++] = i;
		i = pred[i];
	}
	path[k++] = i;
	return k;
}

void mouse_update()
{
	for (int i = 0; i < maxX; i++) {	//full range using maxX and maxY as image size is to be considered
		for (int j = 0; j < maxY; j++) {

			int boxJ = mx / box_dimensions;	//reveresed for a reason. looks kind of odd though :-|
			int boxI = my / box_dimensions;
			int hotCell = (boxI*maxX) + boxJ;

			if (Keyboard::isKeyPressed(Keyboard::Key::S) && sourceSet == false) {
				source = hotCell;
				sourceSet = true;
				colorImgPixels(mx, my, 5, sf::Color::Blue);
				spriteMapTex.loadFromImage(map);
				spriteMap.setTexture(spriteMapTex);
				return;
			}
			if (Keyboard::isKeyPressed(Keyboard::Key::D) && destSet == false) {
				des = hotCell;
				destSet = true;
				colorImgPixels(mx, my, 5, sf::Color::Green);
				spriteMapTex.loadFromImage(map);
				spriteMap.setTexture(spriteMapTex);
				return;
			}
		}
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