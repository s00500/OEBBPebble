#include <pebble.h>

#define NUM_MENU_SECTIONS 1
#define NUM_FIRST_MENU_ITEMS 12

static Window *window;
static MenuLayer *s_menu;

static AppSync s_sync;
static uint8_t s_sync_buffer[500];

#ifdef PBL_SDK_3
static StatusBarLayer *s_status_bar;
#endif

//Menu callbacks
static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return NUM_FIRST_MENU_ITEMS;
    default:
      return 0;
  }
}
static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  switch (section_index) {
    case 0:
      // Draw title text in the section header
      menu_cell_basic_header_draw(ctx, cell_layer, "St. Pölten");
      break;

  }
}


static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Determine which section we're going to draw in
  //switch (cell_index->section) {
    //case 0:
      //There is just one section to draw rows in!
     
     // Iterator variable, keeps the state of the creation serialization process
     static DictionaryIterator iter;

     
     
    Tuple *tuple = dict_read_begin_from_buffer(&iter, s_sync_buffer, sizeof(s_sync_buffer));
    char title[10];
    char subtitle[20];
    unsigned int index = (unsigned int)cell_index->row*3;
     APP_LOG(APP_LOG_LEVEL_ERROR, "%i",index);
    while (tuple) {
       
       if (tuple->key == index){
         //time
         snprintf(subtitle, sizeof(subtitle), "%s", tuple->value->cstring);
       }else if(tuple->key == index + 1){
         //ati
         //snprintf(subtitle, sizeof(subtitle), "Train: %s", new_tuple->value->cstring);
        }else if(tuple->key == index + 2){
         //trainName
         snprintf(title, sizeof(title), "%s", tuple->value->cstring);
    }
     tuple = dict_read_next(&iter);
   }
     
     menu_cell_basic_draw(ctx, cell_layer, title, subtitle, NULL);
    
     

      //break;
 // }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
  switch (cell_index->row) {
    // This is the menu item with the cycling icon
    case 1:
     // ok some callback here...
      // After changing the icon, mark the layer to have it updated
      //layer_mark_dirty(menu_layer_get_layer(menu_layer));
      break;
  }

}


//End Menu


static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context) {
  // We  need SEPERATE Buffers since the string does not get copied into another buffer
   layer_mark_dirty(menu_layer_get_layer(s_menu));
}


static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  // An error occured!
  APP_LOG(APP_LOG_LEVEL_ERROR, "sync error!");
}


static void send_int(int key, int value) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_int(iter, key, &value, sizeof(int), true);
  app_message_outbox_send();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
   //send_int(1,0);
   
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

/*
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");
  Tuple *planTime = dict_find(iterator, 0);
  Tuple *realTime = dict_find(iterator, 1);
  Tuple *trainNumber = dict_find(iterator, 2);
   
 if (trainNumber) {
    char s_buffer[100];
    snprintf(s_buffer, sizeof(s_buffer), "'%s\n%s'", planTime->value->cstring,trainNumber->value->cstring);
    text_layer_set_text(text_layer, s_buffer);
    
  }
   
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
*/

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

   
 #ifdef PBL_COLOR
  window_set_background_color(window, GColorBlue);
 #else
  window_set_background_color(window, GColorBlack);
 #endif

   
   // Create the menu layer
  s_menu = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menu, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });
  #ifdef PBL_SDK_3
   menu_layer_set_normal_colors(s_menu, GColorBlue , GColorWhite);
   menu_layer_set_highlight_colors(s_menu,GColorDukeBlue,GColorWhite);
 #endif
  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(s_menu, window);

  layer_add_child(window_layer, menu_layer_get_layer(s_menu));
   
#ifdef PBL_SDK_3
  // Set up the status bar last to ensure it is on top of other Layers
  s_status_bar = status_bar_layer_create();
  layer_add_child(window_layer, status_bar_layer_get_layer(s_status_bar));
#endif
   
}

static void window_unload(Window *window) {
   menu_layer_destroy(s_menu);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
	.load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
// Register callbacks
/*app_message_register_inbox_received(inbox_received_callback);
app_message_register_inbox_dropped(inbox_dropped_callback);
app_message_register_outbox_failed(outbox_failed_callback);
app_message_register_outbox_sent(outbox_sent_callback);
   
app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum()); */
   
   // Setup AppSync
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
   
   // Setup initial values
  Tuplet initial_values[] = {
     //train 1
     TupletCString(0, "00:01"),
     TupletCString(1, "false"),
     TupletCString(2, "Trai1"),
     
     //train 2
     TupletCString(3, "00:02"),
     TupletCString(4, "false"),
     TupletCString(5, "Trai2"),
     
     //train 3
     TupletCString(6, "00:03"),
     TupletCString(7, "false"),
     TupletCString(8, "Trai3"),
     
     //train 4
     TupletCString(9, "00:04"),
     TupletCString(10, "false"),
     TupletCString(11, "Trai4"),
     
     //train 5
     TupletCString(12, "00:05"),
     TupletCString(13, "false"),
     TupletCString(14, "Trai5"),
     
     //train 6
     TupletCString(15, "00:06"),
     TupletCString(16, "false"),
     TupletCString(17, "Train"),
     
     //train 7
     TupletCString(18, "00:07"),
     TupletCString(19, "false"),
     TupletCString(20, "Train"),
     
     //train 8
     TupletCString(21, "00:08"),
     TupletCString(22, "false"),
     TupletCString(23, "Train"),
     
     //train 9
     TupletCString(24, "00:09"),
     TupletCString(25, "false"),
     TupletCString(26, "Train"),
     
     //train 10
     TupletCString(27, "00:10"),
     TupletCString(28, "false"),
     TupletCString(29, "Train"),
     
     //train 11
     TupletCString(30, "00:11"),
     TupletCString(31, "false"),
     TupletCString(32, "Train"),
     
     //train 12
     TupletCString(33, "00:12"),
     TupletCString(34, "false"),
     TupletCString(35, "Train"),
  };
   
   // Begin using AppSync
  app_sync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer), initial_values, ARRAY_LENGTH(initial_values), sync_changed_handler, sync_error_handler, NULL);

}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}