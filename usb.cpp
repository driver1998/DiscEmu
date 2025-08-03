#include <cstdlib>
#include <string>
#include <filesystem>
#include "usb.h"

void usb_switch_to_device_mode() {
    system("/etc/uhubon.sh device");
}

void usb_gadget_add_msc(std::filesystem::path block_dev) {
    std::filesystem::path path_absolute = std::filesystem::absolute(block_dev);
    system(("./run_usb.sh probe msc '" + path_absolute.string() + "'").c_str());
}

void usb_gadget_add_cdrom(std::filesystem::path iso_path) {
    std::filesystem::path path_absolute = std::filesystem::absolute(iso_path);
    system(("./run_usb.sh probe cdrom '" + path_absolute.string() + "'").c_str());
}

void usb_gadget_add_cdrom() {
    system("./run_usb.sh probe cdrom");
}

void usb_gadget_start() {
    system("./run_usb.sh start");
}

void usb_gadget_stop() {
    system("./run_usb.sh stop");    
}

void usb_gadget_add_rndis() {
    system("./run_usb.sh probe rndis");
}

void usb_gadget_add_ncm() {
    system("./run_usb.sh probe ncm");
}

void usb_gadget_add_mtp() {
    system("./run_usb.sh probe mtp");
}