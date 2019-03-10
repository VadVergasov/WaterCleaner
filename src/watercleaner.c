#include "watercleaner.h"

typedef struct appdata {
    Evas_Object *win;
    Evas_Object *grid;
    Evas_Object *conformant;
    Evas_Object *button;
    Evas_Object *label;
    Evas_Object *time;
    Evas_Object *spinner;
    Evas_Object *popup;
    Evas_Object *circle_spinner;
    Evas_Object *circle_progressbar;
    Evas_Object *more_option
    Eext_Circle_Surface *surface;
} appdata_s;

appdata_s *ad;

void delay(unsigned duration) {
    time_t start = time(NULL), last;
    last = start;
    double end = duration;
    time_t now;
    do {
        if (now - last >= (double)duration / 100.0) {
            eext_circle_object_value_set(
                ad->circle_progressbar,
                eext_circle_object_value_get(ad->circle_progressbar) + 1);
        }
        now = time(NULL);
    } while (difftime(now, start) < end);
}

static void _timeout_cb(void *data, Evas_Object *obj, void *event_info) {
    if (!obj) return;
    evas_object_del(obj);
}

void ready_label(void *data) {
    ad->popup = elm_popup_add(ad->grid);
    elm_object_style_set(ad->popup, "toast/circle");
    elm_popup_orient_set(ad->popup, ELM_POPUP_ORIENT_BOTTOM);
    evas_object_size_hint_weight_set(ad->popup, EVAS_HINT_EXPAND,
                                     EVAS_HINT_EXPAND);
    char *text = i18n_get_text("Ready");
    char icon[100];
    strcat(icon, app_get_resource_path());
    strcat(icon, "/images/check.png");
    elm_object_part_text_set(ad->popup, "title,icon", "icon");
    elm_object_part_text_set(ad->popup, "elm.text", text);

    elm_popup_timeout_set(ad->popup, 2.0);
    evas_object_smart_callback_add(ad->popup, "timeout", _timeout_cb, NULL);

    evas_object_show(ad->popup);
}

static void device_vibrate(int duration, int feedback) {
    haptic_device_h haptic_handle;
    haptic_effect_h effect_handle;
    if (device_haptic_open(0, &haptic_handle) == DEVICE_ERROR_NONE) {
        if (device_haptic_vibrate(haptic_handle, duration, feedback,
                                  &effect_handle) == DEVICE_ERROR_NONE) {
            evas_object_show(ad->circle_progressbar);
            evas_object_hide(ad->button);
            evas_object_hide(ad->spinner);
            evas_object_hide(ad->circle_spinner);
            delay(duration / 1000);
            ready_label(NULL);
            evas_object_hide(ad->circle_progressbar);
            evas_object_show(ad->button);
            evas_object_show(ad->spinner);
            evas_object_show(ad->circle_spinner);
        }
    }
}

static void win_delete_request_cb(void *data, Evas_Object *obj,
                                  void *event_info) {
    ui_app_exit();
}

static void win_back_cb(void *data, Evas_Object *obj, void *event_info) {
    ad = data;
    elm_win_lower(ad->win);
}

void clicked_cb(appdata_s *data, Evas_Object *obj, void *event_info) {
    double val = elm_spinner_value_get(ad->spinner);
    device_vibrate(val * 1000, 100);
    ready_label(data);
}

