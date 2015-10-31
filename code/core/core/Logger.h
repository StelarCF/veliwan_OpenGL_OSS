#pragma once
#include <cstring>
#include <cstdio>

#define LOG_NULL 0 // never used (or used for TODOs)
#define LOG_EMERG 1 // Emergency -> will result in crash
#define LOG_ALERT 2 // Alerts -> might result in crash
#define LOG_CRIT 3 // Critical -> corruption
#define LOG_ERR 4 // Errors -> script errors, file read/write errors, any kind of non-crashing/non-corrupting error
#define LOG_ERROR LOG_ERR
#define LOG_WARN 5 // Warnings -> warnings, anything that may result in errors or that is not conforming to standard
#define LOG_NOTIFY 6 // Notification -> relatively important event happening
#define LOG_INFO 7 // Information -> unimportant event or data happening
#define LOG_DEBUG 8 // Debug -> self-explanatory, only in debug (prefferably commented through script on build)

#pragma clang diagnostic ignored "-Wc++11-compat-deprecated-writable-strings"
#pragma clang diagnostic ignored "-Wextern-initializer"

// TODO: Diversify what files Logger can write to



class Logger {
	char **logarr;
public:
	int current_log_level;
	Logger() {
		current_log_level = 4;
		logarr = new char*[9];
		logarr[0] = new char[15];
		strcpy(logarr[0], "LOG_NULL");
		logarr[1] = new char[15];
		strcpy(logarr[1], "Emergency");
		logarr[2] = new char[15];
		strcpy(logarr[2], "Alert");
		logarr[3] = new char[15];
		strcpy(logarr[3], "Critical");
		logarr[4] = new char[15];
		strcpy(logarr[4], "Error");
		logarr[5] = new char[15];
		strcpy(logarr[5], "Warning");
		logarr[6] = new char[15];
		strcpy(logarr[6], "Notification");
		logarr[7] = new char[15];
		strcpy(logarr[7], "Info");
		logarr[8] = new char[15];
		strcpy(logarr[8], "Debug");
	}
	void debug_log(const char *file, const char *func, int line, int level, const char *format, ...) {
		 /* Handle variable list arguments */
        va_list ap;
        va_start(ap, format);

        /* Make sure we have the correct level */
        if (level < 0 || level > LOG_DEBUG)
                return;

        /* All logs matching a level (and below) should be printed */
        if (level <= current_log_level) {
                /* Print filename,function, line no., log level */
        		#ifdef VELIWAN_DEBUG
                	fprintf(stderr, "[%s, %s(), ln %d] %s: ", file, func, line, logarr[level]);
                #else
                	if(level == LOG_DEBUG)
                		fprintf(stderr, "[%s, %s(), ln %d] %s: ", file, func, line, logarr[level]);
                	else
                		fprintf(stderr, "%s: ", logarr[level]);
                #endif
                /* Print the rest of the information */
                vfprintf(stderr, format, ap);
        }

        /* Reset variable list arguments before exit */
        va_end(ap);
	}
};

#define log(level, format, ...) "If you are getting an error here, it's because you haven't included GameCore.h or you don't have the gc-context"