#include <iostream>
#include <string>
#include <unistd.h> // getopt on POSIX
#include <cstdlib>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>
#include <sstream>
#include "keyAuthentication.h"
#include "aes_file_encryption_functions.h"

#define EMPLOYEE_LINE 1
#define GUEST_LINE 2
#define GALLERY_LINE 3
#define ROOM_NUMBER_COUNT 29

// allowable options:-K <key> -S -R (-E <employeeName> | -G <guestName>) logFileName
struct Args {
    std::string key = "noKey";
    bool state = false;
    bool rooms = false;
    std::string employeeName = "noEName";
    std::string guestName = "noGName";
    std::string logFileName = "test.txt";
    std::string decryptedFileName = "log_temp.txt";
};

void usage(const char* prog) {
    std::cerr << "Gallery Log Read: Prints the current state of the specified log file. Requires authentication key.\n";
    std::cerr << "Usage: " << prog << "\n"
              << " print gallery state:                              -K <key> -S <logfileName>\n" 
              << " print room employee or guest is currently in:     -K <key> -R (-E <employeeName> | -G <guestName>) <logFileName>\n"
              << "  -K <key>\n"
              << "  -S\n"
              << "  -R <room> (optional)(must be followed by -E OR -G)\n"
              << "  -E <employee name> (optional)\n"
              << "  -G <guest name> (optional)\n"
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

void printGalleryState(Args arguments){
    //print log state
    std::ifstream logFile(arguments.decryptedFileName);
    if (!logFile.is_open()) {
        std::cerr << "Error: Could not open log file " << std::endl;
        return;
    }

    std::string employeesInGallery;
    std::string guestsInGallery;
    std::map<int, std::vector<std::string>> roomOccupants;

    std::string line;
    while (std::getline(logFile, line)) {

        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        std::string prefix = line.substr(0, colonPos);
        std::string namesStr = line.substr(colonPos + 1);

        if (prefix == "employees in gallery line") {
            employeesInGallery = namesStr;
        }
        else if (prefix == "guests in gallery line") {
            guestsInGallery = namesStr;
        }
        else if (prefix.find("names in room") != std::string::npos) {

            std::string roomStr = prefix.substr(13);
            int roomNumber = std::stoi(roomStr);

            if (!namesStr.empty()) {
                std::vector<std::string> names;
                std::stringstream ss(namesStr);
                std::string name;
                while (std::getline(ss, name, ',')) {
                    if (!name.empty()) {
                        names.push_back(name);
                    }
                }
                // Sort names alphabetically
                std::sort(names.begin(), names.end());
                roomOccupants[roomNumber] = names;
            }
        }
    }
    logFile.close();

    std::cout << "Employees in gallery: " << employeesInGallery << std::endl;
    std::cout << "Guests in gallery: " << guestsInGallery << std::endl;
    for (const auto& [roomNumber, names] : roomOccupants) {
        std::cout << roomNumber << ": ";
        for (size_t i = 0; i < names.size(); ++i) {
            std::cout << names[i];
            if (i < names.size() - 1) {
                std::cout << ",";
            }
        }
        std::cout << std::endl;
    }
}

void printRoomEnteredByEmployeeOrGuest(Args arguments){
    // prints room currently occupied by specified employee or guest
    // Validate that either -E or -G is specified
    if (arguments.employeeName == "noEName" && arguments.guestName == "noGName") {
        std::cerr << "Error: Either -E or -G must be specified with -R" << std::endl;
        return;
    }
    std::string targetName;
    if (arguments.employeeName != "noEName") {
        targetName = arguments.employeeName;
    } else {
        targetName = arguments.guestName;
    }

    // Check which room the specified employee or guest is currently in
    bool exists = false;
    int inRoom = -1;
    // look for name in either employee or guest gallery line
    if(arguments.employeeName != "noEName") { exists = stringExistsInLine(arguments.decryptedFileName, EMPLOYEE_LINE, targetName); } 
    else { exists = stringExistsInLine(arguments.decryptedFileName, GUEST_LINE, targetName); }
        
    // check to see if they are in a room other than the gallery room
    // if they're not in the gallery
    if(!exists) {
        for(int i = GALLERY_LINE+1; i < 30; ++i) {
            if(stringExistsInLine(arguments.decryptedFileName, i, targetName)){ 
                inRoom = i - (GALLERY_LINE); // adjust room number to match actual room numbering
                exists = true;
                break;
            }
        }
    }
    if(exists && inRoom != -1){ std::cout << targetName << " is currently in Room: " << inRoom << std::endl; }
    else if(exists && inRoom == -1){
        std::cout << targetName << " is currently in Room: Gallery" << std::endl;
    } else {
        std::cerr << "Error: " << targetName << " is not currently in the gallery or any room. Make sure you are using -G and -E correctly." << std::endl;
    }
}

int main(int argc, char* argv[]){
    Args args;
    int opt;

    bool keyIsAuthenticated = false;

    // options that take arguments: K, R, E, G (S is a flag)
    // -T is ignored as timestamp is auto-generated
    const char* optstring = "T:K:SRE:G:h";

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
            case 'T':
                std::cerr << "Timestamp is auto-generated. Do not include -T argument.\n";
                usage(argv[0]);
                return 1;
            case 'K':
                //dont want to store key here
                keyIsAuthenticated = validateKey(std::string(optarg).substr(0, 255));
                if(keyIsAuthenticated){ args.key ="*****"; } //if key is authenticated, overwrite key in args for security, else leave as noKey
                else {args.key = "invalidKey";}
                break;
            case 'S':
                args.state = true; 
                break; 
            case 'R':
                args.rooms = true;
                break;
            case 'E':
                args.employeeName = std::string(optarg).substr(0, 127);
                break;
            case 'G':
                args.guestName = std::string(optarg).substr(0, 127);
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

    // After options, expect a positional logFileName
    if (optind < argc) {
        args.logFileName = argv[optind];
    } else {
        std::cerr << "Error: missing <logFileName>\n";
        usage(argv[0]);
        return 1;
    }

    // Require key
    if (args.key == "noKey" || args.key.empty()) {
        std::cerr << "Error: missing required -K <key>\n";
        usage(argv[0]);
        return 1;
    } else if (!keyIsAuthenticated && args.key == "invalidKey") {
        std::cerr << "Error: invalid key\n";
        return 1;
    } else {
        // means key is valid
    }

    //if args.state == true, print state
    std::ifstream testFile(args.logFileName);    // to see if file exists
    if (testFile.is_open()) {  // if the logfile exits, decrypt before operating on, store as temporary plaintext file
        if (!decryptFile(args.logFileName, args.decryptedFileName, AES_KEY)) {
            std::cerr << "Error: failed to decrypt log file\n";
            return 1;
        } 
    }
    testFile.close();

    //key is verified
    if(args.state == true)
	    printGalleryState(args); 

    //if args.state == false; 
    //if args.rooms and employee/guest name given correctly
    if(args.state == false)
	    printRoomEnteredByEmployeeOrGuest(args);

    // Encrypt back to original file
    if (!encryptFile(args.decryptedFileName, args.logFileName, AES_KEY)) {
        std::cerr << "Error: failed to encrypt log file\n";
        return 1;
    }
    
    // Clean up temporary plaintext file
    std::remove(args.decryptedFileName.c_str());
    return 0; 
}
