#ifndef SENSORS_H
#define SENSORS_H

void setup_sensors();
void read_ultrasonic_sensors();
void read_gps_data();
void display_sensor_data();
float calculate_depth();

extern float distance1;
extern float distance2;
extern float latitude;
extern float longitude;
extern float altitude;
extern TinyGPSPlus gps;

#endif // SENSORS_H