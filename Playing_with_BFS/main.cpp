#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>

constexpr int windowX = 800;
constexpr int windowY = 800;
constexpr int box_dimensions = 4;
constexpr int maxX = windowX / box_dimensions;
constexpr int maxY = windowY / box_dimensions;
constexpr int vertex = maxX * maxY;
using namespace sf;

typedef struct cell {
	RectangleShape box;
	int vertex;
}cell;

//for grid using a 2d vector of cell type
std::vector<std::vector <cell>> gridVector;
std::vector<cell> gridTemp;
//for adjacency list using a hash map of 
//integer key and vector as value
std::vector<int> temp;
std::map<int, std::vector<int>> adjacencyMap;


int pred[vertex] = { -1 };
int path[vertex] = { 0 };
int source = 0, des = 0;

sf::Image map;
sf::Sprite spriteMap;
sf::Texture spriteMapTex;

Clock cl;
Time dt;
float mx, my;

void init(void);
void init(int f);
void mouse_update(void);
void create_adjacencyMap(void);
void bfs(void);
int get_path(void);

int main()
{
	RenderWindow window(VideoMode(windowX, windowY), "Optimized Maps", Style::Close);
	window.setVerticalSyncEnabled(true);
	float t = 0;
	int state = 0;
	init();

	while (window.isOpen())
	{
		Event e;
		while (window.pollEvent(e))
		{
			switch (e.type)
			{
			case Event::Closed:
				window.close();
			case Event::KeyPressed:
				if (Keyboard::isKeyPressed(Keyboard::Enter)) {
					if (state == 0) {
						create_adjacencyMap();
						state = 1;
					}
				}
				else if (Keyboard::isKeyPressed(Keyboard::R)) {
					init(1);
					state = 0;
				}
				break;

			default:
				break;
			}
		}

		///////////////////////

		mx = Mouse::getPosition(window).x;
		my = Mouse::getPosition(window).y;

		//states updating
		if (state == 0)
			mouse_update();
		
		
		if (state == 1) {
			bfs();
			state = 2;
		}

		if (state == 2) {
			int length = get_path();
			for (int k = 0; k < length; k++)
				for (int i = 0; i < gridVector.size(); i++)
					for (int j = 0; j < gridVector[i].size(); j++)
						if (gridVector[i][j].vertex == path[k] && path[k] != source && path[k] != des)
							gridVector[i][j].box.setFillColor(Color::Yellow);
			state = 0;
		}




		/////////////////////////

		window.clear();

		for (int i = 0; i < gridVector.size(); i++)
			for (int j = 0; j < gridVector[i].size(); j++)
				if (gridVector[i][j].box.getFillColor() != sf::Color::Yellow)
					window.draw(gridVector[i][j].box);

		//window.draw(spriteMap);

		for (int i = 0; i < gridVector.size(); i++)
			for (int j = 0; j < gridVector[i].size(); j++)
				if (gridVector[i][j].box.getFillColor() == sf::Color::Yellow || gridVector[i][j].box.getFillColor() == sf::Color::Red ||
					gridVector[i][j].box.getFillColor() == sf::Color::Green )
					window.draw(gridVector[i][j].box);

		window.display();
	}
}


void init(void)
{
	
	//loading image
	map.loadFromFile("Maps/map3.png");
	spriteMapTex.loadFromFile("Maps/map3.png");
	spriteMap.setTexture(spriteMapTex);

	int count = 0;	//used for vertex numbering 
	for (int i = 0; i < maxX; i++) {	//full range using maxX and maxY as image size is to be considered
		gridTemp.clear();
		for (int j = 0; j < maxY; j++) {
			cell c;
			c.box.setFillColor(Color(92, 92, 92, 255));
			
			//segmenting the image as per the box size and sensing the color to modify the maze
			int halfSize = box_dimensions / 2;
			unsigned int boxPixlX = j * box_dimensions + halfSize;	//reversed here
			unsigned int boxPixlY = i * box_dimensions + halfSize;

			if (map.getPixel(boxPixlX, boxPixlY) == sf::Color::White) {
				c.box.setFillColor(sf::Color(92, 92, 92, 255));
				c.box.setSize(Vector2f(box_dimensions, box_dimensions));
				/*c.box.setSize(Vector2f(box_dimensions - 1, box_dimensions - 1));
				c.box.setOutlineThickness(1.f);
				c.box.setOutlineColor(sf::Color::Red);*/
				c.box.setPosition(j * box_dimensions, i * box_dimensions);
				c.vertex = count;
				gridTemp.push_back(c);
			}
			count++;
		}
		gridVector.push_back(gridTemp);
	}

	for (int i = 0; i < vertex; i++) {
		path[i] = 0;
		pred[i] = -1;
		adjacencyMap.clear();
	}

}

void init(int f)
{
	for (int i = 0; i < gridVector.size(); i++) {
		for (int j = 0; j < gridVector[i].size(); j++) {
			gridVector[i][j].box.setFillColor(Color(92, 92, 92, 255));
		}
	}

	for (int i = 0; i < vertex; i++) {
		path[i] = 0;
		pred[i] = -1;
		adjacencyMap.clear();
	}
}

void create_adjacencyMap()
{
	adjacencyMap.clear();
	for (int i = 0; i < gridVector.size(); i++) {
		for (int j = 0; j < gridVector[i].size(); j++) {
			temp.clear();

			//new implementation
			//checking in low to high number order to get ordered or sorted vectors
			//up
			if (i - 1 >= 0) {
				int item = gridVector[i][j].vertex - maxX - 1;
				for (int k = 0; k < gridVector[i - 1].size(); k++) {
					if (gridVector[i - 1][k].vertex > item)
						break;
					if (gridVector[i - 1][k].vertex == item) {
						temp.push_back(item);
						break;
					}
				}
			}
			//left
			if (j - 1 >= 0) {
				if (gridVector[i][j - 1].vertex == gridVector[i][j].vertex - 1)
					temp.push_back(gridVector[i][j - 1].vertex);
			}
			//right
			if (j + 1 < gridVector[i].size()){
				if (gridVector[i][j + 1].vertex == gridVector[i][j].vertex + 1)
					temp.push_back(gridVector[i][j + 1].vertex);
			}
			//down
			if (i + 1 < gridVector.size()) {
				int item = gridVector[i][j].vertex + maxX + 1;
				for (int k = 0; k < gridVector[i + 1].size(); k++) {
					if (gridVector[i + 1][k].vertex > item)
						break;
					if (gridVector[i + 1][k].vertex == item) {
						temp.push_back(item);
						break;
					}
				}
			}

			adjacencyMap.insert(std::make_pair(gridVector[i][j].vertex, temp));
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
	/*
		mouse related updates
		like setting walls or blocks, clearing walls, setting destination and source etc;
		nothing fancy with it...
	*/

	for (int i = 0; i < gridVector.size(); i++) {
		for (int j = 0; j < gridVector[i].size(); j++) {
			int hot = mx > gridVector[i][j].box.getPosition().x && mx < gridVector[i][j].box.getPosition().x + box_dimensions
				&& my > gridVector[i][j].box.getPosition().y && my < gridVector[i][j].box.getPosition().y + box_dimensions;
			
			if (hot && Keyboard::isKeyPressed(Keyboard::Key::S)) {
				source = gridVector[i][j].vertex;
				gridVector[i][j].box.setFillColor(Color::Red);
			}
			if (hot && Keyboard::isKeyPressed(Keyboard::Key::D)) {
				des = gridVector[i][j].vertex;
				gridVector[i][j].box.setFillColor(Color::Green);
			}	
		}
	}
}