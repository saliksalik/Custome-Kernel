#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include<readline/history.h>
#include<readline/readline.h>
#include<map>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <chrono>
#include <thread>
#include <cstring>
#include<sys/wait.h>
#include <filesystem>
#include <unordered_set>

using namespace std;
namespace fs = std::filesystem;

#define MAXCOM 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported

#define clear() printf("\033[H\033[J")

// Function prototypes
void init_shell();
void displayWelcomeMessage();
void displayPrompt(const string& prompt);
vector<string> parseCommand(const string& commandLine);
void addToCommandHistory(const std::string& command);
void displayCommandHistory();
void printDir();
void listFiles(const string& directory);
void createDirectory(const string& directoryName);
void changeDirectory(const string& directory);
void createFile(const string& fileName);
int writeFile(const string& content, const string& fileName);
void countWordsLinesCharacters(const string& fileName);
void copyFile(const std::string& source, const std::string& destination);
void moveFile(const std::string& source, const std::string& destination);
void deleteFile(const string& fileName);
void listProcesses();
void killProcess(const string& processID);
void findProcessByName(const string& processName);
void displaySystemInfo();
void head(const std::string& fileName, int lines );
void tail(const std::string& fileName, int lines );
void sort(const std::string& fileName);
void uniq(const std::string& fileName);
void diff(const std::string& file1, const std::string& file2);
void grep(const std::string& pattern, const std::string& fileName);
int handleSh(char** parsed);
int handleChmod(char** parsed);
void executeCommand(const vector<string>& tokens);
void execArgs(char** parsed);
void execArgsPiped(char** parsed, char** parsedpipe);
int parsePipe(char* str, char** strpiped);
int ownCmdHandler(char** parsed);
void openHelp();


//main function
int main() {
    displayWelcomeMessage();
    init_shell(); // Initialize shell

    char input[MAXCOM];
    string commandLine;
   
    while (true) {
        printDir();
        displayPrompt("$");
        getline(cin, commandLine);
        addToCommandHistory(commandLine);
        vector<string> tokens = parseCommand(commandLine);
        if (!tokens.empty() && tokens[0] == "exit") {
            break; // Exit the loop
        }
        if (!tokens.empty() && tokens[0] == "clear") {
            system("clear");
        }

        // Check if kernel-related command should be executed
        if (tokens.size() == 1 && (tokens[0] == "systeminfo" || tokens[0] == "ps" || tokens[0] == "kill" || tokens[0] == "find")) {
            executeCommand(tokens);
        } else {
            // For other commands, execute normally
            executeCommand(tokens);
        }
    }

    return 0;
}
// Function to initialize the shell
void init_shell() {
   
    std::cout << "\n\n\n\n******************"
              << "************************";
    std::cout << "\n\n\n\t****MY SHELL****";
    std::cout << "\n\n\t-USE AT YOUR OWN RISK-";
    std::cout << "\n\n\n\n*******************"
              << "***********************" << std::endl;
    char* username = getenv("USER");
    std::cout << "\n\n\nUSER is: @" << username << std::endl;
    sleep(1);
   
}
//welcome message
void displayWelcomeMessage() {
    cout << "Welcome to MyShell!" << endl;
    cout << "Type 'help' for assistance." << endl;
}

void displayPrompt(const string& prompt) {
    cout << prompt << " ";
}

vector<string> parseCommand(const string& commandLine) {
    vector<string> tokens;
    string commandCopy = commandLine; // Make a copy of the commandLine string
    string token;
    size_t pos = 0;
    while ((pos = commandCopy.find(' ')) != string::npos) {
        token = commandCopy.substr(0, pos);
        tokens.push_back(token);
        // Erase the token including the space
        commandCopy.erase(0, pos + 1);
    }
    // Add the remaining part of the command line
    if (!commandCopy.empty()) {
        tokens.push_back(commandCopy);
    }
    return tokens;
}

std::vector<std::string> commandHistory;
void addToCommandHistory(const std::string& command) {
    commandHistory.push_back(command);
}

