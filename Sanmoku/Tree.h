#pragma once

#include <iostream>
#include <list>
#include <iterator>

#include "Matrix.h"

///
///	構造を保存するTree
///
class Tree
{

private:
	Matrix			_matrix;	// 自分自身のマトリックス
	std::list<Tree>	_children;	// 子供木構造のリスト、途中で対称性削除のため、listに
	size_t			_nChildrenTie;		// この下の子供の引分数
	size_t			_nChildrenFirst;	// この下の子供の先手の勝数
	size_t			_nChildrenSecond;	// この下の子供の後手の勝数


public:

	//
	// デフォルトコンストラクタは特殊な処理
	// ルートのみ
	//
	Tree()
		: _nChildrenTie(0)
		, _nChildrenFirst(0)
		, _nChildrenSecond(0)
	{}

	//
	// たくさんのパラメータ付のコンストラクタ
	// Param
	//   status : 先手または後手
	//   row : 行(0～MatrixSize-1)
	//   column : 列(0～MatrixSize-1)
//	 parent: 親のツリー
	Tree(Matrix::Status status, uint8_t row, uint8_t column, const Tree & parent)
		: _matrix(status, row, column, parent._matrix.getLevel() + 1, parent._matrix)
		, _nChildrenTie(0)
		, _nChildrenFirst(0)
		, _nChildrenSecond(0)
	{}

	~Tree()
	{}

// 枝かどうかのチェック
	bool isLeaf() const
	{
		if (
			(_children.size() == 0)	// 子供の数が0
			&& (_matrix.hasWinner() || _matrix.isFinish())
		)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

//
// 子供のリストを作成する
//
	static void makeChildren(Tree & tree, Matrix::Status status, uint8_t level)
	{

		// とりあえず空いているところに全部おいてみる
		for (uint8_t i = 0; i < Matrix::MatrixSize; ++i)
		{
			for (uint8_t j = 0; j < Matrix::MatrixSize; ++j)
			{
				if (tree._matrix.getStatus(i, j) == Matrix::Empty)
				{
					Tree child(status, i, j, tree);
					tree._children.push_back(child);
				}

			}
		}

		// 対称性を考慮して不要なものを削除する
		for (
			std::list<Tree>::iterator it1 = tree._children.begin();
			it1 != tree._children.end();
			it1++
			)
		{
			for (
				auto it2 = std::next(it1); // Don't use ++ because ++ increased it1
				it2 != tree._children.end();
				// Nothing to do here because it2 may be undefined when back one erased
				// and fron one kept.
				)
			{
				// In case of two matrix is same in the symetrix, remove back one
				if (it1->_matrix == it2->_matrix) // This is defined in Matrix.h
				{
					//　remove back one
					it2 = tree._children.erase(it2);
				}
				else
				{
					it2++;
				}
			}
		}
	}

	//
	// 探索木をたどり頻度を計算する
	//
	// Param 
	//	tree	探索する木構造
	//	status	先手か、後手のいずれか
	//
	static void calculate(Tree & tree)
	{
		// リーフまで戻る
		for (
			auto it = tree._children.begin();
			it != tree._children.end();
			++it
			)
		{
			if (it->isLeaf())
			{
				switch (it->_matrix.getWinner())
				{
				case Matrix::First:
					tree._nChildrenFirst++;
					break;
				case Matrix::Second:
					tree._nChildrenSecond++;
					break;
				case Matrix::Empty:
					tree._nChildrenTie++;
					break;
				}
			}
			else
				// not leaf
			{
				calculate(*it);
				tree._nChildrenFirst += it->_nChildrenFirst;
				tree._nChildrenSecond += it->_nChildrenSecond;
				tree._nChildrenTie += it->_nChildrenTie;
			}
		}

	}
		
	//
	// 探索木をリストする
	//
	// Param 
	//	tree	探索する木構造
	//	status	先手か、後手のいずれか
	//
	static void list(Tree & tree, Matrix::Status status, uint8_t level)
	{

// 		std::cout << "Level=" << static_cast<size_t>(level) << std::endl;

		// 子供リストを作成する
		makeChildren(tree, status, level);

		// 後手、先手の変更
		Matrix::Status newStatus = Matrix::swithStatus(status);

		bool isContinue = false;
		// 取得した子供に対して再帰で呼び出す
		for (
			auto it = tree._children.begin();
			it != tree._children.end();
			++it
		)
		{
			// 勝敗が決定しない場合のみ
			if (!it->_matrix.hasWinner())
			{
				isContinue = true;
				Tree::list(*it, newStatus, level + 1);
			}
		}

		// 最後のLevel=0(ルートの時のみ）
		if (level == 0)
		{
			Tree::calculate(tree);
		}

	}

	void dump(std::ostream& os) const
	{
		Matrix::outputTabs(os, _matrix.getLevel() );
		os << "★ 先手(" << _nChildrenFirst <<
			") 後手(" << _nChildrenSecond <<
			") 引分(" << _nChildrenTie << ")" << std::endl;

		_matrix.dump(os);

		//すべての子供を出力する
		for (auto it = _children.begin(); it != _children.end(); ++it)
		{
			it->dump(os);
		}
	}

	friend std::ostream& operator<<(std::ostream& os, const Tree & tree);
};

std::ostream& operator<<(std::ostream& os, const Tree & tree)
{

	Matrix::outputTabs(os, tree._matrix.getLevel());
	os << "★ 先手(" << tree._nChildrenFirst <<
		") 後手(" << tree._nChildrenSecond <<
		") 引分(" << tree._nChildrenTie << ")" << std::endl;

	os << tree._matrix;
		
	//すべての子供を出力する
	for (auto it = tree._children.begin(); it != tree._children.end(); ++it)
	{
		os << *it;
	}

	return os;
}