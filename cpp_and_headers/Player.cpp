#include "Player.h"
#include "Board.h"
#include "Game.h"
#include "globals.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

//*********************************************************************
//  AwfulPlayer
//*********************************************************************

class AwfulPlayer : public Player
{
  public:
    AwfulPlayer(string nm, const Game& g);
    virtual bool placeShips(Board& b);
    virtual Point recommendAttack();
    virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
  private:
    Point m_lastCellAttacked;
};

AwfulPlayer::AwfulPlayer(string nm, const Game& g)
 : Player(nm, g), m_lastCellAttacked(0, 0)
{}

bool AwfulPlayer::placeShips(Board& b)
{
      // Clustering ships is bad strategy
    for (int k = 0; k < game().nShips(); k++)
        if ( ! b.placeShip(Point(k,0), k, HORIZONTAL))
            return false;
    return true;
}

Point AwfulPlayer::recommendAttack()
{
    if (m_lastCellAttacked.c > 0)
        m_lastCellAttacked.c--;
    else
    {
        m_lastCellAttacked.c = game().cols() - 1;
        if (m_lastCellAttacked.r > 0)
            m_lastCellAttacked.r--;
        else
            m_lastCellAttacked.r = game().rows() - 1;
    }
    return m_lastCellAttacked;
}

void AwfulPlayer::recordAttackResult(Point /* p */, bool /* validShot */,
                                     bool /* shotHit */, bool /* shipDestroyed */,
                                     int /* shipId */)
{
      // AwfulPlayer completely ignores the result of any attack
}

//*********************************************************************
//  HumanPlayer
//*********************************************************************

bool getLineWithTwoIntegers(int& r, int& c)
{
    bool result(cin >> r >> c);
    if (!result)
        cin.clear();  // clear error state so can do more input operations
    cin.ignore(10000, '\n');
    return result;
}


class HumanPlayer: public Player
{
	public:
		HumanPlayer(string nm, const Game& g);
		virtual bool isHuman() const;
		virtual bool placeShips(Board& b);
		virtual Point recommendAttack();
		virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);

};

HumanPlayer::HumanPlayer(string nm, const Game& g)
	: Player(nm, g)
{}

bool HumanPlayer::isHuman() const
{
	return true;
}

bool HumanPlayer::placeShips(Board& b)
{
	int r;
	int c;
	char dir;
	Direction direct;
	string line;
	for(int s = 0; s < game().nShips(); s++)
	{
		b.display(false);
		for(;;)
		{
			cout << "Enter h or v for direction of " << game().shipName(s) << " (length " << game().shipLength(s) << "): ";
			getline(cin,line);
			if(line[0] == 'h' || line[0] == 'v')
				break;
			cout << "Direction must be h or v." << endl;
		}
		for(;;)
		{
			if(line[0] == 'h')
			{
				cout << "Enter row and column of leftmost cell (e.g, 3 5): ";
				direct = HORIZONTAL;
			}
			else
			{
				cout << "Enter row and column of topmost cell (e.g, 3 5): ";
				direct = VERTICAL;
			}

			if(getLineWithTwoIntegers(r, c))
			{
				Point p(r, c);
				if(b.placeShip(p, s, direct))
					break;
			}
			cout << "The ship cannot be placed there." << endl;
		}
	}
	return true;
}

Point HumanPlayer::recommendAttack()
{
	int r;
	int c;
	cout << "Enter the row and column to attack (e.g, 3 5): ";
	for(;;)
	{
		if(getLineWithTwoIntegers(r, c))
			break;
	}
	Point p(r, c);
	return p;
}

void HumanPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
	//human does not use this function
}


//*********************************************************************
//  MediocrePlayer
//*********************************************************************

class MediocrePlayer: public Player
{
	public:
		MediocrePlayer(string nm, const Game& g);
		~MediocrePlayer();
		virtual bool placeShips(Board& b);
		virtual Point recommendAttack();
		virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
	private:
		//variables
		Point m_p;
		bool m_shotHit;
		bool m_shipDestroyed;
		int m_state;
		bool m_maxHits;
		vector<Point> attacks;

		Point* m_shipPositions;
		Direction* m_shipDirections;

