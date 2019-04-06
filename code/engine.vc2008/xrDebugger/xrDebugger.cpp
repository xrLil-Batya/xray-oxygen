// xrDebuger.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include "../../SDK/include/tbb/include/tbb/parallel_for.h"



int main()
{

	size_t n = 10;

	tbb::parallel_for( size_t(0), n, size_t(1),
	[&](size_t k)
	{		
		std::cout << k <<std::endl;
	}

	);

    std::cout << "Hello World!\n"; 
}