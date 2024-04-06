#include <algorithm>
#include <iostream>
#include <ostream>
#include <vector>

#include <libu8g2arm/u8g2.h>
#include <libu8g2arm/u8g2arm.h>

#include "menu.h"
#include "input.h"

const int PAGE_SIZE = 4;
const int ITEM_Y[PAGE_SIZE] = {0, 16, 32, 48};
const int SCREEN_WIDTH = 128;
const int SCREEN_HEIGHT = 64;
constexpr int ITEM_HEIGHT = SCREEN_HEIGHT / PAGE_SIZE;

void menu_init(Menu *menu, std::vector<MenuItem> *items) {
  menu->items = items;
  menu->view_start_index = 0;
  menu->curr_index = 0;
  menu->page_size = PAGE_SIZE;
}

void menu_draw(Menu *menu, u8g2_t *display) {
  size_t start = menu->view_start_index;
  size_t item_count = menu->items->size();

  if (start >= item_count) {
    start = item_count - std::min(item_count, menu->page_size);
  }

  size_t end = start + menu->page_size;
  if (end >= item_count) {
    end = item_count;
  }

  size_t draw_count = end - start;  

  std::cout << "start=" << start << ", end=" << end << std::endl;

  u8g2_ClearBuffer(display);
  u8g2_SetDrawColor(display, 0);  

  for (size_t i = 0; i < draw_count; i++) {
    size_t index = start + i;
    MenuItem menu_item = menu->items->at(index);

    int isActive = index == menu->curr_index;
    std::cout << (isActive ? "> " : "  ") << menu_item.name << std::endl;    

    if (isActive) {
      u8g2_SetDrawColor(display, 1);
      u8g2_DrawBox(display, 0, ITEM_Y[i], SCREEN_WIDTH, ITEM_HEIGHT);
      u8g2_SetDrawColor(display, 0);
    } else {
      u8g2_SetDrawColor(display, 0);
      u8g2_DrawBox(display, 0, ITEM_Y[i], SCREEN_WIDTH, ITEM_HEIGHT);
      u8g2_SetDrawColor(display, 1);
    }
    u8g2_DrawStr(display, 0, ITEM_Y[i] + 12, menu_item.name.c_str());
  }

  u8g2_SetDrawColor(display, 0);
  u8g2_DrawBox(display, SCREEN_WIDTH - 4, 0, 4, SCREEN_HEIGHT);
  u8g2_SetDrawColor(display, 1);
  int scrollbar_block_height = SCREEN_HEIGHT / item_count;
  u8g2_DrawBox(display, SCREEN_WIDTH - 4 + 2, scrollbar_block_height * (menu->curr_index), 2, scrollbar_block_height);

  u8g2_SendBuffer(display);
}

void update_viewport(Menu *menu) {
  if (menu->curr_index < menu->view_start_index) {
    menu->view_start_index = menu->curr_index;
  }
  if (menu->curr_index >= menu->view_start_index + menu->page_size) {
    menu->view_start_index =
        menu->curr_index + 1 - std::min(menu->curr_index, menu->page_size);
  }
}
int menu_run(Menu *menu, u8g2_t *display) {  
  size_t item_count = menu->items->size();
  bool redraw = true;

  while (true) {
    InputValue c = input_get();
    switch (c) {
    case Up:
      if (menu->curr_index == 0) {
        menu->curr_index = item_count - std::min((size_t)1, item_count);
      } else {
        menu->curr_index--;
      }
      update_viewport(menu);
      redraw = true;
      break;
    case Down:
      menu->curr_index = (menu->curr_index + 1) % item_count;
      update_viewport(menu);
      redraw = true;
      break;
    case Enter: {
      MenuItem curr = menu->items->at(menu->curr_index);
      if (curr.action) {
        int result = curr.action(curr.action_arg);
        if (result < 0) {
          return result + 1;
        }
        redraw = true;
        break;
      } else {
        return 0;
      }
    }
    case Back:
      return 0;
    default:
      break;
    }
    usleep(50000);
    if (redraw) {
      menu_draw(menu, display);
      redraw = false;
    }
  }
}