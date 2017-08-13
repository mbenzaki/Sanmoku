#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>


///
///	MatrixSize * MatrixSize の配列
///
class Matrix
{

public:
	enum Status {
		First = 1,	// 先手
		Empty = 0 ,	// 未決定未定
		Second = -1	// 後手

	};

	// 先手、後手を反転させる
	static Status swithStatus(const Status status)
	{
		switch (status)
		{
		case	Status::First :
			return Status::Second;
			break;
		case	Status::Second:
			return Status::First;
			break;
		default:
			return Status::Empty;
		}
	}

	// 先手、後手を反転させる
	static const char * getString(const Status status)
	{
		switch (status)
		{
		case	Status::First:
			return "先手";
			break;
		case	Status::Second:
			return "後手";
			break;
		default:
			return "引分、未定";
		}
	}

	static const uint8_t	MatrixSize = 3;

private:
	std::pair<int8_t, int8_t>	_current;	// 今回指した位置（0～2のペア、未定は-1,-1)
	std::vector<Status>			_matrix;	// MatrixSize x MatrixSizeの行列(面倒なんで1次元で管理
	uint8_t						_level;		// 特にいらないのだが、タブ出力のため使用
	Status						_status;	// 先手または後手
	Status						_winner;	// 勝ったほうのStauts, 不定な場合は、Status::Empty
	std::string					_reason;	// 買った理由
public:

	//
	// デフォルトコンストラクタ
	//   特殊。ルートのTree用
	Matrix()
		: _current(std::make_pair(-1, -1))
		, _winner(Status::Empty)
		, _level(0)
		, _status(Empty)
		, _reason("引分")
	{
		_matrix.resize(MatrixSize*MatrixSize);
		for (size_t i=0; i<MatrixSize*MatrixSize; ++i)
		{
			_matrix[i] = Status::Empty;
		}
	}

	//
	// たくさんのパラメータ付のコンストラクタ
	// Param
	//   status : 先手または後手
	//   row : 行(0～MatrixSize-1)
	//   column : 列(0～MatrixSize-1)
	//	 level :とりあえずあまり使わないが、先頭からのラベル数
	//	 parent: 親のマトリックス
	Matrix(Status status, uint8_t row, uint8_t column, uint8_t level, const Matrix & parent)
		: _current(std::make_pair(row, column))
		, _winner(Status::Empty)
		, _level(level)
		, _status(status)
		, _matrix(parent._matrix)
		, _reason("引分")
	{
		size_t pos = static_cast<size_t>(row) * MatrixSize + column;
		if (_matrix[pos] != Status::Empty)
		{
			std::stringstream    ss;
			ss << "Row=" << row << ", Column=" << column << " is already setted";
			throw std::exception(ss.str().c_str());
		}
		_matrix[pos] = status;

		checkWinner();
	}

	// コピーコンストラクタはコンパイラ任せ

	~Matrix()
	{
	}

	//
	// 内部のrow, columnのステータスを返す
	// Return
	//   row, column のステータス
	// Param
	//   row : 行(0～MatrixSize-1)
	//   column : 列(0～MatrixSize-1)
	Status getStatus(uint8_t row, uint8_t column) const
	{
		return _matrix[row*MatrixSize + column];
	}

	uint8_t getLevel() const 
	{
		return _level;
	}

	//
	// 全マス埋まったかどうかのチェック
	// Return　trueは埋まった場合、falseはまだ空きがある場合
	//
	bool isFinish() const
	{
		for (
			auto it = this->_matrix.begin();
			it != this->_matrix.end();
			++it
			)
		{
			if (*it == Status::Empty)	return false;
		}

		//全部どちらかの手で終わっている
		return true;
	}

	//
	// 勝者がいるかどうか
	//
	bool hasWinner() const
	{
		return (_winner == First) || (_winner == Second) ;
	}

	Status getWinner() const
	{
		return _winner;
	}

	//
	// 勝者がいるかどうかを決定する
	// Return　勝者、ない場合は、Status::Empty
	void checkWinner() 
	{
		auto vect = this->_matrix;
		std::stringstream	ss;

		// 縦の線でチェック
		// 0列目目からMatrixSizeまで
		for(size_t i=0; i<MatrixSize; ++i)
		{
			if ((vect[i]==vect[i+ MatrixSize]) && vect[i]==vect[i+MatrixSize*2])
			{
				if (vect[i] != Empty)
				{
					this->_winner = vect[i];
					ss << getString(this->_winner) << " 縦線(start=" << static_cast<size_t>(i) << ") Level=" << static_cast<size_t>(_level);
					_reason = ss.str();
				}
			}
		}

		// 横の線でチェック
		// 0行目からMatrixSizeまで
		for (size_t i=0; i<MatrixSize; ++i)
		{
			size_t start = i*MatrixSize;
			if ((vect[start]==vect[start+1]) && vect[start]==vect[start+2])
			{
				if (vect[start] != Empty)
				{
					this->_winner = vect[start];
					ss << getString(this->_winner) << " 横線(start=" << start << ") Level=" << static_cast<size_t>(_level);
					_reason = ss.str();
				}
			}
		}

		//左上から右下の斜め線でチェック
		if ((vect[0] == vect[4]) && vect[0] == vect[8])
		{
			if (vect[0] != Empty)
			{
				this->_winner = vect[0];
				ss << getString(this->_winner) << " 斜め(左上から右下) Level=" << static_cast<size_t>(_level);
				_reason = ss.str();
			}
		}

		//　右上から左下の斜め線でチェック
		if ((vect[2] == vect[4]) && vect[2] == vect[6])
		{
			if (vect[2] != Empty)
			{
				this->_winner = vect[2];
				ss << getString(this->_winner) << " 斜め(右上から左下) Level=" << static_cast<size_t>(_level);
				_reason = ss.str();
			}
		}
	}

/*
	int8_t	getCurrentRow() const
	{
		return _current.first;
	}

	int8_t	getCurrentColumn() const
	{
		return _current.second;
	}
*/
	static void swapVector(size_t i, size_t j, std::vector<Status> & vec)
	{
		auto status = vec[j];
		vec[j] = vec[i];
		vec[i] = status;
	}

