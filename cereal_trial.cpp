#include <bits/stdc++.h>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>

using namespace std;

class Stuff
{
  public:
  	Stuff(){

  	}

	Stuff(map<string, pair<string, int> > data){
	  this->data = data;
	}

	map<string, pair<string, int> > getData() {
		return this->data;
	}

  private:
	std::map<std::string, std::pair<std::string, int> > data;

	friend class cereal::access;

	template <class Archive>
	void serialize( Archive & ar )
	{
	  ar( data );
	}
};

int main(){
	std::stringstream ss;

	{
		map<string, pair<string, int> > local;

		local["abc"] = make_pair("1.txt", 3);
		local["def"] = make_pair("2.txt", 4);
		local["ghi"] = make_pair("3.txt", 5);

		cereal::BinaryOutputArchive oarchive(ss);

		Stuff myStuff(local);

		oarchive( myStuff );

		string ostring = ss.str();
	}

	cereal::BinaryInputArchive iarchive(ss);

	Stuff reconstructed;

	iarchive(reconstructed);

	map<string, pair<string, int> > new_data = reconstructed.getData();

	cout << new_data["def"].second << endl;

  	return 0;
}
