#include <iostream>
#include <cstdio>
#include <SFML/Graphics.hpp>

const int width = 25;
const int height = 15;
const int tileOffset = 50;

char board[height][width];
sf::Sprite sprites[height][width];

enum ETileTypes
{
	EMPTY = 0,
	BORDER,
	PRIZE,
	BODY_PART,
	HEAD,
	COUNT,
	INVALID
};

char prizeTypes[5] = {'0','1','2','3','4'};

sf::Texture* textures[COUNT];

void LoadTextures()
{
	sf::String paths[COUNT];
	paths[EMPTY] = "Resources/EmptySpace.png";
	paths[BORDER] = "Resources/Border.png";
	paths[PRIZE] = "Resources/Prize.png";
	paths[BODY_PART] = "Resources/Bodypart.png";
	paths[HEAD] = "Resources/Head.png";

	for(int i = 0 ; i < COUNT ; ++i)
	{
		textures[i] = new sf::Texture();
		textures[i]->loadFromFile(paths[i]);
	}
}

void BoardRefresh()
{
	memset(board,' ',sizeof(board));
	for(int i = 0 ; i < width ; i ++)
	{
		board[0][i] = '-';
		board[height-1][i] = '-';
	}
	for(int i = 0 ; i < height ; i ++)
	{
		board[i][0] = '|';
		board[i][width-1] = '|';
	}

	board[0][0] = '/';
	board[0][width-1] = '\\';
	board[height-1][0] = '\\';
	board[height-1][width-1] = '/';
}

void BoardInit()
{
	BoardRefresh();
}

void BoardDraw(sf::RenderWindow& renderWindow)
{
	sf::Vector2f tilesize = sf::Vector2f(50.0f,50.0f);
	for(int i = 0 ; i < height ; i ++)
	{
		for(int j = 0 ; j < width ; j ++)
		{
			printf("%c", board[i][j]);
			sprites[i][j].setPosition(0+tilesize.x*j,0+tilesize.y*i);
			if(board[i][j]==' ')
				sprites[i][j].setTexture(*textures[EMPTY]);
			if(board[i][j]=='-' || board[i][j]=='|' || board[i][j]=='/' || board[i][j]=='\\')
				sprites[i][j].setTexture(*textures[BORDER]);
			if(board[i][j]=='@')
				sprites[i][j].setTexture(*textures[HEAD]);
			if(board[i][j]=='#')
				sprites[i][j].setTexture(*textures[BODY_PART]);
			if(board[i][j]=='0' || board[i][j]=='1' || board[i][j]=='2' || board[i][j]=='3' || board[i][j]=='4')
				sprites[i][j].setTexture(*textures[PRIZE]);
			renderWindow.draw(sprites[i][j]);
		}
		printf("\n");
	}

}

class BodyPart
{
public:
	sf::Vector2i position;
	char type;
};

class Prize
{

public:
	sf::Vector2i position;
	char type;

	void Init()
	{
		bool spawnedOnTail = true;
		while (spawnedOnTail)
		{
			position.x = rand() % (width - 2) + 1;
			position.y = rand() % (height - 2) + 1;
			if (board[position.y][position.x] != '#')
				spawnedOnTail = false;
		}
		type = prizeTypes[rand()%sizeof(prizeTypes)];
	}

	void Imprint()
	{
		board[position.y][position.x] = type;
	}

};

class Snake
{
	BodyPart parts[500];
	int length;
	sf::Clock delay;
	sf::Keyboard::Key lastKey;
	bool died;

public:

	void Init()
	{
		length = 1;
		parts[0].position = sf::Vector2i(width/2, height/2);
		parts[0].type = '@';
		lastKey = sf::Keyboard::Key::Space;
		died = false;
	}
	void Move(sf::Keyboard::Key key, Prize& prize)
	{
		if (Died())
		{
			return;
		}
		if(delay.getElapsedTime().asSeconds() >= 0.12f || key!=lastKey)
		{
			delay.restart();
			//input
			UpdateTail();
			if(sf::Keyboard::Key::A == key)
			{
				if (board[parts[0].position.y][parts[0].position.x - 1] == '#')
					died = true;
				parts[0].position.x -= 1; 
			}
			else if(sf::Keyboard::Key::W == key)
			{
				if (board[parts[0].position.y - 1][parts[0].position.x] == '#')
					died = true;
				parts[0].position.y -= 1;
			}
			else if(sf::Keyboard::Key::D == key)
			{
				if (board[parts[0].position.y][parts[0].position.x + 1] == '#')
					died = true;
				parts[0].position.x += 1;
			}
			else if(sf::Keyboard::Key::S == key)
			{
				if (board[parts[0].position.y + 1][parts[0].position.x] == '#')
					died = true;
				parts[0].position.y += 1;
			}
			
			lastKey = key;
			//borders interaction
			if(parts[0].position.x <= 0)
				parts[0].position.x = width-2;
			if(parts[0].position.x >= width-1)
				parts[0].position.x = 1;
			if(parts[0].position.y <= 0)
				parts[0].position.y = height-2;
			if(parts[0].position.y >= height-1)
				parts[0].position.y = 1;	

			//prize interaction
			if(parts[0].position == prize.position)
			{
				parts[length].position = parts[length-1].position;
				parts[length].type = '#';
				length++;
				prize.Init();
			}
		}
	}

	void UpdateTail()
	{
		for(int i = length-1 ; i >= 1 ; --i)
		{
			parts[i].position = parts[i-1].position;
		}
	}
	
	bool Died()
	{
		return died;
	}

	void Imprint()
	{
		for(int i = 0 ; i < length ; i++)
			board[parts[i].position.y][parts[i].position.x] = parts[i].type;
	}

};



int main()
{
	sf::RenderWindow window(sf::VideoMode(1600,900), "Snake");
	LoadTextures();
	srand(NULL);
	Snake snake;
	Prize prize;
	snake.Init();
	prize.Init();
	BoardInit();
	snake.Imprint();
	prize.Imprint();
	BoardDraw(window);
	sf::Keyboard::Key notAllowedKey = sf::Keyboard::Space;
	sf::Keyboard::Key key = sf::Keyboard::Space;

	while(window.isOpen() && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
				window.close();
		}

		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) && notAllowedKey != sf::Keyboard::Key::A)
		{
			key = sf::Keyboard::Key::A;
			notAllowedKey = sf::Keyboard::Key::D;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) && notAllowedKey != sf::Keyboard::Key::S)
		{
			key = sf::Keyboard::Key::S;
			notAllowedKey = sf::Keyboard::Key::W;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) && notAllowedKey != sf::Keyboard::Key::D)
		{
			key = sf::Keyboard::Key::D;
			notAllowedKey = sf::Keyboard::Key::A;
		}
		if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) && notAllowedKey != sf::Keyboard::Key::W)
		{
			key = sf::Keyboard::Key::W;
			notAllowedKey = sf::Keyboard::Key::S;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Return))
		{
			snake.Init();
			prize.Init();
			BoardInit();
			snake.Imprint();
			prize.Imprint(); 
			BoardDraw(window);
		}
		snake.Move(key,prize);
		BoardRefresh();
		snake.Imprint();
		prize.Imprint();
		for(int i = 0; i < 20 ; i ++)
			printf("\n");
		window.clear();
		BoardDraw(window);
		window.display();
	}
	if(window.isOpen())
		window.close();

	system("pause");
}