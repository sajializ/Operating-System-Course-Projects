#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <vector>
#include <string>

#define SIZE 1024

class Utils
{
public:
    static std::vector<const char*> get_filenames(const char* path)
    {
        std::vector<const char*> files;
        DIR *dir = opendir(path);
        struct dirent *entry = readdir(dir);
        while (entry != NULL)
        {
            if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
			    files.push_back(entry->d_name);
            entry = readdir(dir);
        }
        closedir(dir);
	    return files;
    }

    static void add_slash(char* s)
    {
	    size_t size = strlen(s);
	    s[size] = '/';
	    s[size + 1] = '\0';
    }

    static std::vector<const char*> get_folders(const char* path)
    {
	    std::vector<const char*> directories;
        DIR *dir = opendir(path);
        struct dirent *entry = readdir(dir);
        while (entry != NULL)
        {
            if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
			    directories.push_back(entry->d_name);
            entry = readdir(dir);
        }
        closedir(dir);
	    return directories;
    }

    static void write_to_pipe(const std::string& result)
    {
        char desc[SIZE] = {0};
        strcpy(desc, ("bin/pipes/" + std::to_string(getpid())).c_str());
	    int fd = open(desc, O_WRONLY);
        write(fd, result.c_str(), result.size() + 1);
    }

    static void create_pipe_and_read(std::vector<int>& prices, std::vector<std::string> not_finished_addrs, const size_t pid)
    {
        char result[SIZE];
		std::string addr = "bin/pipes/" + std::to_string(pid);
		mkfifo(addr.c_str(), 0666);
		int fd = open(addr.c_str(), O_RDONLY);
		read(fd, result, SIZE);
		if (strcmp(result, "") == 1)
			not_finished_addrs.push_back(addr);
		else
		{
			prices.push_back(atoi(result));
			close(fd);
		}
    }

    static void read_from_pipe(std::vector<int>& prices, const std::string& addr)
    {
        char result[SIZE];
		int fd = open(addr.c_str(), O_RDONLY);
		read(fd, result, SIZE);
		prices.push_back(atoi(result));
		close(fd);
    }

    static bool between(const std::string& start, const std::string& now, const std::string& finish)
    {
        return is_bigger(now, start) && is_bigger(finish, now);
    }

    static bool is_bigger(const std::string& first, const std::string& second)
    {
        if (atoi(first.substr(0, 4).c_str()) > atoi(second.substr(0, 4).c_str()))
            return true;
        if (atoi(first.substr(0, 4).c_str()) == atoi(second.substr(0, 4).c_str()))
        {
            if (atoi(first.substr(5,7).c_str()) > atoi(second.substr(5,7).c_str()))
                return true;
            if (atoi(first.substr(5,7).c_str()) == atoi(second.substr(5,7).c_str()))
            {
                if (atoi(first.substr(8,10).c_str()) >= atoi(second.substr(8,10).c_str()))
                    return true;
            }
        }
        return false;
    }
};