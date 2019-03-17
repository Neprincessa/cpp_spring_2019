#include "numbers.dat"
#include <iostream>
#include <math.h>

inline bool Prime(const int &number) {
	if (number < 2)
		return false;
	for (int i = 2; i <= sqrt(number); i++)
		if (number%i == 0)
			return false;
	return true;
}

int IndexRight(const int &number) {
	int result, indexOfElementLessThanElement;
	int flag = false;
	
	for (int i =0; i<Size; i++) {

		if (Data[i] == number) {
			result = i;
			flag = true;
		}
		else 
			if (Data[i] < number)
				indexOfElementLessThanElement = i;
	}

	if (flag)
		return result;
	else
		return indexOfElementLessThanElement;
}




bool ExistanceLeft(int &number) {
	bool exist = false;	

	for (int i =0; i<Size; i++) {

		if (Data[i] == number)
			exist = true;
	}
	return exist;
}

int AmountOfPrimes(int &left, int &right, int &index) {
	int res = 0;
	for (int i = 0; i<=index;i++) {
		if ((Data[i] >= left))
			if (Prime(Data[i]))
				res++;	
	}
	return res;
}


int main(int argc, char* argv[]) {
	
	if (argc < 3 || argc %2 == 0) 
		return -1;

	int left, right;
	int resultAmountPrimes = 0;
	int index;
	if (argc == 3) {
		left = std::atoi(argv[1]);
		right = std::atoi(argv[2]);

		index = IndexRight(right);
		
		if (!ExistanceLeft(left))
			std::cout<<"0";
		else 
			resultAmountPrimes = AmountOfPrimes(left, right,index);
		std::cout<<resultAmountPrimes;
	}
	else {
		for (int i = 1; i<=argc-2; i++) {

			left = std::atoi(argv[i]);
			right = std::atoi(argv[i+1]);
		
			index = IndexRight(right);
			
			if (!ExistanceLeft(left))
				std::cout<<"0";
			else 
				resultAmountPrimes = AmountOfPrimes(left, right,index);
			std::cout<<resultAmountPrimes<<"\n";
			i++;

		}
	}


	return 0;
}
