#ifndef CONFIG_H
#define CONFIG_H

#define MAX_TERMINAL_LINES 30
#define MAX_EPC_LENGTH 25
#define TIMESTAMP_SECONDS_LENGTH 10

#define LINE_MAX_LENGTH (MAX_EPC_LENGTH + 1 + TIMESTAMP_SECONDS_LENGTH)

#define WHITELIST_FILE "../whitelist.txt"
#define DETECTED_TAGS_FILE "../detected_tags_epcs.txt"

#endif