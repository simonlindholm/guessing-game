#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include "api.h"
#include "make_bot.h"
using namespace std;
using namespace GuessAPI;

int GuessAPI::random(int to) {
	return static_cast<int>(rand() / (RAND_MAX + 1.0) * to);
}

void clearStack() {
	int sz = 400*8 + 1000;
	char* m = (char*)alloca(sz);
	for (int i = 0; i < sz; ++i)
		m[i] = rand()%256;
}

#define trampoline(retexpr, bot, func, args) \
	do { \
		clearStack(); \
		{ \
			volatile char buf[random(400)*8]; \
			retexpr bot->func args ; \
		} \
	} while(0)

int main(int argc, char** argv) {
	int nplayers = argc-1;
	if (nplayers < 2) {
		cout << "Need some bots as arguments.";
		return 1;
	}
	if (nplayers > 52/6) {
		cout << "Too many bots.";
		return 2;
	}
	vector<Bot*> bots;
	for (int i = 1; i < argc; ++i) {
		bots.push_back(make_bot(argv[i]));
	}

	vector<Card> deck;
	for (int r = 0; r < 13; ++r) {
		for (int s = 0; s < 4; ++s) {
			Card c;
			c.rank = r;
			c.suit = s;
			deck.push_back(c);
		}
	}
	random_shuffle(deck.begin(), deck.end(), GuessAPI::random);

	int used = 0;
	for (int i = 0; i < nplayers; ++i) {
		Bot* b = bots[i];
		Card* hidden = new Card[3];
		Card* hand = new Card[3];
		for (int j = 0; j < 3; ++j) {
			hidden[j] = deck[used++];
			hand[j] = deck[used++];
		}
		trampoline(, b, start, (nplayers, i, hidden, hand));
		delete[] hidden;
		delete[] hand;
	}

	cout << "Players: \n";
	for (int i = 0; i < nplayers; ++i) {
		const char* name;
		trampoline(name =, bots[i], name, ());
		cout << " * " << name << endl;
	}
	cout << endl;

	usleep(1000000);
	cout << "START" << endl;

	int turn = 0;
	for (;;) {
		if (blah) {
			++turn;
			turn %= nplayers;
		}
	}

	return 0;
}
