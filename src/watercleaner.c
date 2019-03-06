#include "watercleaner.h"

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *grid;
	Evas_Object *button;
	Evas_Object *label;
} appdata_s;

static void device_vibrate(int duration, int feedback){
	haptic_device_h haptic_handle;
	haptic_effect_h effect_handle;
	if(device_haptic_open(0, &haptic_handle) == DEVICE_ERROR_NONE) {
		if(device_haptic_vibrate(haptic_handle, duration, feedback, &effect_handle) == DEVICE_ERROR_NONE){

		}else{

		}
	}else{

	}
}

void ready_label(appdata_s *ad){
	char text[50] = "<align=center>";
	strcat(text, i18n_get_text("Ready"));
	strcat(text, "</align>");
	elm_object_text_set(ad->label, text);
}

static void win_delete_request_cb(void *data, Evas_Object *obj, void *event_info){
	ui_app_exit();
}

static void win_back_cb(void *data, Evas_Object *obj, void *event_info){
	appdata_s *ad = data;
	elm_win_lower(ad->win);
}

void clicked_cb(appdata_s *data, Evas_Object *obj, void *event_info){
	device_vibrate(5000, 100);
	ecore_timer_add(5, ready_label, data);
}

static void create_base_gui(appdata_s *ad) {
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	evas_object_size_hint_align_set(ad->win, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

	ad->grid = elm_grid_add(ad->win);
	evas_object_size_hint_align_set(ad->grid, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_win_resize_object_add(ad->win, ad->grid);
	evas_object_show(ad->grid);

	ad->label = elm_label_add(ad->grid);
	elm_grid_pack(ad->grid, ad->label, 35, 45, 30, 10);
	evas_object_show(ad->label);

	char *button_text = i18n_get_text("Clean");
	ad->button = elm_button_add(ad->grid);
	elm_object_text_set(ad->button, button_text);
	elm_object_style_set(ad->button, "bottom");
	elm_grid_pack(ad->grid, ad->button, 40, 90, 20, 10);
	evas_object_smart_callback_add(ad->button, "clicked", clicked_cb, ad);
	evas_object_show(ad->button);
	evas_object_show(ad->win);
}

static bool app_create(void *data){
	appdata_s *ad = data;
	create_base_gui(ad);
	return true;
}

static void app_control(app_control_h app_control, void *data){

}

static void app_pause(void *data){

}

static void app_resume(void *data){

}

static void app_terminate(void *data){

}

static void ui_app_lang_changed(app_event_info_h event_info, void *user_data){
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);

	appdata_s *ad = {0,};

	create_base_gui(ad);

	return;
}

static void ui_app_orient_changed(app_event_info_h event_info, void *user_data){
	/*appdata_s *ad = {0,};
	create_base_gui(ad);*/
	return;
}

static void ui_app_region_changed(app_event_info_h event_info, void *user_data){

}

static void ui_app_low_battery(app_event_info_h event_info, void *user_data){

}

static void ui_app_low_memory(app_event_info_h event_info, void *user_data){

}

int main(int argc, char *argv[]){
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {

	}

	return ret;
}
