#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <dirent.h>
#include <ctime>
using namespace std;

/**
Simple little program that will scan the given folder for files that 
are LIKE *nameArg*.sql, checks to see if they are more than one month
old. If they match, the file is deleted. 
*/

void scanFiles(string folderPath, string nameArg, ofstream &log); 
__int64 Delta(const SYSTEMTIME st1, const SYSTEMTIME st2);
bool containsCaseInsensitive(string str1, string str2); 
string getCurrentDateTime(); 

///<summary>main function of the program that checks are parses the arguments, builds the log
///and, hopefully, catches file exceptions for the log</summary>
///<param name="argc">how many arguments are given (always atleast one)</param>
///<param name="argv">array that holds pointers to the first char in the argumment string</param>
///<returns>int of either EXIT_SUCCESS or EXIT_FAILURE (0 or 1 for most machines)</returns>
int main(int argc, char* argv[]) {
	//set folderPath, logPath, and nameArg defaults here, if args are wrong, use the default value

	//holds folder path
	string folderPath = "\\\\baksrv2\\SQL-Backups\\DB-Backups\\mySQL Backups\\*";
	//holds log path
	string logPath = "\\\\nas3\\NOE_Docs$\\RALIM\\Logs\\mySQLbkpLogs\\";
	//holds the string that candidate files must posses
	string nameArg = "Backup";

	if(argc == 4){ // check for correct args, did not previously establish log incase of this
		folderPath = argv[1];
		logPath = argv[2];
		nameArg = argv[3];
	}

	string logName = logPath + "mySQLbkpLog_" + getCurrentDateTime() + ".txt";//build log here
	ofstream log;
	log.exceptions(ofstream::failbit | ofstream::badbit);
	try {
		log.open(logName);
		log << "Starting backup logging at " << getCurrentDateTime() << endl;

		if (argc != 4) {
			std::cout << "WARNING: Found  " << argc << " arguments, using defaults of \\\\baksrv2\\SQL-Backups\\DB-Backups\\mySQL Backups\\*, ";
			std::cout << "\\\\nas3\\NOE_Docs$\\RALIM\\Logs\\mySQLbkpLogs\\, and backup" << endl;
			log << "WARNING: Found  " << argc << " arguments, using defaults of \\\\baksrv2\\SQL-Backups\\DB-Backups\\mySQL Backups\\*, ";
			log << "\\\\nas3\\NOE_Docs$\\RALIM\\Logs\\mySQLbkpLogs\\, and backup" << endl;

			std::cout << "\nNOTE: folder path must end with \\* and the log path must end with \\ (use \\folder end\\\\)" << endl << endl;
			log << "\nNOTE: folder path must end with \\* and the log path must end with \\ (use \\folder end\\\\)" << endl << endl;

			std::cout << "printing incorrect arguments" << endl;
			log << "printing incorrect arguments" << endl;

			for (int i = 0; i < argc; i++) {
				std::cout << "[" << i << "] " << argv[i] << endl;
			}
			//log.close();//for debugging and testing
			//exit(EXIT_FAILURE);
		}

		scanFiles(folderPath, nameArg, log);//make the main call

		//cleanup the log and exit
		log.close();
	} catch (const ofstream::failure &e) {
		cout << "problem writing to log, exiting..." << endl;
		exit(EXIT_FAILURE);
	}
	std::cout << endl;
	//system("Pause");
	return EXIT_SUCCESS;
}

