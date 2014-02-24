#include <pebble.h>

enum {
	KEY_INVERTED = 0,
	KEY_MMDD = 1
};

#define PERSIST_INVERTED 1
#define PERSIST_MMDD 2

static Window *window;
static Layer *canvas;

static Layer *digit[12];
int displayDigit[12];
bool isInverted = false;
bool isMMDD = false;

static unsigned short get_display_hour(unsigned short hour) {
	if (clock_is_24h_style()) {
		return hour;
	}
	unsigned short display_hour = hour % 12;
	return display_hour ? display_hour : 12;
}

void drawHorizontLine(GContext* ctx, GPoint start, GPoint goal) {
	graphics_fill_rect(ctx, GRect(start.x, start.y - 1, goal.x - start.x + 1, 2), 0, GCornerNone);
}

void drawVerticalLine(GContext* ctx, GPoint start, GPoint goal) {
	graphics_fill_rect(ctx, GRect(start.x - 1, start.y, 2, goal.y - start.y + 1), 0, GCornerNone);
}

#define x1 10
#define x2 16
#define x3 24
#define x4 36

#define y1 18
#define y2 37
#define y3 55

void renderNumber(int number, GContext* ctx) {
	if (number == 1) {
		drawHorizontLine(ctx, GPoint( 0,y1), GPoint(x1,y1));
		drawVerticalLine(ctx, GPoint(x1,y1), GPoint(x1,y2));
		drawHorizontLine(ctx, GPoint( 0,y2), GPoint(x1,y2));
		drawVerticalLine(ctx, GPoint(x3, 0), GPoint(x3,y2));
		drawHorizontLine(ctx, GPoint(x3,y2), GPoint(x4,y2));
	} else if (number == 2) {
		drawHorizontLine(ctx, GPoint( 0,y1), GPoint(x3,y1));
		drawHorizontLine(ctx, GPoint(x1,y2), GPoint(x4,y2));
	} else if (number == 3) {
		drawHorizontLine(ctx, GPoint( 0,y1), GPoint(x3,y1));
		drawHorizontLine(ctx, GPoint( 0,y2), GPoint(x3,y2));
	} else if (number == 4) {
		drawVerticalLine(ctx, GPoint(x2, 0), GPoint(x2,y1));
		drawHorizontLine(ctx, GPoint( 0,y2), GPoint(x2,y2));
		drawVerticalLine(ctx, GPoint(x2,y2), GPoint(x2,y3));
	} else if (number == 5) {
		drawHorizontLine(ctx, GPoint(x1,y1), GPoint(x4,y1));
		drawHorizontLine(ctx, GPoint( 0,y2), GPoint(x3,y2));
	} else if (number == 6) {
		drawHorizontLine(ctx, GPoint(x1,y1), GPoint(x4,y1));
		drawHorizontLine(ctx, GPoint(x1,y2), GPoint(x3,y2));
	} else if (number == 7) {
		drawHorizontLine(ctx, GPoint( 0,y1), GPoint(x2,y1));
		drawVerticalLine(ctx, GPoint(x2,y1), GPoint(x2,y3));
	} else if (number == 8) {
		drawHorizontLine(ctx, GPoint(x1,y1), GPoint(x3,y1));
		drawHorizontLine(ctx, GPoint(x1,y2), GPoint(x3,y2));
	} else if (number == 9) {
		drawHorizontLine(ctx, GPoint(x1,y1), GPoint(x3,y1));
		drawHorizontLine(ctx, GPoint( 0,y2), GPoint(x3,y2));
	} else {
		drawVerticalLine(ctx, GPoint(x2,y1), GPoint(x2,y2));
	}
}

void canvas_update_callback(Layer *layer, GContext* ctx) {
	if (isInverted) {
		graphics_context_set_fill_color(ctx, GColorWhite);
	}
	drawHorizontLine(ctx, GPoint(0,56), GPoint(144,56));
	drawHorizontLine(ctx, GPoint(0,112), GPoint(144,112));
	
	drawVerticalLine(ctx, GPoint(36,0), GPoint(36,168));
	drawVerticalLine(ctx, GPoint(72,0), GPoint(72,168));
	drawVerticalLine(ctx, GPoint(108,0), GPoint(108,168));
}

