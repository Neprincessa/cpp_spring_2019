#include <vector>

class Matrix
{
private:
	class Row
	{
	public:
		std::vector<int> v;
		Row() {};
		Row (int size) {
			v.resize(size);
		}
		
		int& operator[](int i) {
			if (i < 0 || i >= v.size())
				throw std::out_of_range("");
			else
				return v[i];
		}
		const int& operator[](int i)  const {
			if (i < 0 || i >= v.size())
				throw std::out_of_range("");
			else
				return v[i];
		}
	};
	std::vector <Row> m_rows;
	int rows, columns;
public:
	
	Matrix(int rows_m, int columns_n) : rows(rows_m), columns(columns_n){
		m_rows = std::vector<Row>(rows);
		for (int i = 0; i < rows; i++)
			m_rows[i] = Row(columns);
	}
	
	int getRows() const{
		return rows;
	}
	
	int getColumns() const{
		return columns;
	}
	
	Row& operator[](int i ) {
		if (i >= rows || i < 0)
			throw std::out_of_range("");
		else
			return m_rows[i];
	}
	const Row& operator[](int i ) const {
		if (i >= rows || i < 0)
			throw std::out_of_range("");
		else
			return m_rows[i];
	}
	
	
	const bool operator==(const Matrix& other) const{
		int flag = 0;
		if (rows == other.getRows() && columns == other.getColumns()) {
			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < columns; j++)
					if (m_rows[i][j] == other.m_rows[i][j])
						flag++;
			}
		}
		if (flag == (rows * columns))
			return true;
		else
			return false;
	}
	
	Matrix& operator*=(int number) {
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < columns; j++)
				m_rows[i][j] *= number;
		return *this;
	}
	
	bool operator!=(Matrix& other) {
		return !(*this == other);
	}
	
};