#include <pebble.h>
#define KEY_HW_TRAVEL_TIME 0
#define KEY_HW_DELAY 1
#define KEY_WH_TRAVEL_TIME 2
#define KEY_WH_DELAY 3

// Main window elements
static Window *s_main_window;
static TextLayer *s_travel_time_layer;

// Store incoming information
static char hw_travel_time_buffer[8];
static char hw_delay_buffer[8];
static char wh_travel_time_buffer[8];
static char wh_delay_buffer[8];
static char travel_time_buffer[128];

// Tick handler
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  
}

// Main window
//  Load
static void main_window_load(Window *window) {
  // Create hello world TextLayer
  s_travel_time_layer = text_layer_create(GRect(0, 0, 144, 50));
  text_layer_set_background_color(s_travel_time_layer, GColorClear);
  text_layer_set_text_color(s_travel_time_layer, GColorBlack);
  text_layer_set_text(s_travel_time_layer, "Hello world");

  // Improve the layout to be more like a watchface
  text_layer_set_font(s_travel_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_travel_time_layer, GTextAlignmentLeft);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_travel_time_layer));
}

//  Unload
static void main_window_unload(Window *window) {
  text_layer_destroy(s_travel_time_layer);
}

// AppMessage
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  Tuple *t = dict_read_first(iterator);
  while (t != NULL) {
    switch (t->key) {
      case KEY_HW_TRAVEL_TIME:
        snprintf(hw_travel_time_buffer, sizeof(hw_travel_time_buffer), "%d", (int)t->value->int32);
        break;
      case KEY_HW_DELAY:
        snprintf(hw_delay_buffer, sizeof(hw_delay_buffer), "%d", (int)t->value->int32);
        break;
      case KEY_WH_TRAVEL_TIME:
        break;
      case KEY_WH_DELAY:
        break;
      default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized", (int)t->key);
    }

    t = dict_read_next(iterator);
  }

  // Construct display
  snprintf(travel_time_buffer, sizeof(travel_time_buffer), "Home-Work:\n %s min (%s min delay)", hw_travel_time_buffer, hw_delay_buffer);
  text_layer_set_text(s_travel_time_layer, travel_time_buffer);
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

// Init/deinit
static void init() {
  // AppMessage
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}


// Main
int main(void) {
  init();
  app_event_loop();
  deinit();
}