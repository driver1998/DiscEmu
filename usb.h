#ifndef USB_H
#define USB_H

#include <filesystem>
void usb_switch_to_device_mode();
void usb_gadget_stop();
void usb_gadget_start();
void usb_gadget_add_cdrom(std::filesystem::path iso_path);
void usb_gadget_add_msc(std::filesystem::path block_dev);
void usb_gadget_add_rndis();
#endif
