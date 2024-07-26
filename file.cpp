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
			if (find_if(file.begin(), file.end(), [&temp](const fileZip& f) { return f.name == temp.name; }) == file.end()) {
				file.push_back(temp);
			}
		}
	}

	ifs.close();
}

bool sendFile(CSocket& server, string fileName) {
	ifstream ifs(fileName, ios::in | ios::binary);
	if (!ifs) {
		cerr << "Error occur while transfer the file!";
		return false;
	}
	// get the size of the file and send to client
	ifs.seekg(0, ios::end);
	long long allSize = ifs.tellg();
	server.Send(&allSize, sizeof(allSize), 0);

	// initialize variable
	vector<char> file(max_chunk);
	bool isSending = true;
	bool flag;

	// start sending the file
	ifs.seekg(0, ios::beg);
	while (!ifs.eof())
	{
		// get the chunk size and send to client
		int chunk_size = min(allSize - ifs.tellg(), max_chunk);
		server.Send(&chunk_size, sizeof(chunk_size), 0);

		// read and send the data, receive the signal 
		// the signal show if the data is correctly transfer
		ifs.read(file.data(), chunk_size);
		server.Send(file.data(), chunk_size, 0);
		server.Receive(&flag, sizeof(flag), 0);

		// if finish transfering the file
		if (allSize == ifs.tellg())
		{
			isSending = false;
			server.Send(&isSending, sizeof(isSending), 0);
			break;
		}

		server.Send(&isSending, sizeof(isSending), 0);
	}

	ifs.close();
	return true;
}