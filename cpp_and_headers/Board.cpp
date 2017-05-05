#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>

using namespace std;

class BoardImpl
{
  public:
    BoardImpl(const Game& g);
	~BoardImpl();
    void clear();
    void block();
    void unblock();
    bool placeShip(Point topOrLeft, int shipId, Direction dir);
    bool unplaceShip(Point topOrLeft, int shipId, Direction dir);
    void display(bool shotsOnly) const;
    bool attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId);
    bool allShipsDestroyed() const;

	private:
		char m_board[MAXROWS][MAXCOLS];
		int m_nShips;
		struct Ship
		{
			Direction m_dir;
			Point m_topOrLeft;
		};

		Ship* m_ships;

		const Game& m_game;
};

BoardImpl::BoardImpl(const Game& g)
	: m_nShips(g.nShips()), m_game(g)
{
	m_ships = new Ship[m_game.nShips()];


	//create the dots
	for(int r = 0; r < m_game.rows(); r++)
	{
		for(int c = 0; c < m_game.cols(); c++)
			m_board[r][c] = '.';
	}

    // This compiles, but may not be correct
}

BoardImpl::~BoardImpl()
{
	delete [] m_ships;
}

void BoardImpl::clear()
{
	//make the board dots again
    for(int r = 0; r < m_game.rows(); r++)
	{
		for(int c = 0; c < m_game.cols(); c++)
			m_board[r][c] = '.';
	}
}

void BoardImpl::block()
{
      // Block cells with 50% probability
    for (int r = 0; r < m_game.rows(); r++)
	{
        for (int c = 0; c < m_game.cols(); c++)
		{
            if (randInt(2) == 0)
            {
				m_board[r][c] = '#';
            }
		}
	}
}

void BoardImpl::unblock()
{
    for (int r = 0; r < m_game.rows(); r++)
	{
        for (int c = 0; c < m_game.cols(); c++)
        {
			if(m_board[r][c] == '#')
				m_board[r][c] = '.'; 
        }
	}
}

bool BoardImpl::placeShip(Point topOrLeft, int shipId, Direction dir)
{
	//check validity of shipId
	if(shipId >= m_game.nShips() || shipId < 0)
		return false;

	//check if ship can fit in board
	if(m_game.isValid(topOrLeft) == false)
		return false;
	if(dir == VERTICAL && topOrLeft.r + m_game.shipLength(shipId) > m_game.rows())
		return false;
	if(dir == HORIZONTAL && topOrLeft.c + m_game.shipLength(shipId) > m_game.cols())
		return false;

	//check ship overlap, block function, and already place shipId for vertical
	if(dir == VERTICAL)
	{
		for(int r = topOrLeft.r; r < topOrLeft.r + m_game.shipLength(shipId); r++)
		{
			if(m_board[r][topOrLeft.c] != '.')
				return false;
		}
	}

	//check ship overlap, block function, and already place shipId for horizontal
	if(dir == HORIZONTAL)
	{
		for(int c = topOrLeft.c; c < topOrLeft.c + m_game.shipLength(shipId); c++)
		{
			if(m_board[topOrLeft.r][c] != '.')
				return false;
		}
	}


	//place ship vertical
	if(dir == VERTICAL)
	{
		for(int r = topOrLeft.r; r < topOrLeft.r + m_game.shipLength(shipId); r++)
			m_board[r][topOrLeft.c] = m_game.shipSymbol(shipId);
		m_ships[shipId].m_dir = dir;
		m_ships[shipId].m_topOrLeft = topOrLeft;
	}

	//place ship horizontal
	if(dir == HORIZONTAL)
	{
		for(int c = topOrLeft.c; c < topOrLeft.c + m_game.shipLength(shipId); c++)
			m_board[topOrLeft.r][c] = m_game.shipSymbol(shipId);
		m_ships[shipId].m_dir = dir;
		m_ships[shipId].m_topOrLeft = topOrLeft;
	}

	//m_nShips++;
	return true;

    // This compiles, but may not be correct
}

bool BoardImpl::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
	//check validity of shipId
	if(shipId >= m_game.nShips() || shipId < 0)
		return false;

	//check if ship can fit in board
	if(m_game.isValid(topOrLeft) == false)
		return false;
	if(dir == VERTICAL && topOrLeft.r + m_game.shipLength(shipId) > m_game.rows())
		return false;
	if(dir == HORIZONTAL && topOrLeft.c + m_game.shipLength(shipId) > m_game.cols())
		return false;

	//check ship overlap, block function, and already place shipId for vertical
	if(dir == VERTICAL)
	{
		for(int r = topOrLeft.r; r < topOrLeft.r + m_game.shipLength(shipId); r++)
		{
			if(m_board[r][topOrLeft.c] != m_game.shipSymbol(shipId))
				return false;
		}
	}

	//check ship overlap, block function, and already place shipId for horizontal
	if(dir == HORIZONTAL)
	{
		for(int c = topOrLeft.c; c < topOrLeft.c + m_game.shipLength(shipId); c++)
		{
			if(m_board[topOrLeft.r][c] != m_game.shipSymbol(shipId))
				return false;
		}
	}

	//remove ship vertical
	if(dir == VERTICAL)
	{
		for(int r = topOrLeft.r; r < topOrLeft.r + m_game.shipLength(shipId); r++)
			m_board[r][topOrLeft.c] = '.';
	}

	//remove ship horizontal
	if(dir == HORIZONTAL)
	{
		for(int c = topOrLeft.c; c < topOrLeft.c + m_game.shipLength(shipId); c++)
			m_board[topOrLeft.r][c] = '.';
	}

	//m_nShips--;
	return true;

    // This compiles, but may not be correct
}

