#include <algorithm>
#include <any>
#include <cstdarg>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <unistd.h>

#include <libu8g2arm/U8g2Controller.h>
#include <libu8g2arm/U8g2lib.h>
#include <libu8g2arm/u8g2.h>
#include <libu8g2arm/u8g2arm.h>
#include <libu8g2arm/u8x8.h>

#include "input.h"
#include "menu.h"
#include "network.h"
#include "usb.h"

const std::string version = "0.0.1";
u8g2_t u8g2 = {};

int action_rndis(std::any arg);
int action_cdrom_emu(std::any arg);
int action_file_browser(std::any arg);
int action_status(std::any arg);
int action_shutdown(std::any arg);
int action_restart(std::any arg);
int action_do_nothing(std::any arg);

std::filesystem::path iso_root = std::filesystem::path("isos");

std::vector<MenuItem> main_menu = {
    MenuItem{.name = "CDROM Emulator",
             .action = action_file_browser,
             .action_arg = iso_root},
    MenuItem{.name = "USB RNDIS", .action = action_rndis},
    MenuItem{.name = "Status", .action = action_status},
    MenuItem{.name = "", .action = action_do_nothing},
    MenuItem{.name = "Shutdown", .action = action_shutdown},
    MenuItem{.name = "Restart", .action = action_restart},
    MenuItem{.name = "", .action = action_do_nothing}};

int action_main_menu(std::any arg) {
  Menu menu;
  menu_init(&menu, &main_menu);
  menu_run(&menu, &u8g2);
  return 0;
}

int action_do_nothing(std::any arg) { return 0; }

int action_errmsg(std::any arg) {
  std::string msg = std::any_cast<std::string>(arg);
  u8g2_ClearBuffer(&u8g2);
  u8g2_SetDrawColor(&u8g2, 1);
  u8g2_DrawStr(&u8g2, 0, 28, msg.c_str());
  u8g2_SendBuffer(&u8g2);

  sleep(3);
  return 0;
}

int action_cdrom_emu(std::any arg) {
  auto path = std::any_cast<std::filesystem::path>(arg);

  usb_gadget_add_cdrom(path);
  usb_gadget_start();

  Menu emu_menu;
  std::vector<MenuItem> emu_menu_items = {
      MenuItem{.name = "Current image", .action = action_do_nothing},
      MenuItem{.name = path.filename().c_str(), .action = action_do_nothing},
      MenuItem{.name = "Eject",
               .action =
                   [](std::any) {
                     usb_gadget_stop();
                     usleep(500 * 1000);
                     return -1;
                   }},
      MenuItem{.name = "", .action = action_do_nothing},
  };
  menu_init(&emu_menu, &emu_menu_items);
  menu_run(&emu_menu, &u8g2);
  return 0;
}

int action_file_browser(std::any arg) {
  auto path = std::any_cast<std::filesystem::path>(arg);
  if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
    action_errmsg(std::string("Failed to open dir"));
    return 0;
  }

  std::vector<MenuItem> dir_menu_items;
  std::vector<MenuItem> iso_menu_items;
  Menu dir_menu;

  dir_menu_items.push_back(MenuItem{.name = "[..]", .action = nullptr});
  for (const auto &entry : std::filesystem::directory_iterator(path)) {
    std::string name;
    std::filesystem::path entry_path = entry.path();
    if (entry.is_directory()) {
      dir_menu_items.push_back(
          MenuItem{.name = '[' + entry_path.filename().string() + ']',
                   .action = action_file_browser,
                   .action_arg = entry.path()});
    } else {
      std::string ext = entry_path.extension().string();
      std::string ext_lower(ext);
      std::transform(ext.begin(), ext.end(), ext_lower.begin(), ::tolower);
      if (ext_lower == ".iso") {
        iso_menu_items.push_back(MenuItem{.name = entry_path.filename(),
                                          .action = action_cdrom_emu,
                                          .action_arg = entry.path()});
      }
    }
  }

  std::sort(dir_menu_items.begin(), dir_menu_items.end(),
            [](MenuItem a, MenuItem b) { return a.name < b.name; });
  std::sort(iso_menu_items.begin(), iso_menu_items.end(),
            [](MenuItem a, MenuItem b) { return a.name < b.name; });

  for (auto const &items : iso_menu_items) {
    dir_menu_items.push_back(items);
  }
  menu_init(&dir_menu, &dir_menu_items);
  menu_run(&dir_menu, &u8g2);
  return 0;
}

