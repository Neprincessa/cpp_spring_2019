#ifndef bigint_h
#define bigint_h
using cell = signed char;

class BigInt
{
	///main variables
	bool sign;
	size_t size;
	size_t cells_amount = 10;
	cell* elements = nullptr;
	
	///to control cells
	void push_back(cell new_element);
	void allocate();
	void delete_zeros();
	bool compareLess( const BigInt& otherNumber) const;

public:
	///constructos, destructors
	BigInt();
	BigInt(int64_t new_element);
	BigInt(const BigInt& other);
	~BigInt();

	///operators and functions
	BigInt abc() const;
	BigInt operator+(const BigInt& other) const;
	BigInt operator-() const;
	BigInt operator-(const BigInt& other) const;
	bool operator==(const BigInt& other) const;
	bool operator!=(const BigInt& other) const;
	bool operator<(const BigInt& other) const;
	bool operator>(const BigInt& other) const;
	bool operator<=(const BigInt& other) const;
	bool operator>=(const BigInt& other) const;
	BigInt& operator=(const BigInt& other);
	friend std::ostream& operator << (std::ostream& os, const BigInt& num);
};

void BigInt::push_back(cell new_element)
{
	elements[size++] = new_element;
	if(size >= cells_amount)
		allocate();
}

void BigInt::allocate()
{
	cells_amount += 5;
	cell* new_elements = new cell[cells_amount];
	for(size_t i = 0; i < size; ++i){
		new_elements[i] = elements[i];
	}
	delete[] elements;
	elements = new_elements;
}

void BigInt::delete_zeros()
{
	for (size_t i = size - 1; i != 0; --i)
	{
		if (elements[i] != 0 )
			break;
		size--;
	}
}


BigInt::BigInt() : sign(false), size(0)
{
	elements = new cell[cells_amount];
	//push_back(0);
}

BigInt::BigInt(int64_t new_element) : size(0)
{
	if(new_element < 0){
		sign = true;
		new_element *= -1;
	}
	else
		sign = false;
	
	elements = new cell[cells_amount];
	
	if(!new_element)
		push_back(0);
	else{
		for(size_t i = 0; new_element>0; ++i){
			push_back(new_element % 10);
			new_element /= 10;
		}
	}
}

BigInt::BigInt(const BigInt& other) : sign(other.sign), size(other.size), cells_amount(other.cells_amount)
{
	elements = new cell[cells_amount];
	for(size_t i = 0; i<size; ++i){
		elements[i] = other.elements[i];
	}
}

BigInt::~BigInt()
{
	delete[] elements;
}

BigInt BigInt::abc() const
{
	if(sign)
		return (-*this);
	else
		return *this;
}

BigInt BigInt::operator+(const BigInt& other) const
{
	size_t min, max;
	bool bigger;
	if (size <= other.size)
	{
		min = size;
		max = other.size;
		bigger = true;
	}
	else
	{
		min = other.size;
		max = size;
		bigger = false;
	}
	BigInt res;
	for (size_t i = 0; i < min; i++) 
	{
		res.push_back(this->elements[i] + other.elements[i]);
	}
	for (size_t i = min; i < max; i++) 
	{
		if(bigger)
			res.push_back(other.elements[i]);
		else
			res.push_back(this->elements[i]);
	}
	cell buf = 0;
	for(size_t i = 0; i < res.size; i++)
	{
		res.elements[i] += buf;
		buf = 0;
		if(res.elements[i] >= 10){
			buf = res.elements[i] / 10;
			res.elements[i] = res.elements[i] % 10;
		}
	}
	if(buf)
		res.push_back(buf);
	return res;
}

BigInt BigInt::operator-() const
{
	BigInt ans(*this);
	if(size == 1 && elements[0] == 0)
		ans.sign = sign;
	else
		ans.sign = !sign;
	return ans;
}

BigInt BigInt::operator-(const BigInt& other) const
{
	return(*this+(-other));
}

bool BigInt::operator==(const BigInt& other) const
{
	if(size != other.size || sign != other.sign)
		return false;
	else{
		for(size_t i = 0; i < size; ++i){
			if(elements[i] != other.elements[i])
				return false;
		}
	}
	return true;
}

bool BigInt::operator!=(const BigInt& other) const
{
	return!(*this==other);
}

BigInt& BigInt::operator=(const BigInt& other)
{
	sign = other.sign;
	size = other.size;
	cells_amount = other.cells_amount;
	
	delete[] elements;
	elements = new cell[cells_amount];
	for(size_t i = 0; i < size; ++i)
	{
		elements[i] = other.elements[i];
	}
	return *this;
}
std::ostream& operator << (std::ostream& os, const BigInt& num)
{
	if(num.sign)
		os << '-';
	for(int i = num.size - 1; i>-1; --i)
		os << static_cast<cell>('0' + num.elements[i]);
	return os;
}


bool BigInt::operator<(const BigInt& other) const 
{
	if (!sign && !other.sign) //positive < negative
		return false;
	else 
		if (sign && !other.sign) //negative < positive
			return true;
		else
		{
			if (sign) //negative  && negative
			{
				this->abc();
				other.abc();
				if (compareLess(other))
					return false;
				else
					return true;
			}

			return (compareLess(other));
		
		}
}


bool BigInt::compareLess(const  BigInt& otherNumber) const
{
	int flag = 0; 
				
			if (size < otherNumber.size ) // first number absolutely larger than second
				return true;
			else
				if (size > otherNumber.size) // first positive 100% bigger than second
					return false;
				else //if they has equal sizes
				{
					for (size_t i = size - 1; i > 0; i++ )
						if (elements[i] > otherNumber.elements[i]) //the fist digit of first number that is bigger than second
							return false;
					return true;
				}
			
}

bool BigInt::operator>(const BigInt& other) const 
{
	if (!sign && other.sign) // positive > negative
		return true;
	else
		if (sign && !other.sign)	//negative > positive
			return false;
		else
		{
			if (sign)
			{
				this->abc();
				other.abc();
				if (compareLess(other))
					return true;
				else
					return false;
			}
			
			return !(compareLess(other));
		}
}


bool BigInt::operator<=(const BigInt& other) const 
{
	if (*this < other) 
		return true;
	else
	{
		if ( *this > other)
			return false;
		return true;
	}
}

bool BigInt::operator>=(const BigInt& other) const 
{
	if (*this > other)
		return true;
	else
	{
		if (*this < other)
			return false; 
		return true;
	}
}

#endif /* bigint_h */