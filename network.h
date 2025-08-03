#ifndef NETWORK_H
#define NETWORK_H

#include <map>
#include <string>

void usb_net_stop();
void usb_rndis_start();
void usb_ncm_start();
std::map<std::string, std::string> get_ip_addr();
#endif
