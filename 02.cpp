#include <iostream>
#include <string>

class big_integer
{
public:
	big_integer(std::string&& rv);
	big_integer(const std::string& s);
	big_integer(const big_integer& bi);
	big_integer(big_integer&& rbi) noexcept;
	~big_integer();
	friend big_integer operator *(big_integer& lhs, big_integer& rhs);
	friend big_integer operator +(big_integer& lhs, big_integer& rhs);
	big_integer& operator = (const big_integer& bi);
	big_integer& operator = (big_integer&& rbi) noexcept;
	std::string get_integer();

private:
	const std::string* bint{ nullptr };
	void delete_ptr();
	void assign(const std::string& s);
};

big_integer::big_integer(std::string&& rv)
{
	assign(rv);
	rv = "";
}

big_integer::big_integer(const std::string& str)
{
	assign(str);
}

big_integer::big_integer(const big_integer& bi)
{
	assign(*bi.bint);
}

big_integer::big_integer(big_integer&& rbi) noexcept
{
	this->bint = rbi.bint;
	rbi.bint = nullptr;
}

void big_integer::assign(const std::string& s)
{
	if (s[0] == 45) { throw std::exception("Positive numbers only!"); }
	for (const auto& it : s)
	{
		if (it < 48 || it > 57) { throw std::exception("String contain not numeric symbol!"); }
	}
	this->bint = new std::string{ s };
}

std::string big_integer::get_integer()
{
	return *bint;
}

void big_integer::delete_ptr()
{
	delete bint;
	bint = nullptr;
}

big_integer::~big_integer()
{
	delete_ptr();
}

int main()
{
	auto number1 = big_integer("9999");
	auto number2 = big_integer("999");
	auto number4 = std::move(number1 + number2);
	auto number3 = std::move(number1 * number2);
	return 0;
}

big_integer operator *(big_integer& lhs, big_integer& rhs)
{
	auto& it_lhs = lhs.bint->rbegin();
	auto& it_rhs = rhs.bint->rbegin();
	//int max_rows{ 0 };
	int max_cols{ 0 };
	int span_count{ 0 };
	int reslt{0};
	int residue{ 0 };
	int row{ 0 };
	int col{ 0 };
	std::string result;

	if (lhs.bint->size() < 3 && rhs.bint->size() < 3)
	{
		return big_integer(std::to_string(std::stoi(rhs.get_integer()) * std::stoi(rhs.get_integer())));
	}

	max_cols = lhs.bint->size() + rhs.bint->size();

	int** multiple_matrix = new int* [rhs.bint->size()];
	for (int i = 0; i < rhs.bint->size(); ++i)
	{
		multiple_matrix[i] = new int[max_cols]{ 0 };
	}
	//--------поразрядное умножение в матрицу
	while (it_rhs != rhs.bint->rend())
	{
		it_lhs = lhs.bint->rbegin();
		span_count = 1;
		residue = 0;
		col = max_cols - 1;
		while (it_lhs != lhs.bint->rend())
		{
			
			reslt = (*it_rhs - 48) * (*it_lhs - 48);
			reslt += residue;
			if (span_count != lhs.bint->size())//счетчик разрядов умножаемого числа
			{
				if (reslt >= 10)
				{
					multiple_matrix[row][col] = reslt % 10;
					residue = reslt / 10;
				}
				else
				{
					multiple_matrix[row][col] = reslt;
					residue = 0;
				}
			}
			else
			{
				if (reslt >= 10)
				{
					multiple_matrix[row][col] = reslt % 10;
					multiple_matrix[row][--col] = reslt / 10;
				}
				else
				{
					multiple_matrix[row][col] = reslt;
				}
			}
			--col;
			++span_count;
			++it_lhs;
		} 
		++row;
		++it_rhs;
	}
	//-------сложение в матрице----------
	result.push_back(static_cast<char>((multiple_matrix[0][(max_cols - 1)] + 48)));
	reslt = 0; residue = 0;
	bool mid_diagonal{ false }; bool calculated{ false };
	int diagonal_iter = rhs.bint->size() + max_cols - 2;
	for (int i = 0; i < diagonal_iter; ++i)
	{
		if (calculated) { break; }
		reslt = 0;
		if (!mid_diagonal)
		{
			row = 0; col = max_cols - i - 2;
		}
		else
		{
			row = i - 2; col = 0;
		}
		if (!row && !col) { mid_diagonal = true; }
		while (row < rhs.bint->size() && col < max_cols)
		{
			reslt += multiple_matrix[row][col];
			++row; ++col;
		}
		reslt += residue;
		if (!reslt) { calculated = true; break; }
		if (reslt >= 10)
		{
			result.push_back(static_cast<char>((reslt % 10) + 48));
			residue = reslt / 10;
		}
		else
		{
			result.push_back(static_cast<char>(reslt + 48));
			residue = 0;
		}
	}
	//-------
	std::reverse(result.begin(), result.end());

	for (unsigned short i = 0; i < rhs.bint->size(); ++i)
	{
		delete[] multiple_matrix[i];
	}
	delete[] multiple_matrix;

	return big_integer(result);
}

big_integer operator +(big_integer& lhs, big_integer& rhs)
{
	auto& it_lhs = lhs.bint->rbegin();
	auto& it_rhs = rhs.bint->rbegin();
	int max_cols{ 0 };
	int reslt{ 0 };
	int residue{ 0 };
	std::string result;

	if (lhs.bint->size() < 3 && rhs.bint->size() < 3)
	{
		return big_integer(std::to_string(std::stoi(rhs.get_integer()) + std::stoi(rhs.get_integer())));
	}

	(lhs.bint->size() >= rhs.bint->size()) ? max_cols = lhs.bint->size() + 1 : max_cols = rhs.bint->size() + 1;

	for (int i = max_cols - 1; i > 0; --i)
	{
		reslt = 0;
		if (it_lhs < lhs.bint->rend() && it_rhs < rhs.bint->rend())
		{
			reslt = (*it_rhs - 48) + (*it_lhs - 48);
		}
		else if(it_lhs == lhs.bint->rend())
		{
			reslt = (*it_rhs - 48);
		}
		else if (it_rhs == rhs.bint->rend())
		{
			reslt = (*it_lhs - 48);
		}
		reslt += residue;
		if (reslt >= 10)
		{
			result.push_back(static_cast<char>((reslt % 10) + 48));
			residue = reslt / 10;
		}
		else
		{
			result.push_back(static_cast<char>((reslt + 48)));
			residue = 0;
		}
		if (it_lhs != lhs.bint->rend()) { ++it_lhs; }
		if (it_rhs != rhs.bint->rend()) { ++it_rhs; }
	}
	if (residue) { result.push_back(static_cast<char>((residue + 48))); }
	std::reverse(result.begin(), result.end());
	return big_integer(result);
}

big_integer& big_integer::operator = (const big_integer& bi)
{
	if (this != &bi) {
		this->delete_ptr();
		this->assign(*bi.bint);
		return *this;
	}
	return *this;
}

big_integer& big_integer::operator = (big_integer&& rbi) noexcept
{
	if (this != &rbi) {
		this->delete_ptr();
		this->assign(*rbi.bint);
		rbi.bint = nullptr;
		return *this;
	}
	return *this;
}