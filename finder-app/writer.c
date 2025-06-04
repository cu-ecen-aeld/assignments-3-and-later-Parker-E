#include <stdio.h>
#include <syslog.h>

int main (int argc, char** argv) {
	openlog(NULL, 0, LOG_USER);
	if (argc != 3) {
		syslog(LOG_ERR, "Invalid number of arguments: %d", argc);
		return 1;
	} else {
		FILE* file_ptr;
		file_ptr = fopen(argv[1], "w");
		if (file_ptr == NULL) {
			syslog(LOG_ERR, "File could not be opened");
			return 1;
		}
		else {
			syslog(LOG_DEBUG, "Writing %s to %s", argv[2], argv[1]);
			fputs(argv[2], file_ptr);
			fclose(file_ptr);
		}
	}
	closelog();
	return 0;
}