void BoardImpl::display(bool shotsOnly) const
{
	if(shotsOnly == false)
	{
		//create the two spaces
		for(int c = 0; c < 2; c++)
			cout << ' ';

		//create the top horizontal numbers
		for(int c = 0; c < m_game.cols(); c++)
			cout << c;
		cout << endl;
		
		//create the horizontal numbers the space and the 
		for(int r = 0; r < m_game.rows(); r++)
		{
			cout << r << ' ';
			for(int c = 0; c < m_game.cols(); c++)
				cout << m_board[r][c];
			cout << endl;
		}
	}

	else
	{
		//create the two spaces
		for(int c = 0; c < 2; c++)
			cout << ' ';

		//create the top horizontal numbers
		for(int c = 0; c < m_game.cols(); c++)
			cout << c;
		cout << endl;
		
		//create the horizontal numbers the space and the 
		for(int r = 0; r < m_game.rows(); r++)
		{
			cout << r << ' ';
			for(int c = 0; c < m_game.cols(); c++)
			{
				if(m_board[r][c] == 'X' || m_board[r][c] == 'o')
					cout << m_board[r][c];
				else
					cout << '.';
			}
			cout << endl;
		}
	}
    // This compiles, but may not be correct
}

bool BoardImpl::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
	//check validity of point
	if(m_game.isValid(p) == false)
		return false;

	//check if previously attacked
	if(m_board[p.r][p.c] == 'X' || m_board[p.r][p.c] == 'o')
		return false;

	//set the shotHit parameter 
	//if its not a: . X o, then it must be a ship
	char shipHitSymbol = '@';
	if(m_board[p.r][p.c] == '.')
	{
		m_board[p.r][p.c] = 'o';
		shotHit = false;
	}
	else
	{
		shipHitSymbol = m_board[p.r][p.c];
		m_board[p.r][p.c] = 'X';
		shotHit = true;
	}

	//check if ship is destroyed
	if(shipHitSymbol != '@')
	{
		int tempShipId;
		for(int s = 0; s < m_game.nShips(); s++)
		{
			if(m_game.shipSymbol(s) == shipHitSymbol)
			{
				tempShipId = s;
				break;
			}
		}

		int count = 0;
		if(m_ships[tempShipId].m_dir == VERTICAL)
		{
			for(int r = m_ships[tempShipId].m_topOrLeft.r; r < m_ships[tempShipId].m_topOrLeft.r + m_game.shipLength(tempShipId); r++)
			{
				if(m_board[r][m_ships[tempShipId].m_topOrLeft.c] == 'X')
					count++;
			}
		}

		else if(m_ships[tempShipId].m_dir == HORIZONTAL)
		{
			for(int c = m_ships[tempShipId].m_topOrLeft.c; c < m_ships[tempShipId].m_topOrLeft.c + m_game.shipLength(tempShipId); c++)
			{
				if(m_board[m_ships[tempShipId].m_topOrLeft.r][c] == 'X')
					count++;
			}
		}

		if(count == m_game.shipLength(tempShipId))
		{
			shipDestroyed = true;
			shipId = tempShipId;
			m_nShips--;
		}
	}

    return true; // This compiles, but may not be correct
}

bool BoardImpl::allShipsDestroyed() const
{
	if(m_nShips == 0)
		return true;
	else
		return false;
    // This compiles, but may not be correct
}

//******************** Board functions ********************************

// These functions simply delegate to BoardImpl's functions.
// You probably don't want to change any of this code.

Board::Board(const Game& g)
{
    m_impl = new BoardImpl(g);
}

Board::~Board()
{
    delete m_impl;
}

void Board::clear()
{
    m_impl->clear();
}

void Board::block()
{
    return m_impl->block();
}

void Board::unblock()
{
    return m_impl->unblock();
}

bool Board::placeShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->placeShip(topOrLeft, shipId, dir);
}

bool Board::unplaceShip(Point topOrLeft, int shipId, Direction dir)
{
    return m_impl->unplaceShip(topOrLeft, shipId, dir);
}

void Board::display(bool shotsOnly) const
{
    m_impl->display(shotsOnly);
}

bool Board::attack(Point p, bool& shotHit, bool& shipDestroyed, int& shipId)
{
    return m_impl->attack(p, shotHit, shipDestroyed, shipId);
}

bool Board::allShipsDestroyed() const
{
    return m_impl->allShipsDestroyed();
}
