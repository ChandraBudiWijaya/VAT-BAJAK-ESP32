#ifndef SD_MANAGER_H
#define SD_MANAGER_H

void setup_sd_card();
void check_and_create_logfile(const char* filename);
void save_data_to_sd(const char* filename, const char* timestamp, float d1, float d2, float lat, float lon, float depth);

#endif // SD_MANAGER_H