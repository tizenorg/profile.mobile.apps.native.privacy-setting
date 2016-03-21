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
 * @file		main_privacy_setting_ug.c
 * @author		Yunjin Lee (yunjin-.lee@samsung.com)
 * @version		1.0
 * @brief
 */

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

#include <libintl.h>
#include <efl_extension.h>

#include "common_utils.h"

#include "privacy_setting_ug.h"
#include "privacy_view.h"

#include <privilege_info.h>

static struct ug_data_s *g_ugd;

struct ug_data_s *get_ug_data()
{
	return g_ugd;
}

static void *on_create(ui_gadget_h ug, enum ug_mode mode, app_control_h service, void *priv)
{
	struct ug_data_s *ugd = (struct ug_data_s *)priv;
	log_if(!ug || !ugd, 1, "!ug || !ugd");

	bindtextdomain(PACKAGE, LOCALEDIR);

	ugd->ug = ug;

	/* Get privacy list */
	log_if(privilege_info_get_privacy_list(&(ugd->privacy_list)) != PRVMGR_ERR_NONE, 1, "Failed to get privacy_list");

	/* Get parent layout */
	ugd->parent_layout = ug_get_parent_layout(ug);
	log_if(!ugd->parent_layout, 1, "ugd->parent_layout is null");

	/* Add bg */
	ugd->bg = elm_bg_add(ugd->parent_layout);
	log_if(!ugd->bg, 1, "ugd->bg is null");

	evas_object_size_hint_weight_set(ugd->bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(ugd->bg);

	/* Add layout */
	ugd->layout = elm_layout_add(ugd->parent_layout);
	log_if(!ugd->layout, 1, "ugd->layout is null");

	elm_layout_theme_set(ugd->layout, "layout", "application", "default");
	evas_object_size_hint_weight_set(ugd->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(ugd->layout);

	elm_object_part_content_set(ugd->layout, "elm.swallow.bg", ugd->bg);

	/* Add naviframe */
	ugd->nf = elm_naviframe_add(ugd->layout);
	log_if(!ugd->nf, 1, "ugd->nf is null");

	/* Create privacy list view */
	create_privacy_list_view(ugd);

	elm_object_part_content_set(ugd->layout, "elm.swallow.content", ugd->nf);
	eext_object_event_callback_add(ugd->nf, EEXT_CALLBACK_BACK, eext_naviframe_back_cb, NULL);
	eext_object_event_callback_add(ugd->nf, EEXT_CALLBACK_MORE, eext_naviframe_more_cb, NULL);
	evas_object_show(ugd->nf);

	return ugd->layout;
}

static void on_start(ui_gadget_h ug, app_control_h service, void *priv)
{
}

static void on_pause(ui_gadget_h ug, app_control_h service, void *priv)
{
}

static void on_resume(ui_gadget_h ug, app_control_h service, void *priv)
{
}

static void on_destroy(ui_gadget_h ug, app_control_h service, void *priv)
{
	LOGD("on_destroy");

	log_if(ug == NULL, 1, "ug is NULL");
	log_if(priv == NULL, 1, "priv is NULL");

	struct ug_data_s *ugd = (struct ug_data_s *)priv;

	if (ugd->theme) {
		elm_theme_free(ugd->theme);
		ugd->theme = NULL;
	}

	evas_object_hide(ugd->layout);
	evas_object_del(ugd->layout);
	ugd->layout = NULL;
}

static void on_message(ui_gadget_h ug, app_control_h msg, app_control_h service, void *priv)
{
}

static void on_event(ui_gadget_h ug, enum ug_event event, app_control_h service, void *priv)
{
	switch (event) {
	case UG_EVENT_LOW_MEMORY:
		break;
	case UG_EVENT_LOW_BATTERY:
		break;
	case UG_EVENT_LANG_CHANGE:
		break;
	case UG_EVENT_ROTATE_PORTRAIT:
		break;
	case UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN:
		break;
	default:
		break;
	}
}

static void on_key_event(ui_gadget_h ug, enum ug_key_event event, app_control_h service, void *priv)
{
	log_if(ug == NULL, 1, "ug is NULL");

	switch (event) {
	case UG_KEY_EVENT_END:
		ug_destroy_me(ug);
		break;
	default:
		break;
	}
}

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	return_if(ops == NULL, , -1, "ops is NULL.");

	struct ug_data_s *ugd = (struct ug_data_s *)malloc(sizeof(struct ug_data_s));
	return_if(ugd == NULL, , -1, "Fail to malloc ugd.");

	ops->create = on_create;
	ops->start = on_start;
	ops->pause = on_pause;
	ops->resume = on_resume;
	ops->destroy = on_destroy;
	ops->message = on_message;
	ops->event = on_event;
	ops->key_event = on_key_event;
	ops->priv = ugd;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	g_ugd = ugd;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	log_if(ops == NULL, 1, "ops is NULL.");

	free(ops->priv);
}

UG_MODULE_API int setting_plugin_reset(app_control_h service, void *priv)
{
	/* nothing to do for Setting>Reset */
	return 0;
}
