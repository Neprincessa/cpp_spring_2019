#pragma once

#include <iostream>
#include <fstream>
#include <algorithm>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <string>
#include <exception>


class Sort_by_threads
{
	size_t memLimit; //максимум доступной памяти
	int threadCount = 2; //число потоков
	size_t MemMaxThread; //максимум доступной памяти каждому из потоков
	uint64_t * buf; //указатель на выделенную память
	std::ifstream inputStream; //входной поток для данных
	std::queue<std::string> outputFiles; //очередь из названий выходных файлов


	void split(uint64_t * const buffer, const int id); //метод для разделения на memLimit блоки и сортировка их

	void merge(const std::string & fileName1, const std::string & fileName2, uint64_t * const buffer, const int id, const int iter, const int file); //объединение двух файлов в один с сохранением отсортированности

	void Tsort(const int id, std::string & res); //разделение сортировки на потоки

public:
	Sort_by_threads(size_t _memLimit, std::string _data) : // конструктор, попутно задает пределы для памяти
		memLimit(_memLimit), 
		MemMaxThread (memLimit / threadCount),
		buf(new uint64_t[memLimit / sizeof(uint64_t)]),
		inputStream(std::ifstream(_data, std::ios::binary))
	{
	}

	void sort(std::string & res){ //основной метод, открывает исходник и запускает потоки
		if(!inputStream.good())
			throw std::invalid_argument("can't open file");
		std::vector<std::thread> threads;
		for (int i = 0; i < threadCount; ++i) {
			threads.emplace_back(&Sort_by_threads::Tsort, this, i, std::ref(res));
		}
		for (int i = 0; i < threadCount; ++i) {
			threads[i].join();
		}
	}
	
	~Sort_by_threads(){ //очистка выделенной памяти по буфер
		delete [] buf;
	}
	
};
