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

#include "privacy_setting_ug.h"

typedef struct {
	char* pkgid;
	GList* applist;
	GList* privlist;
} pkg_data_s;

typedef struct item_data {
	int index;
	char *title;
	char *pkgid;
	bool status;
} item_data_s;

void create_privacy_package_list_view(struct ug_data_s *ugd, item_data_s *selected_id);

void create_privacy_list_view(struct ug_data_s *ugd);
