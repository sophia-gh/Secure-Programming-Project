#include <iostream>
#include <string>
#include <unistd.h> // getopt on POSIX
#include <cstdlib>

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