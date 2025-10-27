#include <iostream>
#include <string>
#include <unistd.h> // getopt on POSIX
#include <cstdlib>
#include <fstream>
#include <vector>
#include <sstream>

#define EMPLOYEE_LINE 1
#define GUEST_LINE 2
#define GALLERY_LINE 3


struct Args {
    std::string timestamp = "0";
    std::string key = "noKey";
    bool arrival = false;
    bool leaving = false;
    std::string roomNumber = "noRoomNumber";
    std::string employeeName = "noEName";
    std::string guestName = "noGName";
    std::string logFileName = "test.txt";
};

void usage(const char* prog) {
    std::cerr << "Usage: " << prog
              << " -T <timestamp> -K <key> (-A | -L) -R <roomNumber> -E <employeeName> -G <guestName> <logFileName>\n"
              << "  -T <timestamp>\n"
              << "  -K <key>\n"
              << "  -A    arrival (mutually exclusive with -L)\n"
              << "  -L    leaving (mutually exclusive with -A) (must be in room to leave)\n"
              << "  -R <room> (must be in gallery to arrive in room) \n"
              << "  -E <employee name>\n"
              << "  -G <guest name>\n"
              << "  -h    show this help\n";
}

bool stringExistsInLine(const std::string& filename, int lineNumber, const std::string& searchString) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int currentLine = 1;
    
    while (currentLine < lineNumber && std::getline(file, line)) {
        currentLine++;
    }
    
    if (currentLine == lineNumber && std::getline(file, line)) {
        std::vector<std::string> fields;
        std::stringstream ss(line);
        std::string field;
        
        while (std::getline(ss, field, ',')) {
            field.erase(0, field.find_first_not_of(" \t\n\r"));
            field.erase(field.find_last_not_of(" \t\n\r") + 1);
            
            if (field == searchString) {
                return true;
            }
        }
    }
    
    return false;
}

bool appendToLine(const std::string& filename, int lineNumber, const std::string& dataToAdd) {
    std::ifstream inFile(filename);
    std::ofstream tempFile("temp.txt");
    
    if (!inFile.is_open() || !tempFile.is_open()) {
        return false;
    }
    
    std::string line;
    int currentLine = 1;
    bool lineExists = false;
    
    while (std::getline(inFile, line)) {
        if (currentLine == lineNumber) {
            tempFile << line << dataToAdd << "," << '\n';
            lineExists = true;
        } else {
            tempFile << line << '\n';
        }
        currentLine++;
    }

    if (!lineExists) {

        while (currentLine < lineNumber) {
            tempFile << "\n";
            currentLine++;
        }

        tempFile << dataToAdd << "," << '\n';
    }
    
    inFile.close();
    tempFile.close();
    

    std::remove(filename.c_str());
    std::rename("temp.txt", filename.c_str());
    
    return true;
}

int addRoomNumber(const std::string& filename, std::string number) {
    std::ifstream inFile(filename);
    std::ofstream tempFile("temp.txt");
    
    if (!inFile.is_open() || !tempFile.is_open()) {
        return 0;
    }
    
    std::vector<std::string> allLines;
    std::string line;
    

    while (std::getline(inFile, line)) {
        allLines.push_back(line);
    }
    inFile.close();
    
    std::string numberPrefix = number + ":";
    

    bool numberExists = false;
    for (const auto& l : allLines) {
        if (l.find(numberPrefix) == 0) { 
            numberExists = true;
            break;
        }
    }
    
    if (numberExists) {
        return 0; 
    }
    

    inFile.open(filename);
    if (!inFile.is_open()) {
        return 0;
    }
    
    int currentLine = 1;
    int addedLineNumber = 0;
    std::string prevLine = "";
    bool prevLineWasBlank = false;
    int prevLineNumber = 0;
    
    while (std::getline(inFile, line)) {

        bool isBlank = true;
        for (char c : line) {
            if (!std::isspace(static_cast<unsigned char>(c))) {
                isBlank = false;
                break;
            }
        }
        

        if (isBlank && prevLineWasBlank && addedLineNumber == 0) {

            tempFile << numberPrefix << "\n";
            tempFile << "l" << number << ":\n";
            addedLineNumber = prevLineNumber;
        } else if (prevLineWasBlank && !isBlank && addedLineNumber == 0) {
            tempFile << "\n" << line << "\n";
        } else {
            tempFile << line << "\n";
        }
        
        prevLine = line;
        prevLineWasBlank = isBlank;
        prevLineNumber = currentLine;
        currentLine++;
    }

    if (prevLineWasBlank && addedLineNumber == 0) {
        
        inFile.clear();
        inFile.seekg(0);
        
        std::vector<std::string> lines;
        while (std::getline(inFile, line)) {
            lines.push_back(line);
        }
        
        if (lines.size() >= 2) {
            bool lastLineBlank = true;
            bool secondLastLineBlank = true;
            
            for (char c : lines[lines.size() - 1]) {
                if (!std::isspace(static_cast<unsigned char>(c))) {
                    lastLineBlank = false;
                    break;
                }
            }
            
            for (char c : lines[lines.size() - 2]) {
                if (!std::isspace(static_cast<unsigned char>(c))) {
                    secondLastLineBlank = false;
                    break;
                }
            }
            
            if (lastLineBlank && secondLastLineBlank) {

                inFile.close();
                tempFile.close();
                
                std::ifstream inFile2(filename);
                std::ofstream tempFile2("temp.txt");
                
                for (size_t i = 0; i < lines.size() - 2; ++i) {
                    tempFile2 << lines[i] << "\n";
                }
                tempFile2 << numberPrefix << "\n";
                tempFile2 << "l" << number << ":\n";
                addedLineNumber = lines.size() - 1;
                
                inFile2.close();
                tempFile2.close();
            }
        }
    }
    
    inFile.close();
    tempFile.close();
    
    // Replace original file with temp file
    std::remove(filename.c_str());
    std::rename("temp.txt", filename.c_str());
    
    return addedLineNumber;
}

