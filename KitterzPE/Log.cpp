#include "Log.h"
namespace Log{
	


	void log(string msg) {
		fstream File("log.txt", ofstream::app);
		File << msg << "\n";
		File.close();
	}
}