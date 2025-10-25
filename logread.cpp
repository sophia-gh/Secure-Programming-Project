#include <iostream>
#include <string>
#include <unistd.h> // getopt on POSIX
#include <cstdlib>

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
    std::cerr << "Usage: " << prog
              << " print gallery state:                    -K <key> -S <logfileName>\n" 
              << " print list of people in specified room: -K <key> -R <roomNumber> (-E <employeeName> | -G <guestName>) <logFileName>\n"
              << "  -K <key>\n"
              << "  -S \n"
              << "  -R <room> (optional)(must be followed by -E OR -G) \n"
              << "  -E <employee name> (optional)\n"
              << "  -G <guest name> (optional)\n"
              << "  -h    show this help\n";
}

void printGalleryState(){
    //print log state
}

void printRoomEnteredByEmployeeOrGuest(){
    //yeah print that stuff idk >:3
}

int main(int argc, char* argv[]){
    Args args;
    int opt;

    // options that take arguments: K, R, E, G (S is a flag)
    const char* optstring = "K:SR:E:G:h";

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
            case 'K':
                args.key = optarg;
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
    }

    //if args.state == true, print state
    //key is verified
    printGalleryState(); 

    //if args.state == false; 
    //if args.roomNumber and employee/guest name given correctly
    printRoomEnteredByEmployeeOrGuest();

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