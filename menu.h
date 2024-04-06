#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
#include <any>
#include <functional>

#include <libu8g2arm/u8g2.h>

typedef struct {
    std::string name;
    std::function<int(std::any)> action;
    std::any action_arg;
} MenuItem;

typedef struct {
    std::vector<MenuItem>* items;
    size_t curr_index;
    size_t view_start_index;
    size_t page_size;
    bool active_scroll;
    bool active_need_scroll;
    int active_x;    
} Menu;

void menu_init(Menu* menu, std::vector<MenuItem>* items);
void menu_draw(Menu* menu, u8g2_t* display);
int menu_run(Menu* menu, u8g2_t* display);
#endif