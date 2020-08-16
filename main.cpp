#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <conio.h>
#include <fstream>
#include <map>
#include <tuple>

using namespace std;

#pragma pack(push, 1)	// Za da moje razmera na obektite da e po-m aluk
						// Kircho mi kaza za tova :)
class Boxer;

class Match
{
public:
	Match() : opponent(nullptr), won(false) {}
	Match(const Boxer & boxer, bool won) : opponent(&boxer), won(won) {}
	Match(const Match & match) : opponent(match.opponent), won(match.won) {}
	Match(const Match && match) : opponent(match.opponent), won(match.won) {}
	Match &operator=(const Match & match)
	{
		this->opponent = match.opponent;
		this->won = match.won;
	}
	Match &operator=(Match && match)
	{
		this->opponent = match.opponent;
		this->won = match.won;
		match.opponent = nullptr;
		return *this;
	}

	const Boxer &Opponent() const
	{
		return *this->opponent;
	}

	bool Won() const
	{
		return this->won;
	}

	friend void SwapMatchParty(Boxer &, const Boxer *, const Boxer *);

	friend vector<tuple<Match *, size_t, string, string>> ConvertArray(vector<Boxer> &);

	friend void RevertArray(vector<Boxer> &);

private:
	const Boxer * opponent;
	bool won;
};

class Boxer
{
public:
	Boxer(string name, string last_name) : name(new string(name)), last_name(new string(last_name)), matches(new Match[1]), matches_count(0) {}
	Boxer(Boxer && boxer) : name(boxer.name), last_name(boxer.last_name), matches(boxer.matches), matches_count(boxer.matches_count)
	{
		boxer.name = nullptr;
		boxer.last_name = nullptr;
		boxer.matches = nullptr;
	}
	~Boxer()
	{
		if (this->matches)
		{
			delete[] this->matches;
			this->matches = nullptr;
		}
		if (this->name)
		{
			delete this->name;
			this->name = nullptr;
		}
		if (this->last_name)
		{
			delete this->last_name;
			this->last_name = nullptr;
		}
	}

	Boxer &operator=(const Boxer & boxer)
	{
		delete this->name;
		delete this->last_name;
		this->name = new string(*boxer.name);
		this->last_name = new string(*boxer.last_name);
		this->matches = (Match *) realloc(this->matches, boxer.matches_count * sizeof(Match));
		this->matches_count = boxer.matches_count;
		memcpy_s(this->matches, this->matches_count * sizeof(Match), boxer.matches, this->matches_count * sizeof(Match));
		return *this;
	}

	Boxer &operator=(Boxer && boxer)
	{
		this->name = boxer.name;
		this->last_name = boxer.last_name;
		this->matches = boxer.matches;
		this->matches_count = boxer.matches_count;
		boxer.name = nullptr;
		boxer.last_name = nullptr;
		boxer.matches = nullptr;
		return *this;
	}

	bool NewMatch(const Boxer & boxer, bool won)
	{
		if (this == &boxer)
		{
			return false;
		}
		this->matches = (Match *) realloc(this->matches, (++this->matches_count) * sizeof(Match));
		this->matches[this->matches_count - 1] = Match(boxer, won);
		return true;
	}

	string GetName() const
	{
		return *this->name;
	}

	string GetLastName() const
	{
		return *this->last_name;
	}

	vector<Match> GetMatches() const
	{
		vector<Match> _matches;
		for (size_t z = 0; z < matches_count; z++)
		{
			_matches.push_back(this->matches[z]);
		}
		return _matches;
	}

	friend void SwapMatchParty(Boxer &, const Boxer *, const Boxer *);

	friend vector<tuple<Match *, size_t, string, string>> ConvertArray(vector<Boxer> &);

	friend void RevertMatch(Boxer &, Match);

	friend void RevertArray(vector<Boxer> &);

private:
	string *name, *last_name;
	Match *matches;
	size_t matches_count;
};
#pragma pack(pop) // Za da se vurne kum normalno

void SwapMatchParty(Boxer & boxer, const Boxer * first, const Boxer * second)
{
	for (size_t z = 0; z < boxer.matches_count; z++)
	{
		if (boxer.matches[z].opponent == first)
		{
			boxer.matches[z].opponent = second;
		}
		else if (boxer.matches[z].opponent == second)
		{
			boxer.matches[z].opponent = first;
		}
	}
}

vector<tuple<Match *, size_t, string, string>> ConvertArray(vector<Boxer> & array)
{
	vector<tuple<Match *, size_t, string, string>> data;
	map<Boxer *, size_t> boxers_enum;
	for (vector<Boxer>::iterator z = array.begin(); z < array.end(); z++)
	{
		boxers_enum[&(*z)] = boxers_enum.size();
		data.push_back(tuple<Match *, size_t, string, string>(z->matches, z->matches_count, z->GetName(), z->GetLastName()));
	}
	for (vector<Boxer>::iterator z = array.begin(); z < array.end(); z++)
	{
		z->name = nullptr;
		z->last_name = nullptr;
		for (size_t z1 = 0; z1 < z->matches_count; z1++)
		{
			z->matches[z1].opponent = (const Boxer *) boxers_enum[const_cast<Boxer *>(z->matches[z1].opponent)];
		}
		z->matches = nullptr;
	}
	return data;
}

