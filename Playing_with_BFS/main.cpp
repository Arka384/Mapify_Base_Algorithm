#include <SFML/Graphics.hpp>
#include <iostream>
#include <list>
#include <vector>
#include <map>

constexpr int windowX = 1080;	//yeah don't foreget to change them according to image size
constexpr int windowY = 1080;	//otherwise you will get an exception... duhhh..
constexpr int box_dimensions = 2;
constexpr int maxX = (int)(windowX / box_dimensions);
constexpr int maxY = (int)(windowY / box_dimensions);
constexpr int vertex = maxX * maxY;
std::string mapPath = "Maps/map8.png";

//std::map<int, int> pred;	//pred is a hash map now.
std::map<int, int> predFromSource, predFromDest;
std::list<int> sourceQueue, destQueue;
std::map<int, bool> sourceVisited, destVisisted;
int source = 0, des = 0;
bool sourceSet = false, destSet = false;
bool bfsExecuted = false;

sf::Image map;
sf::Sprite spriteMap;
sf::Texture spriteMapTex;

float mx, my;

void init(int state);
void mouse_update(void);
int sourceBfs(void);
int destBfs(void);
void bfs(void);
std::vector<int> getPath(int sb, int db);
std::vector<int> getN8Adjacents(int currItem, int boxPixlX, int boxPixlY);
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
			if (!bfsExecuted) {
				bfs();
				bfsExecuted = true;
			}
				
			state = 2;
		}

		if (state == 2) {
			


			
			/*spriteMapTex.loadFromImage(map);
			spriteMap.setTexture(spriteMapTex);*/



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

	sourceQueue.clear();
	destQueue.clear();
	sourceVisited.clear();
	destVisisted.clear();
	predFromSource.clear();
	predFromDest.clear();
	bfsExecuted = false;
	sourceSet = false;
	destSet = false;
}

void bfs() {
	sourceVisited[source] = true;
	sourceQueue.push_back(source);

	destVisisted[des] = true;
	destQueue.push_back(des);
	int sb = -1, db = -1;

	while (sb == -1 && db == -1) {
		sb = sourceBfs();
		db = destBfs();
	}

	std::vector<int> path = getPath(sb, db);
	for (auto i = path.begin(); i != path.end(); i++)
	{
		int currentCell = *i;
		int iIndex = currentCell / maxX;
		int jIndex = currentCell - (iIndex * maxX);
		int x = jIndex * box_dimensions + (box_dimensions / 2);
		int y = iIndex * box_dimensions + (box_dimensions / 2);
		colorImgPixels(x, y, 1, sf::Color::Red);
	}

	spriteMapTex.loadFromImage(map);
	spriteMap.setTexture(spriteMapTex);
	
	/*std::cout << predFromSource[sb] << "\n";
	std::cout << predFromSource[db] << "\n";
	std::cout << predFromDest[sb] << "\n";
	std::cout << predFromDest[db] << "\n";*/

	/*std::cout << "first\n";
	for (auto i = predFromSource.begin(); i != predFromSource.end(); i++)
		std::cout << i->first << " " << i->second << "\n";
	std::cout << "second\n";
	for (auto i = predFromDest.begin(); i != predFromDest.end(); i++)
		std::cout << i->first << " " << i->second << "\n";*/
}

int sourceBfs()
{
//	while (sourceQueue.empty() != 1) {
	if (sourceQueue.empty() == 1)
		return 0;

		int x = sourceQueue.front();

		/*if (x == des) {
			predFromSource[source] = -1;
			return 1;
		}*/
		sourceQueue.pop_front();

		int currItem = x;
		int i = currItem / maxX;
		int j = currItem - (i*maxX);
		int halfSize = box_dimensions / 2;
		unsigned int boxPixlX = j * box_dimensions + halfSize;	//reversed here
		unsigned int boxPixlY = i * box_dimensions + halfSize;
		std::vector<int> queueTemp;
		queueTemp = getN8Adjacents(currItem, boxPixlX, boxPixlY);

		//now all the adjacents of x are in queueTemp
		for (auto k = queueTemp.begin(); k != queueTemp.end(); k++) {
			int vertexNum = *k;
			if (sourceVisited[vertexNum] == false) {
				sourceVisited[vertexNum] = true;
				sourceQueue.push_back(vertexNum);
				predFromSource[vertexNum] = x;
				if (predFromDest.find(vertexNum) != predFromDest.end())
					return vertexNum;
			}
		}
//	}
		return -1;
}

