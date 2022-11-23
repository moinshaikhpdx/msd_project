#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

/* Bus Operation types */
#define READ 1 /* Bus Read */
#define WRITE 2 /* Bus Write */
#define INVALIDATE 3 /* Bus Invalidate */
#define RWIM 4 /* Bus Read With Intent to Modify */
/* Snoop Result types */
#define NOHIT 0 /* No hit */
#define HIT 1 /* Hit */
#define HITM 2 /* Hit to modified line */
/* L2 to L1 message types */
#define GETLINE 1 /* Request data for modified line in L1 */
#define SENDLINE 2 /* Send requested cache line to L1 */
#define INVALIDATELINE 3 /* Invalidate a line in L1 */
#define EVICTLINE 4 /* Evict a line from L1 */
// this is when L2's replacement policy causes eviction of a line that
// may be present in L1. It could be done by a combination of GETLINE
// (if the line is potentially modified in L1) and INVALIDATELINE.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Used to represent states in MESI protocol */
#define invalid 0 
#define exclusive 1  
#define shared 2 
#define modified 3 
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#define address_length 32 /// addres length
///////////////////////////////////////////////////////////////////////////////////

struct {
		short int PLRU; ///////////// LSB being the start of the root ////////// {d,e,f,g,b,c,a}////////////////// 
		int line [8]; //////////// Tag array//////// {Messi bits[1:0],tagbits} ////////////// 
	} cache[32768];
	
using namespace std;

