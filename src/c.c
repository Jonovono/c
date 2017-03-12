#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
#include <dirent.h>

typedef int bool;
#define true 1
#define false 0

#define RESETCOLOR "\033[0m"
#define BLUE  "\033[1;4;22;34m"
#define COMMENT ".comment"
#define DOT "."

bool dirOrFileExists(const char *dir);
void makeDir(const char *path);
void printFiles(const char *path);
int fileOrDirectory(const char *path);
void checkComment(const char *file, const char *path);
void printComment(const char *filename, const char *path);
int length(const char *string);
void addComment(const char *file, char *path, const char *comment, bool append);
void printAllOrder(const char *path, bool desc);
void printUsage();
void printCurrentComment(const char *path);
void put_multiline(const char *s,int width);
void strip(char *s);

int main (int argc, const char *argv[]) {
	// Get the current directory
	char cwd[1024];
	if (!getcwd(cwd, sizeof(cwd))) {
		perror("getcwd() error");
	}

	int numargs, all = 0, asc = 0, desc = 0, push = 0, uk = 0, help = 0, current = 0;
	const char *c;

	numargs = argc;

	// TODO: Convert to getopt
	while (--numargs > 0) {
		c = argv[numargs];

		if (!strcmp(argv[numargs], "all"))
			all = 1;
		else if (!strcmp(argv[numargs], "-h") || !strcmp(argv[numargs], "--help"))
			help = 1;
		else if (!strcmp(argv[numargs], "."))
			current = 1;
		else if (!strcmp(argv[numargs], "-p") || !strcmp(argv[numargs], "--push"))
			push = 1;
		else if (!strcmp(argv[numargs], "-"))
			desc = 1;
		else if (!strcmp(argv[numargs], "+"))
			asc = 1;
		else
			uk++;
	}

	int args;
	args = argc;

	if (uk > 2) {
		printUsage();
		return EXIT_FAILURE;
	}

	if (args == 1) {
			printUsage();
			return EXIT_FAILURE;
	} else if (args == 2) {
		// Show the help menu
		if (help) {
			printUsage();
			return EXIT_FAILURE;
		}
		// Show all the files
		else if (all) {
			makeDir(cwd);
			printFiles(cwd);
		}
		// Show comment for the current directory
		else if (current) {
			// checkComment(argv[1], cwd);
			printCurrentComment(cwd);
		}
		// Show comment for the entered file
		else {
			// char *file = argv[1];
			makeDir(cwd);
			checkComment(argv[1], cwd);
		}
	} else if (args == 3) {
		if (current) {                               // Add comment to the current directory
			// char *file;
			// file = "CURRENT";
			addComment(argv[1], cwd, argv[2], false);
		} else if (all && desc) {                    // Print all files in descending order
			makeDir(cwd);
			printAllOrder(cwd, true);
		} else if (all && asc) {                     // Print all files in ascending order
			makeDir(cwd);
			printAllOrder(cwd, false);
		} else {                                     // Add comment to to given file
			makeDir(cwd);
			addComment(argv[1], cwd, argv[2], false);
		}
	} else if (args == 4) {
		if (current && push) { // Push comment to the current directory
			// Add a comment to the given filename
			addComment(argv[1], cwd, argv[3], true);
		} else if (push) {       // Push comment to given file
			makeDir(cwd);
			addComment(argv[1], cwd, argv[3], true);
		}
	} else {
		// Not certain
		printUsage();
	}
	return EXIT_SUCCESS;
}

//Checks if a directory exists.
bool dirOrFileExists(const char *dir) {
	struct stat st;
	if (stat(dir, &st) == 0) {
		return true;
	} else {
		return false;
	}
}

// Makes the comment directory at the path given
void makeDir(const char *path) {
	// Append this to the path of the directory we want to put it in
	char *s = NULL;
	asprintf(&s, "%s/" COMMENT, path);

	if (!dirOrFileExists(s)) {
		mkdir(s, S_IRWXU);
	}

	free(s);
}

// Prints all the files in the current directory
void printFiles(const char *path) {
	// Print the current directories comment
	printCurrentComment(path);
	printf("\n");

	DIR *mydir = opendir(path);
	struct dirent *entry = NULL;

	while((entry = readdir(mydir))) { // If we get EOF, the expression is NULL and the loop stops
		// We will not show the dot files
		if (entry->d_name[0] == '.') {
			continue;
		}
		checkComment(entry->d_name, path);
	}
	closedir(mydir);
}

// Checks if entered a file or directory
// Return 0 if directory
// Return 1 if File
// Else return -1
int fileOrDirectory(const char *path) {
	struct stat s;

	if (stat(path, &s) == 0) {
		if (s.st_mode & S_IFDIR) {
			// Is a directory
			return 0;
		} else if (s.st_mode & S_IFREG) {
			// Is a file
			return 1;
		}
	} else {
		// Error
		printf("Error occurred\n");
	}

	return -1;
}

// Checks if entered file/dir has a comment
// If so prints it. If not just prints file/dir name
void checkComment(const char *file, const char *path) {
	// First add the comment path to the end of the current path
	if (dirOrFileExists(file)) {
		char *dir = NULL;
		char *branch = NULL;
		const char *leaf = DOT;
		int ford = fileOrDirectory(file);
		if (ford == 0) {
			// Directory was entered
			// Appends the directory name to the end of current path
			asprintf(&branch, "%s/%s", path, file);
			makeDir(branch);
		} else if (ford == 1) {
			// File is entered
			// makeDir(path);

			branch = strdup(path);
			leaf = file;
		} else {
			// Unknown what was entered
			 printf("Not sure what to do here...");
			 return;
		}

		asprintf(&dir, "%s/" COMMENT "/%s.comment", branch, leaf);
		
		if (dirOrFileExists(dir)) {
			printComment(file, dir);
		} else {
			printf(BLUE "%s" RESETCOLOR "\n", file);
		}

		free(dir);
	} else {
		printf("Sorry cant find a file called %s\n", file);
	}
}

