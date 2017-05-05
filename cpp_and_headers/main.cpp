#include "Game.h"
#include "Player.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

bool addStandardShips(Game& g)
{
	return g.addShip(5, 'A', "aircraft carrier")  &&
           g.addShip(4, 'B', "battleship")  &&
           g.addShip(3, 'D', "destroyer")  &&
           g.addShip(3, 'S', "submarine")  &&
           g.addShip(2, 'P', "patrol boat");
}

int main()
{
      // Comment out the following line to get the same sequence of
      // random numbers generated every time the program is run (useful
      // for getting reproducible results when debugging).
    srand(static_cast<unsigned int>(time(0)));

    const int NTRIALS = 10;

    cout << "Select one of these choices for an example of the game:" << endl;
    cout << "  1.  A mini-game between two mediocre players" << endl;
    cout << "  2.  A good player against a human player" << endl;
    cout << "  3.  A " << NTRIALS
         << "-game match between a mediocre and an awful player, with no pauses"
         << endl;
	cout << "  4.  A " << NTRIALS
         << "-game match between a good and a mediocre player, with no pauses"
         << endl;
	cout << "  5.  A mediocre player against a good player" << endl;
	cout << "  6.  Run all AI " << NTRIALS << " times" << endl;
    cout << "Enter your choice: ";
    string line;
    getline(cin,line);
    if (line.empty())
    {
        cout << "You did not enter a choice" << endl;
    }
    else if (line[0] == '1')
    {
        Game g(2, 3);
        g.addShip(2, 'R', "rowboat");
        Player* p1 = createPlayer("mediocre", "Popeye", g);
        Player* p2 = createPlayer("mediocre", "Bluto", g);
        cout << "This mini-game has one ship, a 2-segment rowboat." << endl;
        g.play(p1, p2);
        delete p1;
        delete p2;
    }
    else if (line[0] == '2')
    {
        Game g(10, 10);
        addStandardShips(g);
        Player* p1 = createPlayer("good", "Good Guy Greg", g);
        Player* p2 = createPlayer("human", "Human", g);
        g.play(p1, p2);
        delete p1;
        delete p2;
    }
    else if (line[0] == '3')
    {
        int nMediocreWins = 0;

        for (int k = 1; k <= NTRIALS; k++)
        {
            cout << "============================= Game " << k
                 << " =============================" << endl;
            Game g(10, 10);
            addStandardShips(g);
            Player* p1 = createPlayer("awful", "Awful Audrey", g);
            Player* p2 = createPlayer("mediocre", "Mediocre Mimi", g);
            Player* winner = (k % 2 == 1 ?
                                g.play(p1, p2, false) : g.play(p2, p1, false));
            if (winner == p2)
                nMediocreWins++;
            delete p1;
            delete p2;
        }
        cout << "The mediocre player won " << nMediocreWins << " out of "
             << NTRIALS << " games." << endl;
          // We'd expect a mediocre player to win most of the games against
          // an awful player.  Similarly, a good player should outperform
          // a mediocre player.
    }
	else if (line[0] == '4')
    {
        int nGoodWins = 0;
		int nMediocreWins = 0;
		int nNULL = 0;

        for (int k = 1; k <= NTRIALS; k++)
        {
            cout << "============================= Game " << k
                 << " =============================" << endl;
            Game g(10, 10);
            addStandardShips(g);
            Player* p1 = createPlayer("good", "Good Guy Greg", g);
            Player* p2 = createPlayer("mediocre", "Mediocre Mimi", g);
            Player* winner = (k % 2 == 1 ?
                                g.play(p1, p2, false) : g.play(p2, p1, false));
            if (winner == p1)
                nGoodWins++;
			else if (winner == p2)
				nMediocreWins++;
			else if (winner == NULL)
				nNULL++;
            delete p1;
            delete p2;
        }
        cout << "The good player won " << nGoodWins << " out of "
             << NTRIALS << " games." << endl;
		cout << "The mediocre player won " << nMediocreWins << " out of "
             << NTRIALS << " games." << endl;
		cout << "There were " << NTRIALS - nGoodWins - nMediocreWins - nNULL<< " errors out of "
			 << NTRIALS << " games." << endl;
	}
	else if (line[0] == '5')
    {
        Game g(10, 10);
        addStandardShips(g);
        Player* p1 = createPlayer("mediocre", "Mediocre Midori", g);
        Player* p2 = createPlayer("good", "Good Guy Greg", g);
        g.play(p1, p2);
        delete p1;
        delete p2;
    }
	else if (line[0] == '6')
	{
		int nP1Wins = 0;
		int nP2Wins = 0;
		int nNULL = 0;

        for (int k = 1; k <= NTRIALS; k++)
        {
            cout << "============================= Game " << k
                 << " =============================" << endl;
            Game g(10, 10);
            addStandardShips(g);
            Player* p1 = createPlayer("good", "Good Guy Greg", g);
            Player* p2 = createPlayer("good", "George", g);
            Player* winner = (k % 2 == 1 ?
                                g.play(p1, p2, false) : g.play(p2, p1, false));
            if (winner == p1)
                nP1Wins++;
			else if (winner == p2)
				nP2Wins++;
			else if (winner == NULL)
				nNULL++;
            delete p1;
            delete p2;
        }
		cout << "====================================" << endl;
		cout << "Good vs. Good" << endl;
        cout << "The good(1) player won " << nP1Wins << " out of "
             << NTRIALS << " games." << endl;
		cout << "The good(2) player won " << nP2Wins << " out of "
             << NTRIALS << " games." << endl;
		cout << "There were " << NTRIALS - nP1Wins - nP2Wins - nNULL << " errors out of "
			 << NTRIALS << " games." << endl;


		nP1Wins = 0;
		nP2Wins = 0;
		nNULL = 0;

        for (int k = 1; k <= NTRIALS; k++)
        {
            cout << "============================= Game " << k
                 << " =============================" << endl;
            Game g(10, 10);
            addStandardShips(g);
            Player* p1 = createPlayer("good", "Good Guy Greg", g);
            Player* p2 = createPlayer("mediocre", "Mediocre Melvin", g);
            Player* winner = (k % 2 == 1 ?
                                g.play(p1, p2, false) : g.play(p2, p1, false));
            if (winner == p1)
                nP1Wins++;
			else if (winner == p2)
				nP2Wins++;
			else if (winner == NULL)
				nNULL++;
            delete p1;
            delete p2;
        }
		cout << "====================================" << endl;
		cout << "Good vs. Mediocre" << endl;
        cout << "The good player won " << nP1Wins << " out of "
             << NTRIALS << " games." << endl;
		cout << "The mediocre player won " << nP2Wins << " out of "
             << NTRIALS << " games." << endl;
		cout << "There were " << NTRIALS - nP1Wins - nP2Wins - nNULL << " errors out of "
			 << NTRIALS << " games." << endl;

		nP1Wins = 0;
		nP2Wins = 0;
		nNULL = 0;

        for (int k = 1; k <= NTRIALS; k++)
        {
            cout << "============================= Game " << k
                 << " =============================" << endl;
            Game g(10, 10);
            addStandardShips(g);
            Player* p1 = createPlayer("good", "Good Guy Greg", g);
            Player* p2 = createPlayer("awful", "Awful Aaron", g);
            Player* winner = (k % 2 == 1 ?
                                g.play(p1, p2, false) : g.play(p2, p1, false));
            if (winner == p1)
                nP1Wins++;
			else if (winner == p2)
				nP2Wins++;
			else if (winner == NULL)
				nNULL++;
            delete p1;
            delete p2;
        }
		cout << "====================================" << endl;
		cout << "Good vs. Awful" << endl;
        cout << "The good player won " << nP1Wins << " out of "
             << NTRIALS << " games." << endl;
		cout << "The awful player won " << nP2Wins << " out of "
             << NTRIALS << " games." << endl;
		cout << "There were " << NTRIALS - nP1Wins - nP2Wins - nNULL << " errors out of "
			 << NTRIALS << " games." << endl;

		nP1Wins = 0;
		nP2Wins = 0;
		nNULL = 0;

        for (int k = 1; k <= NTRIALS; k++)
        {
            cout << "============================= Game " << k
                 << " =============================" << endl;
            Game g(10, 10);
            addStandardShips(g);
            Player* p1 = createPlayer("mediocre", "Good Guy Greg", g);
            Player* p2 = createPlayer("awful", "Awful Aaron", g);
            Player* winner = (k % 2 == 1 ?
                                g.play(p1, p2, false) : g.play(p2, p1, false));
            if (winner == p1)
                nP1Wins++;
			else if (winner == p2)
				nP2Wins++;
			else if (winner == NULL)
				nNULL++;
            delete p1;
            delete p2;
        }
		cout << "====================================" << endl;
		cout << "Mediocre vs. Awful" << endl;
        cout << "The mediocre player won " << nP1Wins << " out of "
             << NTRIALS << " games." << endl;
		cout << "The awful player won " << nP2Wins << " out of "
             << NTRIALS << " games." << endl;
		cout << "There were " << NTRIALS - nP1Wins - nP2Wins - nNULL << " errors out of "
			 << NTRIALS << " games." << endl;

		nP1Wins = 0;
		nP2Wins = 0;
		nNULL = 0;

        for (int k = 1; k <= NTRIALS; k++)
        {
            cout << "============================= Game " << k
                 << " =============================" << endl;
            Game g(10, 10);
            addStandardShips(g);
            Player* p1 = createPlayer("mediocre", "Mediocre Malak", g);
            Player* p2 = createPlayer("mediocre", "Mediocre Melvin", g);
            Player* winner = (k % 2 == 1 ?
                                g.play(p1, p2, false) : g.play(p2, p1, false));
            if (winner == p1)
                nP1Wins++;
			else if (winner == p2)
				nP2Wins++;
			else if (winner == NULL)
				nNULL++;
            delete p1;
            delete p2;
        }
		cout << "====================================" << endl;
		cout << "Mediocre vs. Mediocre" << endl;
        cout << "The mediocre(1) player won " << nP1Wins << " out of "
             << NTRIALS << " games." << endl;
		cout << "The mediocre(2) player won " << nP2Wins << " out of "
             << NTRIALS << " games." << endl;
		cout << "There were " << NTRIALS - nP1Wins - nP2Wins - nNULL << " errors out of "
			 << NTRIALS << " games." << endl;

		nP1Wins = 0;
		nP2Wins = 0;
		nNULL = 0;

        for (int k = 1; k <= NTRIALS; k++)
        {
            //cout << "============================= Game " << k
            //     << " =============================" << endl;
            Game g(10, 10);
            addStandardShips(g);
            Player* p1 = createPlayer("awful", "Awful Aragorn", g);
            Player* p2 = createPlayer("awful", "Awful Aaron", g);
            Player* winner = (k % 2 == 1 ?
                                g.play(p1, p2, false) : g.play(p2, p1, false));
            if (winner == p1)
                nP1Wins++;
			else if (winner == p2)
				nP2Wins++;
			else if (winner == NULL)
				nNULL++;
            delete p1;
            delete p2;
        }
		cout << "====================================" << endl;
		cout << "Awful vs. Awful" << endl;
        cout << "The awful(1) player won " << nP1Wins << " out of "
             << NTRIALS << " games." << endl;
		cout << "The awful(2) player won " << nP2Wins << " out of "
             << NTRIALS << " games." << endl;
		cout << "There were " << NTRIALS - nP1Wins - nP2Wins - nNULL << " errors out of "
			 << NTRIALS << " games." << endl;

	}
    else
    {
       cout << "That's not one of the choices." << endl;
    }
}