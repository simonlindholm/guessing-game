
namespace GuessAPI {

struct Card {
	int rank, suit;
	int value() const {
		return rank*4 + suit;
	}
};

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
			Card drawn;
			int ind; // returned by placedCard
		} pdata;
	};
};

class Bot {
	public:
		virtual ~Bot() = 0;
		virtual const char* winPhrase() const = 0;
		virtual void start(int nplayers, Card hidden[], Card hand[]) = 0;
		virtual const char* name() const = 0;
		virtual Action move() = 0;
		virtual int placedCard(int byPlayer, Card card) = 0;
		virtual void madeMove(int player, const ActionResult& move) = 0;
};

}
