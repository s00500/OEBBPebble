#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static TextLayer *text_layer1;
static TextLayer *text_layer2;

static MenuLayer *s_menu;

static AppSync s_sync;
static uint8_t s_sync_buffer[100];

#ifdef PBL_SDK_3
static StatusBarLayer *s_status_bar;
#endif

static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context) {
  // We  need 3 Buffers since the string does not get copied into another buffer
  static char train_buffer[32];
   static char actual_buffer[32];
   static char departure_buffer[32];
   
  APP_LOG(APP_LOG_LEVEL_INFO, "%d",(int)key);
  if(key==2){
     memset ( train_buffer, 0, sizeof(train_buffer));
     snprintf(train_buffer, sizeof(train_buffer), "Train: %s", new_tuple->value->cstring);
     APP_LOG(APP_LOG_LEVEL_INFO, train_buffer);
     text_layer_set_text(text_layer, train_buffer);
  }
  else if(key==1){
     memset ( actual_buffer, 0, sizeof(actual_buffer));
     snprintf(actual_buffer, sizeof(actual_buffer), "Actual: %s", new_tuple->value->cstring);
     APP_LOG(APP_LOG_LEVEL_INFO, actual_buffer);
     text_layer_set_text(text_layer1, actual_buffer);
   }
  else if(key==0){
     memset ( departure_buffer, 0, sizeof(departure_buffer));
     snprintf(departure_buffer, sizeof(departure_buffer), "Departure: %s", new_tuple->value->cstring);
     APP_LOG(APP_LOG_LEVEL_INFO, departure_buffer);
     text_layer_set_text(text_layer2, departure_buffer);
   }
   APP_LOG(APP_LOG_LEVEL_INFO, "end");
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
  text_layer_set_text(text_layer, "");
   send_int(1,0);
   
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

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


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

   
 #ifdef PBL_COLOR
  window_set_background_color(window, GColorBlue);
 #else
  window_set_background_color(window, GColorBlack);
 #endif

  text_layer = text_layer_create((GRect) { .origin = { 0, 50 }, .size = { bounds.size.w, 24 } });
  text_layer1 = text_layer_create((GRect) { .origin = { 0, 74 }, .size = { bounds.size.w, 24 } });
  text_layer2 = text_layer_create((GRect) { .origin = { 0, 98 }, .size = { bounds.size.w, 24 } });
   
   text_layer_set_text_color(text_layer, GColorWhite);
   text_layer_set_text_color(text_layer1, GColorWhite);
   text_layer_set_text_color(text_layer2, GColorWhite);
   
    #ifdef PBL_COLOR
     text_layer_set_background_color(text_layer, GColorBlue);
     text_layer_set_background_color(text_layer1, GColorBlue);
     text_layer_set_background_color(text_layer2, GColorBlue);
    #else
     text_layer_set_background_color(text_layer, GColorBlack);
     text_layer_set_background_color(text_layer1, GColorBlack);
     text_layer_set_background_color(text_layer2, GColorBlack);
    #endif
   

    text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_font(text_layer1, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_font(text_layer2, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
   
   
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(text_layer1, GTextAlignmentCenter);
  text_layer_set_text_alignment(text_layer2, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  layer_add_child(window_layer, text_layer_get_layer(text_layer1));
  layer_add_child(window_layer, text_layer_get_layer(text_layer2));
   
  #ifdef PBL_SDK_3
  // Set up the status bar last to ensure it is on top of other Layers
  s_status_bar = status_bar_layer_create();
  layer_add_child(window_layer, status_bar_layer_get_layer(s_status_bar));
#endif
   
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
   text_layer_destroy(text_layer1);
   text_layer_destroy(text_layer2);
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
     TupletCString(0, "00:00"),
     TupletCString(1, "ati"),
      TupletCString(2, "Train"),
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