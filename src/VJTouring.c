#include <pebble.h>
#define KEY_TRAVELTIMES_DATA 0

// Main window elements
static Window *s_main_window;
static MenuLayer *s_menu_layer;

// AppSync
static AppSync s_sync;
static uint8_t s_sync_buffer[128];

// Data structure for travel time
typedef struct TravelTime {
  char *name;
  int travelTime;
  int delay;
} TravelTime;

// Travel data storage (max 8)
static int numberOfTravelTimes;
static TravelTime travelTimesData[8];

// Example data
static TravelTime dummyData[] = {
  {.name = "Home", .travelTime = 25, .delay = 2},
  {.name = "Work", .travelTime = 40, .delay = 2}
};

// Main window
//  Menu
static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return 1;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return 2;
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  menu_cell_basic_header_draw(ctx, cell_layer, "Travel times");
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  static TravelTime tt;
  tt = dummyData[cell_index->row];
  static char subTitle[32];
  snprintf(subTitle, sizeof(subTitle), "%d min (%d min delay)", tt.travelTime, tt.delay);
  menu_cell_basic_draw(ctx, cell_layer, tt.name, subTitle, NULL);
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Something selected!");
}

//  Load
static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(s_menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

//  Unload
static void main_window_unload(Window *window) {
  menu_layer_destroy(s_menu_layer);
}

// AppSync
//  

//  Changed
static void sync_changed_handler(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context) {
  // Update UI here

}

//  Error
static void sync_error_handler(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  // An error occured!

}

// Init/deinit
static void init() {
  // AppSync
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  // Setup initial values
  Tuplet initial_values[] = {
    TupletCString(KEY_TRAVELTIMES_DATA, "0;"),
  };

  // Begin using AppSync
  app_sync_init(&s_sync, s_sync_buffer, sizeof(s_sync_buffer), initial_values, ARRAY_LENGTH(initial_values), sync_changed_handler, sync_error_handler, NULL);

  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
  app_sync_deinit(&s_sync);
}

// Main
int main(void) {
  init();
  app_event_loop();
  deinit();
}