int destBfs() {

//	while (destQueue.empty() != 1) {
	if (destQueue.empty() == 1)
		return 0;

		int x = destQueue.front();

		/*if (x == source) {
			predFromDest[des] = -1;
			return 1;
		}*/
		destQueue.pop_front();

		int currItem = x;
		int i = currItem / maxX;
		int j = currItem - (i*maxX);
		int halfSize = box_dimensions / 2;
		unsigned int boxPixlX = j * box_dimensions + halfSize;	//reversed here
		unsigned int boxPixlY = i * box_dimensions + halfSize;
		std::vector<int> queueTemp;
		queueTemp = getN8Adjacents(currItem, boxPixlX, boxPixlY);

		//now all the adjacents of x are in queueTemp
		for (auto k = queueTemp.begin(); k != queueTemp.end(); k++) {
			int vertexNum = *k;
			if (destVisisted[vertexNum] == false) {
				destVisisted[vertexNum] = true;
				destQueue.push_back(vertexNum);
				predFromDest[vertexNum] = x;
				if (predFromSource.find(vertexNum) != predFromSource.end())
					return vertexNum;
			}
		}
//	}
		return -1;
}

std::vector<int> getPath(int sb, int db) {
	std::vector<int> path;

	int index = (sb > db) ? sb : db;
	while (predFromSource.find(index) != predFromSource.end()) {
		int current = predFromSource.find(index)->second;
		path.push_back(current);
		index = current;
	}
	
	index = (sb > db) ? sb : db;
	while (predFromDest.find(index) != predFromDest.end()) {
		int current = predFromDest.find(index)->second;
		path.push_back(current);
		index = current;
	}

	return path;
}


std::vector<int> getN8Adjacents(int currItem, int boxPixlX, int boxPixlY)
{
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

	//top left
	int topleftPixX = boxPixlX - box_dimensions;
	int topleftPixY = boxPixlY - box_dimensions;
	if (topleftPixX > 0 && topleftPixY > 0) {
		if (compareColorValues(map.getPixel(topleftPixX, topleftPixY))) {
			int item = currItem - maxX - 1;
			queueTemp.push_back(item);
		}
	}
	//top right
	int toprightPixX = boxPixlX + box_dimensions;
	int toprightPixY = boxPixlY - box_dimensions;
	if (toprightPixX < map.getSize().x && toprightPixY > 0) {
		if (compareColorValues(map.getPixel(toprightPixX, toprightPixY))) {
			int item = currItem - maxX + 1;
			queueTemp.push_back(item);
		}
	}
	//bottom left
	int bottomleftPixX = boxPixlX - box_dimensions;
	int bottomleftPixY = boxPixlY + box_dimensions;
	if (bottomleftPixX > 0 && bottomleftPixY < map.getSize().y) {
		if (compareColorValues(map.getPixel(bottomleftPixX, bottomleftPixY))) {
			int item = currItem + maxX - 1;
			queueTemp.push_back(item);
		}
	}
	//bottom right
	int bottomrightPixX = boxPixlX + box_dimensions;
	int bottomrightPixY = boxPixlY + box_dimensions;
	if (bottomrightPixX < map.getSize().x && bottomrightPixY < map.getSize().y) {
		if (compareColorValues(map.getPixel(bottomrightPixX, bottomrightPixY))) {
			int item = currItem + maxX + 1;
			queueTemp.push_back(item);
		}
	}

	return queueTemp;
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