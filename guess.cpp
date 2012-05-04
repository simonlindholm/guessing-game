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
	char* m = (char*)alloca(1000);
	for (int i = 0; i < 1000; ++i)
		m[i] = 0;
}

#define trampoline(bot, func, args) \
	{ \
		char buf[random(400)*8]; \
		clearStack(); \
		bot->func args ; \
	}

int main(int argc, char** argv) {
	if (argc < 3) {
		cout << "Need some bots as arguments.";
		return 1;
	}
	vector<Bot*> bots;
	for (int i = 1; i < argc; ++i) {
		bots.push_back(make_bot(argv[i]));
	}

	int nplayers = (int)bots.size();
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

	for (int i = 0; i < nplayers; ++i) {
		Bot* b = bots[i];
		Card* hidden = new Card[3];
		Card* hand = new Card[3];
		trampoline(b, start, (nplayers, hidden, hand));
		delete[] hidden;
		delete[] hand;
	}

	return 0;
}