		//functions
		Point randomAttack();
		Point precisionAttack();
		bool recursiveAllShips(Board& b, Point start, int tries);
		Point recursiveShip(Board& b, int shipId, Point& start, Point startOrigin, Direction& dir);

};

MediocrePlayer::MediocrePlayer(string nm, const Game& g)
	:Player(nm, g), m_shotHit(false), m_shipDestroyed(false), m_state(1), m_maxHits(false)
{
	m_shipPositions = new Point[this->game().nShips()];
	m_shipDirections = new Direction[this->game().nShips()];
}

MediocrePlayer::~MediocrePlayer()
{
	delete [] m_shipPositions;
	delete [] m_shipDirections;
}

bool MediocrePlayer::placeShips(Board& b)
{
	b.block();
	Point start(0,0);
	int tries = 0;
	bool placed = recursiveAllShips(b, start, tries);
	b.unblock();
	return placed;
}

Point MediocrePlayer::recommendAttack()
{
	if(m_state == 1)
		return randomAttack();
	else
	{
		Point checkAttack = precisionAttack();
		if(checkAttack.r == -100)
		{
			m_state = 1;
			return randomAttack();
		}
		else
			return checkAttack;
	}
}

void MediocrePlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
	m_shotHit = shotHit;
	m_shipDestroyed = shipDestroyed;

	if(m_maxHits == true)
	{
		m_maxHits = false;
		m_state = 1;
	}
	else if(m_shipDestroyed == false && m_shotHit == true && m_state == 1)
	{
		m_p = p;
		m_state = 2;
	}
	else if(m_shipDestroyed == true)
	{
		m_maxHits = false;
		m_state = 1;
	}
}

Point MediocrePlayer::randomAttack()
{
	Point newAttack;
	newAttack = this->game().randomPoint();
	for(int i = 0; i < attacks.size(); i++)
	{
		if(attacks.at(i).r == newAttack.r && attacks.at(i).c == newAttack.c)
			return randomAttack();
	}
	attacks.push_back(newAttack);
	return newAttack;
}

Point MediocrePlayer::precisionAttack()
{
	int awayFromAttack = randInt(4) + 1;
	int direction = randInt(4);
	Point newAttack;
	newAttack.r = m_p.r;
	newAttack.c = m_p.c;

	//initialize the checks after the loop
	int hitCount = 0;
	int maximumHits = 0;
	if(newAttack.r - 4 >= 0)
		maximumHits = maximumHits + 4;
	else
		maximumHits = newAttack.r + maximumHits;
	if(newAttack.c + 4 <= this->game().cols() - 1)
		maximumHits = maximumHits + 4;
	else
		maximumHits = this->game().cols() - 1 - newAttack.c + maximumHits;
	if(newAttack.r + 4 <= this->game().rows() - 1)
		maximumHits = maximumHits + 4;
	else
		maximumHits = this->game().rows() - 1 - newAttack.r + maximumHits;
	if(newAttack.c - 4 >= 0)
		maximumHits = maximumHits + 4;
	else
		maximumHits = newAttack.c + maximumHits;

	//check all four directions for four spaces
	for(int k = 1; k < 5; k++)
	{
		//check north
		newAttack.r = m_p.r;
		newAttack.c = m_p.c;
		newAttack.r = newAttack.r - k;
		for(int i = 0; i < attacks.size(); i++)
		{
			if(attacks.at(i).r == newAttack.r && attacks.at(i).c == newAttack.c)
			{
				hitCount++;
				break;
			}
		}

		//check east
		newAttack.r = m_p.r;
		newAttack.c = m_p.c;
		newAttack.c = newAttack.c + k;
		for(int i = 0; i < attacks.size(); i++)
		{
			if(attacks.at(i).r == newAttack.r && attacks.at(i).c == newAttack.c)
			{
				hitCount++;
				break;
			}
		}
		//check south
		newAttack.r = m_p.r;
		newAttack.c = m_p.c;
		newAttack.r = newAttack.r + k;
		for(int i = 0; i < attacks.size(); i++)
		{
			if(attacks.at(i).r == newAttack.r && attacks.at(i).c == newAttack.c)
			{
				hitCount++;
				break;
			}
		}
		//check west
		newAttack.r = m_p.r;
		newAttack.c = m_p.c;
		newAttack.c = newAttack.c - k;
		for(int i = 0; i < attacks.size(); i++)
		{
			if(attacks.at(i).r == newAttack.r && attacks.at(i).c == newAttack.c)
			{
				hitCount++;
				break;
			}
		}
	}

	if(hitCount == maximumHits)
	{
		m_maxHits = true;
		Point p(-100, -100);
		return p;
	}

	newAttack.r = m_p.r;
	newAttack.c = m_p.c;
	bool check = false;
	bool isValid = false;
	switch(direction)
	{
		case 0:
			//attack north
			newAttack.r = newAttack.r - awayFromAttack;
			if(this->game().isValid(newAttack))
			{
				isValid = true;
				for(int i = 0; i < attacks.size(); i++)
				{
					if(attacks.at(i).r == newAttack.r && attacks.at(i).c == newAttack.c)
						check = true;
				}
			}
			break;
		case 1:
			//attack east
			newAttack.c = newAttack.c + awayFromAttack;
			if(this->game().isValid(newAttack))
			{
				isValid = true;
				for(int i = 0; i < attacks.size(); i++)
				{
					if(attacks.at(i).r == newAttack.r && attacks.at(i).c == newAttack.c)
						check = true;
				}
			}
			break;
		case 2:
			//attack south
			newAttack.r = newAttack.r + awayFromAttack;
			if(this->game().isValid(newAttack))
			{
				isValid = true;
				for(int i = 0; i < attacks.size(); i++)
				{
					if(attacks.at(i).r == newAttack.r && attacks.at(i).c == newAttack.c)
						check = true;
				}
			}
			break;
		case 3:
			//attack west
			newAttack.c = newAttack.c - awayFromAttack;
			if(this->game().isValid(newAttack))
			{
				isValid = true;
				for(int i = 0; i < attacks.size(); i++)
				{
					if(attacks.at(i).r == newAttack.r && attacks.at(i).c == newAttack.c)
						check = true;
				}
			}
			break;
	}
	if(check == true || isValid == false)
		return precisionAttack();
	attacks.push_back(newAttack);
	return newAttack;
}

