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
void makeDir(char *path);
void printFiles(char *path);
int fileOrDirectory(const char *path);
void checkComment(const char *file, char *path);
void printComment(const char *filename, char *path);
int length(const char *string);
void addComment(const char *file, char *path, const char *comment, bool append);
void printAllOrder(char *path, bool desc);
void printUsage();
void printCurrentComment(char *path);
void put_multiline(const char *s,int width);
void strip(char *s);
char *allocFilename(const char *format, const char *base, const char *leaf);
void freeFilename(char *filename);

int main (int argc, const char *argv[]) {
	// Get the current directory
	char cwd[1024];
	if (!getcwd(cwd, sizeof(cwd)))
		perror("getcwd() error");	

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
		return 1;
	}

	switch (args) {
		// Show the comment for the current directory
		case 1:
				printUsage();
				return 1;
				break;
		case 2: 
				// Show the help menu
				if (help) {
					printUsage();
					return 1;
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
				break;
		case 3: 
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
				break;

		case 4: 
				if (current && push) { // Push comment to the current directory
					// Add a comment to the given filename
					addComment(argv[1], cwd, argv[3], true);
				} else if (push) {       // Push comment to given file
					makeDir(cwd);
					addComment(argv[1], cwd, argv[3], true);
				}
				break;

				// Not certain
		default:
				printUsage();
	}
	return 0;
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
void makeDir(char *path) {
	// Append this to the path of the directory we want to put it in
	char *s = allocFilename("%s/%s", path, COMMENT);

	if (!dirOrFileExists(s)) {
		mkdir(s, S_IRWXU);
	}

	freeFilename(s);
}

// Prints all the files in the current directory
void printFiles(char *path) {
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
		} else {
			// Something else
			return -1;
		}
	} else {
		// Error
		printf("Error occurred\n");
	}
}

// Checks if entered file/dir has a comment
// If so prints it. If not just prints file/dir name
void checkComment(const char *file, char *path) {
	char *dir;
	char *s;

	// First add the comment path to the end of the current path
	if (dirOrFileExists(file)) {
		int ford = fileOrDirectory(file);
		switch(ford) {
			// Directory was entered
			case 0: 
				// Appends the directory name to the end of current path
				dir = allocFilename("%s/%s", path, file);
				makeDir(dir);

				// Add .comment directory to that path
				char *commentDir = allocFilename("%s/%s", dir, COMMENT);

				// Ands ..comment to that path to get the comment for that directory
				char *fin = allocFilename("%s/%s.comment", commentDir, DOT);

				if (dirOrFileExists(fin)) {
					printComment(file, fin);
				} else {
					printf(BLUE "%s" RESETCOLOR "\n", file);
				}

				freeFilename(commentDir);
				freeFilename(fin);
				freeFilename(dir);
				break;
				// File is entered
			case 1:	
				// makeDir(path);
				// Adds /.comment to the end of the current path
				s = allocFilename("%s/%s", path, COMMENT);

				//Next add the file to the end of that path
				char *r = allocFilename("%s/%s.comment", s, file);


				if (dirOrFileExists(r)) {
					printComment(file, r);
				} else {
					printf(BLUE "%s" RESETCOLOR "\n", file);
				}

				freeFilename(s);
				freeFilename(r);
				break;
				// Unknown what was entered
			default:
					 printf("Not sure what to do here...");
					 break;
		}
	} else {
		printf("Sorry cant find a file called %s\n", file);
	}
}

// Prints the comment for the current directory
void printCurrentComment(char *path) {
	makeDir(path);
	// Append /.comment to the end of the current path
	char *dir = allocFilename("%s/%s", path, COMMENT);

	// Append ..comment to the end of that path
	char *fin = allocFilename("%s/%s.comment", dir, DOT);

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

	freeFilename(dir);
	freeFilename(fin);
}

// Print the comment for the given filename
void printComment(const char *filename, char *path) {
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
	char *dir;

	if (dirOrFileExists(file)) {
		int ford = fileOrDirectory(file);
		switch(ford) {
			// File was entered
			case 0: 
				// Adds the file entered to the end of the current path
				dir = allocFilename("%s/%s", path, file);
				// Creates the directory .comment at that path
				makeDir(dir);

				// Add .comment directory to the path
				char * commentDir = allocFilename("%s/%s", dir, COMMENT);

				// Add ..comment in that folder
				char * fin = allocFilename("%s/%s.comment", commentDir, DOT);

				FILE *fp2;
				if (append) {
					fp2 = fopen(fin, "a");
					fprintf(fp2, " %s", comment);
				} else {
					fp2 = fopen(fin, "w+");		
					fprintf(fp2, "%s", comment);
				}
				fclose(fp2);

				freeFilename(dir);
				freeFilename(commentDir);
				freeFilename(fin);

				break;
				// File is entered
			case 1:
					makeDir(path);

					// This will get a string with the path/.comment
					// Append this to the path of the directory we want to put it in
					char * s = allocFilename("%s/%s", path, COMMENT);

					// Now add the file we want to that
					char * full = allocFilename("%s/%s.comment", s, file);

					FILE *fp;
					if (append) {
						fp = fopen(full, "a");
						fprintf(fp, " %s", comment);
					} else {
						fp = fopen(full, "w+");		
						fprintf(fp, "%s", comment);
					}
					fclose(fp);

					freeFilename(s);
					freeFilename(full);
					break;
					// Unknown what was entered
			default: printf("Not sure what to do here...");
					 break;
		}
	} else {
		printf("Sorry cant find a file called %s\n", file);
	}
}

// Prints all the files in either ascending or descending order
void printAllOrder(char *newpath, bool desc) {
	printCurrentComment(newpath);
	printf("\n");

	FILE *fp;
	int status;
	char path[1035];

	/* Open the command for reading. */
	if (desc) {
		fp = popen("/bin/ls -tr  ", "r");
	} else {
		fp = popen("/bin/ls -t  ", "r");
	}
	if (fp == NULL) {
		printf("Failed to run command\n" );
		exit;
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
	int n, i = 0;
	char t[100];
	while(1 == sscanf(s,"%99s%n",t,&n)) {
		if(i + strlen(t) > width)
			puts(""), i=0, printf("\t");
		printf("%s%s",i?++i," ":"",t);i+=strlen(t);
		s += n;
	}
}

void strip(char *s) {
	char *p2 = s;
	while(*s != '\0') {
		if(*s != '\t' && *s != '\n') {
			*p2++ = *s++;
		} else {
			++s;
		}
	}
	*p2 = '\0';
}

char *allocFilename(const char *format, const char *base, const char *leaf ) {
	char *s = malloc(snprintf(NULL, 0, format, base, leaf) + 1);
	sprintf(s, format, base, leaf);
	return s;
}

void freeFilename(char *filename) {
	free(filename);
}
