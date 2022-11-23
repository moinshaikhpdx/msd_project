#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
using namespace std;


class cache_simulator
{
	
	struct {
		short int PLRU; ///////////// LSB being the start of the root ////////// {d,e,f,g,b,c,a}////////////////// 
		int line [8]; //////////// Tag array//////// {Messi bits[1:0],tagbits} ////////////// 
	} cache[32768];


	public:

	void update_state(unsigned int state, unsigned int set, unsigned int way);	
	int check_state(unsigned int state, unsigned int set, unsigned int way);

    //util
    unsigned int HexToDec(string inAddr);
};
unsigned int cache_simulator::HexToDec(string inAddr){                                      // Function For Hex To Unsigned Dec Address Converstion
	//cout<<"[INFO]The input Hex Address is :"<<inAddr<<endl;
	 int len;
	 unsigned int dec = 0;
	 int base = 1;
	int i;

	len = inAddr.size();
	i = len-1;
	if(!(len <= 8))
		cout<<"[WARNING]: HexToDec Function -> Address Out Of Range "<<endl;
	else{

	   // cout<<"value of size is :"<<len<<endl;
        if(len < 8) {
            short int diff_len;
            string s = "0";
            diff_len = 8 - len;
            for (int i = 0; i < diff_len; i++) {
                s += inAddr;
            }
            //cout<<"[INFO]: The Corrected Hex Addr is -> "<<inAddr<<endl;
        }
	   do
	   {
	      //i = len;
	        if (inAddr[i] >= '0' && inAddr[i] <= '9') {

                 dec += (int(inAddr[i]) - 48) * base;

                 base = base * 16;

                }
			else if (inAddr[i] >= 'A' && inAddr[i] <= 'F') {
                 //cout<<"BASE is "<<base<<endl;
                 dec += (int(inAddr[i]) - 55) * base;

                 base = base * 16;}
            else if(inAddr[i] >= 'a' && inAddr[i] <= 'f')
            {
               // cout<<"BASE is "<<base<<endl;
                dec += (int(inAddr[i]) - 87) * base;
                base = base*16;
            }
                	i--;
            //cout<<"[DEC]   value     is -->"<<dec<<endl;
	    }while(i>=0);
	}
  return dec;
}
void cache_simulator::update_state(unsigned int state, unsigned int set, unsigned int way)
{
      cache[set].line[way] = (cache[set].line[way] & ~(3<<11)) | (state<<11);

}

int cache_simulator::check_state(unsigned int state, unsigned int set, unsigned int way)
{
	return (cache[set].line[way] >> 11);
}

int main(int argc, char* argv[])
{

   cache_simulator cache_sim;


   if(argc == 1)
   {
	cout<<"Please provide trace path and mode";
   }
   else
   {
     string path = argv[1];
     string mode = argv[2];

 

     cout <<"\n\n\n Execution mode = "<<mode<<"\n\n\n";

     ifstream my_file(path);
     if(my_file.is_open())
     {
	 string line;
	 while(getline(my_file, line))
              {
                  istringstream iss(line);
                  string s;
		  int command;
		  string address;
                  int string_count = 0;
                  while (getline(iss, s, ' '))
                     {
                      if(string_count == 0)
                         {
		            command = stoi(s);
                            string_count = 1;
                         }
                      else
                         {
			   address = s;
                         }
                     }
		      cout<<"Command = "<<command<<" Address = "<<address<<"\n";
              cout<<cache_sim.HexToDec(address)<<endl;
		      switch (command) {
			      case 0: break;
			      case 2: break;
			      case 3: break;
			      case 4: break;
			      case 5: break;
			      case 6: break;
			      case 7: break;
			      case 8: break;
			      case 9: break;
			}
               }
     }
     else
    {
   	cout<<"Unable to open any file";
    }

   }
   return 0;
  
}