bool MediocrePlayer::recursiveAllShips(Board& b, Point start, int tries)
{
	if(tries == 10)
		return false;
	int placedAllShips = 0;
	Direction dir;
	for(int shipId = 0; shipId < this->game().nShips(); shipId++)
	{
		if(b.placeShip(start, shipId, VERTICAL))
		{
			m_shipPositions[shipId] = start;
			m_shipDirections[shipId] = VERTICAL;
			placedAllShips++;
		}
		else if(b.placeShip(start, shipId, HORIZONTAL))
		{
			m_shipPositions[shipId] = start;
			m_shipDirections[shipId] = HORIZONTAL;
			placedAllShips++;
		}
		else
		{
			if(start.c == this->game().cols() - 1 && start.r == this->game().rows() - 1)
			{
				Point startPlusOne(0, 0);
				Point startOrigin(start.r, start.c);
				Point temp = recursiveShip(b, shipId, startPlusOne, startOrigin, dir);
				if(this->game().isValid(temp))
				{
					start = temp;
					m_shipPositions[shipId] = startPlusOne;
					m_shipDirections[shipId] = dir;
					placedAllShips++;
				}
			}
			else if(start.c == this->game().cols() - 1)
			{
				Point startPlusOne(start.r + 1, 0);
				Point startOrigin(start.r, start.c);
				Point temp = recursiveShip(b, shipId, startPlusOne, startOrigin, dir);
				if(this->game().isValid(temp))
				{
					start = temp;
					m_shipPositions[shipId] = startPlusOne;
					m_shipDirections[shipId] = dir;
					placedAllShips++;
				}
			}
			else
			{
				Point startPlusOne(start.r, start.c + 1);
				Point startOrigin(start.r, start.c);
				Point temp = recursiveShip(b, shipId, startPlusOne, startOrigin, dir);
				if(this->game().isValid(temp))
				{
					start = temp;
					m_shipPositions[shipId] = startPlusOne;
					m_shipDirections[shipId] = dir;
					placedAllShips++;
				}
			}
		}
	}

	if(placedAllShips == this->game().nShips())
		return true;
	else
	{
		for(int shipId = 0; shipId < this->game().nShips(); shipId++)
		{
			bool check = b.unplaceShip(m_shipPositions[shipId], shipId, m_shipDirections[shipId]);
		}
		return recursiveAllShips(b, start, tries + 1);
	}
}

