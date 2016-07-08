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
 * @file        privacy_view.h
 * @author      Yunjin Lee <yunjin-.lee@samsung.com>
 * @version     1.0
 * @brief
 */


#include <Elementary.h>
#include <ui-gadget.h>
#include <ui-gadget-module.h>
#include <glib.h>
#include <app.h>

#include "privacy_setting.h"

typedef struct {
	char* pkgid;
	char* label;
	char* icon;
	GList* applist;
	GList* privlist;
	bool change; /* toggle */
	bool status; /* original status */
	bool is_global;
} pkg_data_s;

typedef struct item_data {
	int index;
	char *title;
	char *pkgid;
	char *privacy;
	char *menu;
	char *icon;
	bool status;
} item_data_s;

typedef struct {
	char* pkg_id;
	int count;
	int monitor_policy;
} pg_data_s;

typedef struct {
	int index;
	char *label;
	char *description;
	uid_t user_id;
	char *pkg_id;
	char *privacy_id;
	char *icon;
	bool status;
} pg_item_data_s;

void create_privacy_package_list_view(struct app_data_s *ad);

void create_privacy_list_view(struct app_data_s *ad);

void create_privacy_menu_view(struct app_data_s *ad);

void create_privacy_guard_list_view(struct app_data_s *ad);

void create_privacy_guard_package_list_view(struct app_data_s *ad);

