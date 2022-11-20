
class cache_simulator;
{
	uint16_t *tag;
	uint8_t *lru;
	uint8_t *mesi_bits;

	int *operation;
	int *addr
	int size;

	cache_simulator(int init_size)
	{
		size = init_size;
		tag = new uint16_t[size];
	        lru = new uint8_t[size];
		mesi_bits = new uint8_t[size];	
	}

	read_trace(uint8_t *operation, string *addr, string PATH);// seperate instruction and operation
 	reset_cache();
	print_valid_cache();	



}

void cache_simulkator::reset_cache();
{
	for.....
		tag[i] =(uint8_t*) 0;
		lru[i] = 0;
		mesi/...
}

int main()
{


   return 0;
  
}
