#include "robodash/detail/gui.hpp"
#include "liblvgl/lvgl.h"
#include "robodash/detail/assets.h"
#include "robodash/detail/styles.h"
#include "robodash/view.hpp"
#include <memory>

const int view_menu_width = 192;

// ============================== UI Elements ============================== //

lv_obj_t *screen;
lv_obj_t *view_cont;

lv_obj_t *shade;
lv_obj_t *view_menu;
lv_obj_t *view_list;
lv_obj_t *alert_cont;
lv_obj_t *alert_btn;
lv_obj_t *anim_label;

lv_anim_t anim_sidebar_open;
lv_anim_t anim_sidebar_close;
lv_anim_t anim_shade_hide;
lv_anim_t anim_shade_show;

rd::View *current_view;

// ============================ Helper Functions============================ //

bool valid_view(rd::View *view) {
	if (view == NULL) return false;

	for (int i = 0; i < lv_obj_get_child_cnt(view_list); i++) {
		lv_obj_t *child = lv_obj_get_child(view_list, i);
		rd::View *reg_view = (rd::View *)lv_obj_get_user_data(child);
		if (reg_view == view) return true;
	}

	return false;
}

// ============================== UI Callbacks ============================== //

void view_focus_cb(lv_event_t *event) {
	rd::View *view = (rd::View *)lv_event_get_user_data(event);
	if (valid_view(view)) view->focus();
}

void views_btn_cb(lv_event_t *event) {
	show_shade();
	show_menu();
}

void close_cb(lv_event_t *event) {
	if (lv_obj_get_child_cnt(alert_cont) > 0) {
		lv_obj_clear_flag(alert_btn, LV_OBJ_FLAG_HIDDEN);
	}

	lv_obj_add_flag(alert_cont, LV_OBJ_FLAG_HIDDEN);

	hide_shade();
	hide_menu();
}

void alert_btn_cb(lv_event_t *event) {
	if (!lv_obj_has_flag(alert_cont, LV_OBJ_FLAG_HIDDEN)) return;
	lv_obj_add_flag(alert_btn, LV_OBJ_FLAG_HIDDEN);
	lv_obj_clear_flag(alert_cont, LV_OBJ_FLAG_HIDDEN);
	show_shade();
}

// =========================== UI Initialization =========================== //

