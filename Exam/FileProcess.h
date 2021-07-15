#pragma once
#include <string>
#include <fstream>
#include <vector>
#include "Exceptions.h"

namespace file_process {
	size_t fileSize(const std::string& filename);

	bool fileExists(const std::string& filename);

	void createFile(const std::string& filename);

	template<typename T>
	void appendToFileBinary(const std::string& filename, const T& object);

	template<typename T>
	void readFromFileBinary(const std::string& filename, std::vector<T>& objects);
	
	template<typename T>
	void writeToFileBinary(const std::string& filename, const std::vector<T>& objects);

	template<typename T>
	void appendToFileBinary(const std::string& filename, const T& object) {
		size_t size = fileSize(filename);
		if (size == 0)
			throw FileNotLoadedException{ "Couldn't read objects from file. " };

		std::ios::openmode mode = std::ios::in | std::ios::binary | std::ios::app;
		std::ofstream out{ filename, mode };

		if (!out.is_open())
			throw FileNotLoadedException{ "Couldn't read objects from file. " };

		out.write(reinterpret_cast<char*>(&object), sizeof(T));

		out.close();
	}

	template<typename T>
	void readFromFileBinary(const std::string& filename, std::vector<T>& objects) {
		const size_t objectSize = sizeof(T);
		size_t objectsCount = fileSize(filename) / objectSize;

		objects.clear();

		std::ios::openmode mode = std::ios::binary | std::ios::in;
		std::ifstream in{ filename, mode };

		if (!in.is_open())
			throw FileNotLoadedException{ "Couldn't read objects from file. " };

		char* object = new char[objectSize];
		for (size_t _ = 0; _ < objectsCount; _++) {
			in.read(object, objectSize);
			objects.push_back(*reinterpret_cast<T*>(object));
		}

		delete[] object;
		in.close();
	}

	template<typename T>
	void writeToFileBinary(const std::string& filename, const std::vector<T>& objects)
	{
		const size_t objectSize = sizeof(T);

		std::ios::openmode mode = std::ios::out | std::ios::trunc | std::ios::binary;
		std::ofstream out{ filename, mode };

		if (!out.is_open())
			throw FileNotLoadedException{ "Couldn't write objects to the file. " };

		for (const T& object : objects)
			out.write(reinterpret_cast<char*>(const_cast<T*>(&object)), objectSize);

		out.close();
	}

	
}