static void create_base_gui(appdata_s *ad) {
    ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
    evas_object_size_hint_align_set(ad->win, EVAS_HINT_FILL, EVAS_HINT_FILL);
    elm_win_autodel_set(ad->win, EINA_TRUE);

    if (elm_win_wm_rotation_supported_get(ad->win)) {
        int rots[4] = {0, 90, 180, 270};
        elm_win_wm_rotation_available_rotations_set(ad->win,
                                                    (const int *)(&rots), 4);
    }

    evas_object_smart_callback_add(ad->win, "delete,request",
                                   win_delete_request_cb, NULL);
    eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb,
                                   ad);

    ad->conformant = elm_conformant_add(ad->win);
    evas_object_size_hint_align_set(ad->conformant, EVAS_HINT_FILL,
                                    EVAS_HINT_FILL);
    elm_win_resize_object_add(ad->win, ad->conformant);
    evas_object_show(ad->conformant);

    ad->grid = elm_grid_add(ad->conformant);
    elm_object_content_set(ad->conformant, ad->grid);
    evas_object_show(ad->grid);

    ad->time = elm_label_add(ad->grid);
    elm_grid_pack(ad->grid, ad->time, 30, 25, 40, 10);
    char t[50] = "<align=center>";
    strcat(t, i18n_get_text("Time"));
    strcat(t, "</align>");
    elm_object_text_set(ad->time, t);
    evas_object_show(ad->time);

    ad->surface = eext_circle_surface_conformant_add(ad->conformant);

    ad->circle_progressbar =
        eext_circle_object_progressbar_add(ad->grid, ad->surface);
    ;
    eext_circle_object_value_min_max_set(ad->circle_progressbar, 0, 100);
    eext_circle_object_value_set(ad->circle_progressbar, 0);
    elm_grid_pack(ad->grid, ad->circle_progressbar, 30, 35, 40, 10);

    ad->spinner = elm_spinner_add(ad->grid);
    elm_grid_pack(ad->grid, ad->spinner, 30, 35, 40, 10);
    elm_object_style_set(ad->spinner, "circle");
    ad->circle_spinner =
        eext_circle_object_spinner_add(ad->spinner, ad->surface);
    elm_spinner_min_max_set(ad->spinner, 1, 60);
    elm_spinner_value_set(ad->spinner, 5);
    eext_circle_object_spinner_angle_set(ad->circle_spinner, 6);
    eext_rotary_object_event_activated_set(ad->circle_spinner, EINA_TRUE);
    elm_spinner_wrap_set(ad->spinner, EINA_TRUE);
    evas_object_show(ad->circle_spinner);
    evas_object_show(ad->spinner);

    ad->label = elm_label_add(ad->grid);
    elm_grid_pack(ad->grid, ad->label, 35, 45, 30, 10);
    evas_object_show(ad->label);

    char *button_text = i18n_get_text("Clean");
    ad->button = elm_button_add(ad->grid);
    elm_object_text_set(ad->button, button_text);
    elm_object_style_set(ad->button, "bottom");
    elm_grid_pack(ad->grid, ad->button, 30, 90, 40, 10);
    evas_object_smart_callback_add(ad->button, "clicked", clicked_cb, ad);
    evas_object_show(ad->button);
    evas_object_show(ad->win);
}

static bool app_create(void *data) {
    ad = data;
    create_base_gui(data);
    return true;
}

static void app_control(app_control_h app_control, void *data) {}

static void app_pause(void *data) {}

static void app_resume(void *data) {}

static void app_terminate(void *data) {}

static void ui_app_lang_changed(app_event_info_h event_info, void *user_data) {
    char *locale = NULL;
    system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE,
                                     &locale);
    elm_language_set(locale);
    free(locale);
    create_base_gui(ad);

    return;
}

static void ui_app_orient_changed(app_event_info_h event_info,
                                  void *user_data) {
    create_base_gui(ad);
    return;
}

static void ui_app_region_changed(app_event_info_h event_info,
                                  void *user_data) {}

static void ui_app_low_battery(app_event_info_h event_info, void *user_data) {}

static void ui_app_low_memory(app_event_info_h event_info, void *user_data) {}

int main(int argc, char *argv[]) {
    appdata_s ad = {
        0,
    };
    int ret = 0;

    ui_app_lifecycle_callback_s event_callback = {
        0,
    };
    app_event_handler_h handlers[5] = {
        NULL,
    };

    event_callback.create = app_create;
    event_callback.terminate = app_terminate;
    event_callback.pause = app_pause;
    event_callback.resume = app_resume;
    event_callback.app_control = app_control;

    ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
                             APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
    ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
                             APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
    ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
                             APP_EVENT_DEVICE_ORIENTATION_CHANGED,
                             ui_app_orient_changed, &ad);
    ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
                             APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed,
                             &ad);
    ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
                             APP_EVENT_REGION_FORMAT_CHANGED,
                             ui_app_region_changed, &ad);

    ret = ui_app_main(argc, argv, &event_callback, &ad);
    if (ret != APP_ERROR_NONE) {
    }

    return ret;
}
