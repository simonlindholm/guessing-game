#include <string>
#include "make_bot.h"

GuessAPI::Bot* make_bot(const char* name) {
	string s = name;
	if (s == "a") {
		return new ABot();
	}
	throw new blah;
}
