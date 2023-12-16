// Project Identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

#include <vector>
#include <iostream>
#include <algorithm> // std::sort
#include <getopt.h>
#include <string> // needed for VS
#include <deque>
#include <string>
#include <queue>
#include "P2random.h"
using namespace std;

struct Player
{
	size_t numArrows = 0;
	size_t quiverCap = 0;
};

class Zombie
{
public:
	Zombie(string name_in, uint32_t dist_in, uint32_t speed_in, uint32_t health_in)
	{
		name = name_in;
		dist = dist_in;
		speed = speed_in;
		health = health_in;
		round = 1;
		eta = dist / speed;
		alive = true;
	}

	string name;
	uint32_t dist;
	uint32_t speed;
	uint32_t health;
	uint32_t eta;
	uint32_t round;
	bool alive;
};

uint32_t getETA(const Zombie *zom)
{
	return zom->dist / zom->speed;
}

class ZombieCompare
{
public:
	bool operator()(const Zombie *a, const Zombie *b)
	{
		if (getETA(a) == getETA(b))
		{
			if (a->health == b->health)
			{
				return a->name > b->name;
			}
			return a->health > b->health;
		}
		return getETA(a) > getETA(b);
	}
};

class MostActive
{
public:
	bool operator()(const Zombie *a, const Zombie *b) const
	{
		if (a->round == b->round)
		{
			return a->name > b->name;
		}
		return a->round < b->round;
	}
};

class LeastActive
{
public:
	bool operator()(const Zombie *a, const Zombie *b) const
	{

		if (a->round == b->round)
		{
			
				return a->name > b->name;
		
			
		}
		return a->round > b->round;
	}
};

class Game
{
public:
	void get_options(int argc, char **argv);
	void read_data();
	void medianLife(uint32_t rounds);
	void generateZombies(size_t numZombs);
	void generateRandos(size_t numGenZombs);
	void playGame(uint32_t round);
	void shoot(Player *player);
	void printStats();
	uint32_t getMed();
	void clear();
	
	Game()
	{
		quiver_cap = 0;
		verbose = false;
		stats = false;
		fileRound = 0;
		currRound = 1;
	}

private:
	priority_queue<Zombie *, vector<Zombie *>, ZombieCompare> myPQ;
	priority_queue<uint32_t, vector<uint32_t>, less<uint32_t>> maxHeap;	   // Max heap for lower half
	priority_queue<uint32_t, vector<uint32_t>, greater<uint32_t>> minHeap;
	priority_queue<Zombie*, vector<Zombie*>, MostActive> most;
    priority_queue<Zombie*, vector<Zombie*>, LeastActive> least;
	vector<Zombie *> destroyed;
	vector<Zombie *> zombies;
	size_t quiver_cap;
	uint32_t max_dist;
	uint32_t max_speed;
	uint32_t numEntries;
	uint32_t max_health;
	uint32_t seed;
	bool verbose = false;
	bool stats = false;
	bool median = false;
	bool killed = false;
	bool continuePlay = true;
	bool notEaten = true;
	string lastZomb;
	uint32_t fileRound;
	bool empty = false;
	uint32_t currRound;
};

// Read and process command line options.
void Game::get_options(int argc, char **argv)
{

	opterr = false;
	int option_index = 0;
	int choice;

	option longOpts[] = {{"statistics", required_argument, nullptr, 's'},
						 {"help", no_argument, nullptr, 'h'},
						 {"median", no_argument, nullptr, 'm'},
						 {"verbose", no_argument, nullptr, 'v'},
						 {nullptr, 0, nullptr, '\0'}};

	while ((choice = getopt_long(argc, argv, "s:hmv", longOpts, &option_index)) != -1)
	{
		switch (choice)
		{
		case 's':
		{
			int num = atoi(optarg);
			numEntries = static_cast<uint32_t>(num);
			stats = true;
			break;
		}
		case 'v':
			verbose = true;
			break;
		case 'm':
			median = true;
			break;
		case 'h':
			std::cout << "This program reads in a puzzle that contains symbols,\n"
					  << "that one can solve to get out of the maize\n"
					  << " It then outputs the solution of getting out\n"
					  << "in the command line arguments (the output option), which\n"
					  << "defaults to a map, solved by either a stack or queue,\n"
					  << "Usage: \'./zombb\n\t[--verbose | -v]\n"
					  << "\t[--median| -m]\n"
					  << "\t[--s | -statistics] <num>\n"
					  << "\t[--help | -h]\n"
					  << std::endl;
			exit(0);
		default:
			cerr << "Error: invalid option"
				 << "\n";
			exit(1);
		}
	}
}

int main(int argc, char **argv)
{
	ios_base::sync_with_stdio(false);

	Game game;
	// Read and process the command line options.
	game.get_options(argc, argv);

	game.read_data();

	// player attack zombies

	// if there are 0 zombies in whole file, you won battle
	return 0;
}

