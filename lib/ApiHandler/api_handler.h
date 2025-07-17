#ifndef API_HANDLER_H
#define API_HANDLER_H

void setup_modem();
bool connect_gprs();
void sendDataToAPI(float dist1, float dist2, float lat, float lon, float alt, const char* timestamp);

#endif // API_HANDLER_H