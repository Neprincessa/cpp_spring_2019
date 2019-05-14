#include "sort.h"

std::atomic<int> finishedStep = 0;
std::atomic<int> finishedSort = 0;
std::condition_variable condition;
std::mutex sortDoneMutex;
std::mutex queueMutex;
std::mutex streamReadMutex;
std::mutex iterationFinishMutex;

void Sort_by_threads::split(uint64_t * const buffer, const int id) //buffer - указатель на выделенную потоку память, id - номер потока
{
	int file = 0;
	while (inputStream.good()) {//пока не кончился файл читаем
		{
			std::unique_lock<std::mutex> lock(streamReadMutex);
			inputStream.read(reinterpret_cast<char *>(buffer), MemMaxThread);
		}
		
		std::streamsize readAmount = inputStream.gcount() / sizeof(uint64_t);//сколько прочитали чисел
		
		if (readAmount)//если наш файл был непустым то обрабатываем
		{
			std::sort(buffer, buffer + readAmount);//сортируем блок
			std::string outputFileName = std::to_string(0) + '-' + std::to_string(id) + '-' + std::to_string(file);
			std::ofstream outputStream(outputFileName, std::ios::binary | std::ios::out);
			outputStream.write(reinterpret_cast<char*>(buffer), readAmount * sizeof(uint64_t));//сохраняем на диск
			outputFiles.push(outputFileName);
			++file;
		}
	}
	
}

void Sort_by_threads::merge(const std::string & fileName1, const std::string & fileName2, uint64_t * const buffer, const int id, const int iter, const int file)//id iter и file индексы для итогового файла(составляем аналогично)
{
	std::ifstream file1(fileName1, std::ios::binary | std::ios::in), file2(fileName2, std::ios::binary | std::ios::in);
	const std::string name = std::to_string(iter) + '-' + std::to_string(id) + '-' + std::to_string(file);
	std::ofstream out(name, std::ios::binary | std::ios::out);
	
	const size_t limit = (MemMaxThread / sizeof(uint64_t) / 4), limitM = 2 * limit;
	
	uint64_t * const bufL = buffer, * const bufR = (buffer + limit), * const bufM = (buffer + limitM); //делим общий буфер на три части, левому, правому файлам и итогу
	
	file1.read(reinterpret_cast<char*>(bufL), limit * sizeof(uint64_t));
	size_t readL = file1.gcount() / sizeof(uint64_t);//сохраняем сколько прочитали из 1 файла
	file2.read(reinterpret_cast<char*>(bufR), limit * sizeof(uint64_t));
	size_t readR = file2.gcount() / sizeof(uint64_t);//сохраняем сколько прочитали из 2 файла
	
	size_t left = 0, middle = 0, right = 0; //индексы для частей буфера
	//левый и правый индексы от исходных файлов, серединный - итоговый
	
	
	while (file1.good() || file2.good() || left < readL || right < readR) {//пока файлы не пустые или не пройдены оба буфера
		if (left == readL && file1.good()) {//закончился левый буфер но файл не пройден полностью
			file1.read(reinterpret_cast<char *>(bufL), limit * sizeof(uint64_t));
			readL = file1.gcount() / sizeof(uint64_t);
			left = 0;
		}
		if (right == readR && file2.good()) {//закончился правый буфер но файл не пройден полностью
			file2.read(reinterpret_cast<char *>(bufR), limit * sizeof(uint64_t));
			readR = file2.gcount() / sizeof(uint64_t);
			right = 0;
		}
		if (middle == limitM) {//заполнен итоговый буфер
			out.write(reinterpret_cast<char *>(bufM), middle * sizeof(uint64_t));
			middle = 0;
		}
		
		if (left < readL && right < readR) {//проходим по двум буферам
			if (bufR[right] < bufL[left]) {//число из правого меньше - записываем в итоговый
				bufM[middle] = bufR[right];
				++right;
			}
			else {
				bufM[middle] = bufL[left];//число из левого меньше - записываем в итоговый
				++left;
			}
		}
		else if (left == readL && right < readR) {//закончился левый буфер и его файл а правый нет
			bufM[middle] = bufR[right];
			++right;
		}
		else if (right == readR && left < readL) {//закончился правый буфер и его файл а левый нет
			bufM[middle] = bufL[left];
			++left;
		}
		++middle;//сдвигаем индекс итогового буфера
	}
	
	out.write(reinterpret_cast<char *>(bufM), middle * sizeof(uint64_t));// пишем остаток в файл
	std::unique_lock<std::mutex> queueLock(queueMutex);
	outputFiles.push(name);//сохраняем имя файла
}


void Sort_by_threads::Tsort(const int id, std::string & res) { //res - имя итогового файла
	uint64_t * const buffer = buf + id * MemMaxThread / sizeof(uint64_t);
	int iteration = 0, file = 0;
	
	split(buffer, id);//подготавливаем исходник
	
	++iteration;
	{
		std::unique_lock<std::mutex> lock(iterationFinishMutex);
		++finishedStep;

		condition.notify_all();

		while (finishedStep < threadCount)//ждем пока все потоки закончат делить файл
			condition.wait(lock);
	}
	while (outputFiles.size() >= 2) {//пока у нас остается больше чем 1 файл
		std::unique_lock<std::mutex> queueLock(queueMutex);
		std::string tmp1 = outputFiles.front();//берем 1 файл из очереди
		outputFiles.pop();
		std::string tmp2 = outputFiles.front();//берем 2 файл из очереди
		outputFiles.pop();
		queueLock.unlock();
		merge(tmp1, tmp2, buffer, id, iteration, file);//объединяем их
		std::remove(tmp1.c_str());//удаляем промежуточные файлы
		std::remove(tmp2.c_str());
		++file;
	}
	std::unique_lock<std::mutex> fLock(sortDoneMutex);
	++finishedSort;
	try
	{
		if (finishedSort == threadCount) {//все потоки завершили работу, текущий был последним
			if (outputFiles.empty())//если нет ничего в очереди ??? ошибка
				throw std::logic_error("no output files");
			else
				res = outputFiles.front();//воызращаем имя результата
		}
	}
	catch(std::logic_error &) {
		globalExceptionPtr = std::current_exception(); 
	}
}
