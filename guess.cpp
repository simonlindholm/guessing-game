#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include "api.h"
#include "make_bot.h"
using namespace std;
using namespace GuessAPI;

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

	for (int i = 0; i < nplayers; ++i) {
		Bot* b = bots[i];
		Card* hidden = new Card[3];
		Card* hand = new Card[3];
		b->start(nplayers, hidden, hand);
		delete[] hidden;
		delete[] hand;
	}

	return 0;
}