void RevertMatch(Boxer & boxer, Match match)
{
	boxer.matches = (Match *) realloc(boxer.matches, (++boxer.matches_count) * sizeof(Match));
	memcpy_s(&boxer.matches[boxer.matches_count - 1], sizeof(Match), &match, sizeof(Match));
}

void RevertArray(vector<Boxer> & array)
{
	for (vector<Boxer>::iterator z = array.begin(); z < array.end(); z++)
	{
		for (size_t z1 = 0; z1 < z->matches_count; z1++)
		{
			z->matches[z1].opponent = &array.at((size_t) z->matches[z1].opponent);
		}
	}
}

int main()
{
	cout << "Will you enter new data about boxers? (Y / N)\n";
	char ch;
	do
	{
		ch = _getch();
	} while (ch != 'y' && ch != 'n' && ch != 'Y' && ch != 'N');
	if (ch == 'y' || ch == 'Y')
	{
		vector<Boxer> boxers; // Prakticheski e otzad e masiv, no s "obvivka" :)
		do
		{
			cout << "Enter boxer's name: ";
			string name = "";
			getline(cin, name);
			cout << "Enter boxer's last name: ";
			string last_name = "";
			getline(cin, last_name);
			boxers.push_back(Boxer(name, last_name));
			cout << "Are you going to enter data for another boxer? (Y / N)\n";
			do
			{
				ch = _getch();
			} while (ch != 'y' && ch != 'n' && ch != 'Y' && ch != 'N');
		} while (ch == 'y' || ch == 'Y');
		for (vector<Boxer>::iterator z = boxers.begin(); z < boxers.end() - 1; z++)
		{
			for (vector<Boxer>::iterator z1 = z + 1; z1 < boxers.end(); z1++)
			{
				cout << "(1) " << z->GetName() << ' ' << z->GetLastName() << " VS (2) " << z1->GetName() << ' ' << z1->GetLastName() << "\nWho won?\n";
				do
				{
					ch = _getch();
				} while (ch != '1' && ch != '2');
				z->NewMatch(*z1, ch == '1');
				z1->NewMatch(*z, ch == '2');
			}
		}
		cout << "Do you want to save new boxers in file? Warning: This will overwrite old data. (Y / N)\n";
		do
		{
			ch = _getch();
		} while (ch != 'y' && ch != 'n' && ch != 'Y' && ch != 'N');
		if (ch == 'y' || ch == 'Y')
		{
			fstream file(
#if defined _DEBUG
				"C:\\Users\\Kiril\\source\\repos\\Testing Project C++\\Release\\boxers.dat"
#else
				"boxers.dat"
#endif
				, ios::out | ios::trunc | ios::binary);
			if (!file.is_open())
			{
				cout << "Failed to open file!\n";
				return -1;
			}
			vector<tuple<Match *, size_t, string, string>> converted = ConvertArray(boxers);
			enum class LeadByte
			{
				BOXER,
				MATCH,
			} lead_byte;
			for (vector<Boxer>::iterator z = boxers.begin(); z < boxers.end(); z++)
			{
				lead_byte = LeadByte::BOXER;
				file.write((char *) &lead_byte, sizeof(LeadByte));
				file.write(converted.front()._Get_rest()._Get_rest()._Myfirst._Val.c_str(), converted.front()._Get_rest()._Get_rest()._Myfirst._Val.size() + 1);
				file.write(converted.front()._Get_rest()._Get_rest()._Get_rest()._Myfirst._Val.c_str(), converted.front()._Get_rest()._Get_rest()._Get_rest()._Myfirst._Val.size() + 1);
				lead_byte = LeadByte::MATCH;
				size_t matches_count = converted.front()._Get_rest()._Myfirst._Val;
				for (size_t z1 = 0; z1 < matches_count; z1++)
				{
					file.write((char *) &lead_byte, sizeof(LeadByte));
					file.write((char *) &converted.front()._Myfirst._Val[z1], sizeof(Match));
				}
				delete[] converted.front()._Myfirst._Val;
				converted.erase(converted.begin());
			}
		}
	}
	fstream file(
#if defined _DEBUG
		"C:\\Users\\Kiril\\source\\repos\\Testing Project C++\\Release\\boxers.dat"
#else
		"boxers.dat"
#endif
		, ios::in | ios::binary);
	if (!file.is_open())
	{
		cout << "Failed to open file!\n";
		return -1;
	}
	vector<Boxer> boxers;
	while (!file.eof())
	{
		enum class LeadByte
		{
			BOXER,
			MATCH,
		} lead_byte;
		file.read((char *) &lead_byte, sizeof(LeadByte));
		if (file.eof())
		{
			break;
		}
		switch (lead_byte)
		{
		case LeadByte::BOXER:
			{
				string name = "", last_name = "";
				file.read(&ch, sizeof(char));
				while (ch)
				{
					name += ch;
					file.read(&ch, sizeof(char));
				}
				file.read(&ch, sizeof(char));
				while (ch)
				{
					last_name += ch;
					file.read(&ch, sizeof(char));
				}
				boxers.push_back(Boxer(name, last_name));
			}
			break;
		case LeadByte::MATCH:
			{
				Match match;
				file.read((char *) &match, sizeof(Match));
				RevertMatch(boxers.back(), match);
			}
			break;
		default:
			cout << "Error! Invalid leading byte!\n";
			return -1;
			break;
		}
	}
	file.close();
	RevertArray(boxers);
	size_t longest = 0, second_longest = 0;
	{
		map<Boxer *, double> win_lost_ratio;
		for (vector<Boxer>::iterator z = boxers.begin(); z < boxers.end(); z++)
		{
			if (z->GetName().size() + z->GetLastName().size() + 1 > longest)
			{
				second_longest = longest;
				longest = z->GetName().size() + z->GetLastName().size() + 1;
			}
			else if (z->GetName().size() + z->GetLastName().size() + 1 > second_longest)
			{
				second_longest = z->GetName().size() + z->GetLastName().size() + 1;
			}
			size_t won = 1, lost = 1;
			vector<Match> matches = z->GetMatches();
			for (vector<Match>::iterator z1 = matches.begin(); z1 < matches.end(); z1++)
			{
				z1->Won() ? ++won : ++lost;
			}
			win_lost_ratio[&(*z)] = (double) won / (double) lost;
		}
		for (vector<Boxer>::iterator z = boxers.begin(); z < boxers.end() - 1; z++)
		{
			for (vector<Boxer>::iterator z1 = z; z1 < boxers.end(); z1++)
			{
				for (vector<Boxer>::iterator z2 = boxers.begin(); z2 < boxers.end(); z2++)
				{
					SwapMatchParty(*z2, &(*z), &(*z1));
				}
				if (win_lost_ratio[&(*z)] < win_lost_ratio[&(*z1)])
				{
					swap(win_lost_ratio[&(*z)], win_lost_ratio[&(*z1)]);
					swap(*z, *z1);
				}
			}
		}
	}
	if ("Boxer"s.size() > longest)
	{
		longest = "Boxer"s.size();
	}
	if ("Match VS."s.size() > second_longest)
	{
		second_longest = "Match VS."s.size();
	}
	cout	<< (char) 0xDA << string(longest, (char) 0xC4) << (char) 0xC2 << string(second_longest, (char) 0xC4) << (char) 0xC2 << string("Won?"s.size(), (char) 0xC4) << (char) 0xBF << '\n'
			<< (char) 0xB3 << "Boxer" << string(longest - "Boxer"s.size(), ' ') << (char) 0xB3 << "Match VS." << string(second_longest - "Match VS."s.size(), ' ') << (char) 0xB3 << "Won?" << (char) 0xB3 << '\n'
			<< (char) 0xC3 << string(longest, (char) 0xC4) << (char) 0xC5 << string(second_longest, (char) 0xC4) << (char) 0xC5 << string("Won?"s.size(), (char) 0xC4) << (char) 0xB4 << '\n';
	for (vector<Boxer>::iterator z = boxers.begin(); z < boxers.end(); z++)
	{
		vector<Match> matches = z->GetMatches();
		for (vector<Match>::iterator z1 = matches.begin(); z1 < matches.end(); z1++)
		{
			cout << (char) 0xB3;
			if (z1 == matches.begin())
			{
				cout << z->GetName() << ' ' << z->GetLastName() << string(longest - (z->GetName().size() + z->GetLastName().size() + 1), ' ');
			}
			else
			{
				cout << string(longest, ' ');
			}
			cout << (char) 0xB3;
			cout << z1->Opponent().GetName() << ' ' << z1->Opponent().GetLastName() << string(second_longest - (z->GetName().size() + z->GetLastName().size() + 1), ' ');
			cout << (char) 0xB3 << (z1->Won() ? "Yes " : "No  ") << (char) 0xB3 << '\n';
		}
		cout << (z == --boxers.end() ? (char) 0xC0 : (char) 0xC3) << string(longest, (char) 0xC4) << (z == --boxers.end() ? (char) 0xC1 : (char) 0xC5) << string(second_longest, (char) 0xC4) << (z == --boxers.end() ? (char) 0xC1 : (char) 0xC5) << string("Won?"s.size(), (char) 0xC4) << (z == --boxers.end() ? (char) 0xD9 : (char) 0xB4) << '\n';
	}
	_getch();
	return 0;
}
