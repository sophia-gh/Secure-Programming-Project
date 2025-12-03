/*
*    logapend.cpp
*    
*    Parses command line arguments in the format: ./logappend -K <key> (-A | -L) -R <roomNumber> -E <employeeName> -G <guestName> <logFileName>
*    Securely appends the appropriate information to the appropriate lines in the log file.
*    Requires authentication key to proceed.
*    
*    Log File Format:
*    Line 1: employees in gallery line: names of all employees currently in any room in the gallery, comma separated
*    Line 2: guests in gallery line: names of all guests currently in any room in the gallery, comma separated
*    Line 3: gallery line: combined names of all employees and guests currently in any room in the gallery, comma separated
*    Line 4-28: names in room <room number>: combined names of all employees and guests currently in rooms 1-25, comma separated
*/

#include <iostream>
#include <string>
#include <unistd.h> // getopt on POSIX
#include <ctime> // for timestamp
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <sstream> 
#include "keyAuthentication.h" // containes validateKey function and the stored hash of the system authentication key


#define EMPLOYEE_LINE 1
#define GUEST_LINE 2
#define GALLERY_LINE 3
#define ROOM_NUMBER_COUNT 29

// Structure to hold parsed command line arguments, default values assigned
struct Args {
    std::string timestamp = "0";
    std::string key = "noKey";
    bool arrival = false;
    bool leaving = false;
    std::string roomNumber = "noRoomNumber";
    std::string employeeName = "noEName";
    std::string guestName = "noGName";
    std::string logFileName = "test.txt";
    std::string fullCommand = "none";
};

void usage(const char* prog) {
    std::cerr << "Gallery Log Append: Appends arrival and leaving information to the specified gallery log file. Requires authentication key.\n";
    std::cerr << "Usage: " << prog
              << " -K <key> (-A | -L) -R <roomNumber> (-E <employeeName> | -G <guestName>) <logFileName>\n"
              << "Options:\n"
              << "       -K <key>            TEXT: authentication key to grant access to the log\n"
              << "       -A                  arrival (mutually exclusive with -L)\n"
              << "       -L                  leaving (mutually exclusive with -A) (must be in room to leave)\n"
              << "       -R <room number>    TEXT: (max: 25) (must be in gallery to arrive in room, must be in room to leave) \n"
              << "       -E <employee name>  TEXT: name (mutually exclusive with -G)\n"
              << "       -G <guest name>     TEXT: name (mutually exclusive with -E)\n"
              << "                           (Guest/Employee name must not contain numbers or special characters.)\n"
              << "       -h                  show this help message\n\n";
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
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            line.erase(0, pos+1);
        }

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