///<summary>This core function of the program will search the folder for names matching <c>nameArg</c>, 
///check if the file is more than a month old, and delete it if it is.</summary>
///<param name="folderPath"> The directory where the files to be scanned are </param>
///<param name="nameArg"> only consider files that contain this string (case insensitive) </param>
///<param name="log"> ofstream to write log entries to </param>
///<returns> void </returns>
void scanFiles(const string folderPath, const string nameArg, ofstream &log) {
	WIN32_FIND_DATA data;
	HANDLE hFind = FindFirstFile(folderPath.c_str(), &data);
	string path = folderPath.substr(0, folderPath.size() - 1);
	SYSTEMTIME fileST;
	SYSTEMTIME currST;

	if (hFind != INVALID_HANDLE_VALUE) {
		while (FindNextFile(hFind, &data)) {
			
			string fName = data.cFileName;
			string fullName = path + fName;

			log << "Checking " + fullName << endl;
			int NSIZE = fName.size();
			if (containsCaseInsensitive(fName, nameArg) && fName.substr(NSIZE -4, NSIZE) == ".sql") {

				FileTimeToSystemTime(&data.ftCreationTime, &fileST);
				GetSystemTime(&currST);
				__int64 diff = Delta(fileST, currST);
				diff = diff / 10000000;

				//if the file age is less than 1 month old
				if (diff < 2592000) {
					std::cout << fName << " is less than a month old, skipping" << endl;
					log << fName << " is less than a month old, skipping" << endl;
				} else {
					std::cout << fullName << " is older than 1 month, deleting ..." << endl;
					log << fullName << " is older than 1 month, deleting ..." << endl;
					//TODO: Add exception handling? Failure options?
					if (remove(fullName.c_str()) != 0) { //continue on to next file instead of stopping?
						std::cout << "Error deleting file!" << endl;
						log << "Error deleting file!" << endl;
					} else {
						std::cout << fullName << " deleted!" << endl;
						log << fullName << " deleted!" << endl;
					}
				}		
			}

			std::cout << "Finished with " << fullName << endl;
			log << "Finished with " << fullName << endl;
		}
		FindClose(hFind);
	}
	log << "Finished file cleanup at " << getCurrentDateTime() << endl; 
	log.close();
}
///<summary>computes the difference in milliseconds of two SYSTEMTIME objects</summary>
///<param name="st1">SYSTEMTIME start</param>
///<param name="st2">SYSTEMTIME end</param>
///<returns> __int64 of the difference in ms</returns>
__int64 Delta(const SYSTEMTIME st1, const SYSTEMTIME st2) {
	union timeunion {
		FILETIME fileTime;
		ULARGE_INTEGER ul;
	};

	timeunion ft1;
	timeunion ft2;

	SystemTimeToFileTime(&st1, &ft1.fileTime);
	SystemTimeToFileTime(&st2, &ft2.fileTime);

	return ft2.ul.QuadPart - ft1.ul.QuadPart;
}

///<summary>performs a case insensitive contains() function.</summary>
///<param name="str1">The text to be search (big)</param>
///<param name="str2">The text to see if str1 contains (small)</param>
///<returns>true if str1 cotains str2, false other wise</returns>
bool containsCaseInsensitive(string str1, string str2) {

	for (char &c : str1) {
		c = tolower(c);
	}

	for (char &c : str2) {
		c = tolower(c);
	}

	if (str1.find(str2) == string::npos) {
		return false;
	} else { return true; }
}

///<summary>Returns the current date and time in yyyymmdd_hhmmss format</summary>
///<returns>string of the date and time</returns>
string getCurrentDateTime() {
	stringstream currentDateTime;
	// current date/time based on current system
	time_t ttNow = time(0);
	tm  ptmNow;

	localtime_s(&ptmNow, &ttNow);

	currentDateTime << 1900 + ptmNow.tm_year;

	//month
	if (ptmNow.tm_mon < 9)
		//Fill in the leading 0 if less than 10
		currentDateTime << "0" << 1 + ptmNow.tm_mon;
	else
		currentDateTime << (1 + ptmNow.tm_mon);

	//day
	if (ptmNow.tm_mday < 10)
		currentDateTime << "0" << ptmNow.tm_mday << "_";
	else
		currentDateTime << ptmNow.tm_mday << "_";

	//hour
	if (ptmNow.tm_hour < 10)
		currentDateTime << "0" << ptmNow.tm_hour;
	else
		currentDateTime << ptmNow.tm_hour;

	//min
	if (ptmNow.tm_min < 10)
		currentDateTime << "0" << ptmNow.tm_min;
	else
		currentDateTime << ptmNow.tm_min;

	//sec
	if (ptmNow.tm_sec < 10)
		currentDateTime << "0" << ptmNow.tm_sec;
	else
		currentDateTime << ptmNow.tm_sec;

	return currentDateTime.str();
}