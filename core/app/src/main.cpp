#include <assembler/Assembler.h>
#include <assembler/Build.h>
#include <assembler/Linker.h>
#include <assembler/ObjectFile.h>
#include <assembler/Preprocessor.h>
#include <util/Logger.h>
#include <util/File.h>
#include <util/Directory.h>

#include <filesystem>
#include <sstream>
#include <fstream>

void clearFile(std::string filePath) {
	// don't clear nonexistent files
	if (!std::filesystem::exists(filePath)) {
		return;
	}

	std::ofstream file(filePath, std::ofstream::out | std::ofstream::trunc);
	file.close();
}

int main() {
    lgr::log(lgr::Logger::LogType::TEST, "Testing Preprocessor");

    // test creating a file and its attributes
	Process process = Process("-lib library1 -L .\\tests\\libs -I .\\tests\\include -o preprocessorTest .\\tests\\src\\preprocessorTest." + SOURCE_EXTENSION);
	File file = File(".\\tests\\src\\preprocessorTest." + SOURCE_EXTENSION);
	clearFile(".\\tests\\src\\preprocessorTest." + PROCESSED_EXTENSION);

	// test preprocessing
	Preprocessor preprocessor = Preprocessor(&process, file);
	File processed_file = preprocessor.preprocess();

	Assembler assembler = Assembler(&process, processed_file);
	File output_file = assembler.assemble();

	ObjectFile object_file = ObjectFile();
	object_file.read_object_file(output_file);

	File exe_file = File(".\\tests\\preprocessorTest." + EXECUTABLE_EXTENSION);
	Linker linker({object_file}, exe_file);

	ObjectFile exe_obj_file = ObjectFile();
	exe_obj_file.read_object_file(exe_file);
}