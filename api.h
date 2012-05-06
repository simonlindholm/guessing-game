#pragma once

namespace GuessAPI {

struct Card {
	int rank, suit;
	int value() const {
		return rank*4 + suit;
	}
	bool isValid() const {
		return 0 <= rank && rank < 13 &&
			0 <= suit && suit < 4;
	}
};

bool operator==(const Card& a, const Card& b) {
	return a.value() == b.value();
}
bool operator<(const Card& a, const Card& b) {
	return a.value() < b.value();
}

struct Action {
	enum {
		TYPE_GUESS,
		TYPE_PLAY
	} type;
	int targetPlayer;
	union {
		struct {
			int ind;
			Card guess;
		} gdata;
		struct {
			Card given;
		} pdata;
	};
};

struct ActionResult {
	Action action;
	union {
		struct {
			bool correct;
		} gdata;
		struct {
			Card* drawn; // null, or a temporary copy
			int ind; // returned by placedCard
		} pdata;
	};
};

class Bot {
	public:
		virtual ~Bot() = 0;
		virtual const char* winPhrase() const = 0;
		virtual void start(int nplayers, int turn, Card hidden[], Card hand[]) = 0; // arrays always contain current state
		virtual const char* name() const = 0;
		virtual Action move() = 0;
		virtual int placedCard(int byPlayer, Card card) = 0; // 0-3, or -1; invalid value = random
		virtual void madeMove(int player, const ActionResult& move) = 0;
};

int random(int to);

}