class cache_simulator
{
	
unsigned int HexToDec(string inAddr){                                      // Function For Hex To Unsigned Dec Address Converstion
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

	    cout<<"value of size is :"<<len<<endl;
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

//	cache *data_structre;
//	int *operation;
//	int *addr;
//	int size;
//	cache_simulator(int init_size)
//	{
//		data_structure = new cache[init_size];	
//	}
//	void read_trace(short int *operation, string *addr, string PATH);// seperate instruction and operation


  	void reset_cache();
	void print_valid_cache();

///// Mark faust's functions 
	int GetSnoopResult(unsigned int Address);
    void BusOperation(int BusOp, unsigned int Address, int SnoopResult);
    void PutSnoopResult(unsigned int Address, int SnoopResult);
    void MessageToCache(int Message, unsigned int Address) ;
    
////////////// Sandeep's functions ///////////////////
     void updatePLRU(unsigned int set,unsigned int way);
     int getLRU(unsigned int set);
     int bitExtracted(unsigned int addr, int k, int p);            ///////////// used to select tag or index or byte offset from addr////////////https://www.geeksforgeeks.org/extract-k-bits-given-position-number/
 void write _cache( unsigned int addr);
////////////////Moins's functions /////////////////////
///   void update_state( unsigned int state, unsigned int set,  unsigned int way);
///   int check_state( unsigned int set,  unsigned int way);
///        void snoop_update (unsigned int addr);

///////////////////jask's functions ////////////////////////
  void read_cache( unsigned int addr);
  int check_hit(unsigned int set,unsigned int tag);
  int get_addr(int tag,int set);
};

int cache_simulator::check_hit(unsigned int set, unsigned int tag){
  	int way_temp;;
	 for(int i = 0; i < 8 ; i++)
	 {
		 if(tag == bitExtracted(cache[set].line[i],11,1)
			 way_temp = i;
		 else
	         way_temp = 8;
	 }
	 return way_temp;
}

int cache_simulator::get_addr(int tag,int set){
	
}

void cache_simulator::read_cache(unsigned int addr){
	int tag_temp,temp_index;
	short int way_temp,current_state;
	tag_temp = bitExtracted(addr,11,22);
	set_temp = bitExtracted(addr,15,7);
	way_temp = check_hit(set_temp,tag_temp);
	current_state = check_state(set_temp,way_temp);
	if(way_temp == 8)
	{// miss occured
		cout<<"[INFO] CACHE MISS"<<endl;
	     if(check_for_empty_way(set_temp) == 8)
		 { ///replace line                               
			
			 if(check_state(set_temp,getLRU(set_temp)) == modified){
				 MessageToCache(GETLINE,get_addr(set_temp,getLRU(set_temp)));
				 BusOperation(WRITE,get_addr(set_temp,getLRU(set_temp)),GetSnoopResult(get_addr(set_temp,getLRU(set_temp))));              
				 MessageToCache(INVALIDATE,get_addr(set_temp,getLRU(set_temp)));
				 BusOperation(READ,addr,GetSnoopResult(addr));
				 MessageToCache(SENDLINE,get_addr(set_temp,getLRU(set_temp)));
			 }
			 else
			 {// empty way found
			 BusOperation(READ,addr,GetSnoopResult(addr));
			 MessageToCache(SENDLINE,get_addr(set_temp,getLRU(set_temp)));
			
			 }
			  cache[set_temp].line[getLRU(set_temp)] = tag_temp;
			  if(GetSnoopResult(addr) == NOHIT)
			  update_state(exclusive,set_temp,getLRU(set_temp));
			  else if(GetSnoopResult(addr) == HIT)
		      update_state(shared,set_temp,getLRU(set_temp));  
			  updatePLRU(set_temp,way_temp);
				 
	}
	else{////way hit
		cout<<"[INFO] CACHE HIT "<<endl;
		update_state(current_state,set_temp,way_temp);
		updatePLRU(set_temp,way_temp);
		MessageToCache(SENDLINE,addr);
	}
}



void cache_simulator::updatePLRU(unsigned int set,unsigned int way) {
	switch (way)
{
case 0: cache[set].PLRU= (cache[set].PLRU | 0x45); break;
case 1: cache[set].PLRU= (cache[set].PLRU & 0xbf)| 0x05;break;
case 2: cache[set].PLRU= (cache[set].PLRU & 0xfb)| 0x21;break;
case 3: cache[set].PLRU= (cache[set].PLRU & 0xdb)| 0x01;break;
case 4: cache[set].PLRU= (cache[set].PLRU & 0xfe)| 0x12;break;
case 5: cache[set].PLRU= (cache[set].PLRU & 0xee)| 0x02;break;
case 6: cache[set].PLRU= (cache[set].PLRU & 0x0c)| 0x08;break;
case 7: cache[set].PLRU= (cache[set].PLRU & 0x04);break;	
	}	
} 

int cache_simulator::getLRU(unsigned int set)
{
	if(!(cache[set].PLRU & 0x01)) ////////////!root a
	{
     if(!(cache[set].PLRU & 0x04))/////////////! root b 
     {
      	if(!(cache[set].PLRU & 0x40))/////////////! root d 
      	return(0);
      	else if (cache[set].PLRU & 0x40)/////////////// root d
      	return(1);
	 }
	 else if(cache[set].PLRU & 0x04)/////////////root b  
     {
     	if(!(cache[set].PLRU & 0x20))/////////////! root e
      	return(2);
      	else if (cache[set].PLRU & 0x20)/////////////// root e
      	return(3);
	 }
	}
	 ///////////////////////////half //////////////////////////////////////
	else if (cache[set].PLRU & 0x01)//////////root a 
	{
	 if(!(cache[set].PLRU & 0x02))/////////////! root c
     {
      	if(!(cache[set].PLRU & 0x10))/////////////! root f
      	return(4);
      	else if (cache[set].PLRU & 0x10)/////////////// root f
      	return(5);
	 }
	 else if(cache[set].PLRU & 0x02)/////////////root c 
     {
     	if(!(cache[set].PLRU & 0x08))/////////////! root g
      	return(6);
      	else if (cache[set].PLRU & 0x08)/////////////// root g
      	return(7);
	 }
	
}
}

/////////extracts bits // https://www.geeksforgeeks.org/extract-k-bits-given-position-number/
int cache_simulator::bitExtracted (unsigned int addr, int k, int p) 
{
    return (((1 << k) - 1) & (number >> (p - 1)));
}


/* Simulate the reporting of snoop results by other caches */
int cache_simulator::GetSnoopResult(unsigned int Address) {
	/* returns HIT, NOHIT, or HITM */
	return(0);
}


/*
Used to simulate a bus operation and to capture the snoop results of last level
caches of other processors
*/
void cache_simulator::BusOperation (int BusOp, unsigned int Address, int SnoopResult) {
	SnoopResult = GetSnoopResult(Address);
if (NormalMode)
	printf("BusOp: %d, Address: %h, Snoop Result: %d\n",BusOp,Address, SnoopResult);
}


/* Report the result of our snooping bus operations performed by other caches */
void cache_simulator::PutSnoopResult(unsigned int Address, int SnoopResult) {
if (NormalMode)
	printf("SnoopResult: Address %h, SnoopResult: %d\n", Address, SnoopResult);
}

/* Used to simulate communication to our upper level cache */
void cache_simulator::MessageToCache(int Message, unsigned int Address) {
if (NormalMode)
	printf("L2: %d %h\n", Message, Address);
}

int main(int argc, char* argv[])
{
   if(argc == 1)
   {
	cout<<"Please provide trace path";
   }
   else
   {
     string path = argv[1];
     ifstream my_file(path);
     if(my_file.is_open())
     {
	 string line;
	 while(getline(my_file, line))
              {
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

               }
     }
     else
    {
   	cout<<"Unable to open any file";
    }

   }
   return 0;
  
}