int action_status(std::any arg) {
  std::map<std::string, std::string> ip_map = get_ip_addr();
  Menu status_menu;
  std::vector<MenuItem> status_menu_items = {
      MenuItem{.name = "Version"},
      MenuItem{.name = version},
  };

  for (auto const &pair : ip_map) {
    status_menu_items.push_back(MenuItem{.name = pair.first + " IP"});
    status_menu_items.push_back(MenuItem{.name = pair.second});
  }

  menu_init(&status_menu, &status_menu_items);
  menu_run(&status_menu, &u8g2);
  return 0;
}

int action_rndis(std::any arg) {
  u8g2_ClearBuffer(&u8g2);
  u8g2_SetDrawColor(&u8g2, 1);
  u8g2_DrawStr(&u8g2, 0, 28, "Initializing...");
  u8g2_SendBuffer(&u8g2);

  rndis_start();

  Menu rndis_menu;
  std::vector<MenuItem> rndis_menu_items = {
      MenuItem{.name = "Board IP", .action = action_do_nothing},
      MenuItem{.name = "192.168.42.1", .action = action_do_nothing},      
      MenuItem{.name = "Disconnect",
               .action =
                   [](std::any) {
                     rndis_stop();
                     return -1;
                   }},
      MenuItem{.name = "", .action = action_do_nothing},
  };
  menu_init(&rndis_menu, &rndis_menu_items);
  menu_run(&rndis_menu, &u8g2);
  return 0;
}

int action_shutdown(std::any arg) {
  u8g2_ClearBuffer(&u8g2);
  u8g2_SetDrawColor(&u8g2, 1);
  u8g2_DrawStr(&u8g2, 0, 28, "Shutting down...");
  u8g2_SendBuffer(&u8g2);

  system("halt");
  sleep(1000);
  return 0;
}

int action_restart(std::any arg) {
  u8g2_ClearBuffer(&u8g2);
  u8g2_SetDrawColor(&u8g2, 1);
  u8g2_DrawStr(&u8g2, 0, 28, "Restarting...");
  u8g2_SendBuffer(&u8g2);

  system("reboot");
  sleep(1000);
  return 0;
}

int main(void) {
  usb_switch_to_device_mode();

  if (!std::filesystem::exists(iso_root)) {
    std::filesystem::create_directory(iso_root);
  } else if (!std::filesystem::is_directory(iso_root)) {
    std::cout << "cannot create isos directory: file already exists"
              << std::endl;
    return 1;
  }

  int success;
  success = input_init();
  if (!success) {
    std::cout << "failed to initialize input" << std::endl;
    return 1;
  }

  u8x8_t *p_u8x8 = u8g2_GetU8x8(&u8g2);

  const int RES_PIN = 22;
  const int DC_PIN = 21;
  u8g2_Setup_ssd1306_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_arm_linux_hw_spi,
                                     u8x8_arm_linux_gpio_and_delay);

  u8x8_SetPin(p_u8x8, U8X8_PIN_SPI_CLOCK, U8X8_PIN_NONE);
  u8x8_SetPin(p_u8x8, U8X8_PIN_SPI_DATA, U8X8_PIN_NONE);
  u8x8_SetPin(p_u8x8, U8X8_PIN_RESET, RES_PIN);
  u8x8_SetPin(p_u8x8, U8X8_PIN_DC, DC_PIN);
  u8x8_SetPin(p_u8x8, U8X8_PIN_CS, U8X8_PIN_NONE);
  u8x8_SetPin(p_u8x8, U8X8_PIN_MENU_SELECT, 18);

  success = u8g2arm_arm_init_hw_spi(p_u8x8, 0, 0, 10);
  if (!success) {
    std::cout << "failed to initialize display" << std::endl;
    return 1;
  }

  u8x8_InitDisplay(p_u8x8);
  u8x8_SetPowerSave(p_u8x8, 0);
  u8g2_SetFont(&u8g2, u8g2_font_6x13_tf); // choose a suitable font

  action_main_menu(NULL);

  u8x8_ClearDisplay(p_u8x8);
  input_stop();
  return 0;
}