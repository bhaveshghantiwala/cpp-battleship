#include "Game.h"
#include "Board.h"
#include "Player.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;

class GameImpl
{
  public:
    GameImpl(int nRows, int nCols);
	~GameImpl();
    int rows() const;
    int cols() const;
    bool isValid(Point p) const;
    Point randomPoint() const;
    bool addShip(int length, char symbol, string name);
    int nShips() const;
    int shipLength(int shipId) const;
    char shipSymbol(int shipId) const;
    string shipName(int shipId) const;
    Player* play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause);

	private:
		//variables
		int m_nRows;
		int m_nCols;
		struct Ship
		{
			int m_shipLength;
			char m_shipSymbol;
			string m_shipName;
			Ship* m_next;
		};
		Ship* m_head;
		Ship* m_tail;
		int m_nShips;
		
		//functions
		Ship* findShip(int shipId) const;
};

void waitForEnter()
{
    cout << "Press enter to continue: ";
    cin.ignore(10000, '\n');
}

GameImpl::GameImpl(int nRows, int nCols)
	:m_nRows(nRows), m_nCols(nCols), m_head(NULL), m_tail(NULL), m_nShips(0)
{
    
}

GameImpl::~GameImpl()
{
	Ship* p = m_head;
	Ship* q;
	while(p != NULL)
	{
		q = p->m_next;
		delete p;
		p = q;
	}
}


int GameImpl::rows() const
{
    return m_nRows;  
}

int GameImpl::cols() const
{
    return m_nCols;  
}

bool GameImpl::isValid(Point p) const
{
    return p.r >= 0  &&  p.r < rows()  &&  p.c >= 0  &&  p.c < cols();
}

Point GameImpl::randomPoint() const
{
    return Point(randInt(rows()), randInt(cols()));
}

bool GameImpl::addShip(int length, char symbol, string name)
{
	if(m_head == NULL)
	{
		m_head = new Ship;
		m_head->m_shipLength = length;
		m_head->m_shipSymbol = symbol;
		m_head->m_shipName = name;
		m_head->m_next = NULL;
		m_tail = m_head;
	}
	else
	{
		Ship* temp = new Ship;
		temp->m_shipLength = length;
		temp->m_shipSymbol = symbol;
		temp->m_shipName = name;
		temp->m_next = NULL;
		m_tail->m_next = temp;
		m_tail = temp;
	}
	m_nShips++;
    return true;  
}

int GameImpl::nShips() const
{
    return m_nShips;  
}

int GameImpl::shipLength(int shipId) const
{
    return findShip(shipId)->m_shipLength;  
}

char GameImpl::shipSymbol(int shipId) const
{
	return findShip(shipId)->m_shipSymbol;  
}

string GameImpl::shipName(int shipId) const
{
    return findShip(shipId)->m_shipName;  
}

Player* GameImpl::play(Player* p1, Player* p2, Board& b1, Board& b2, bool shouldPause)
{
	//place player 1 ships on board 1
	int numberTries = 0;
	if(p1->isHuman())
		cout << p1->name() << " must place " << this->nShips() << " ships." << endl;
	for( ; numberTries < MAX_PLACEMENT_TRIES; numberTries++)
	{
		if(p1->placeShips(b1))
			break;
		else
			b1.clear();
	}
	if(numberTries == MAX_PLACEMENT_TRIES)
		return NULL;

	//place player 2 ships on board 2
	numberTries = 0;
	if(p2->isHuman())
		cout << p2->name() << " must place " << this->nShips() << " ships." << endl;
	for( ; numberTries < MAX_PLACEMENT_TRIES; numberTries++)
	{
		if(p2->placeShips(b2))
			break;
		else
			b2.clear();
	}
	if(numberTries == MAX_PLACEMENT_TRIES)
		return NULL;

	//create the variables for attack
	Point attack;
	bool validShot;
	bool shotHit;
	bool shipDestroyed;
	int shipId;

	//play the game

	int numCols = p1->game().cols();
	int numRows = p1->game().rows();
	int maxIter = numCols*numRows;

	for(int i = 0; i < maxIter; i++)
	{
		//reset shot hit and shot destroyed
		validShot = false;
		shotHit = false;
		shipDestroyed = false;

		//player 1's turn
		cout << p1->name() << "'s turn.  Board for " << p2->name() << ":" << endl;
		b2.display(p1->isHuman());
		attack = p1->recommendAttack();
		validShot = b2.attack(attack, shotHit, shipDestroyed, shipId);
		p1->recordAttackResult(attack, validShot, shotHit, shipDestroyed, shipId);
		if(validShot)
		{
			if(shipDestroyed)
				cout << p1->name() << " attacked (" << attack.r << "," << attack.c << ") and destroyed the " << this->shipName(shipId) <<  ", resulting in:" << endl;
			else if(shotHit)
				cout << p1->name() << " attacked (" << attack.r << "," << attack.c << ") and hit something, resulting in:" << endl;
			else
				cout << p1->name() << " attacked (" << attack.r << "," << attack.c << ") and missed, resulting in:" << endl;
			b2.display(p1->isHuman());
			
			if(b2.allShipsDestroyed())
			{
				if(p2->isHuman())
					b1.display(false);
				cout << p1->name() << " wins!" << endl;
				return p1;
			}
		}
		else
		{
			cout << p1->name() << " wasted a shot at (" << attack.r << "," << attack.c << ")." << endl;
		}
		if(shouldPause)
			waitForEnter();

		//reset shot hit and shot destroyed
		validShot = false;
		shotHit = false;
		shipDestroyed = false;

		//player 2's turn
		cout << p2->name() << "'s turn.  Board for " << p1->name() << ":" << endl;
		b1.display(p2->isHuman());
		attack = p2->recommendAttack();
		validShot = b1.attack(attack, shotHit, shipDestroyed, shipId);
		p2->recordAttackResult(attack, validShot, shotHit, shipDestroyed, shipId);
		if(validShot)
		{
			if(shipDestroyed)
				cout << p2->name() << " attacked (" << attack.r << "," << attack.c << ") and destroyed the " << this->shipName(shipId) <<  ", resulting in:" << endl;
			else if(shotHit)
				cout << p2->name() << " attacked (" << attack.r << "," << attack.c << ") and hit something, resulting in:" << endl;
			else
				cout << p2->name() << " attacked (" << attack.r << "," << attack.c << ") and missed, resulting in:" << endl;
			b1.display(p2->isHuman());
			
			if(b1.allShipsDestroyed())
			{
				if(p1->isHuman())
					b2.display(false);
				cout << p2->name() << " wins!" << endl;
				return p2;
			}
		}
		else
		{
			cout << p2->name() << " wasted a shot at (" << attack.r << "," << attack.c << ")." << endl;
		}
		if(shouldPause)
			waitForEnter();
	}
	
}