void Game::read_data()
{
	//auto begin = zombies.begin();
	//auto end = (numEntries < zombies.size()) ? zombies.begin() + numEntries: zombies.end();

	//priority_queue<Zombie*, vector<Zombie*>, MostActive> most(begin, end);
    //priority_queue<Zombie*, vector<Zombie*>, LeastActive> least(begin,end);
	string junk;

	getline(cin, junk);
	Player *player = new Player;
	cin >> junk >> quiver_cap;
	player->quiverCap = quiver_cap;
	cin >> junk >> seed;
	cin >> junk >> max_dist >> junk >> max_speed >> junk >> max_health;
	P2random::initialize(seed, max_dist, max_speed, max_health);
	getline(cin, junk);

	size_t numGenZombs = 0;
	size_t numZombs = 0;
	cin >> junk;

	while (continuePlay)
	{
		player->numArrows = player->quiverCap;
		if (verbose)
		{
			cout << "Round: " << currRound << "\n";
		}
		if (junk == "---")
		{
			cin >> junk;
			cin >> fileRound;
		}

		playGame(currRound);

		if (currRound == fileRound)
		{
			cin >> junk;
			cin >> numGenZombs;
			cin >> junk;
			cin >> numZombs;
			if(continuePlay)
			{
			generateRandos(numGenZombs);
			generateZombies(numZombs);
			}
			cin >> junk;
		}
		shoot(player);
		if (empty)
		{
			if (cin.eof())
			{
				continuePlay = false;
			}
		}
		if (median && !killed)
		{
			cout << "At the end of round " << currRound << ", the median zombie lifetime is " << getMed() << "\n";
		}

		currRound++;
	}
	if (!killed)
	{
		cout << "VICTORY IN ROUND " << currRound - 1 << "! " << destroyed[destroyed.size() - 1]->name << " was the last zombie.\n";
	}

	delete player;
	player = nullptr;
	if (stats)
	{
		printStats();
	}

	clear();
}

void Game::clear()
{
	for (auto z : zombies)
	{
		delete z;
		z = nullptr;
	}
	zombies.clear();
}
void Game::medianLife(uint32_t rounds)
{ // Min heap for upper half

		// Insert into the appropriate heap
		if (maxHeap.empty() || rounds <= maxHeap.top())
		{
			maxHeap.push(rounds);
		}
		else
		{
			minHeap.push(rounds);
		}

		// Balance the heaps
		if (maxHeap.size() > minHeap.size() + 1)
		{
			minHeap.push(maxHeap.top());
			maxHeap.pop();
		}
		else if (minHeap.size() > maxHeap.size())
		{
			maxHeap.push(minHeap.top());
			minHeap.pop();
		}
	
}

uint32_t Game::getMed()
{
	if (maxHeap.size() == minHeap.size())
	{
		return (maxHeap.top() + minHeap.top()) / 2;
	}
	else
	{
		return maxHeap.top();
	}
}