bool deleteNameFromLine(const std::string& filename, int lineNumber, const std::string& nameToDelete) {
    std::ifstream inFile(filename);
    std::ofstream tempFile("temp.txt");
    
    if (!inFile.is_open() || !tempFile.is_open()) {
        return false;
    }
    
    std::string line;
    int currentLine = 1;
    bool foundAndDeleted = false;
    
    while (std::getline(inFile, line)) {
        if (currentLine == lineNumber) {
            std::vector<std::string> fields;
            std::stringstream ss(line);
            std::string field;
            
            // Split by commas and keep only non-matching fields
            while (std::getline(ss, field, ',')) {
                // For trailing comma case: check if field ends with the name
                // Remove any trailing whitespace first
                std::string trimmedField = field;
                trimmedField.erase(0, trimmedField.find_first_not_of(" \t\n\r"));
                trimmedField.erase(trimmedField.find_last_not_of(" \t\n\r") + 1);
                
                // Check if the trimmed field matches the name to delete
                // OR if the original field ends with the name (for trailing comma case)
                if (trimmedField != nameToDelete && 
                    !(field.length() >= nameToDelete.length() && 
                      field.substr(0, nameToDelete.length()) == nameToDelete &&
                      (field.length() == nameToDelete.length() || 
                       std::isspace(static_cast<unsigned char>(field[nameToDelete.length()]))))) {
                    fields.push_back(field);
                } else {
                    foundAndDeleted = true;
                }
            }
            
            // Reconstruct the line with commas
            if (!fields.empty()) {
                for (size_t i = 0; i < fields.size(); ++i) {
                    tempFile << fields[i];
                    if (i < fields.size() - 1) {
                        tempFile << ",";
                    }
                }
            }
            tempFile << '\n';
        } else {
            tempFile << line << '\n';
        }
        currentLine++;
    }
    
    inFile.close();
    tempFile.close();
    
    std::remove(filename.c_str());
    std::rename("temp.txt", filename.c_str());
    
    return foundAndDeleted;
}

int findRoomNumberLine(const std::string& filename, const std::string& number) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return 0; 
    }
    
    std::string line;
    int currentLine = 1;
    std::string searchPattern = number + ":";
    
    while (std::getline(file, line)) {
        if (line.find(searchPattern) == 0) {
            return currentLine;
        }
        currentLine++;
    }
    
    return 0;
}