	bool operator==(const Matrix & rhs) const
	{
		// 対称性を考慮して同じものかどうかを判断する。

		const std::vector<Status> & lvec = this->_matrix;
		const std::vector<Status> & rvec = rhs._matrix;

		// 回転せず内容が同じかどうかを判断する
		if (lvec == rvec)	return true;

		// ちょっと面倒なので、3x3しか考えてません
		std::vector<Status> newVect=rvec;

		// 真ん中の縦線でミラーを作り同じかどうかをチェックする。
		// 左上0:(0,0)と右上2:(0,2)の交換
		swapVector(0, 2, newVect);
		// 3(1,0)と5(1,3)の交換
		swapVector(3, 5, newVect);
		// 左下6(2,0)と右下8(2,2)の交換
		swapVector(6, 8, newVect);
		if (lvec == newVect)	return true;

		// 真ん中の横線でミラーを作り同じかどうかをチェックする。
		newVect = rvec;
		// 左上0:(0,0)と左下6:(2,0)の交換
		swapVector(0, 6, newVect);
		// 1(0,1)と7(2,1)の交換
		swapVector(1, 7, newVect);
		// 左下2(0,2)と右下8(2,2)の交換
		swapVector(2, 8, newVect);
		if (lvec == newVect)	return true;

		// 左上右下の線でミラーを作り同じかどうかをチェックする。
		newVect = rvec;
		// 1:(0,1)と3:(1,0)の交換
		swapVector(1, 3, newVect);
		// 左下6(2,0)と右上2(0,2)の交換
		swapVector(6, 2, newVect);
		// 7(2,1)と5(1,2)の交換
		swapVector(7, 5, newVect);
		if (lvec == newVect)	return true;

		// 右上左下の線でミラーを作り同じかどうかをチェックする。
		newVect = rvec;
		// 7:(1,0)と3:(0,1)の交換
		swapVector(7, 3, newVect);
		// 左上0(0,0)と右下8(2,2)の交換
		swapVector(0, 8, newVect);
		// 1(0,1)と5(1,2)の交換
		swapVector(1, 5, newVect);
		if (lvec == newVect)	return true;

		return false;
	}


	friend std::ostream& operator<<(std::ostream& os, const Matrix & matrix);

	static void outputTabs(std::ostream & os, size_t numOfTabs)
	{
		for (size_t i = 0; i < numOfTabs; ++i)	os << "\t";
	}

	void dump(std::ostream & os) const
	{

		outputTabs(os, _level);

		if ((_current.first == -1) && (_current.second == -1))
		{
			os << "☆ ルート" << std::endl;
		}
		else
		{
			switch(_status) 
			{
				case Status::First :
					os << "☆ 先手"; 
					break;
				case Status::Second:
					os << "☆ 後手";
					break;
				default:
					os << "☆ バグ";
					break;

			}
			os << "(" <<
				static_cast<int32_t> (_current.first) << ", " <<
				static_cast<int32_t> (_current.second) << ")" << std::endl;
		}

		for (size_t i = 0; i < Matrix::MatrixSize; ++i)
		{
			outputTabs(os, _level);
			for (size_t j = 0; j < Matrix::MatrixSize; ++j)
			{
				switch (_matrix[i*Matrix::MatrixSize + j])
				{
				case Matrix::First:
					os << "○ ";
					break;
				case Matrix::Empty:
					os << "－ ";
					break;
				case Matrix::Second:
					os << "× ";
					break;
				default:
					os << "？ ";
					break;

				}
			}
			if (
				(i == (Matrix::MatrixSize - 1)) &&
				(hasWinner() || isFinish())
				)
			{
				switch (_winner)
				{
				case Status::First:
					os << " (先手の勝) " << _reason.c_str();
					break;
				case Status::Second:
					os << " (後手の勝) " << _reason.c_str();
					break;
				case Status::Empty:
					os << " (引分)" << _reason.c_str();
					break;
				}
			}

			os << std::endl;
		}
	}

};

std::ostream& operator<<(std::ostream& os, const Matrix & matrix)
{
	matrix.dump(os);
	return os;
}