/*void Game::printStats()
{
	cout << "Zombies still active: " << myPQ.size() << "\n";
	cout << "First zombies killed:\n";

	size_t printCount = min(numEntries, static_cast<uint32_t>(destroyed.size()));
	size_t i = 0;

	for (const auto &zombie : destroyed)
	{
		cout << zombie->name << " " << i + 1 << "\n";
		++i;
		if (i >= printCount)
			break;
	}

	cout << "Last zombies killed:\n";

	auto rbegin = destroyed.rbegin(); // Reverse iterator pointing to the last element
	uint32_t j = min(numEntries, static_cast<uint32_t>(destroyed.size()));
	std::for_each(rbegin, rbegin + j, [&j](const Zombie *elem)
				  {
        cout << elem->name << " " << j << "\n";
		--j; });

	cout << "Most active zombies:\n";
	
	while(!most.empty())
	{
		cout << most.top()->name << " " << most.top()->round << "\n";
		most.pop();
	}
	cout << "Least active zombies:\n";
	while(!least.empty())
	{
		cout << least.top()->name << " " << least.top()->round << "\n";
		least.pop();
	}
}*/
void Game::printStats()
{
	cout << "Zombies still active: " << myPQ.size() << "\n";
	cout << "First zombies killed:\n";
	if (destroyed.size() < numEntries)
	{
		for (size_t i = 0; i < destroyed.size(); ++i)
		{
			cout << destroyed[i]->name << " " << i + 1 << "\n";
		}
	}
	else
	{
		for (size_t i = 0; i < numEntries; ++i)
		{
			cout << destroyed[i]->name << " " << i + 1 << "\n";
		}
	}
	cout << "Last zombies killed:\n";
	uint32_t count = numEntries;
	size_t count1 = destroyed.size() - 1;
	if (numEntries >= destroyed.size())
	{
		for (size_t i = count1 + 1; i-- > 0;)
		{
			cout << destroyed[i]->name << " " << count1 + 1 << "\n";
			count1--;
		}
	}
	else
	{
		for (size_t i = count1 + 1; i-- > count1 - numEntries + 1;)
		{
			cout << destroyed[i]->name << " " << count << "\n";
			count--;
		}
	}
	cout << "Most active zombies:\n";
	priority_queue<Zombie *, vector<Zombie *>, MostActive> most;
	for (size_t i = 0; i < zombies.size(); ++i)
	{
		most.push(zombies[i]);
	}
	size_t sizer = most.size();
	if (sizer < numEntries)
	{
		for (size_t i = 0; i < sizer; ++i)
		{
			cout << most.top()->name << " " << most.top()->round << "\n";
			most.pop();
		}
	}
	else
	{
		for (size_t i = 0; i < numEntries; ++i)
		{
			cout << most.top()->name << " " << most.top()->round << "\n";
			most.pop();
		}
	}
	cout << "Least active zombies:\n";
	priority_queue<Zombie *, vector<Zombie *>, LeastActive> least;
	for (size_t i = 0; i < zombies.size(); ++i)
	{
		least.push(zombies[i]);
	}
	sizer = least.size();
	if (sizer < numEntries)
	{

		for (size_t i = 0; i < sizer; ++i)
		{
			cout << least.top()->name << " " << least.top()->round << "\n";
			least.pop();
		}
	}
	else
	{
		for (size_t i = 0; i < numEntries; ++i)
		{
			cout << least.top()->name << " " << least.top()->round << "\n";
			least.pop();
		}
	}
}
void Game::generateZombies(size_t numZombs)
{
	if (numZombs > 0)
	{
		string junk;
		for (size_t i = 0; i < numZombs; ++i)
		{
			string name;
			uint32_t dist;
			uint32_t speed;
			uint32_t health;
			cin >> name >> junk >> dist >> junk >> speed >> junk >> health;
			Zombie *zombie = new Zombie{name, dist, speed, health};
			if (verbose)
			{
				// Created: paoletti0 (distance: 25, speed: 20, health: 1)
				cout << "Created: " << name << " (distance: " << dist << ", speed: "
					 << speed << ", health: " << health << ")\n";
			}
			zombies.push_back(zombie);
			myPQ.push(zombie);
			//most.push(zombie);
			//least.push(zombie);
			empty = false;
		}
	}
}
void Game::generateRandos(size_t numGenZombs)
{
	while (numGenZombs > 0)
	{
		string name = P2random::getNextZombieName();
		uint32_t distance = P2random::getNextZombieDistance();
		uint32_t speed = P2random::getNextZombieSpeed();
		uint32_t health = P2random::getNextZombieHealth();
		Zombie *zombie = new Zombie{name, distance, speed, health};
		if (verbose)
		{
			cout << "Created: " << name << " (distance: " << distance << ", speed: "
				 << speed << ", health: " << health << ")\n";
		}
		zombies.push_back(zombie);
		myPQ.push(zombie);
		//most.push(zombie);
		//least.push(zombie);
		empty = false;
		numGenZombs--;
	}
}

void Game::playGame(uint32_t round)
{
	size_t killer = 0;
	// remmeber info abt keeping info for next round
	if (zombies.size() == 0)
	{
		return;
	}
	for (size_t i = 0; i < zombies.size(); ++i)
	{
		zombies[i]->dist -= min(zombies[i]->dist, zombies[i]->speed);

		if (zombies[i]->alive && (zombies[i]->dist != 0 || zombies[i]->health > 0))
		{
			zombies[i]->round++;
			if (verbose)
			{
				cout << "Moved: " << zombies[i]->name << " (distance: " << zombies[i]->dist << ", speed: "
					 << zombies[i]->speed << ", health: " << zombies[i]->health << ")\n";
			}
		}

		if (zombies[i]->dist == 0 && zombies[i]->alive && notEaten)
		{
				killer = i;
				notEaten = false;

			killed = true;
		}
	}
	if (killed)
	{
		cout << "DEFEAT IN ROUND " << round << "! " << zombies[killer]->name << " ate your brains!\n";
		verbose = false;
		continuePlay = false;
	}
}

void Game::shoot(Player *player)
{
	if (zombies.empty())
	{
		return;
	}
	while (player->numArrows > 0 && continuePlay && !myPQ.empty())
	{
		// subract how many health from arrows so its not subracting by one
		if (player->numArrows < myPQ.top()->health)
		{
			myPQ.top()->health -= static_cast<uint32_t>(player->numArrows);
			player->numArrows = 0;
		}
		else
		{
			player->numArrows -= static_cast<size_t>(myPQ.top()->health);
			myPQ.top()->health = 0;
		}
		if (myPQ.top()->health == 0)
		{
			if (verbose)
			{
				cout << "Destroyed: " << myPQ.top()->name << " (distance: " << myPQ.top()->dist << ", speed: "
					 << myPQ.top()->speed << ", health: " << myPQ.top()->health << ")\n";
			}
			destroyed.push_back(myPQ.top());
			myPQ.top()->alive = false;
			if( median)
			{
				medianLife(myPQ.top()->round);
			}
			myPQ.pop();
		}
	}
	if (myPQ.empty())
	{
		empty = true;
	}
}