void aA_update_callback(Layer *layer, GContext* ctx) { if (isInverted) { graphics_context_set_fill_color(ctx, GColorWhite); } renderNumber(displayDigit[0], ctx); }
void aB_update_callback(Layer *layer, GContext* ctx) { if (isInverted) { graphics_context_set_fill_color(ctx, GColorWhite); } renderNumber(displayDigit[1], ctx); }
void aC_update_callback(Layer *layer, GContext* ctx) { if (isInverted) { graphics_context_set_fill_color(ctx, GColorWhite); } renderNumber(displayDigit[2], ctx); }
void aD_update_callback(Layer *layer, GContext* ctx) { if (isInverted) { graphics_context_set_fill_color(ctx, GColorWhite); } renderNumber(displayDigit[3], ctx); }

void bA_update_callback(Layer *layer, GContext* ctx) { if (isInverted) { graphics_context_set_fill_color(ctx, GColorWhite); } renderNumber(displayDigit[4], ctx); }
void bB_update_callback(Layer *layer, GContext* ctx) { if (isInverted) { graphics_context_set_fill_color(ctx, GColorWhite); } renderNumber(displayDigit[5], ctx); }
void bC_update_callback(Layer *layer, GContext* ctx) { if (isInverted) { graphics_context_set_fill_color(ctx, GColorWhite); } renderNumber(displayDigit[6], ctx); }
void bD_update_callback(Layer *layer, GContext* ctx) { if (isInverted) { graphics_context_set_fill_color(ctx, GColorWhite); } renderNumber(displayDigit[7], ctx); }

void cA_update_callback(Layer *layer, GContext* ctx) { if (isInverted) { graphics_context_set_fill_color(ctx, GColorWhite); } renderNumber(displayDigit[8], ctx); }
void cB_update_callback(Layer *layer, GContext* ctx) { if (isInverted) { graphics_context_set_fill_color(ctx, GColorWhite); } renderNumber(displayDigit[9], ctx); }
void cC_update_callback(Layer *layer, GContext* ctx) { if (isInverted) { graphics_context_set_fill_color(ctx, GColorWhite); } renderNumber(displayDigit[10], ctx); }
void cD_update_callback(Layer *layer, GContext* ctx) { if (isInverted) { graphics_context_set_fill_color(ctx, GColorWhite); } renderNumber(displayDigit[11], ctx); }

void process_tuple(Tuple *t) {
	int key = t->key;
	int value = t->value->int32;
	char string_value[32];
	strcpy(string_value, t->value->cstring);

	if (key == KEY_INVERTED) {
		if (strcmp(string_value, "on") == 0) {
			isInverted = true;
		} else {
			isInverted = false;
		}
		
		if (isInverted) {
			window_set_background_color(window, GColorBlack);
		} else {
			window_set_background_color(window, GColorWhite);
		}

		layer_mark_dirty(canvas);
		for(int i = 0; i < 12; i++) {
			layer_mark_dirty(digit[i]);
		}
	} else if (key == KEY_MMDD) {
		if (strcmp(string_value, "on") == 0) {
			isMMDD = true;
		} else {
			isMMDD = false;
		}
		
		struct tm *tick_time;
		time_t temp = time(NULL);
		tick_time = localtime(&temp);
		
		int day = tick_time->tm_mday;
		int month = tick_time->tm_mon + 1;
		if (isMMDD) {
			if (displayDigit[5] != month % 10) {
				displayDigit[5] = month % 10;
				layer_mark_dirty(digit[7]);
			}
			if (displayDigit[4] != month / 10 % 10) {
				displayDigit[4] = month / 10 % 10;
				layer_mark_dirty(digit[6]);
			}
			if (displayDigit[7] != day % 10) {
				displayDigit[7] = day % 10;
				layer_mark_dirty(digit[5]);
			}
			if (displayDigit[6] != day / 10 % 10) {
				displayDigit[6] = day / 10 % 10;
				layer_mark_dirty(digit[4]);
			}
		} else {
			if (displayDigit[5] != day % 10) {
				displayDigit[5] = day % 10;
				layer_mark_dirty(digit[5]);
			}
			if (displayDigit[4] != day / 10 % 10) {
				displayDigit[4] = day / 10 % 10;
				layer_mark_dirty(digit[4]);
			}
			if (displayDigit[7] != month % 10) {
				displayDigit[7] = month % 10;
				layer_mark_dirty(digit[7]);
			}
			if (displayDigit[6] != month / 10 % 10) {
				displayDigit[6] = month / 10 % 10;
				layer_mark_dirty(digit[6]);
			}	
		}
	}
}

