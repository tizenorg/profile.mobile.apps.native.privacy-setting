/**
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *	Licensed under the Apache License, Version 2.0 (the "License");
 *	you may not use this file except in compliance with the License.
 *	You may obtain a copy of the License at
 *
 *	http://www.apache.org/licenses/LICENSE-2.0
 *
 *	Unless required by applicable law or agreed to in writing, software
 *	distributed under the License is distributed on an "AS IS" BASIS,
 *	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *	See the License for the specific language governing permissions and
 *	limitations under the License.
 */
/*
 * @file		main_privacy_setting.c
 * @author		Yunjin Lee (yunjin-.lee@samsung.com)
 * @version		1.0
 * @brief
 */

#include <bundle.h>

#include <libintl.h>
#include <efl_extension.h>

#include "common_utils.h"

#include "privacy_setting.h"
#include "privacy_view.h"

static void win_delete_request_cb(void *data, Evas_Object * obj, void *event_info)
{
	ui_app_exit();
}

static void app_start(void* data)
{
	struct app_data_s* ad = (struct app_data_s*)data;

	/* Add conformant to window */
	ad->conform = elm_conformant_add(ad->win);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	/* Add layout to conformant */
	ad->layout = elm_layout_add(ad->conform);
	evas_object_size_hint_weight_set(ad->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_theme_set(ad->layout, "layout", "application", "default");
	elm_object_content_set(ad->conform, ad->layout);
	evas_object_show(ad->layout);

	/* Add naviframe to layout */
	ad->nf = elm_naviframe_add(ad->layout);
	log_if(!ad->nf, 1, "ad->nf is null");

	/* Create privacy menu view */
	create_privacy_menu_view(ad);

	elm_object_part_content_set(ad->layout, "elm.swallow.content", ad->nf);
	eext_object_event_callback_add(ad->nf, EEXT_CALLBACK_BACK, eext_naviframe_back_cb, NULL);
	eext_object_event_callback_add(ad->nf, EEXT_CALLBACK_MORE, eext_naviframe_more_cb, NULL);
	evas_object_show(ad->nf);

	return ;
}

static bool app_create(void * data)
{
	struct app_data_s *ad = (struct app_data_s*)data;

	elm_app_base_scale_set(1.8);

	ad->win = elm_win_add(NULL, "setting-privacy", ELM_WIN_BASIC);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}
	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
	evas_object_show(ad->win);

	app_start(ad);

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
}

static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
}
static void app_terminate(void *data)
{
}

static void ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
}


int main(int argc, char *argv[])
{
	struct app_data_s ad = {0, };
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0, };
	app_event_handler_h handlers[2] = {NULL, };

	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "ui_app_main() is failed. err = %d", ret);

	return 0;
}