Point MediocrePlayer::recursiveShip(Board& b, int shipId, Point& start, Point startOrigin, Direction& dir)
{
	if(start.r == startOrigin.r && start.c == startOrigin.c)
	{
		Point p(100, 100);
		return p;
	}
	if(b.placeShip(start, shipId, VERTICAL))
	{
		dir = VERTICAL;
		return start;
	}
	if(b.placeShip(start, shipId, HORIZONTAL))
	{
		dir = HORIZONTAL;
		return start;
	}
	
	if(start.c == this->game().cols() - 1 && start.r == this->game().rows() - 1)
	{
		Point startOver(0, 0);
		start = startOver;
		return recursiveShip(b, shipId, start, startOrigin, dir);
	}
	else if(start.c == this->game().cols() - 1)
	{
		Point nextRow(start.r + 1, 0);
		start = nextRow;
		return recursiveShip(b, shipId, start, startOrigin, dir);
	}
	else
	{
		Point nextCol(start.r, start.c + 1);
		start = nextCol;
		return recursiveShip(b, shipId, start, startOrigin, dir);
	}
}


// Remember that Mediocre::placeShips(Board& b) must start by calling
// b.block(), and must call b.unblock() just before returning.

//*********************************************************************
//  GoodPlayer
//*********************************************************************

class GoodPlayer: public Player
{
	public:
		GoodPlayer(string nm, const Game& g);
		~GoodPlayer();
		virtual bool placeShips(Board& b);
		virtual Point recommendAttack();
		virtual void recordAttackResult(Point p, bool validShot, bool shotHit,
                                                bool shipDestroyed, int shipId);
	private:
		//variables
		int* m_shipLength;
		//attack mode 1: checker board style attack
		//attack mode 2: find direction of next attack if mode 1 made a hit
		//attack mode 3: continue in direction if mode 2 made a hit
		//attack mode 4: if 3 or 5 continues and then misses without destroying a ship, go in oposite direction
		//attack mode 5: if 4 continues and then misses without destroying a ship, go in perpendicular direction
		//set back to 4 if 5 continues and then misses without destroying a ship

		//***optimizations***
		//-change checkerboard magnitude based on the smallest ship length left on opponents board
		//-if you hit extra ships in mode 3, 4, or 5 use those points in mode 2 (use stacks)
		int m_attackMode;
		Point m_checkerBoardPos;
		int m_nextAttackDistance;
		vector<Point> allAttacks;
		vector<Point> hitAttacks;
		vector<Point> missedAttacks;

		//functions
		bool recursiveShip(Board& b, int shipId, int tries);
		int greatestShipLength();
		int smallestShipLength();
		Point checkerBoardAttack(int magnitude);
		Point attackDown();
		Point attackRight();
		Point attackLeft();
		Point attackUp();
		int smallestShipLeft();
		//pick one might not need both
		//void opositeDirection();
		//void changeDirection();
};

GoodPlayer::GoodPlayer(string nm, const Game& g)
	:Player(nm, g), m_attackMode(1), m_nextAttackDistance(1)
{
	m_shipLength = new int[greatestShipLength() + 1];
	for(int i = 0; i < greatestShipLength() + 1; i++)
		m_shipLength[i] = 0;
	for(int shipId = 0; shipId < this->game().nShips(); shipId++)
		m_shipLength[this->game().shipLength(shipId)]++;
	m_checkerBoardPos.r = 0;
	m_checkerBoardPos.c = 0 - smallestShipLeft();
}

GoodPlayer::~GoodPlayer()
{
	delete [] m_shipLength;
}

bool GoodPlayer::placeShips(Board& b)
{
	if(greatestShipLength() > this->game().cols() || greatestShipLength() > this->game().rows())
		return false;
	int count = 0;
	for(int shipId = 0; shipId < this->game().nShips(); shipId++)
	{
		int tries = 0;
		if(recursiveShip(b, shipId, tries))
			count++;
	}
	if(count == this->game().nShips())
		return true;
	else
	{
		b.clear();
		return false;
	}
}

