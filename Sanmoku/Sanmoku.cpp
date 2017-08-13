// Sanmoku.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include <iostream>
#include <fstream>
#include "Matrix.h"
#include "Tree.h"

int main()
{

	// ルートマトリックス
	Tree root;

	// Treeさん完全解をリストして
	Tree::list(root, Matrix::First, 0);

	// ダンプ
	std::ofstream ofs("result.txt");
	if (ofs.bad())
	{ 
		std::cerr << "Can not open file result.txt" << std::endl;
	}
	else
	{
		ofs << root;
	}

	std::cout << "Input some char ";
	int i = getchar();

    return 0;
}

