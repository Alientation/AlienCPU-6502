#include <string>
#include <cstdint>

#ifndef FILE_H
#define FILE_H

class File;
class FileWriter;
class FileReader;

static const std::string SEPARATOR = "\\";

class File {
	public:
		static bool isValidFileName(const std::string fileName) {
			return fileName.find_first_of("\\/:*?\"<>|") == std::string::npos && fileName.size() > 0;
		}

		static bool isValidFileExtension(const std::string fileExtension) {
			return fileExtension.find_first_of("\\/:*?\"<>|") == std::string::npos && fileExtension.size() > 0;
		}

		static bool isValidFileDirectory(const std::string directory) {
			return directory.find_first_of("*?\"<>|") == std::string::npos;
		}

		File(std::string fileName, std::string fileExtension, std::string fileDirectory, bool createFileIfNotPresent = false);
		File(std::string filePath, bool createFileIfNotPresent = false);
		~File();

		std::string getFileName();
		std::string getExtension();
		std::string getFilePath();
		std::string getFileDirectory();
		int getFileSize();
		bool exists();
		void create();
	private:
		std::string fileName;
		std::string fileExtension;
		std::string fileDirectory;
};

class FileWriter {
	public:
		FileWriter(File* file);
		~FileWriter();
		void writeString(std::string text);
		void writeByte(char byte);
		void writeBytes(char* bytes);
		void close();
	private:
		File* file;
		std::ofstream* fileStream;
		bool closed;
};

class FileReader {
	public:
		FileReader(File* file);
		~FileReader();
		std::string readAll();
		char readByte();
		char peekByte();
		char* readBytes(unsigned int numBytes);
		char* readToken(char tokenDelimiter);
		bool hasNextByte();
		void close();
	private:
		File* file;
		std::ifstream* fileStream;
		bool closed;
};


#endif