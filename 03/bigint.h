#ifndef bigint_h
#define bigint_h
using cell = signed char;

class BigInt
{
	bool sign;
	size_t size;
	size_t cells_amount = 10;
	cell* elements = nullptr;

	void push_back(cell new_element);
	void allocate();
	void delete_zeros();
	bool compareAbsolute( const BigInt& otherNumber) const;

public:
	BigInt();
	BigInt(int64_t new_element);
	BigInt(const BigInt& other);
	~BigInt();

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
	push_back(0);
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
	BigInt res;
	res.size = 0;
	size_t max = size > other.size ? size : other.size;
	if(sign == other.sign){
		cell buf = 0;
		for(size_t i = 0; i < max; i++){
			if (i < size)
				buf += elements[i];
			if (i < other.size)
				buf += other.elements[i];
			res.push_back(buf);
			buf = 0;
		}
		res.sign = sign;
		cell tmp = 0;


		for(size_t i = 0; i < res.size; i++){	//if digit > 10
			res.elements[i] += tmp;
			tmp = 0;
			if(res.elements[i] >= 10){
				tmp = res.elements[i] / 10;
				res.elements[i] = res.elements[i] % 10;
			}
		}
		if(tmp)
			res.push_back(tmp);
	}
	else{

		cell this_sign;
		cell other_sign;

		if(this->abc() < other.abc()){
			this_sign = -1;
			other_sign = 1;
			res.sign = other.sign;
		}
		else
		{
			this_sign = 1;
			other_sign = -1;
			res.sign = sign;
			if (this->abc() == other.abc()) {
				res.sign = false;
			}
		}

		cell buf = 0;
		for(size_t i = 0; i < max; i++){
			if(i < size)
				buf += this_sign * elements[i];
			if(i < other.size)
				buf += other_sign * other.elements[i];
			res.push_back(buf);
			buf = 0;
		}


		for(size_t i = 0; i < res.size - 1; i++)  //if digit < 0
		{
			signed char tmp = res.elements[i];
			int tmp_int = (int)tmp;

			if(res.elements[i] < 0){
				res.elements[i+1] -= 1;
				res.elements[i] += 10;
			}
		}

		if(res.elements[res.size - 1] < 0)
		{
			res.elements[res.size - 1] *= -1;
		}
		res.delete_zeros();

	}

	if(!res.size)
		res.size++;

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
	if (!sign && other.sign)
			return false;
		else if (sign && !other.sign)
			return true;
		else if (*this == other)
			return false;
		return sign ? !compareAbsolute(other) : compareAbsolute(other);
}


bool BigInt::compareAbsolute(const  BigInt& otherNumber) const//check if this less than other
{

	if (size < otherNumber.size)
			return true;
		else if (size > otherNumber.size)
			return false;

		for (int i = size - 1; i > -1; i-- )
			if (elements[i] != otherNumber.elements[i])
				return (elements[i] < otherNumber.elements[i]);
		return false;
}

bool BigInt::operator>(const BigInt& other) const
{
	if (!sign && other.sign)
			return true;
		else if (sign && !other.sign)
			return false;
		else if (*this == other)
			return false;
		return sign ? compareAbsolute(other) : !compareAbsolute(other);
}


bool BigInt::operator<=(const BigInt& other) const
{
	return !(*this > other);
}

bool BigInt::operator>=(const BigInt& other) const
{

	return !(*this < other);
}

#endif /* bigint_h */
