#include <bits/stdc++.h>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>

using namespace std;

int main(){
	std::stringstream ostream;

	{//Serialization
		map<string, pair<string, int> > local;

		local["abc"] = make_pair("1.txt", 3);
		local["def"] = make_pair("2.txt", 4);
		local["ghi"] = make_pair("3.txt", 5);

		// cereal::JSONOutputArchive oarchive(ostream);
		cereal::BinaryOutputArchive oarchive(ostream);

		oarchive( local );
	}

	string ostring = ostream.str(); //Transmit this string

	{//Deserialization
		std::stringstream istream(ostring);

		// cereal::JSONInputArchive iarchive(ss);
		cereal::BinaryInputArchive iarchive(istream);

		map<string, pair<string, int> > new_data;

		iarchive(new_data);

		// cout << new_data["def"].first << endl;
	}


}

