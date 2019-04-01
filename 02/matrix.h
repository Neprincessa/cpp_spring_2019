#include <vector>

class Matrix
{
private:
	class Row
	{
	private:
		size_t size;
		std::vector<int> v;	
	public:		
		Row() 
		{
		}
		Row (size_t size): size(size), v(std::vector<int>(size))
		{
		}
		int& operator[](size_t i) 
		{
			if (i < 0 || i >= v.size())
				throw std::out_of_range("");
			else
				return v[i];
		}
		const int& operator[](size_t i)  const 
		{
			if (i < 0 || i >= v.size())
				throw std::out_of_range("");
			else
				return v[i];
		}
	};
	std::vector <Row> m_rows;
	size_t rows, columns;
public:
	
	Matrix(size_t rows_m, size_t columns_n) : rows(rows_m), columns(columns_n)
	{
		m_rows = std::vector<Row>(rows);
		for (size_t i = 0; i < rows; i++)
			m_rows[i] = Row(columns);
	}
	
	size_t getRows() const
	{
		return rows;
	}
	
	size_t getColumns() const
	{
		return columns;
	}
	
	Row& operator[](size_t i ) 
	{
		if (i >= rows || i < 0)
			throw std::out_of_range("");
		else
			return m_rows[i];
	}
	const Row& operator[](size_t i ) const 
	{
		if (i >= rows || i < 0)
			throw std::out_of_range("");
		else
			return m_rows[i];
	}
	
	
	bool operator==(const Matrix& other) const
	{
		int flag = 0;
		if (rows == other.getRows() && columns == other.getColumns()) {
			for (size_t i = 0; i < rows; i++) {
				for (size_t j = 0; j < columns; j++)
					if (m_rows[i][j] == other.m_rows[i][j])
						flag++;
			}
		}
		
		return (flag == (rows * columns));
	}
	
	Matrix& operator*=(int number) 
	{
		for (size_t i = 0; i < rows; i++)
			for (size_t j = 0; j < columns; j++)
				m_rows[i][j] *= number;
		return *this;
	}
	
	bool operator!=(Matrix& other) {
		return !(*this == other);
	}
	
};

