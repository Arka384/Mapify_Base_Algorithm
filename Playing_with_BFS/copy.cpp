//#include <SFML/Graphics.hpp>
//#include <iostream>
//#include <list>
//constexpr int window_dimensions = 800;
//constexpr int box_dimensions = 20;
//constexpr int max = window_dimensions / box_dimensions;
//constexpr int vertex = max * max;
//using namespace sf;
//
//typedef struct cell {
//	RectangleShape box;
//	int wall;
//	int visited;
//	int vertex;
//}cell;
//cell matrix[max][max];
//int adj_matrix[vertex][vertex] = { 0 };
//int pred[vertex] = { -1 };
//int path[vertex] = { 0 };
//int source = 0, des = 0;
//sf::Vector2i desIndex = sf::Vector2i(max / 2, max / 2);
//sf::Vector2i prevDesIndex;
//
//Clock cl;
//Time dt;
//float mx, my;
//
//void init(void);
//void init(int f);
//void mouse_update(void);
//void create_adj_mat(void);
//void add_edge(int, int);
//void bfs(void);
//int get_path(void);
//
//int main()
//{
//	RenderWindow window(VideoMode(window_dimensions, window_dimensions), "HAHAHAHAA..", Style::Close);
//	float t = 0;
//	int state = 1;
//	init();
//
//	while (window.isOpen())
//	{
//		//dt = cl.restart();
//		//t += dt.asSeconds();
//
//		Event e;
//		while (window.pollEvent(e))
//		{
//			switch (e.type)
//			{
//			case Event::Closed:
//				window.close();
//			case Event::KeyPressed:
//				if (Keyboard::isKeyPressed(Keyboard::Enter)) {
//					if (state == 0) {
//						create_adj_mat();
//						state = 1;
//					}
//				}
//				else if (Keyboard::isKeyPressed(Keyboard::R)) {
//					init();
//					state = 0;
//				}
//				break;
//
//			default:
//				break;
//			}
//		}
//
//		///////////////////////
//
//		mx = Mouse::getPosition(window).x;
//		my = Mouse::getPosition(window).y;
//
//		//states updating
//		//if (state == 0)
//		mouse_update();
//
//
//		if (state == 1)
//		{
//			create_adj_mat();
//			bfs();
//			state = 2;
//		}
//
//		if (state == 2)
//		{
//			int length = get_path();
//			for (int k = 0; k < length; k++)
//				for (int i = 0; i < max; i++)
//					for (int j = 0; j < max; j++)
//						if (matrix[i][j].vertex == path[k] && path[k] != source && path[k] != des)
//							matrix[i][j].box.setFillColor(Color::Yellow);
//			state = 1;
//		}
//
//
//
//
//		/////////////////////////
//
//		window.clear();
//
//		for (int i = 0; i < max; i++)
//			for (int j = 0; j < max; j++)
//				window.draw(matrix[i][j].box);
//
//		window.display();
//	}
//}
//
//
//void init(void)
//{
//	int count = 0;
//	for (int i = 0; i < max; i++)
//	{
//		for (int j = 0; j < max; j++)
//		{
//			matrix[i][j].box.setSize(Vector2f(box_dimensions - 2, box_dimensions - 2));
//			matrix[i][j].box.setFillColor(Color(92, 92, 92, 255));
//			matrix[i][j].box.setOutlineThickness(2.f);
//			matrix[i][j].box.setOutlineColor(Color::Black);
//			matrix[i][j].box.setPosition(i * box_dimensions, j * box_dimensions);
//			matrix[i][j].wall = 0;
//			matrix[i][j].visited = 0;
//			matrix[i][j].vertex = count;
//			count++;
//		}
//	}
//
//	for (int i = 0; i < vertex; i++)
//	{
//		path[i] = 0;
//		pred[i] = -1;
//		for (int j = 0; j < vertex; j++)
//			adj_matrix[i][j] = 0;
//	}
//
//	//setting source
//	source = matrix[0][0].vertex;
//	matrix[0][0].box.setFillColor(sf::Color::Red);
//	//for des 
//	des = matrix[max / 2][max / 2].vertex;
//	matrix[max / 2][max / 2].box.setFillColor(sf::Color::Green);
//}
//
//void init(int f)
//{
//	for (int i = 0; i < max; i++)
//	{
//		for (int j = 0; j < max; j++)
//		{
//			if (matrix[i][j].vertex != source && matrix[i][j].vertex != des && matrix[i][j].wall == 0) {
//				matrix[i][j].box.setFillColor(Color(92, 92, 92, 255));
//				matrix[i][j].visited = 0;
//			}
//		}
//	}
//
//	for (int i = 0; i < vertex; i++)
//	{
//		path[i] = 0;
//		pred[i] = -1;
//		for (int j = 0; j < vertex; j++)
//			adj_matrix[i][j] = 0;
//	}
//
//}
//
//void create_adj_mat()
//{
//	for (int i = 0; i < max; i++)
//	{
//		for (int j = 0; j < max; j++)
//		{
//			if (matrix[i][j].wall == 0)
//			{
//				if (j + 1 < max && matrix[i][j + 1].wall == 0)
//					add_edge(matrix[i][j].vertex, matrix[i][j + 1].vertex);
//				if (j - 1 > 0 && matrix[i][j - 1].wall == 0)
//					add_edge(matrix[i][j].vertex, matrix[i][j - 1].vertex);
//				if (i + 1 < max && matrix[i + 1][j].wall == 0)
//					add_edge(matrix[i][j].vertex, matrix[i + 1][j].vertex);
//				if (i - 1 > 0 && matrix[i - 1][j].wall == 0)
//					add_edge(matrix[i][j].vertex, matrix[i - 1][j].vertex);
//			}
//
//		}
//	}
//}
//
//void add_edge(int a, int b)
//{
//
//	if (b < 0 || b > vertex - 1)
//		return;
//	adj_matrix[a][b] = 1;
//	adj_matrix[b][a] = 1;
//}
//
//void bfs()
//{
//	std::list<int> queue;
//	bool visited[vertex] = { false };
//
//	visited[source] = true;
//	queue.push_back(source);
//
//	while (queue.empty() != 1)
//	{
//		int x = queue.front();
//
//		if (x == des) {
//			pred[source] = -1;
//			return;
//		}
//		queue.pop_front();
//
//		for (int i = 0; i < vertex; i++)
//		{
//			if (adj_matrix[x][i] == 1 && visited[i] == false)
//			{
//				visited[i] = true;
//				queue.push_back(i);
//				pred[i] = x;
//			}
//		}
//	}
//}
//
//
//int get_path()
//{
//	int k = 0, i = des;
//	while (pred[i] != -1)
//	{
//		path[k++] = i;
//		i = pred[i];
//	}
//	path[k++] = i;
//	return k;
//}
//
//void mouse_update()
//{
//	for (int i = 0; i < max; i++)
//	{
//		for (int j = 0; j < max; j++)
//		{
//			int hot = mx > matrix[i][j].box.getPosition().x && mx < matrix[i][j].box.getPosition().x + box_dimensions
//				&& my > matrix[i][j].box.getPosition().y && my < matrix[i][j].box.getPosition().y + box_dimensions;
//
//			if (hot && Keyboard::isKeyPressed(Keyboard::Key::S))
//			{
//				source = matrix[i][j].vertex;
//				matrix[i][j].box.setFillColor(Color::Red);
//			}
//			if (hot && Keyboard::isKeyPressed(Keyboard::Key::D))
//			{
//				if (matrix[i][j].wall == 1)
//					continue;
//				des = matrix[i][j].vertex;
//				matrix[i][j].box.setFillColor(Color::Green);
//				init(1);
//			}
//
//			if (hot && Mouse::isButtonPressed(Mouse::Button::Left))
//			{
//				matrix[i][j].wall = 1;
//				matrix[i][j].box.setFillColor(Color::Black);
//				create_adj_mat();
//			}
//
//			if (hot && Mouse::isButtonPressed(Mouse::Button::Right))
//			{
//				matrix[i][j].wall = 0;
//				matrix[i][j].box.setFillColor(Color(92, 92, 92, 255));
//				create_adj_mat();
//			}
//		}
//	}
//}



//old implementation //too slow
		//for (auto k = adjm.begin(); k != adjm.end(); k++) {
		//	//std::cout << k->first << " " << k->second << "\n";
		//	if (k->first == x && visited[k->second] == false) {
		//		visited[k->second] = true;
		//		queue.push_back(k->second);
		//		pred[k->second] = x;
		//	}
		//	/*else if (k->second == x && visited[k->first] == false) {
		//		visited[k->first] = true;
		//		queue.push_back(k->first);
		//		pred[k->first] = x;
		//	}*/
		//	//std::cout << queue.back() << "\n";
		//}