void displayCommandHistory() {
    std::cout << "Command History:" << std::endl;
    for (size_t i = 0; i < commandHistory.size(); ++i) {
        std::cout << "[" << i+1 << "]: " << commandHistory[i] << std::endl;
    }
}
// Function to print current directory
void printDir() {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd)); // Get current working directory
    std::cout << "\nDir: " << cwd << std::endl;
}
//this function will list files present in current directory
void listFiles(const string& directory) {
    DIR* dir;
    struct dirent* entry;

    if ((dir = opendir(directory.c_str())) != nullptr) {
        std::cout << "Files in directory '" << directory << "':" << std::endl;
        while ((entry = readdir(dir)) != nullptr) {
            std::cout << entry->d_name << std::endl;
        }
        closedir(dir);
    } else {
        std::cerr << "Error: Unable to open directory '" << directory << "'" << std::endl;
    }
}
//create directory with mkdir linux command
void createDirectory(const string& directory) {
    if (mkdir(directory.c_str(), 0777) == 0) {
        std::cout << "Directory '" << directory << "' created successfully" << std::endl;
    } else {
        std::cerr << "Error: Unable to create directory '" << directory << "'" << std::endl;
    }
}
//change directory
void changeDirectory(const string& directory) {
    if (chdir(directory.c_str()) == 0) {
        std::cout << "Changed directory to '" << directory << "'" << std::endl;
    } else {
        std::cerr << "Error: Unable to change directory to '" << directory << "'" << std::endl;
    }
}

//file creation with touch linux command
void createFile(const string& fileName) {
    int input;
    string content;
    std::ofstream file(fileName);
    if (file.is_open()) {
        std::cout << "File '" << fileName << "' created successfully" << std::endl;
        file.close();
        cout<<"enter 1 to write contents to file"<<endl;
        cin>>input;
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (input == 1) {
            cout << "Enter content to write to the file (press Enter then Ctrl+D to finish):\n";
            getline(cin, content);
            writeFile(content, fileName);
       }
        else{
        cout<<"No contents are written to file"<<endl;}
}
     else {
        std::cerr << "Error: Unable to create file '" << fileName << "'" << std::endl;
    }
   
}
//write contents to file
int writeFile(const string& content, const string& fileName) {
    string userinput;
    string command = "echo \"" + content + "\" > " + fileName;
    system(command.c_str());
    cout<<"content written succesfully"<<endl;
    cout<<"use wc kernal command to count words in "<<fileName<<endl;
    cin>>userinput;
    if(userinput=="wc"){
    countWordsLinesCharacters(fileName);
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return 1;
    }
    else{
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return 0;
}
return 1;
}

