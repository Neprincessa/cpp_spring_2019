#include "sort.h"

std::atomic<bool> finished = false;
std::atomic<int> finishedStep = 0;
std::atomic<int> finishedSort = 0;
std::condition_variable condition;
std::mutex sortDoneMutex;
std::mutex queueMutex;
std::mutex streamReadMutex;
std::mutex iterationFinishMutex;

void Sort_by_threads::split(uint64_t * const buffer, const int id)
{
	int file = 0;
	while (inputStream.good()) {
		{
			std::unique_lock<std::mutex> lock(streamReadMutex);
			inputStream.read(reinterpret_cast<char *>(buffer), MemMaxThread);
		}
		
		std::streamsize readAmount = inputStream.gcount() / sizeof(uint64_t);
		
		if (readAmount)
		{
			std::sort(buffer, buffer + readAmount);
			std::string outputFileName = std::to_string(0) + '-' + std::to_string(id) + '-' + std::to_string(file);
			std::ofstream outputStream(outputFileName, std::ios::binary | std::ios::out);
			outputStream.write(reinterpret_cast<char*>(buffer), readAmount * sizeof(uint64_t));
			outputFiles.push(outputFileName);
			++file;
		}
	}
	
}

void Sort_by_threads::merge(const std::string & fileName1, const std::string & fileName2, uint64_t * const buffer, const int id, const int iter, const int file)
{
	std::ifstream file1(fileName1, std::ios::binary | std::ios::in), file2(fileName2, std::ios::binary | std::ios::in);
	const std::string name = std::to_string(iter) + '-' + std::to_string(id) + '-' + std::to_string(file);
	std::ofstream out(name, std::ios::binary | std::ios::out);
	
	const size_t limit = (MemMaxThread / sizeof(uint64_t) / 4), limitM = 2 * limit;
	
	uint64_t * const bufL = buffer, * const bufR = (buffer + limit), * const bufM = (buffer + limitM);
	
	file1.read(reinterpret_cast<char*>(bufL), limit * sizeof(uint64_t));
	size_t readL = file1.gcount() / sizeof(uint64_t);
	file2.read(reinterpret_cast<char*>(bufR), limit * sizeof(uint64_t));
	size_t readR = file2.gcount() / sizeof(uint64_t);
	
	size_t left = 0, middle = 0, right = 0;
	
	
	while (file1.good() || file2.good() || left < readL || right < readR) {
		if (left == readL && file1.good()) {
			file1.read(reinterpret_cast<char *>(bufL), limit * sizeof(uint64_t));
			readL = file1.gcount() / sizeof(uint64_t);
			left = 0;
		}
		if (right == readR && file2.good()) {
			file2.read(reinterpret_cast<char *>(bufR), limit * sizeof(uint64_t));
			readR = file2.gcount() / sizeof(uint64_t);
			right = 0;
		}
		if (middle == limitM) {
			out.write(reinterpret_cast<char *>(bufM), middle * sizeof(uint64_t));
			middle = 0;
		}
		
		if (left < readL && right < readR) {
			if (bufR[right] < bufL[left]) {
				bufM[middle] = bufR[right];
				++right;
			}
			else {
				bufM[middle] = bufL[left];
				++left;
			}
		}
		else if (left == readL && right < readR) {
			bufM[middle] = bufR[right];
			++right;
		}
		else if (right == readR && left < readL) {
			bufM[middle] = bufL[left];
			++left;
		}
		++middle;
	}
	
	out.write(reinterpret_cast<char *>(bufM), middle * sizeof(uint64_t));
	std::unique_lock<std::mutex> queueLock(queueMutex);
	outputFiles.push(name);
}


void Sort_by_threads::Tsort(const int id, std::string & res) {
	uint64_t * const buffer = buf.get() + id * MemMaxThread / sizeof(uint64_t);
	int iteration = 0, file = 0;
	
	split(buffer, id);
	
	++iteration;
	{
		std::unique_lock<std::mutex> lock(iterationFinishMutex);
		++finishedStep;

		condition.notify_all();

		while (finishedStep < threadCount)
			condition.wait(lock);
	}
	while (outputFiles.size() >= 2) {
		std::unique_lock<std::mutex> queueLock(queueMutex);
		std::string tmp1 = outputFiles.front();
		outputFiles.pop();
		std::string tmp2 = outputFiles.front();
		outputFiles.pop();
		queueLock.unlock();
		merge(tmp1, tmp2, buffer, id, iteration, file);
		++file;
	}
	std::unique_lock<std::mutex> fLock(sortDoneMutex);
	++finishedSort;
	if (finishedSort == threadCount) {
		if (outputFiles.empty())
			throw std::logic_error("no output files");
		else
			res = std::string("all done, result in: " + outputFiles.front());
	}
}
