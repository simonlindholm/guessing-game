#include <iostream>
#include <memory>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include "api.h"
#include "make_bot.h"
using namespace std;
using namespace GuessAPI;

struct BotState {
	vector<Card> hidden, hand;
	vector<bool> guessed;
	short left;
	bool lost;
};

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

	int deckPos = 0;
	vector<BotState> botState(nplayers);
	for (int i = 0; i < nplayers; ++i) {
		Bot* b = bots[i];
		BotState& bs = botState[i];
		bs.guessed.assign(3, false);
		bs.left = 3;
		bs.lost = false;
		for (int j = 0; j < 3; ++j) {
			bs.hidden.push_back(deck[deckPos++]);
			bs.hand.push_back(deck[deckPos++]);
		}
		trampoline(, b, start, (nplayers, i, &bs.hidden[0], &bs.hand[0]));
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
	int playersLeft = nplayers;
	for (;;) {
		try {
			Action move;
			trampoline(move =, bots[turn], move, ());
			ActionResult result;
			result.action = move;

			int targetInd = move.targetPlayer;
			Bot* target = bots[targetInd];
			BotState& turnState = botState[turn];
			BotState& targetState = botState[targetInd];

			if (targetState.lost)
				throw "Target player not playing.";

			bool again = false;
			if (!targetState.lost) {
				if (move.type == Action::TYPE_GUESS) {
					int ind = move.gdata.ind;
					Card guess = move.gdata.guess;
					if (!guess.isValid())
						throw "Invalid guess.";
					if (ind < 0 || ind >= 3)
						throw "Invalid guess index.";
					if (targetState.guessed[ind])
						throw "Card already guessed.";

					bool correct = (targetState.hand[ind] == guess);
					targetState.guessed[ind] = correct;
					result.gdata.correct = correct;
					again = correct;

					for (int i = 0; i < nplayers; ++i) {
						trampoline(, bots[i], madeMove, (turn, result));
					}

					if (correct && !--targetState.left) {
						targetState.lost = true;
						if (--playersLeft == 1) {
							// Game has ended!
							break;
						}
					}
				}
				else {
					Card given = move.pdata.given;
					if (!given.isValid())
						throw "Invalid played card.";

					// Clean the data passed between clients.
					result.action.gdata.ind = 0;
					result.action.gdata.guess = Card();
					result.action.pdata.given = given;

					auto it = std::find(turnState.hand.begin(), turnState.hand.end(), given);
					if (it == turnState.hand.end())
						throw "Played card not in hand.";

					turnState.hand.erase(it);
					shared_ptr<Card> drawn;
					if (deckPos < 52) {
						drawn = shared_ptr<Card>(new Card(deck[deckPos++]));
						turnState.hand.push_back(*drawn);
					}

					vector<int> validInd;
					for (int ind = 0; ind < 3; ++ind) {
						if (targetState.guessed[ind]) continue;
						Card c = targetState.hidden[ind];
						if (c.rank == given.rank || c.suit == given.suit)
							validInd.push_back(ind);
					}
					if (validInd.empty())
						validInd.push_back(-1);

					int chosenInd;
					trampoline(chosenInd = , target, placedCard, (turn, given));

					if (binary_search(validInd.begin(), validInd.end(), chosenInd)) {
						result.pdata.ind = chosenInd;
					}
					else {
						// Invalid index provided, use a random one.
						result.pdata.ind = validInd[random((int)validInd.size())];
					}

					for (int i = 0; i < nplayers; ++i) {
						result.pdata.drawn = (i == turn ? drawn.get() : 0);
						trampoline(, bots[i], madeMove, (turn, result));
					}
				}
			}

			if (!again) {
				++turn;
				turn %= nplayers;
			}
		}
		catch (const char* err) {
			// Invalid data
			++turn;
			turn %= nplayers;
		}
	}

	// Find the winner.
	int winner = -1;
	for (int i = 0; i < nplayers; ++i) {
		if (!botState[i].lost) {
			winner = i;
			break;
		}
	}
	cout << endl << "The winner is";
	cout << '.' << flush; usleep(400000);
	cout << '.' << flush; usleep(400000);
	cout << ". " << flush; usleep(1000000);
	const char* name;
	trampoline(name =, bots[winner], name, ());
	cout << name << "!" << endl;

	const char* winPhrase;
	trampoline(winPhrase =, bots[winner], winPhrase, ());
	cout << "\n\"" << winPhrase << "\"" << endl;

	return 0;
}