// Function to count words, lines, and characters in files (wc)
void countWordsLinesCharacters(const string& fileName) {
    ifstream file(fileName);
    if (file.is_open()) {
        string line;
        int wordCount = 0;
        int lineCount = 0;
        int charCount = 0;
        while (getline(file, line)) {
            lineCount++;
            charCount += line.size();
            istringstream iss(line);
            string word;
            while (iss >> word) {
                wordCount++;
            }
        }
        cout << "Words: " << wordCount << ", Lines: " << lineCount << ", Characters: " << charCount << endl;
        file.close();
    } else {
        cerr << "Error: Unable to open file '" << fileName << "'" << endl;
    }
}
//copy one file contents to other using cp linux command
void copyFile(const std::string& source, const std::string& destination) {
    try {
        fs::copy(source, destination, fs::copy_options::recursive);
        std::cout << "Copied: " << source << " to " << destination << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}
// move files using mv linux command
void moveFile(const std::string& source, const std::string& destination) {
    try {
        fs::rename(source, destination);
        std::cout << "Moved: " << source << " to " << destination << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}
//delete file with rm linux command
void deleteFile(const string& fileName) {
    if (remove(fileName.c_str()) == 0) {
        std::cout << "File '" << fileName << "' deleted successfully" << std::endl;
    } else {
        std::cerr << "Error: Unable to delete file '" << fileName << "'" << std::endl;
    }
}
//list processes using ps linux command
void listProcesses() {
    std::system("ps aux");
}
//kill process using kill linux command
void killProcess(const string& processID) {
    std::string command = "kill " + processID;
    std::system(command.c_str());
}
//this function will find processes with find linux command
void findProcessByName(const string& processName) {
   std::string command = "pgrep " + processName;
    std::cout << "Processes with name '" << processName << "':" << std::endl;
    int result=std::system(command.c_str());
    if (result != 0) {
    std::cerr << "Command execution failed with exit code: " << result << std::endl;
}
}
//this function will display system info with systeminfo linux command
void displaySystemInfo() {
     std::ifstream cpuInfo("/proc/cpuinfo");
    std::string line;
    int cpuCount = 0;
    std::string cpuModel;

    if (cpuInfo.is_open()) {
        while (std::getline(cpuInfo, line)) {
            if (line.find("processor") != std::string::npos) {
                cpuCount++;
            } else if (line.find("model name") != std::string::npos) {
                size_t pos = line.find(":");
                if (pos != std::string::npos) {
                    cpuModel = line.substr(pos + 2); // Extract CPU model name
                }
            }
        }
        cpuInfo.close();
    }

    std::cout << "CPU count: " << cpuCount << std::endl;
    std::cout << "CPU model: " << cpuModel << std::endl;

    // Memory information
    long pageSize = sysconf(_SC_PAGE_SIZE);
    long totalMemory = sysconf(_SC_PHYS_PAGES) * pageSize;
    long freeMemory = sysconf(_SC_AVPHYS_PAGES) * pageSize;

    std::cout << "Total memory: " << totalMemory / (1024 * 1024) << " MB" << std::endl;
    std::cout << "Free memory: " << freeMemory / (1024 * 1024) << " MB" << std::endl;

}


//head to display first lines
void head(const std::string& fileName, int lines = 10) {
    std::ifstream file(fileName);
    if (file.is_open()) {
        std::string line;
        for (int i = 0; i < lines && std::getline(file, line); ++i) {
            std::cout << line << std::endl;
        }
        file.close();
    } else {
        std::cerr << "Error: Unable to open file '" << fileName << "'" << std::endl;
    }
}

// Function to implement tail command
void tail(const std::string& fileName, int lines = 10) {
    std::ifstream file(fileName);
    if (file.is_open()) {
        std::vector<std::string> buffer(lines);
        size_t count = 0;
        while (std::getline(file, buffer[count % lines])) {
            count++;
        }
        size_t start = count > lines ? (count % lines) : 0;
        size_t size = std::min(count, static_cast<size_t>(lines));
        for (size_t i = 0; i < size; ++i) {
            std::cout << buffer[(start + i) % lines] << std::endl;
        }
        file.close();
    } else {
        std::cerr << "Error: Unable to open file '" << fileName << "'" << std::endl;
    }
}

// Function to implement sort command
void sort(const std::string& fileName) {
    std::ifstream file(fileName);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::vector<std::string> elements;
            std::string element;
            while (iss >> element) {
                elements.push_back(element);
            }
            std::sort(elements.begin(), elements.end());
            for (const auto& sortedElement : elements) {
                std::cout << sortedElement << " ";
            }
            std::cout << std::endl;
        }
        file.close();
    } else {
        std::cerr << "Error: Unable to open file '" << fileName << "'" << std::endl;
    }
}

void uniq(const std::string& fileName) {
    std::ifstream file(fileName);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            unordered_set<std::string> uniqueElements;
            std::string element;
            while (iss >> element) {
                uniqueElements.insert(element);
            }
            for (const auto& uniqueElement : uniqueElements) {
                std::cout << uniqueElement << " ";
            }
            std::cout << std::endl;
        }
        file.close();
    } else {
        std::cerr << "Error: Unable to open file '" << fileName << "'" << std::endl;
    }
}
// Function to implement diff command
void diff(const std::string& file1, const std::string& file2) {
    std::ifstream f1(file1), f2(file2);
    if (f1.is_open() && f2.is_open()) {
        std::string line1, line2;
        while (std::getline(f1, line1) && std::getline(f2, line2)) {
            if (line1 != line2) {
                std::cout << "< " << line1 << std::endl;
                std::cout << "> " << line2 << std::endl;
            }
        }
        while (std::getline(f1, line1)) {
            std::cout << "< " << line1 << std::endl;
        }
        while (std::getline(f2, line2)) {
            std::cout << "> " << line2 << std::endl;
        }
        f1.close();
        f2.close();
    } else {
        std::cerr << "Error: Unable to open files '" << file1 << "' and '" << file2 << "'" << std::endl;
    }
}
// Function to implement grep command to find pattern
void grep(const std::string& pattern, const std::string& fileName) {
    std::ifstream file(fileName);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.find(pattern) != std::string::npos) {
                std::cout << line << std::endl;
            }
        }
        file.close();
    } else {
        std::cerr << "Error: Unable to open file '" << fileName << "'" << std::endl;
    }
}

//function to handle sh command
int handleSh(char** parsed) {
    if (parsed[1] == NULL) {
        std::cerr << "Usage: sh script.sh" << std::endl;
        return -1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            std::cout << "Shell script execution completed with status: " << WEXITSTATUS(status) << std::endl;
        }
    }

    return 0;
}