void addLog(Args arguments) {
        // open log file, make sure its not currently open 
            // someHow check the key == correct key, not sure how
        // put a bunch of if statements here to check if things are correct
        //.... some things to do not necessarily in order \/\/\/
        // add ",employeeName" to first line in file
        // add ",guestName" to second line in file
        // if roomNumber = no roomNumber
            // add name to line that starts with G:, for gallery as a whole
        //if -A - R <roomNumber> is given, make sure name is in the line that starts with G:, if it isnt print error 
            // no one can arrive in a room without entering the gallery first
        // find line that starts with roomNumber
        // if -A==true, add ",name" to roomNumber line
        // if -L==true, remove ",name" from roomNumber line
            // if -L == true but ",name" is not on roomNumber line, throw error 255
	
	//key check

	// current probs:
	//     G: and #: still not appearing at beginning of line
	//     I don't think the l#: was showing either


        //add employeeName
	std::string name = "0";

	if(arguments.employeeName != "noEName")
	    name = arguments.employeeName;
	else
            name = arguments.guestName;
        
        std::cout << "Checking for name: " << name << std::endl;

	if(arguments.arrival && arguments.roomNumber != "noRoomNumber") {
            bool exists = stringExistsInLine(arguments.logFileName, GALLERY_LINE, name);
	    if(exists) {
		//add to room number line and remove from gallery line 
         	int lineNumber = addRoomNumber(arguments.logFileName, arguments.roomNumber);
	        appendToLine(arguments.logFileName, lineNumber, name);
		deleteNameFromLine(arguments.logFileName, GALLERY_LINE, name);
	    }
	    else {
		std::cerr << "Person is not in the gallery" << std::endl;
	    }
	}

	if(arguments.leaving && arguments.roomNumber != "noRoomNumber") {
	    int lineNumber = findRoomNumberLine(arguments.logFileName, arguments.roomNumber);
            bool exists = stringExistsInLine(arguments.logFileName, lineNumber, name);
	    if(exists) {
		//add to room number line and remove from gallery line 
	        appendToLine(arguments.logFileName, lineNumber + 1, name);
		deleteNameFromLine(arguments.logFileName, lineNumber, name);
	    }
	    else {
		std::cerr << "Error 255: Person is not in that room" << std::endl;
	    }
	}
	else if(arguments.leaving && arguments.roomNumber == "noRoomNumber") {
            bool exists = stringExistsInLine(arguments.logFileName, GALLERY_LINE, name);
	    if(exists) {
		//add to room number line and remove from gallery line 
	        appendToLine(arguments.logFileName, GALLERY_LINE + 1, name);
		deleteNameFromLine(arguments.logFileName, GALLERY_LINE, name);
	    }
	    else {
		std::cerr << "Error 255: Person is not in gallery room" << std::endl;
	    }
	}
        
	// DO TIMESTAMP
	if(arguments.employeeName != "noEName") {
                std::cout << "Entering employee add" << std::endl;
	    if(!stringExistsInLine(arguments.logFileName, EMPLOYEE_LINE, name)) {
		bool added = appendToLine(arguments.logFileName, EMPLOYEE_LINE, name);
		std::cout << "Adding?" << added << std::endl;
	    }
	}
	else
            if(!stringExistsInLine(arguments.logFileName, GUEST_LINE, name))
	        appendToLine(arguments.logFileName, GUEST_LINE, name);

	if(arguments.arrival && arguments.roomNumber == "noRoomNumber") 
            appendToLine(arguments.logFileName, GALLERY_LINE, name);
        
}

int main(int argc, char* argv[]) {
    Args args;
    int opt;

    // options that take arguments: T, K, R, E, G  (A and L are flags)
    const char* optstring = "T:K:ALR:E:G:h";

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
            case 'T':
                args.timestamp = optarg;
                break;
            case 'K':
                args.key = optarg;
                break;
            case 'A':
                args.arrival = true;
                break;
            case 'L':
                args.leaving = true;
                break;
            case 'R':
                args.roomNumber = optarg;
                break;
            case 'E':
                args.employeeName = optarg;
                break;
            case 'G':
                args.guestName = optarg;
                break;
            case 'h':
                usage(argv[0]);
                return 0;
            case '?':
            default:
                usage(argv[0]);
                return 1;
        }
    }
    // add checks to make sure only allowable usage, currently will result in some errors if improper inputs
    // ex: make sure -R is always followed buy <roomNumber
    // make sure -E and -G are always followed by <name>
    // must include -K <key>  

    // After options, expect a positional logFileName
    if (optind < argc) {
        args.logFileName = argv[optind];
    } else {
        std::cerr << "Error: missing <logFileName>\n";
        usage(argv[0]);
        return 1;
    }
    
    //Make sure log file if empty line 3 contains a G: before it.

    // Require key
    if (args.key == "noKey" || args.key.empty()) {
        std::cerr << "Error: missing required -K <key>\n";
        usage(argv[0]);
        return 1;
    }

    // Enforce mutually exclusive flags -A and -L
    if (args.arrival && args.leaving) {
        std::cerr << "Error: -A and -L are mutually exclusive\n";
        usage(argv[0]);
        return 1;
    } else if(args.leaving) {
        //logread -S given log
        //check for given name and room number
        //make sure there is an entry with -A -R <roomNumber> 
        //if not print error (log state does not align: name never entered room)
    }

    //add info to logfile, make sure things are correct. 
    addLog(args);

    // Example debug output
    std::cout << "Parsed arguments:\n";
    std::cout << "  timestamp:    " << args.timestamp << "\n";
    std::cout << "  key:          " << args.key << "\n";
    std::cout << "  arrival:      " << (args.arrival ? "yes" : "no") << "\n";
    std::cout << "  leaving:      " << (args.leaving ? "yes" : "no") << "\n";
    std::cout << "  roomNumber:   " << args.roomNumber << "\n";
    std::cout << "  employeeName: " << args.employeeName << "\n";
    std::cout << "  guestName:    " << args.guestName << "\n";
    std::cout << "  logFileName:  " << args.logFileName << "\n";

    return 0;
}