void in_received_handler(DictionaryIterator *iter, void *context) {
	Tuple *t = dict_read_first(iter);
	if(t) {
		process_tuple(t);
	}
  	while(t != NULL) {
		t = dict_read_next(iter);
		if(t) {
      		process_tuple(t);
		}
	}
}

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
	int hour = get_display_hour(tick_time->tm_hour);
	int minute = tick_time->tm_min;
	if (displayDigit[1] != hour % 10) {
		displayDigit[1] = hour % 10;
		layer_mark_dirty(digit[1]);
	}
	if (displayDigit[0] != hour / 10 % 10) {
		displayDigit[0] = hour / 10 % 10;
		layer_mark_dirty(digit[0]);
	}
	if (displayDigit[3] != minute % 10) {
		displayDigit[3] = minute % 10;
		layer_mark_dirty(digit[3]);
	}
	if (displayDigit[2] != minute / 10 % 10) {
		displayDigit[2] = minute / 10 % 10;
		layer_mark_dirty(digit[2]);
	}
	
	int day = tick_time->tm_mday;
	int month = tick_time->tm_mon + 1;
	if (isMMDD) {
		if (displayDigit[5] != month % 10) {
			displayDigit[5] = month % 10;
			layer_mark_dirty(digit[7]);
		}
		if (displayDigit[4] != month / 10 % 10) {
			displayDigit[4] = month / 10 % 10;
			layer_mark_dirty(digit[6]);
		}
		if (displayDigit[7] != day % 10) {
			displayDigit[7] = day % 10;
			layer_mark_dirty(digit[5]);
		}
		if (displayDigit[6] != day / 10 % 10) {
			displayDigit[6] = day / 10 % 10;
			layer_mark_dirty(digit[4]);
		}
	} else {
		if (displayDigit[5] != day % 10) {
			displayDigit[5] = day % 10;
			layer_mark_dirty(digit[5]);
		}
		if (displayDigit[4] != day / 10 % 10) {
			displayDigit[4] = day / 10 % 10;
			layer_mark_dirty(digit[4]);
		}
		if (displayDigit[7] != month % 10) {
			displayDigit[7] = month % 10;
			layer_mark_dirty(digit[7]);
		}
		if (displayDigit[6] != month / 10 % 10) {
			displayDigit[6] = month / 10 % 10;
			layer_mark_dirty(digit[6]);
		}	
	}
	
	int year = tick_time->tm_year + 1900;
	if (displayDigit[11] != year % 10) {
		displayDigit[11] = year % 10;
		layer_mark_dirty(digit[11]);
	}
	if (displayDigit[10] != year / 10 % 10) {
		displayDigit[10] = year / 10 % 10;
		layer_mark_dirty(digit[10]);
	}
	if (displayDigit[9] != year / 100 % 10) {
		displayDigit[9] = year / 100 % 10;
		layer_mark_dirty(digit[9]);
	}
	if (displayDigit[8] != year / 1000 % 10) {
		displayDigit[8] = year / 1000 % 10;
		layer_mark_dirty(digit[8]);
	}
}

