
#include <algorithm>
#include <cctype>
#include <locale>
#include <fstream>
#include <map>
#include <iostream>
#include <cstring>

#include "read.hpp"

using namespace std;

typedef map<string, read_handle_function> read_handler_map;
static read_handler_map handlers;

// trim from start
static inline
string& ltrim(string& s)
{
	s.erase(s.begin(), find_if(s.begin(), s.end(),
			not1(ptr_fun<int, int>(std::isspace))));
	return s;
}

// trim from end
static inline
string& rtrim(string& s)
{
	s.erase(find_if(s.rbegin(), s.rend(),
		not1(ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

static inline
string&
trim(string& s)
{
	return ltrim(rtrim(s));
}

vector<string>
split(const string& str, const string& sep)
{
	vector<string> ret;
	char *buf((char*)str.c_str());
	char *pch(strtok(buf, sep.c_str()));

	while(pch != NULL) {
		ret.push_back(string(pch));
		pch = strtok(NULL, sep.c_str());
	}

	return ret;
}

static inline
string
parse_fact_name(const string& name)
{
	if(name[0] == '!') {
		string ret(name);
		ret.erase(0, 1);
		return ret;
	}

	return name;
}

static inline
fact_arguments
parse_fact_arguments(const string& args)
{
	fact_arguments vec(split(args.c_str(), ","));

	for(size_t i(0); i < vec.size(); ++i)
		trim(vec[i]);

	return vec;
}

void
add_read_handler(const string& fact, read_handle_function fn)
{
	handlers[fact] = fn;
}

void
read_file(const string& file)
{
	ifstream fp(file.c_str());
	string line;

	while(!fp.eof()) {
		getline(fp, line);

		vector<string> vec(split(line, ")"));

		for(size_t i(0); i < vec.size(); ++i) {
			string fact(trim(vec[i]));

			if(std::isalpha(fact[0]) || fact[0] == '!') {
				vector<string> vec2(split(fact, "("));

				if(vec2.size() == 2) {
					string fact_name(parse_fact_name(trim(vec2[0])));
					read_handler_map::const_iterator it(handlers.find(fact_name));

					if(it != handlers.end()) {
						read_handle_function fn(it->second);

						fn(parse_fact_arguments(trim(vec2[1])));
					}

#if 0
					cout << fact_name << "(";
					for(size_t j(0); j < fact_args.size(); ++j) {
						cout << fact_args[j] << ", ";
					}

					cout << ")" << endl;
#endif
				}
			}
		}
	}

}

node_t
read_node(const fact_argument& _s)
{
	string s(_s);

	s.erase(0, 1);

	return (node_t)atoi(s.c_str());
}

int
read_int(const fact_argument& s)
{
	return (int)atoi(s.c_str());
}
