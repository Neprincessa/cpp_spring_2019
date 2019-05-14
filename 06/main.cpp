#include "sort.h"

int main(int argc, char ** argv){
	std::string res;
	try{
		if(argc == 2){
			Sort_by_threads thr(6 * 1024 * 1024, argv[1]);
			thr.sort(res);
			std::cout << "all done, result in: " << res << std::endl;
		}
		else
			throw std::invalid_argument("incorrect number of arguments");
	}
	catch(std::logic_error err){
		std::cerr << err.what() << std::endl;
	}
	return 0;
}