void create_ui() {
	screen = lv_scr_act();
	lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

	view_cont = lv_obj_create(screen);
	lv_obj_set_size(view_cont, 480, 240);
	lv_obj_add_style(view_cont, &style_bg, 0);
	lv_obj_align(view_cont, LV_ALIGN_TOP_LEFT, 0, 0);

	// ---------------------------- Top Buttons ---------------------------- //

	lv_obj_t *views_open_btn = lv_btn_create(screen);
	lv_obj_set_size(views_open_btn, 32, 32);
	lv_obj_add_style(views_open_btn, &style_core_button, 0);
	lv_obj_add_style(views_open_btn, &style_core_button_pr, LV_STATE_PRESSED);
	lv_obj_align(views_open_btn, LV_ALIGN_TOP_RIGHT, -4, 4);
	lv_obj_add_event_cb(views_open_btn, views_btn_cb, LV_EVENT_PRESSED, NULL);

	lv_obj_t *open_img = lv_img_create(views_open_btn);
	lv_img_set_src(open_img, &stack);
	lv_obj_set_style_img_recolor(open_img, color_text, 0);
	lv_obj_set_style_img_recolor_opa(open_img, LV_OPA_COVER, 0);
	lv_obj_align(open_img, LV_ALIGN_CENTER, 0, 0);

	alert_btn = lv_btn_create(screen);
	lv_obj_set_size(alert_btn, 32, 32);
	lv_obj_add_style(alert_btn, &style_core_button, 0);
	lv_obj_add_style(alert_btn, &style_core_button_pr, LV_STATE_PRESSED);
	lv_obj_align(alert_btn, LV_ALIGN_TOP_RIGHT, -42, 4);
	lv_obj_add_event_cb(alert_btn, alert_btn_cb, LV_EVENT_PRESSED, NULL);
	lv_obj_add_flag(alert_btn, LV_OBJ_FLAG_HIDDEN);

	lv_obj_t *alert_img = lv_img_create(alert_btn);
	lv_img_set_src(alert_img, LV_SYMBOL_BELL);
	lv_obj_set_style_img_recolor(alert_img, color_text, 0);
	lv_obj_set_style_img_recolor_opa(alert_img, LV_OPA_COVER, 0);
	lv_obj_align(alert_img, LV_ALIGN_CENTER, 0, 0);

	// ------------------------------- Shade ------------------------------- //

	shade = lv_obj_create(screen);
	lv_obj_set_size(shade, lv_pct(100), lv_pct(100));
	lv_obj_add_style(shade, &style_core_shade, 0);
	lv_obj_add_flag(shade, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_event_cb(shade, close_cb, LV_EVENT_PRESSED, NULL);

	// ----------------------------- View Menu ----------------------------- //

	view_menu = lv_obj_create(screen);
	lv_obj_set_size(view_menu, view_menu_width, 240);
	lv_obj_align(view_menu, LV_ALIGN_TOP_RIGHT, 0, 0);
	lv_obj_add_style(view_menu, &style_core_bg, 0);
	lv_obj_add_flag(view_menu, LV_OBJ_FLAG_HIDDEN);

	lv_obj_t *title = lv_label_create(view_menu);
	lv_label_set_text(title, "Select View");
	lv_obj_add_style(title, &style_text_large, 0);
	lv_obj_align(title, LV_ALIGN_TOP_LEFT, 12, 12);

	lv_obj_t *views_close_btn = lv_btn_create(view_menu);
	lv_obj_set_size(views_close_btn, 32, 32);
	lv_obj_add_style(views_close_btn, &style_transp, 0);
	lv_obj_add_style(views_close_btn, &style_transp, LV_STATE_PRESSED);
	lv_obj_align(views_close_btn, LV_ALIGN_TOP_RIGHT, -4, 4);
	lv_obj_add_event_cb(views_close_btn, close_cb, LV_EVENT_PRESSED, NULL);

	lv_obj_t *close_img = lv_img_create(views_close_btn);
	lv_img_set_src(close_img, LV_SYMBOL_CLOSE);
	lv_obj_align(close_img, LV_ALIGN_CENTER, 0, 0);

	view_list = lv_list_create(view_menu);
	lv_obj_set_size(view_list, lv_pct(100) - 8, lv_pct(100) - 32);
	lv_obj_add_style(view_list, &style_core_list, 0);
	lv_obj_align(view_list, LV_ALIGN_TOP_LEFT, 4, 36);

	anim_label = lv_label_create(view_menu);
	lv_label_set_text(anim_label, "Animations are disabled\nfor this view");
	lv_obj_add_style(anim_label, &style_text_small, 0);
	lv_obj_add_style(anim_label, &style_text_centered, 0);
	lv_obj_align(anim_label, LV_ALIGN_BOTTOM_MID, 0, -2);
	lv_obj_add_flag(anim_label, LV_OBJ_FLAG_HIDDEN);
	lv_obj_set_style_text_font(anim_label, &lv_font_montserrat_10, 0);

	// -------------------------- Alert Container -------------------------- //

	alert_cont = lv_obj_create(screen);
	lv_obj_set_size(alert_cont, 320, lv_pct(100));
	lv_obj_align(alert_cont, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_style(alert_cont, &style_transp, 0);
	lv_obj_clear_flag(alert_cont, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_set_flex_align(
	    alert_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START
	);
	lv_obj_set_flex_flow(alert_cont, LV_FLEX_FLOW_COLUMN);
}

void create_anims() {
	// ------------------------- Sidebar Animations ------------------------- //

	lv_anim_init(&anim_sidebar_open);
	lv_anim_set_var(&anim_sidebar_open, view_menu);
	lv_anim_set_time(&anim_sidebar_open, 200);
	lv_anim_set_exec_cb(&anim_sidebar_open, &anim_x_cb);

	anim_sidebar_close = anim_sidebar_open;

	lv_anim_set_path_cb(&anim_sidebar_open, &lv_anim_path_ease_out);
	lv_anim_set_values(&anim_sidebar_open, view_menu_width, 0);

	lv_anim_set_values(&anim_sidebar_close, 0, view_menu_width);
	lv_anim_set_deleted_cb(&anim_sidebar_close, &anim_del_cb);
	lv_anim_set_path_cb(&anim_sidebar_close, &lv_anim_path_ease_out);

	// -------------------------- Shade Animations -------------------------- //

	lv_anim_init(&anim_shade_hide);
	lv_anim_set_var(&anim_shade_hide, shade);
	lv_anim_set_time(&anim_shade_hide, 200);
	lv_anim_set_exec_cb(&anim_shade_hide, &anim_opa_cb);

	anim_shade_show = anim_shade_hide;

	lv_anim_set_values(&anim_shade_hide, 144, 0);
	lv_anim_set_deleted_cb(&anim_shade_hide, &anim_del_cb);
	lv_anim_set_values(&anim_shade_show, 0, 144);
}

// ============================== UI Functions ============================== //

void hide_menu() {
	if (lv_obj_has_flag(view_menu, LV_OBJ_FLAG_HIDDEN)) return;

	if (current_view->has_flag(rd::ViewFlag::NoAnimation))
		lv_obj_add_flag(view_menu, LV_OBJ_FLAG_HIDDEN);
	else
		lv_anim_start(&anim_sidebar_close);
}

void show_menu() {
	if (!lv_obj_has_flag(view_menu, LV_OBJ_FLAG_HIDDEN)) return;
	lv_obj_clear_flag(view_menu, LV_OBJ_FLAG_HIDDEN);
	if (!current_view->has_flag(rd::ViewFlag::NoAnimation)) lv_anim_start(&anim_sidebar_open);
}

void hide_shade() {
	if (lv_obj_has_flag(shade, LV_OBJ_FLAG_HIDDEN)) return;

	if (current_view->has_flag(rd::ViewFlag::NoAnimation))
		lv_obj_add_flag(shade, LV_OBJ_FLAG_HIDDEN);
	else
		lv_anim_start(&anim_shade_hide);
}

void show_shade() {
	if (!lv_obj_has_flag(shade, LV_OBJ_FLAG_HIDDEN)) return;
	lv_obj_clear_flag(shade, LV_OBJ_FLAG_HIDDEN);
	if (!current_view->has_flag(rd::ViewFlag::NoAnimation)) lv_anim_start(&anim_shade_show);
}

// =============================== Initialize =============================== //

bool initialized = false;

void initialize() {
	if (initialized) return;

	create_styles();
	create_ui();
	create_anims();

	initialized = true;
}