GameImpl::Ship* GameImpl::findShip(int shipId) const
{
	Ship* p = m_head;
	for(int i = 0; i < shipId; i++)
		p = p->m_next;
	return p;
}

//******************** Game functions *******************************


Game::Game(int nRows, int nCols)
{
    if (nRows < 1  ||  nRows > MAXROWS)
    {
        cout << "Number of rows must be >= 1 and <= " << MAXROWS << endl;
        exit(1);
    }
    if (nCols < 1  ||  nCols > MAXCOLS)
    {
        cout << "Number of columns must be >= 1 and <= " << MAXCOLS << endl;
        exit(1);
    }
    m_impl = new GameImpl(nRows, nCols);
}

Game::~Game()
{
    delete m_impl;
}

int Game::rows() const
{
    return m_impl->rows();
}

int Game::cols() const
{
    return m_impl->cols();
}

bool Game::isValid(Point p) const
{
    return m_impl->isValid(p);
}

Point Game::randomPoint() const
{
    return m_impl->randomPoint();
}

bool Game::addShip(int length, char symbol, string name)
{
    if (length < 1)
    {
        cout << "Bad ship length " << length << "; it must be >= 1" << endl;
        return false;
    }
    if (length > rows()  &&  length > cols())
    {
        cout << "Bad ship length " << length << "; it won't fit on the board"
             << endl;
        return false;
    }
    if (!isascii(symbol)  ||  !isprint(symbol))
    {
        cout << "Unprintable character with decimal value " << symbol
             << " must not be used as a ship symbol" << endl;
        return false;
    }
    if (symbol == 'X'  ||  symbol == '.'  ||  symbol == 'o')
    {
        cout << "Character " << symbol << " must not be used as a ship symbol"
             << endl;
        return false;
    }
    int totalOfLengths = 0;
    for (int s = 0; s < nShips(); s++)
    {
        totalOfLengths += shipLength(s);
        if (shipSymbol(s) == symbol)
        {
            cout << "Ship symbol " << symbol
                 << " must not be used for more than one ship" << endl;
            return false;
        }
    }
    if (totalOfLengths + length > rows() * cols())
    {
        cout << "Board is too small to fit all ships" << endl;
        return false;
    }
    return m_impl->addShip(length, symbol, name);
}

int Game::nShips() const
{
    return m_impl->nShips();
}

int Game::shipLength(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipLength(shipId);
}

char Game::shipSymbol(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipSymbol(shipId);
}

string Game::shipName(int shipId) const
{
    assert(shipId >= 0  &&  shipId < nShips());
    return m_impl->shipName(shipId);
}

Player* Game::play(Player* p1, Player* p2, bool shouldPause)
{
    if (p1 == NULL  ||  p2 == NULL  ||  nShips() == 0)
        return NULL;
    Board b1(*this);
    Board b2(*this);
    return m_impl->play(p1, p2, b1, b2, shouldPause);
}