Point GoodPlayer::recommendAttack()
{
	if(m_attackMode == 1)
		return checkerBoardAttack(smallestShipLength());
	else if(m_attackMode == 2)
		return attackDown();
	else if(m_attackMode == 3)
		return attackRight();
	else if(m_attackMode == 4)
		return attackLeft();
	else if(m_attackMode == 5)
		return attackUp();
}
void GoodPlayer::recordAttackResult(Point p, bool validShot, bool shotHit, bool shipDestroyed, int shipId)
{
	//adding the point attacked to the vectors
	if(shotHit)
	{
		allAttacks.push_back(p);
		hitAttacks.push_back(p);
	}
	else
	{
		allAttacks.push_back(p);
		missedAttacks.push_back(p);
	}

	if(shipDestroyed)
	{
		m_shipLength[this->game().shipLength(shipId)]--;
	}

	bool inHitAttacks = false;
	for(int i = 0; i < hitAttacks.size(); i++)
	{
		if(p.r == hitAttacks.at(i).r && p.c == hitAttacks.at(i).c)
		{
			inHitAttacks = true;
			break;
		}
	}

	//setting attack mode
	if(m_attackMode == 1 && inHitAttacks == true)
	{
		m_nextAttackDistance = 1;
		m_attackMode = 2;
	}
	else if(m_attackMode == 2 && shotHit == false)
	{
		m_nextAttackDistance = 1;
		m_attackMode = 3;
	}
	else if(m_attackMode == 3 && shotHit == false)
	{
		m_nextAttackDistance = 1;
		m_attackMode = 4;
	}
	else if(m_attackMode == 4 && shotHit == false)
	{
		m_nextAttackDistance = 1;
		m_attackMode = 5;
	}
	else if(m_attackMode == 5 && shotHit == false)
	{
		m_nextAttackDistance = 1;
		m_attackMode = 1;
	}
}

bool GoodPlayer::recursiveShip(Board& b, int shipId, int tries)
{
	if(tries == 10)
		return false;
	int col = randInt(this->game().cols());
	int row = randInt(this->game().rows());
	Point p(row, col);
	int dir = randInt(2);
	Direction d;
	switch(dir)
	{
		case 0:
			d = VERTICAL;
			break;
		case 1:
			d = HORIZONTAL;
			break;
	}
	Point up(p.r - 1, p.c);
	Point right(p.r, p.c + 1);
	Point down(p.r + 1, p.c);
	Point left(p.r, p.c - 1);

	if(this->game().isValid(up))
	{
		if(! b.placeShip(up, shipId, d))
			return recursiveShip(b, shipId, tries + 1);
		b.unplaceShip(up, shipId, d);
	}

	if(this->game().isValid(right))
	{
		if(! b.placeShip(right, shipId, d))
			return recursiveShip(b, shipId, tries + 1);
		b.unplaceShip(right, shipId, d);
	}

	if(this->game().isValid(down))
	{
		if(! b.placeShip(down, shipId, d))
			return recursiveShip(b, shipId, tries + 1);
		b.unplaceShip(down, shipId, d);
	}

	if(this->game().isValid(left))
	{
		if(! b.placeShip(left, shipId, d))
			return recursiveShip(b, shipId, tries + 1);
		b.unplaceShip(left, shipId, d);
	}
	
	if(b.placeShip(p, shipId, d))
		return true;
	else
	{
		b.unplaceShip(p, shipId, d);
		return recursiveShip(b, shipId, tries + 1);
	}
}

int GoodPlayer::greatestShipLength()
{
	int greatestLength = 0;
	for(int shipId = 0; shipId < this->game().nShips(); shipId++)
	{
		if(this->game().shipLength(shipId) > greatestLength)
			greatestLength = this->game().shipLength(shipId);
	}
	return greatestLength;
}

int GoodPlayer::smallestShipLength()
{
	int smallestLength = 100;
	for(int shipId = 0; shipId < this->game().nShips(); shipId++)
	{
		if(this->game().shipLength(shipId) < smallestLength)
			smallestLength = this->game().shipLength(shipId);
	}
	return smallestLength;
}

