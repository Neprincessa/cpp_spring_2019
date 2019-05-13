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
	size_t memLimit;
	int threadCount = 2;
	size_t MemMaxThread;
	std::unique_ptr<uint64_t>  buf;
	std::ifstream inputStream;
	std::queue<std::string> outputFiles;


	void split(uint64_t * const buffer, const int id);

	void merge(const std::string & fileName1, const std::string & fileName2, uint64_t * const buffer, const int id, const int iter, const int file);

	void Tsort(const int id, std::string & res);

public:
	Sort_by_threads(size_t _memLimit, std::string _data) :
		memLimit(_memLimit), 
		MemMaxThread (memLimit / threadCount),
		buf(new uint64_t[memLimit / sizeof(uint64_t)]),
		inputStream(std::ifstream(_data, std::ios::binary))
	{
	}

	void sort(std::string & res){
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
	
};