//function to handle chmod command
int handleChmod(char** parsed) {
    if (parsed[1] == NULL || parsed[2] == NULL) {
        std::cerr << "Usage: chmod mode filename" << std::endl;
        return -1;
    }

    mode_t mode = strtol(parsed[1], NULL, 8);
    if (chmod(parsed[2], mode) == -1) {
        perror("chmod");
        return -1;
    }

    std::cout << "Permissions changed successfully for '" << parsed[2] << "'" << std::endl;
    return 0;
}
//this function will execute commands
void executeCommand(const vector<string>& tokens) {
    if (tokens[0] == "exit") {
        exit(0);
    } else if (tokens[0] == "history") {
        displayCommandHistory();
    } else if (tokens[0] == "ls") {
        if (tokens.size() > 1) {
            listFiles(tokens[1]);
        } else {
            listFiles(".");
        }
    } else if (tokens[0] == "cd") {
        if (tokens.size() > 1) {
            changeDirectory(tokens[1]);
        } else {
            cout << "Usage: cd <directory>" << endl;
        }
    } else if (tokens[0] == "mkdir") {
        if (tokens.size() > 1) {
            createDirectory(tokens[1]);
        } else {
            cout << "Usage: mkdir <directory>" << endl;
        }
    } else if (tokens[0] == "touch") {
        if (tokens.size() > 1) {
            createFile(tokens[1]);
        } else {
            cout << "Usage: touch <file>" << endl;
        }
    } else if (tokens[0] == "rm") {
        if (tokens.size() > 1) {
            deleteFile(tokens[1]);
        } else {
            cout << "Usage: rm <file>" << endl;
        }
    } else if (tokens[0] == "systeminfo") {
        displaySystemInfo();
    }
     else if (tokens[0] == "ps") {
        listProcesses();
    } else if (tokens[0] == "kill") {
        if (tokens.size() > 1) {
            killProcess(tokens[1]);
        } else {
            cout << "Usage: kill <processID>" << endl;
        }
    } else if (tokens[0] == "find") {
        if (tokens.size() > 1) {
            findProcessByName(tokens[1]);
        } else {
            cout << "Usage: find <processName>" << endl;
        }
        }
       
        else if (tokens[0] == "head") {
        if (tokens.size() > 1) {
            if (tokens.size() > 2) {
                head(tokens[1], stoi(tokens[2]));
            } else {
                head(tokens[1]);
            }
        } else {
            cout << "Usage: head <filename> [lines]" << endl;
        }
    } else if (tokens[0] == "tail") {
        if (tokens.size() > 1) {
            if (tokens.size() > 2) {
                tail(tokens[1], stoi(tokens[2]));
            } else {
                tail(tokens[1]);
            }
        } else {
            cout << "Usage: tail <filename> [lines]" << endl;
        }
    } else if (tokens[0] == "grep") {
        if (tokens.size() > 2) {
            grep(tokens[1], tokens[2]);
        } else {
            cout << "Usage: grep <pattern> <filename>" << endl;
        }
    } else if (tokens[0] == "sort") {
        if (tokens.size() > 1) {
            sort(tokens[1]);
        } else {
            cout << "Usage: sort <filename>" << endl;
        }
    } else if (tokens[0] == "uniq") {
        if (tokens.size() > 1) {
            uniq(tokens[1]);
        } else {
            cout << "Usage: uniq <filename>" << endl;
        }
    } else if (tokens[0] == "diff") {
        if (tokens.size() > 2) {
            diff(tokens[1], tokens[2]);
        } else {
            cout << "Usage: diff <file1> <file2>" << endl;
        }}
    else {
        cout << "Command not found: " << tokens[0] << endl;
    }
}


// Function to execute single command
void execArgs(char** parsed) {
    pid_t pid = fork(); // Fork a child process
    if (pid == -1) {
        std::cout << "\nFailed forking child.." << std::endl;
        return;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed) < 0) { // Execute command in child process
            std::cout << "\nCould not execute command.." << std::endl;
        }
        exit(0);
    } else {
        wait(NULL); // Wait for child process to finish
        return;
    }
}

