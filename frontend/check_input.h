#ifndef CHECK_FRONT_MAIN_H
#define CHECK_FRONT_MAIN_H

bool check_argc (int argc, const char **argv);
bool check_files (int num_of_files,...);
bool check_file (FILE *file, const char *filename);

#endif /* CHECK_FRONT_MAIN_H */
