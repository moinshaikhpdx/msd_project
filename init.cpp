#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

int main(int argc, char* argv[])
{
	string line;
//	ifstream my_file("exec.trace");
 	string path = argv[1];
	cout<<argv[2];
	ifstream my_file(path);
	if(my_file.is_open())
	{
		while(getline(my_file, line))
		{
	       //		cout<<line[0];
			istringstream iss(line);
			string s;
			int string_count = 0;
			while (getline(iss, s, ' '))
			{
				if(string_count == 0)
				{
					cout<<"Command : "<<s<<"\n";
					string_count = 1;
				}
				else
				{
					cout<<"Address : "<<s<<"\n";
				}

			}
				cin.ignore();
				cout<<"\n";
		}
		my_file.close();
	}
	return 0;
}