bool appendLineToFile(const std::string& filename, const std::string& lineToAppend) {
    std::ifstream inFile(filename);
    std::ofstream tempFile("temp.txt");
    
    if (!inFile.is_open() || !tempFile.is_open()) {
        return false;
    }
    
    std::vector<std::string> allLines;
    std::string line;
    
    // Read all existing lines into vector
    while (std::getline(inFile, line)) {
        allLines.push_back(line);
    }
    inFile.close();
    
    allLines.push_back(lineToAppend);
    
    // Write all lines back to temp file
    for (const auto& l : allLines) {
        tempFile << l << "\n";
    }
    
    tempFile.close();
    
    // Replace original file
    std::remove(filename.c_str());
    std::rename("temp.txt", filename.c_str());
    
    return true;
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

bool deleteNameFromLine(const std::string& filename, int lineNumber, const std::string& nameToDelete) {
    std::ifstream inFile(filename);
    std::ofstream tempFile("temp.txt");
    
    if (!inFile.is_open() || !tempFile.is_open()) {
        return false;
    }
    
    auto trim = [](const std::string& s) {
        size_t a = s.find_first_not_of(" \t\r\n");
        if (a == std::string::npos) return std::string();
        size_t b = s.find_last_not_of(" \t\r\n");
        return s.substr(a, b - a + 1);
    };

    std::string line;
    int currentLine = 1;
    bool foundAndDeleted = false;
    
    while (std::getline(inFile, line)) {
        if (currentLine == lineNumber) {
            std::string prefix;
            std::string rest;
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                prefix = line.substr(0, colonPos + 1);
                rest = line.substr(colonPos + 1);
            } else {
                prefix.clear();
                rest = line;
            }

            std::vector<std::string> kept;
            std::stringstream ss(rest);
            std::string field;
            while (std::getline(ss, field, ',')) {
                std::string t = trim(field);
                if (t.empty()) continue; 
                if (t == nameToDelete && !foundAndDeleted) {
                    foundAndDeleted = true;
                } else {
                    kept.push_back(t);
                }
            }

            tempFile << prefix;
            if (!kept.empty()) {
                for (size_t i = 0; i < kept.size(); ++i) {
                    if (i) tempFile << ',';
                    tempFile << kept[i];
                }
                tempFile << ",\n";
            } else {
                tempFile << "\n";
            }
        } else {
            tempFile << line << '\n';
        }
        ++currentLine;
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
    std::string searchPattern = "names in room "+ number + ":";
    
    while (std::getline(file, line)) {
        if (line.find(searchPattern) == 0) {
            return currentLine;
        }
        currentLine++;
    }
    
    return 0;
}

void setUpFile(const std::string& filename) {
    // attempt to open the given filename as 'inFile'
    std::ifstream inFile(filename);

    // if the given filename does not already exist, attempt to create it
    if(!inFile.is_open()) {
        std::ofstream createFile(filename);
        if (!createFile) {
            std::cerr << "Error: Could not create log file " << filename << "\n";
            return;
        }
        createFile.close();
        // attempt to open the newly created file for reading
        inFile.open(filename);
        if (!inFile.is_open()) {
            std::cerr << "Error: Could not reopen log file " << filename << "\n";
            return;
        }
    }

    // open temporary file for writing
    std::ofstream tempFile("temp.txt");
    if(!tempFile.is_open()) {
        std::cerr << "Error: Could not create temporary file for setup.\n";
        return;
    }

    std::vector<std::string> allLines;
    std::string line;
    
    // Read all existing lines in inFile into vector
    while (std::getline(inFile, line)) {
        allLines.push_back(line);
    }
    inFile.close(); // close original file after reading

    // write required lines to temp file if they do not already exist
    size_t currentLine = 1;
    while (currentLine < ROOM_NUMBER_COUNT) {
        if (currentLine - 1 < allLines.size()) {
            line = allLines[currentLine - 1];
        } else {
            line = "";
        }

        if(currentLine == 1) {
            if(line.find("employees in gallery line:") == std::string::npos) {
                line.insert(0, "employees in gallery line:");
            }
        }
        else if(currentLine == 2) {
            if(line.find("guests in gallery line:") == std::string::npos) {
                line.insert(0, "guests in gallery line:");
            }
        }
        else if(currentLine == 3) {
            if(line.find("names in gallery line:") == std::string::npos) {
                line.insert(0, "names in gallery line:");
            }
        }
        else if(currentLine > 3) {
            std::string lineNumber = std::to_string(currentLine-3);
            std::string toSearch = "names in room " + lineNumber + ":";
            if(line.find(toSearch) == std::string::npos)
                line.insert(0, toSearch);
        }

        tempFile << line << '\n';
        ++currentLine;
    }

    for (size_t i = ROOM_NUMBER_COUNT - 1; i < allLines.size(); i++) {
        tempFile << allLines[i] << '\n';
    }
    tempFile.close();

    // Replace original file with temporary file and name it to the original filename
    std::remove(filename.c_str());
    std::rename("temp.txt", filename.c_str());
}

// Input validation for applicable arguments
void safeLog(Args arguments) {

    // Room Number Input Check
    // Must be 25 or less (maximum of 25 rooms)
    bool roomNumCorrect = true;
    if(arguments.roomNumber != "noRoomNumber") {
        for(size_t i = 0; i < arguments.roomNumber.size(); ++i) {
            // Make sure roomNumber is all numeric. Also checks for negatives
            // which would contain '-'.
            if(!std::isdigit(arguments.roomNumber[i]))
                roomNumCorrect = false;
        } 
        try {
            int roomNum = std::stoi(arguments.roomNumber);
            // Max room number = 25
            if(roomNum > 25)
                roomNumCorrect = false;
        } catch(const std::invalid_argument& e) {
            roomNumCorrect = false;
        }
    }
    if(!roomNumCorrect) {
        std::cerr << "Incorrect Room Number Value. Correct usage: -R <roomNum> (max: 25)" << std::endl;
        exit(1);
    }

    // Employee / Guest Name Validation
    // Name cannot have numbers or special symbols in it
    // User can not provide both guest and employee name.
    if(arguments.employeeName != "noEName" && arguments.guestName != "noGName") {
        std::cerr << "Employee and Guest Name are mutually exclusive. Use '-h' for help." << std::endl;
        exit(1);
    }
    
    //store given name 
    std::string name = "";
    if(arguments.employeeName != "noEName") {
        name = arguments.employeeName;
    } else {
        name = arguments.guestName;
    }

    bool nameCorrect = true;
    //name must not inlude numbers or special characters
    std::string invalidNameCharacters = "1234567890_~`\";:/<>,!@#$%^&*()+=|\\?\n";
    for(size_t i = 0; i < name.size(); ++i) {
        for(size_t j = 0; j < invalidNameCharacters.size(); ++j) {
            if(name[i] == invalidNameCharacters[j]) {
                // if name contains invalid characters, it isn't correct
                nameCorrect = false;
            }
        }
    }
    if(!nameCorrect) {
        std::cerr << "Invalid Employee / Guest Value. Name must not contain numbers or special characters." << std::endl;
        exit(1);
    }
}

void addLog(Args arguments) {
    //Store name based on whether employee or guest in variable 'name'
	std::string name = "0";
	if(arguments.employeeName != "noEName") { name = arguments.employeeName; } 
    else { name = arguments.guestName; }

    // if using: ./logappend -K <key> -A -R <number> (-E|-G) <Name> <logFileName> 
    // user must be in the gallery to arrive in room <number>
    // user can arrive in another room from current room without
	if(arguments.arrival && arguments.roomNumber != "noRoomNumber") {  // user is arriving in a specific room
        bool exists = false;
        int inRoom = -1;
        // look for name in either employee or guest gallery line
        if(arguments.employeeName != "noEName") { exists = stringExistsInLine(arguments.logFileName, EMPLOYEE_LINE, name); } 
        else { exists = stringExistsInLine(arguments.logFileName, GUEST_LINE, name); }
        
        // check to see if they are in a room other than the gallery room
	    for(int i = GALLERY_LINE+1; i < 30; ++i) {
	        if(stringExistsInLine(arguments.logFileName, i, name)){ 
                inRoom = i; 
                exists = true;
                break;
            }
        }
        
        // if a user is in either the employee or guest gallery line, then they are in the gallery room or a specific room
        // if a user is in a specific room already, they can move to another room directly
	    if(exists) {
            // if name is in a numbered room already, remove from that room before adding to new room
            if(inRoom != -1) { deleteNameFromLine(arguments.logFileName, inRoom, name); }
            
            // add name to new room line
            int lineNumber = findRoomNumberLine(arguments.logFileName, arguments.roomNumber);
	        appendToLine(arguments.logFileName, lineNumber, name);
            
            // remove name from gallery and employee or guest line
		    deleteNameFromLine(arguments.logFileName, GALLERY_LINE, name);
            if(arguments.employeeName != "noEName") {
                deleteNameFromLine(arguments.logFileName, EMPLOYEE_LINE, name);
            } else {
                deleteNameFromLine(arguments.logFileName, GUEST_LINE, name);
            } 
        }
	    else {
		    std::cerr << "Invalid Usage. Use -h for usage information." << std::endl;
            exit(1);
	    }
	}

    // if using ./logappend -K <key> -L -R <number> (-E|-G) <Name> <logFileName>
    // user is leaving a specific room <number>
    // user will be added back to gallery line and employee or guest line
	if(arguments.leaving && arguments.roomNumber != "noRoomNumber") {
	    int lineNumber = findRoomNumberLine(arguments.logFileName, arguments.roomNumber);
        bool exists = stringExistsInLine(arguments.logFileName, lineNumber, name);
	    if(exists) {
		    //remove from room line and add to gallery line
		    deleteNameFromLine(arguments.logFileName, lineNumber, name);
            appendToLine(arguments.logFileName, GALLERY_LINE, name);
            if(arguments.employeeName != "noEName") {
                appendToLine(arguments.logFileName, EMPLOYEE_LINE, name);
            } else {
                appendToLine(arguments.logFileName, GUEST_LINE, name);
            }
	    }
	    else {
		    std::cerr << "Invalid Usage. Use -h for usage information." << std::endl;
            exit(1);
	    }
	}
	else if(arguments.leaving && arguments.roomNumber == "noRoomNumber") {
        // user is leaving the gallery entirely 
        
        // check that name is in the gallery line
        bool exists = stringExistsInLine(arguments.logFileName, GALLERY_LINE, name);
        // make sure the name is also in either employee or guest line, and that the wrong tag isnt being used for the wrong type of person
        // ex: no -E guestName or -G employeeName
        if(arguments.employeeName != "noEName") { 
            if (stringExistsInLine(arguments.logFileName, EMPLOYEE_LINE, name)) { exists = exists; } 
            else {
                std::cerr << "Invalid Usage. Use -h for usage information." << std::endl;
                exit(1);
            } 
        } 
        else {  
            if (stringExistsInLine(arguments.logFileName, GUEST_LINE, name)) { exists = exists; }
            else {
                std::cerr << "Invalid Usage. Use -h for usage information." << std::endl;
                exit(1);
            }
        }

	    //Checking if they are also in a room
        int inRoom = -1;
	    for(int i = GALLERY_LINE+1; i < 30; ++i) {
	        if(stringExistsInLine(arguments.logFileName, i, name))
			inRoom = i;
	    }

        // if name is in the gallery line and the -E and -G tag matches their type, move on to remove name from any other line/room it appears in
	    if(exists) {
		// If they are in that room (not -1) then remove them
		    if(inRoom != -1)
		    	deleteNameFromLine(arguments.logFileName, inRoom, name);
                if(arguments.employeeName != "noEName") {
                    deleteNameFromLine(arguments.logFileName, EMPLOYEE_LINE, name);
                } else {
                    deleteNameFromLine(arguments.logFileName, GUEST_LINE, name);
                }
		        deleteNameFromLine(arguments.logFileName, GALLERY_LINE, name);
	    }
	    else {
		    std::cerr << "Invalid Usage. Use -h for usage information." << std::endl;
            exit(1);
	    }
	}
        
    // only add name to gallery and employee/guest line if the name does not already exist in those line
	if(arguments.employeeName != "noEName" && arguments.arrival && arguments.roomNumber == "noRoomNumber") {
        if(!stringExistsInLine(arguments.logFileName, EMPLOYEE_LINE, name)) {
		    appendToLine(arguments.logFileName, EMPLOYEE_LINE, name);
            appendToLine(arguments.logFileName, GALLERY_LINE, name);
        }
	} else if(arguments.employeeName == "noEName" && arguments.arrival && arguments.roomNumber == "noRoomNumber") {
        if(!stringExistsInLine(arguments.logFileName, GUEST_LINE, name)) {
	        appendToLine(arguments.logFileName, GUEST_LINE, name);
            appendToLine(arguments.logFileName, GALLERY_LINE, name);
        }
    }
    // Finally, append the full command to the end of the log file for record keeping
    appendLineToFile(arguments.logFileName, arguments.fullCommand);
}

int main(int argc, char* argv[]) {
    Args args;
    int opt;
    if(argc == 1) {
        usage(argv[0]);
        return(1);
    }

    bool keyIsAuthenticated = false;
    // options that take arguments: K, R, E, G  (A and L are flags)
    // timestamp is auto generated but if a user supplies the -T argument an error message is shown
    // -B batch file log append is not implemented
    const char* optstring = "T:K:BALR:E:G:h";

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        case 'B':
            std::cerr << "Batch file log append is not implemented. Do not include -B argument.\n";
            usage(argv[0]);
            return 1;
        case 'T':
            std::cerr << "Timestamp is auto-generated. Do not include -T argument.\n";
            usage(argv[0]);
            return 1;
        case 'K':
            //dont want to store key here
            keyIsAuthenticated = validateKey(std::string(optarg).substr(0, 255));
            if(keyIsAuthenticated){ args.key ="*****"; } //if key is valid, overwrite key in args for security, else leave as noKey
            else {args.key = "invalidKey";} //makes sure that no invalid keys are stored as those inputs may be malicious, instead just store as invalidKey
            break;
        case 'R':
            args.roomNumber = std::string(optarg).substr(0, 3);
            break;
        case 'E':
            args.employeeName = std::string(optarg).substr(0, 127);
            break;
        case 'G':
            args.guestName = std::string(optarg).substr(0, 127);
            break;
        case 'A':
            args.arrival = true;
            break;
        case 'L':
            args.leaving = true;
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

    // Generate timestamp of current time in
    // format: 'yyyy-mm-dd/hh:mm:ss'
    std::time_t rawtime;
    std::time(&rawtime);

    struct tm* timeinfo = std::localtime(&rawtime);
    
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d/%H:%M:%S", timeinfo);
    
    args.timestamp = std::string(buffer);

    // After options, expect a positional logFileName
    if (optind < argc) {
        args.logFileName = argv[optind];
    } else {
        std::cerr << "Error: missing <logFileName>\n";
        usage(argv[0]);
        return 1;
    }
    
    

    // Require key
    if (args.key == "noKey" || args.key.empty()) {         //if key is somehow never overwritten, it means no key was given
        std::cerr << "Error: missing required -K <key>\n";
        usage(argv[0]);
        return 1;
    } else if (!keyIsAuthenticated && args.key == "invalidKey") {  
        std::cerr << "Error: invalid key\n";
        return 1;
    } else {
        // means key is valid
    }

    //only after key is validated
    setUpFile(args.logFileName);
    
    // Enforce mutually exclusive flags -A and -L
    if (args.arrival && args.leaving) {
        std::cerr << "Error: -A and -L are mutually exclusive\n";
        usage(argv[0]);
        return 1;
    }

    // Validate input to program, exits if incorrect.
    safeLog(args);
    // Add info to log
    args.fullCommand = args.timestamp + " " + argv[0] + " " + (args.employeeName != "noEName" ? "Employee " + args.employeeName : "Guest " + args.guestName) + " " 
                        + (args.arrival ? "arrival" : "") + (args.leaving ? "leaving" : "") + " " + (args.roomNumber != "noRoomNumber" ? "room " + args.roomNumber : "gallery");
    addLog(args);

    // Example debug output
    /* std::cout << "Parsed arguments:\n";
    std::cout << "  timestamp:    " << args.timestamp << "\n";
    std::cout << "  key:          " << args.key << "\n";
    std::cout << "  arrival:      " << (args.arrival ? "yes" : "no") << "\n";
    std::cout << "  leaving:      " << (args.leaving ? "yes" : "no") << "\n";
    std::cout << "  roomNumber:   " << args.roomNumber << "\n";
    std::cout << "  employeeName: " << args.employeeName << "\n";
    std::cout << "  guestName:    " << args.guestName << "\n";
    std::cout << "  logFileName:  " << args.logFileName << "\n"; */

    return 0;
}