// Prints the comment for the current directory
void printCurrentComment(const char *path) {
	makeDir(path);
	// Append /.comment to the end of the current path
	// Append ..comment to the end of that path
	char *fin = NULL;
	asprintf(&fin, "%s/" COMMENT "/" DOT ".comment", path);

	if (dirOrFileExists(fin)) {
		// Open that file to print the conents
		FILE *fp;
		char ch;
		char newch[1000];
		fp = fopen(fin, "r");
		int i = 0;
		while (1) {
			ch = fgetc(fp);
			newch[i] = ch;
			if (ch == EOF) {
				newch[i] = '\0';
				break;
			}
			i++;
		}
		printf(BLUE "Current Directory:" RESETCOLOR "\t");
		put_multiline(newch, 50);
		printf("\n");

		fclose(fp);
	} else {
		printf(BLUE "Current directory" RESETCOLOR "\thas no comment\n");
	}

	free(fin);
}

// Print the comment for the given filename
void printComment(const char *filename, const char *path) {
	FILE *fp;
	char ch;
	char newch[1000];
	fp = fopen(path, "r");
	printf(BLUE "%s", filename);
	printf(RESETCOLOR "");
	int i = 0;
	printf("\t");
	while (1) {
		ch = fgetc(fp);
		newch[i] = ch;
		if (ch == EOF) {
			newch[i] = '\0';
			break;
		}
		// printf("%c", ch);
		i++;
	}
	// printf("%s\n", newch);
	put_multiline(newch, 50);
	printf("\n");

	fclose(fp);
}

// Adds a comment to the given file/directory
void addComment(const char *file, char *path, const char *comment, bool append) {
	if (dirOrFileExists(file)) {
		char *dir = NULL;
		const char *leaf = DOT;
		int ford = fileOrDirectory(file);
		if (ford == 0) {
			// Adds the file entered to the end of the current path
			asprintf(&dir, "%s/%s", path, file);
		} else if (ford == 1) {
			// File is entered
			dir = path;
			leaf = file;
		} else {
			// Unknown what was entered
			printf("Not sure what to do here...");
			return;
		}

		makeDir(path);

		char *full = NULL;
		asprintf(&full, "%s/" COMMENT "/%s.comment", dir, leaf);

		//I dont know why author made full NULL again (Maybe Its a Typo). It gives "c" Runtime Segmentation Fault.
		//However Code Works Just Fine When Line Below Is Commented.
		//free(full);

		FILE *fp;
		if (append) {
			fp = fopen(full, "a");
			fprintf(fp, " %s", comment);
		} else {
			fp = fopen(full, "w+");		
			fprintf(fp, "%s", comment);
		}

		// Free the pointer full
		free(full);
		fclose(fp);

		if (ford == 0) {
			free(dir);
		}
	} else {
		printf("Sorry cant find a file called %s\n", file);
	}
}

// Prints all the files in either ascending or descending order
void printAllOrder(const char *newpath, bool desc) {
	printCurrentComment(newpath);
	printf("\n");

	FILE *fp;
	char path[1035];

	/* Open the command for reading. */
	if (desc) {
		fp = popen("/bin/ls -tr  ", "r");
	} else {
		fp = popen("/bin/ls -t  ", "r");
	}
	if (fp == NULL) {
		printf("Failed to run command\n" );
		exit(EXIT_FAILURE);
	}

	/* Read the output a line at a time - output it. */
	while (fgets(path, sizeof(path)-1, fp) != NULL) {
		strip(path);
		// printf("%s", path);
		checkComment(path, newpath);
	}
	/* close */
	pclose(fp);
}

// Usage for the program
void printUsage() {
	fputs(BLUE "c\n" RESETCOLOR"  by @Jonovono - Add comments to files or directories.\n\n", stderr);
	fputs("Usage:\n", stderr);
	fputs("  c all to list all the files with their comments.\n", stderr);
	fputs("  c all (+ or -) to list all the files in ascending or descending order.\n", stderr);
	fputs("  c <filename> to view that files comment if there is one.\n", stderr);
	fputs("  c <filename> \"comment\" give the entered file the entered comment\n", stderr);
	fputs("  c <filename> -p \"{comment}\" to append comment to that files comment.\n", stderr);
	fputs("  Replace <filename> with . to do the commands on the current directory.\n\n", stderr);
}

// If comment is long will print it in a nice column, ish
void put_multiline(const char *s, int width) {
	// See tput cols or the COLUMNS env var
	int n, i = 0;
	char t[100];
	while(1 == sscanf(s,"%99s%n",t,&n)) {
		if(i + strlen(t) > width)
			puts(""), i=0, printf("\t");
		printf("%s%s",i?++i," ":"",t);i+=strlen(t);
		s += n;
	}
}

// Strip trailing new line or tab characters
void strip(char *s) {
	char *p2 = s;
	while(*s != '\0') {
		if(*s != '\t' && *s != '\n') {
			p2 = s;           // Move the second pointer if the current char is non-whitespace(newline/tab)
		}

        s++;                  // Increment current pointer
	}

    // It is always true that p < s
    // at the end, so it is safe to access
    // p2 + 1
	*(p2+1) = '\0';
}