// Function to execute piped commands
void execArgsPiped(char** parsed, char** parsedpipe) {
    int pipefd[2];
    pid_t p1, p2;

    if (pipe(pipefd) < 0) { // Create pipe
        std::cout << "\nPipe could not be initialized" << std::endl;
        return;
    }

    p1 = fork(); // Fork first child
    if (p1 < 0) {
        std::cout << "\nCould not fork" << std::endl;
        return;
    }

    if (p1 == 0) { // Child process 1
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (execvp(parsed[0], parsed) < 0) { // Execute first command
            std::cout << "\nCould not execute command 1.." << std::endl;
            exit(0);
        }
    } else { // Parent process
        p2 = fork(); // Fork second child
        if (p2 < 0) {
            std::cout << "\nCould not fork" << std::endl;
            return;
        }

        if (p2 == 0) { // Child process 2
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);

            if (execvp(parsedpipe[0], parsedpipe) < 0) { // Execute second command
                std::cout << "\nCould not execute command 2.." << std::endl;
                exit(0);
            }
        } else { // Parent process
            wait(NULL);
            wait(NULL); // Wait for both children to finish
        }
    }
}

// Function to parse piped commands
int parsePipe(char* str, char** strpiped) {
    int i;
    for (i = 0; i < 2; i++) {
        strpiped[i] = strsep(&str, "|"); // Split string by pipe character
        if (strpiped[i] == NULL)
            break;
    }

    if (strpiped[0] == NULL)
        return -1;

    return 0;
}
// Function to handle built-in commands
int ownCmdHandler(char** parsed) {
    int i, switchOwnArg = 0;
    char* swArg[MAXLIST];

    for (i = 0; i < MAXLIST; i++) {
        swArg[i] = parsed[i];
        if (parsed[i] == NULL)
            break;
        if (strcmp(swArg[0], "exit") == 0)
            exit(0);
        if (strcmp(swArg[0], "cd") == 0) { // Change directory
            chdir(swArg[1]);
            return 1;
        }
        if (strcmp(swArg[0], "help") == 0) { // Display help
            openHelp();
            return 1;
        }
        if (strcmp(swArg[0], "ls") == 0) { // List files in directory
            system("ls");
            return 1;
        }
        if (strcmp(swArg[0], "pwd") == 0) { // List files in directory
            printDir();
            return 1;
        }
        if (strcmp(swArg[0], "mkdir") == 0) { // Create directory
           createDirectory(parsed[1]);
            return 1;
        }
        if (strcmp(swArg[0], "rm") == 0) { // Remove file or directory
            deleteFile(parsed[1]);
            return 1;
        }
        if (strcmp(swArg[0], "touch") == 0) { // Create file
            createFile(parsed[1]);
            return 1;
        }
        if (strcmp(swArg[0], "systeminfo") == 0) { // Display system information
            displaySystemInfo();
            return 1;
        }
        if (strcmp(swArg[0], "ps") == 0) { // List processes
            listProcesses();
            return 1;
        }
        if (strcmp(swArg[0], "kill") == 0) { // Kill process
            killProcess(swArg[1]);
            return 1;
        }
        if (strcmp(parsed[0], "cp") == 0) { // Copy file or directory
            if (parsed[1] != NULL && parsed[2] != NULL) {
                copyFile(parsed[1], parsed[2]);
                return 1;
            } else {
                std::cerr << "Usage: cp <source> <destination>" << std::endl;
                return -1;
            }
        }
        if (strcmp(parsed[0], "mv") == 0) { // Move file or directory
            if (parsed[1] != NULL && parsed[2] != NULL) {
                moveFile(parsed[1], parsed[2]);
                return 1;
            } else {
                std::cerr << "Usage: mv <source> <destination>" << std::endl;
                return -1;
            }
        }  
        if (strcmp(parsed[0], "writefile") == 0) {
        // Check if arguments are provided
        if (parsed[1] == NULL || parsed[2] == NULL) {
            std::cerr << "Usage: writefile <content> <filename>" << std::endl;
            return -1;
        }
        writeFile(parsed[1], parsed[2]);
        return 1;
    }
   
        if (strcmp(parsed[0], "chmod") == 0) {         //handle chmod
        return handleChmod(parsed);
        }
        if(strcmp(parsed[0], "sh") == 0) {             //handle sh
        return handleSh(parsed);}
        switchOwnArg = i;
    }

    if (switchOwnArg == 1) {
        std::cout << "\nMissing arguments" << std::endl;
        return -1;
    }

    return 0;
}
// Function to display help information
void openHelp() {
    std::cout << "\n***WELCOME TO MY SHELL HELP***"
              << "\n-Use the shell at your own risk..."
              << "\nList of Commands supported:"
              << "\n>cd"
              << "\n>ls"
              << "\n>exit"
              << "\n>all other general commands available in UNIX shell"
              << "\n>pipe handling"
              << "\n>improper space handling" << std::endl;
    return;
}