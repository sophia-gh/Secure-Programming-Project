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

// allowable options:-K <key> -S -R <roomNumber> (-E <employeeName> | -G <guestName>) logFileName
struct Args {
    std::string key = "noKey";
    bool state = false;
    std::string roomNumber = "noRoomNumber";
    std::string employeeName = "noEName";
    std::string guestName = "noGName";
    std::string logFileName = "test.txt";
};

void usage(const char* prog) {
    std::cerr << "Gallery Log Read: Prints the current state of the specified log file. Requires authentication key.\n";
    std::cerr << "Usage: " << prog
              << " print gallery state:                              -K <key> -S <logfileName>\n" 
              << " print list of people currently in specified room: -K <key> -R <roomNumber> (-E <employeeName> | -G <guestName>) <logFileName>\n"
              << "  -K <key>\n"
              << "  -S\n"
              << "  -R <room> (optional)(must be followed by -E OR -G)\n"
              << "  -E <employee name> (optional)\n"
              << "  -G <guest name> (optional)\n"
              << "  -h    show this help\n";
}

void printGalleryState(Args arguments){
    //print log state
    std::ifstream logFile(arguments.logFileName);
    if (!logFile.is_open()) {
        std::cerr << "Error: Could not open log file " << arguments.logFileName << std::endl;
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


    std::cout << employeesInGallery << std::endl;


    std::cout << guestsInGallery << std::endl;


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

    std::ifstream logFile(arguments.logFileName);
    if (!logFile.is_open()) {
        std::cerr << "Error: Could not open log file " << arguments.logFileName << std::endl;
        return;
    }

    std::vector<int> roomsEntered;
    std::string line;

    while (std::getline(logFile, line)) {
        // Find the colon separator
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        std::string prefix = line.substr(0, colonPos);
        std::string namesStr = line.substr(colonPos + 1);

        if (prefix.find("names in room") != std::string::npos) {

            std::string roomStr = prefix.substr(13); 
            int roomNumber = std::stoi(roomStr);
            

            if (!namesStr.empty()) {
                std::stringstream ss(namesStr);
                std::string name;
                bool found = false;
                
                while (std::getline(ss, name, ',')) {
                    if (name == targetName) {
                        found = true;
                        break;
                    }
                }
                
                if (found) {
                    roomsEntered.push_back(roomNumber);
                }
            }
        }
    }

    logFile.close();

    // Print rooms in chronological order (as they appear in the log)
    if (roomsEntered.empty()) {
        std::cout << std::endl; // Print empty line if no rooms found
    } else {
        for (size_t i = 0; i < roomsEntered.size(); ++i) {
            std::cout << roomsEntered[i];
            if (i < roomsEntered.size() - 1) {
                std::cout << ",";
            }
        }
        std::cout << std::endl;
    }
}

int main(int argc, char* argv[]){
    Args args;
    int opt;

    bool keyIsAuthenticated = false;

    // options that take arguments: K, R, E, G (S is a flag)
    const char* optstring = "K:SR:E:G:h";

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
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
    // if -S make sure no other arguments are given, must be [...] -S <logFileName>
    // ect ....

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
    //key is verified
    if(args.state == true)
	    printGalleryState(args); 

    //if args.state == false; 
    //if args.roomNumber and employee/guest name given correctly
    if(args.state == false)
	    printRoomEnteredByEmployeeOrGuest(args);

    // Example debug output
    std::cout << "Parsed arguments:\n";
    std::cout << "  key:          " << args.key << "\n";
    std::cout << "  state:      " << (args.state ? "yes" : "no") << "\n";
    std::cout << "  roomNumber:   " << args.roomNumber << "\n";
    std::cout << "  employeeName: " << args.employeeName << "\n";
    std::cout << "  guestName:    " << args.guestName << "\n";
    std::cout << "  logFileName:  " << args.logFileName << "\n";

    return 0; 
}
