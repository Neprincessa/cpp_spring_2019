#include "numbers.dat"
#include <iostream>
#include <math.h>

inline bool Prime(const int &number) {
	if (number < 2)
		return false;

	int root = sqrt(number);

	for (int i = 2; i <= root; i++)
		if (number%i == 0)
			return false;
	return true;
}

int SearchLeft(int &left){
	int pos = 0;
	while (pos < Size) {
		if (Data[pos] == left)
			return pos;
		else
			pos++;
	}
	return -1;
}

int SearchRight(int &right){
	int pos = Size - 1;
	while (pos >= 0) {
		if (Data[pos] == right)
			return pos;
		else
			pos--;
	}
	return -1;
}



//int IndexRight(const int &number) {
//	int result, indexOfElementLessThanElement;
//	int flag = false;
//	for (int i =0; i<Size; i++) {
//		if (Data[i] == number) {
//			result = i;
//			flag = true;
//		}
//		else
//			if (Data[i] < number)
//				indexOfElementLessThanElement = i;
//	}
//	if (flag)
//		return result;
//	else
//		return indexOfElementLessThanElement;
//}
//bool ExistanceLeft(int &number) {
//	bool exist = false;
//	for (int i =0; i<Size; i++) {
//		if (Data[i] == number)
//			exist = true;
//	}
//	return exist;
//}


int AmountOfPrimes(int left, int right) {
	int res = 0;
		for (int i = left; i <= right; i++) {
		if(Prime(Data[i])) {
			res++;
			while(i < right-1 && Data[i+1] == Data[i]){ //don't count similar numbers
				i++;
				res++;
			}
		}
		else
			while(i < right-1 && Data[i+1] == Data[i]) //don't count similar numbers
				i++;
	}
	return res;
}


int main(int argc, char* argv[]) {
	
	if (argc < 3 || argc %2 == 0) 
		return -1;

	int left, right, l,r;
	int resultAmountPrimes = 0;
	int index;

	for (int i = 1; i < argc; i++) {

		left = std::atoi(argv[i]);
		right = std::atoi(argv[i+1]);
		
		l = SearchLeft(left);
		r = SearchRight(right);
			
		if (l < 0 || r < 0)
			resultAmountPrimes = 0;
		else
			resultAmountPrimes = AmountOfPrimes(l, r);
		std::cout << resultAmountPrimes << std::endl;
		i++;

	}


	return 0;
}
