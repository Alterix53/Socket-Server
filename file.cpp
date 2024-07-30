#include "File.h"

using namespace std;
const int max_chunk = 1024 * 128;
fileZip getFileInfo(string fileInfo) {

	// use stringstream for easier detach
	stringstream file(fileInfo);
	fileZip fileMake;
	string temp = "";

	// split the file name from the string
	getline(file, temp, ' ');
	fileMake.name = temp;

	// get the size and size type from the file
	fileMake.size = 0;
	getline(file, temp);
	int loop = 0;			// variable hold the position of the loop

	// detach the size from the temp string
	for (int i = 0; i < (int)temp.length() && isdigit(temp[i]); i++) {
		fileMake.size = fileMake.size * 10 + (temp[i] - '0');
		loop++;
	}

	// after the loop is complete
	// the loop variable points to the first position of the file type
	fileMake.sizeType = temp.substr(loop, temp.length() - loop + 1);

	return fileMake;
}

void readInputFile(string fileName, vector<fileZip>& file) {
	ifstream ifs(fileName);
	if (!ifs) {
		cerr << "Cannot open file: " << fileName << endl;
		exit(1);
	}

	string s;
	while (getline(ifs, s)) {
		if (!s.empty()) {
			fileZip temp = getFileInfo(s);
			file.push_back(temp);
		}
	}

	ifs.close();
}

bool sendFile(CSocket& server, string fileName) {
	ifstream ifs(fileName, ios::in | ios::binary);
	if (!ifs.is_open()) {
		cerr << "Error occur while transfer the file!";
		return false;
	}
	// get the size of the file and send to client
	ifs.seekg(0, ios::end);
	long long allSize = ifs.tellg();
	server.Send(&allSize, sizeof(allSize), 0);
	// int retryCount = 10;

	// initialize variable
	vector<char> file(max_chunk);
	bool isFinished = false;
	 // bool isSendingCorrectly;

	// start sending the file
	ifs.seekg(0, ios::beg);
	while (!ifs.eof())
	{
		// get the chunk size and send to client
		int Buffer = min(allSize - ifs.tellg(), max_chunk);
		// server.Send(&Buffer, sizeof(Buffer), 0);

		// read and send the data, receive the signal 
		// the signal show if the data is correctly transfer
		ifs.read(file.data(), Buffer);
		server.Send(file.data(), Buffer, 0);
		// server.Receive(&isSendingCorrectly, sizeof(isSendingCorrectly), 0);

		// if finish transfering the file
		if (allSize == ifs.tellg())
		{
			isFinished = true;
			//server.Send(&isFinished, sizeof(isFinished), 0);
			break;
		}

		//server.Send(&isFinished, sizeof(isFinished), 0);
	}

	ifs.close();
	return true;
}