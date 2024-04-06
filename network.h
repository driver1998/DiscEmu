#ifndef NETWORK_H
#define NETWORK_H

#include <map>
#include <string>

void rndis_stop();
void rndis_start();
std::map<std::string, std::string> get_ip_addr();
#endif
