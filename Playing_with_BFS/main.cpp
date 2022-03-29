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
std::vector<std::vector <cell>> mat;
std::vector<cell> matTemp;
//for adjacency list using a hash map of 
//integer key and vector as value
std::vector<int> temp;
std::map<int, std::vector<int>> adjm;


int pred[vertex] = { -1 };
int path[vertex] = { 0 };
int source = 0, des = 0;
sf::Vector2i desIndex = sf::Vector2i(maxX / 2, maxY / 2);
sf::Vector2i prevDesIndex;

sf::Image map;
sf::Sprite spriteMap;
sf::Texture spriteMapTex;

Clock cl;
Time dt;
float mx, my;

void init(void);
void init(int f);
void mouse_update(void);
void create_adj_mat(void);
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
						create_adj_mat();
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
				for (int i = 0; i < mat.size(); i++)
					for (int j = 0; j < mat[i].size(); j++)
						if (mat[i][j].vertex == path[k] && path[k] != source && path[k] != des)
							mat[i][j].box.setFillColor(Color::Yellow);
			state = 0;
		}




		/////////////////////////

		window.clear();

		for (int i = 0; i < mat.size(); i++)
			for (int j = 0; j < mat[i].size(); j++)
				if (mat[i][j].box.getFillColor() != sf::Color::Yellow)
					window.draw(mat[i][j].box);

		//window.draw(spriteMap);

		for (int i = 0; i < mat.size(); i++)
			for (int j = 0; j < mat[i].size(); j++)
				if (mat[i][j].box.getFillColor() == sf::Color::Yellow || mat[i][j].box.getFillColor() == sf::Color::Red ||
					mat[i][j].box.getFillColor() == sf::Color::Green )
					window.draw(mat[i][j].box);

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
		matTemp.clear();
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
				matTemp.push_back(c);
			}
			count++;
		}
		mat.push_back(matTemp);
	}

	for (int i = 0; i < vertex; i++) {
		path[i] = 0;
		pred[i] = -1;
		adjm.clear();
	}

}

void init(int f)
{
	for (int i = 0; i < mat.size(); i++) {
		for (int j = 0; j < mat[i].size(); j++) {
			mat[i][j].box.setFillColor(Color(92, 92, 92, 255));
		}
	}

	for (int i = 0; i < vertex; i++) {
		path[i] = 0;
		pred[i] = -1;
		adjm.clear();
	}
}

void create_adj_mat()
{
	adjm.clear();
	for (int i = 0; i < mat.size(); i++) {
		for (int j = 0; j < mat[i].size(); j++) {
			temp.clear();

			//old algorithm
			//this will no longer work as there is no concept of walls in new approach
				/*if (j + 1 < mat[i].size() && mat[i][j + 1].wall == 0)
					add_edge(mat[i][j].vertex, mat[i][j + 1].vertex);
				if (j - 1 > 0 && mat[i][j - 1].wall == 0)
					add_edge(mat[i][j].vertex, mat[i][j - 1].vertex);
				if (i + 1 < mat.size() && mat[i + 1][j].wall == 0)
					add_edge(mat[i][j].vertex, mat[i + 1][j].vertex);
				if (i - 1 > 0 && mat[i - 1][j].wall == 0)
					add_edge(mat[i][j].vertex, mat[i - 1][j].vertex);*/


			//new algorithm implementation
			//checking in low to high number order to get ordered or sorted vectors
			//up
			if (i - 1 >= 0) {
				int item = mat[i][j].vertex - maxX - 1;
				for (int k = 0; k < mat[i - 1].size(); k++) {
					if (mat[i - 1][k].vertex > item)
						break;
					if (mat[i - 1][k].vertex == item) {
						temp.push_back(item);
						break;
					}
				}
			}
			//left
			if (j - 1 >= 0) {
				if (mat[i][j - 1].vertex == mat[i][j].vertex - 1)
					temp.push_back(mat[i][j - 1].vertex);
			}
			//right
			if (j + 1 < mat[i].size()){
				if (mat[i][j + 1].vertex == mat[i][j].vertex + 1)
					temp.push_back(mat[i][j + 1].vertex);
			}
			//down
			if (i + 1 < mat.size()) {
				int item = mat[i][j].vertex + maxX + 1;
				for (int k = 0; k < mat[i + 1].size(); k++) {
					if (mat[i + 1][k].vertex > item)
						break;
					if (mat[i + 1][k].vertex == item) {
						temp.push_back(item);
						break;
					}
				}
			}

			adjm.insert(std::make_pair(mat[i][j].vertex, temp));
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

		//in this approach
		//accessing the first vector element with random access.
		//it's reference vector are accessed sequentially.
		/*for (int k = 0; k < adjm[x].size(); k++) {
			int vertexNum = adjm[x][k];
			if (visited[vertexNum] == false) {
				visited[vertexNum] = true;
				queue.push_back(vertexNum);
				pred[vertexNum] = x;
			}
		}*/

		//in this new approach adjm is a hash map of integer key and a vector of int as value
		//directly access the vector with the unique key (here x)
		for (auto k = adjm[x].begin(); k != adjm[x].end(); k++) {
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

	for (int i = 0; i < mat.size(); i++) {
		for (int j = 0; j < mat[i].size(); j++) {
			int hot = mx > mat[i][j].box.getPosition().x && mx < mat[i][j].box.getPosition().x + box_dimensions
				&& my > mat[i][j].box.getPosition().y && my < mat[i][j].box.getPosition().y + box_dimensions;
			
			if (hot && Keyboard::isKeyPressed(Keyboard::Key::S)) {
				source = mat[i][j].vertex;
				mat[i][j].box.setFillColor(Color::Red);
			}
			if (hot && Keyboard::isKeyPressed(Keyboard::Key::D)) {
				des = mat[i][j].vertex;
				mat[i][j].box.setFillColor(Color::Green);
			}	
		}
	}
}