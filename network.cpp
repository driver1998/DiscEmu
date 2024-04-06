
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string>
#include <string_view>
#include <unistd.h>

#include "network.h"
#include "usb.h"

void rndis_start() {
  usb_gadget_add_rndis();
  usb_gadget_start();
  usleep(500 * 1000);
  system("ifconfig usb0 192.168.42.1");
  system("/etc/init.d/S80dnsmasq restart");
}

void rndis_stop() {
  usb_gadget_stop();
  system("/etc/init.d/S80dnsmasq stop");
  usleep(500 * 1000);
}

std::map<std::string, std::string> get_ip_addr() {
  struct ifaddrs *interfaces = nullptr;
  struct ifaddrs *addr = nullptr;

  std::map<std::string, std::string> ip_map;

  int result = getifaddrs(&interfaces);
  if (result == 0) {
    struct ifaddrs *curr = interfaces;
    while (curr) {
      if (curr->ifa_addr && curr->ifa_addr->sa_family == AF_INET &&
          std::strcmp(curr->ifa_name, "lo")) {
        auto if_name = std::string(curr->ifa_name);
        auto ip_addr =
            std::string(inet_ntoa(((sockaddr_in *)curr->ifa_addr)->sin_addr));
        ip_map.insert(
            std::map<std::string, std::string>::value_type(if_name, ip_addr));
      }
      curr = curr->ifa_next;
    }
    freeifaddrs(interfaces);
  }
  return ip_map;
}