void handle_init(void) {
	window = window_create();
	window_stack_push(window, true);
	
	app_message_register_inbox_received(in_received_handler);           
	app_message_open(512, 512);
	
	isInverted = persist_exists(PERSIST_INVERTED) ? persist_read_bool(PERSIST_INVERTED) : false;
	isMMDD = persist_exists(PERSIST_MMDD) ? persist_read_bool(PERSIST_MMDD) : false;
	
	if (isInverted) {
		window_set_background_color(window, GColorBlack);
	} else {
		window_set_background_color(window, GColorWhite);
	}
	
	struct tm *tick_time;
	time_t temp = time(NULL);
	tick_time = localtime(&temp);
	
	int hour = get_display_hour(tick_time->tm_hour);
	int minute = tick_time->tm_min;
	if (displayDigit[1] != hour % 10) {
		displayDigit[1] = hour % 10;
	}
	if (displayDigit[0] != hour / 10 % 10) {
		displayDigit[0] = hour / 10 % 10;
	}
	if (displayDigit[3] != minute % 10) {
		displayDigit[3] = minute % 10;
	}
	if (displayDigit[2] != minute / 10 % 10) {
		displayDigit[2] = minute / 10 % 10;
	}
	
	int day = tick_time->tm_mday;
	int month = tick_time->tm_mon + 1;
	if (isMMDD) {
		if (displayDigit[5] != month % 10) {
			displayDigit[5] = month % 10;
		}
		if (displayDigit[4] != month / 10 % 10) {
			displayDigit[4] = month / 10 % 10;
		}
		if (displayDigit[7] != day % 10) {
			displayDigit[7] = day % 10;
		}
		if (displayDigit[6] != day / 10 % 10) {
			displayDigit[6] = day / 10 % 10;
		}
	} else {
		if (displayDigit[5] != day % 10) {
			displayDigit[5] = day % 10;
		}
		if (displayDigit[4] != day / 10 % 10) {
			displayDigit[4] = day / 10 % 10;
		}
		if (displayDigit[7] != month % 10) {
			displayDigit[7] = month % 10;
		}
		if (displayDigit[6] != month / 10 % 10) {
			displayDigit[6] = month / 10 % 10;
		}	
	}
	
	int year = tick_time->tm_year + 1900;
	if (displayDigit[11] != year % 10) {
		displayDigit[11] = year % 10;
	}
	if (displayDigit[10] != year / 10 % 10) {
		displayDigit[10] = year / 10 % 10;
	}
	if (displayDigit[9] != year / 100 % 10) {
		displayDigit[9] = year / 100 % 10;
	}
	if (displayDigit[8] != year / 1000 % 10) {
		displayDigit[8] = year / 1000 % 10;
	}
	
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	
	canvas = layer_create(bounds);
	layer_set_update_proc(canvas, canvas_update_callback);
	layer_add_child(window_layer, canvas);
	
	digit[0] = layer_create(GRect(0,0,35,55));
	digit[1] = layer_create(GRect(37,0,35,55));
	digit[2] = layer_create(GRect(73,0,35,55));
	digit[3] = layer_create(GRect(109,0,35,55));
	
	digit[4] = layer_create(GRect(0,57,35,55));
	digit[5] = layer_create(GRect(37,57,35,55));
	digit[6] = layer_create(GRect(73,57,35,55));
	digit[7] = layer_create(GRect(109,57,35,55));
	
	digit[8] = layer_create(GRect(0,113,35,55));
	digit[9] = layer_create(GRect(37,113,35,55));
	digit[10] = layer_create(GRect(73,113,35,55));
	digit[11] = layer_create(GRect(109,113,35,55));
	
	layer_set_update_proc(digit[0], aA_update_callback);
	layer_set_update_proc(digit[1], aB_update_callback);
	layer_set_update_proc(digit[2], aC_update_callback);
	layer_set_update_proc(digit[3], aD_update_callback);
	
	layer_set_update_proc(digit[4], bA_update_callback);
	layer_set_update_proc(digit[5], bB_update_callback);
	layer_set_update_proc(digit[6], bC_update_callback);
	layer_set_update_proc(digit[7], bD_update_callback);
	
	layer_set_update_proc(digit[8], cA_update_callback);
	layer_set_update_proc(digit[9], cB_update_callback);
	layer_set_update_proc(digit[10], cC_update_callback);
	layer_set_update_proc(digit[11], cD_update_callback);
	
	layer_add_child(window_layer, digit[0]);
	layer_add_child(window_layer, digit[1]);
	layer_add_child(window_layer, digit[2]);
	layer_add_child(window_layer, digit[3]);
	layer_add_child(window_layer, digit[4]);
	layer_add_child(window_layer, digit[5]);
	layer_add_child(window_layer, digit[6]);
	layer_add_child(window_layer, digit[7]);
	layer_add_child(window_layer, digit[8]);
	layer_add_child(window_layer, digit[9]);
	layer_add_child(window_layer, digit[10]);
	layer_add_child(window_layer, digit[11]);
	
	tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

void handle_deinit(void) {
	tick_timer_service_unsubscribe();
	window_destroy(window);
	persist_write_bool(PERSIST_INVERTED, isInverted);
	persist_write_bool(PERSIST_MMDD, isMMDD);
}

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
}