Point GoodPlayer::checkerBoardAttack(int magnitude)
{
	m_checkerBoardPos.c += magnitude;
	//add the allAttacks check
	if(this->game().isValid(m_checkerBoardPos) == false)
	{
		int row = m_checkerBoardPos.r + 1;
		int col = row % magnitude;
		m_checkerBoardPos.r = row;
		m_checkerBoardPos.c = col;
	}

	for(int i = 0; i < allAttacks.size(); i++)
	{
		if(allAttacks.at(i).r == m_checkerBoardPos.r && allAttacks.at(i).c == m_checkerBoardPos.c)
			return checkerBoardAttack(magnitude); //finish
	}
	return m_checkerBoardPos;
}


Point GoodPlayer::attackDown()
{
	Point newAttack;
	newAttack.r = m_checkerBoardPos.r + m_nextAttackDistance;
	newAttack.c = m_checkerBoardPos.c;
	m_nextAttackDistance++;
	if(this->game().isValid(newAttack) == false)
	{
		m_nextAttackDistance = 1;
		m_attackMode = 3;
		return attackRight();
	}
	for(int i = 0; i < allAttacks.size(); i++)
	{
		if(allAttacks.at(i).r == newAttack.r && allAttacks.at(i).c == newAttack.c)
		{
			m_nextAttackDistance = 1;
			m_attackMode = 3;
			return attackRight();
		}
	}
	return newAttack;
}
Point GoodPlayer::attackRight()
{
	Point newAttack;
	newAttack.r = m_checkerBoardPos.r;
	newAttack.c = m_checkerBoardPos.c + m_nextAttackDistance;
	m_nextAttackDistance++;
	if(this->game().isValid(newAttack) == false)
	{
		m_nextAttackDistance = 1;
		m_attackMode = 4;
		return attackLeft();
	}
	for(int i = 0; i < allAttacks.size(); i++)
	{
		if(allAttacks.at(i).r == newAttack.r && allAttacks.at(i).c == newAttack.c)
		{
			m_nextAttackDistance = 1;
			m_attackMode = 4;
			return attackLeft();
		}
	}
	return newAttack;
}
Point GoodPlayer::attackLeft()
{
	Point newAttack;
	newAttack.r = m_checkerBoardPos.r;
	newAttack.c = m_checkerBoardPos.c - m_nextAttackDistance;
	m_nextAttackDistance++;
	if(this->game().isValid(newAttack) == false)
	{
		m_nextAttackDistance = 1;
		m_attackMode = 5;
		return attackUp();
	}
	for(int i = 0; i < allAttacks.size(); i++)
	{
		if(allAttacks.at(i).r == newAttack.r && allAttacks.at(i).c == newAttack.c)
		{
			m_nextAttackDistance = 1;
			m_attackMode = 5;
			return attackUp();
		}
	}
	return newAttack;
}
Point GoodPlayer::attackUp()
{
	Point newAttack;
	newAttack.r = m_checkerBoardPos.r - m_nextAttackDistance;
	newAttack.c = m_checkerBoardPos.c;
	m_nextAttackDistance++;
	if(this->game().isValid(newAttack) == false)
	{
		m_nextAttackDistance = 1;
		m_attackMode = 1;
		return checkerBoardAttack(smallestShipLength());
	}
	for(int i = 0; i < allAttacks.size(); i++)
	{
		if(allAttacks.at(i).r == newAttack.r && allAttacks.at(i).c == newAttack.c)
		{
			m_nextAttackDistance = 1;
			m_attackMode = 1;
			return checkerBoardAttack(smallestShipLength());
		}
	}
	return newAttack;
}

int GoodPlayer::smallestShipLeft()
{
	int smallestShipSize = 1;
	for(int i = 0; i < greatestShipLength() + 1; i++)
	{
		if(m_shipLength[i] != 0)
		{
			smallestShipSize = i;
			break;
		}
	}
	return smallestShipSize;
}


//*********************************************************************
//  createPlayer
//*********************************************************************

Player* createPlayer(string type, string nm, const Game& g)
{
    static string types[] = {
        "human", "awful", "mediocre", "good"
    };
    
    int pos;
    for (pos = 0; pos != sizeof(types)/sizeof(types[0])  &&
                                                     type != types[pos]; pos++)
        ;
    switch (pos)
    {
      case 0:  return new HumanPlayer(nm, g);
      case 1:  return new AwfulPlayer(nm, g);
      case 2:  return new MediocrePlayer(nm, g);
      case 3:  return new GoodPlayer(nm, g);
      default: return NULL;
    }
}
