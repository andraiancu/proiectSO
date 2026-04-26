//structura report, prototipuri functii

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
//cum arata un raport
typedef struct {
    int report_id;
    char inspector_name[50];
    float latitude;
    float longitude;
    char category[30];
    int severity;
    time_t timestamp;
    char description[256];
} Report;
//definirea rolurilor
typedef enum {
    ROLE_NONE,
    ROLE_MANAGER,
    ROLE_INSPECTOR
} Role;

//headere de functii
void add_report(const char *district, const char *user, Role role);
void list_reports(const char *district, Role role);
int check_permission(const char *path, Role role, mode_t required_bit);
void get_mode_string(mode_t mode, char *str);
void view_report(const char *district, int id, Role role);
void remove_report(const char *district, int id, Role role);
void filter_reports(const char *district, const char *condition);
int parse_condition(const char *input, char *field, char *op, char *value);
int match_condition(Report *r, const char *field, const char *op, const char *value);
#endif
