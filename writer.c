#include <stdio.h>
#include <syslog.h>

int main(int argc, char** argv) {
	openlog(NULL, 0, LOG_USER);
	if (argc != 3) {
		syslog(LOG_ERR, "Invalid number of arguments: %d", argc);
	}
	FILE* file_ptr;
	file_ptr = fopen(argv[1], "w");
	if (file_ptr == NULL) {
		syslog(LOG_ERR, "File could not be created/opened.");
	}
	else {
		syslog(LOG_DEBUG, "Writing %s to %s", argv[1], argv[2]);
		file_ptr << argv[2];
		fclose(file_ptr);
	}
	return 0;
}
