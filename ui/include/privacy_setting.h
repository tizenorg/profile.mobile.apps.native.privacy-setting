/**
 * Copyright (c) 2016 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*
 * @file        privacy_setting.h
 * @author      Yunjin Lee <yunjin-.lee@samsung.com>
 * @version     1.0
 * @brief
 */

#ifndef __UI_PRIVACY_SETTING_H__
#define __UI_PRIVACY_SETTING_H__

#include <Elementary.h>
#include <ui-gadget.h>
#include <ui-gadget-module.h>
#include <glib.h>
#include <app.h>

#define PRIVACY_MENU_SETTING "Privacy Setting"
#define PRIVACY_MENU_GUARD_SETTING "Privacy Guard Setting"

struct app_data_s {
	Evas_Object *parent_layout;
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *bg;
	Evas_Object *layout;
	Evas_Object *nf;
	void *data;
	Elm_Theme *theme;

	GList *privacy_list;
	char* privacy;
};

struct app_data_s *get_app_data();

#endif /* __UI_PRIVACY_SETTING_H__ */
