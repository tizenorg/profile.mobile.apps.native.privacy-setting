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
 * @file        common_utils.c
 * @author      Yunjin Lee (yunjin-.lee@samsung.com)
 * @version     1.0
 * @brief
 */

#include <efl_extension.h>

#include "common_utils.h"

static void gl_language_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
	elm_genlist_realized_items_update(obj);
}

Evas_Object *common_genlist_add(Evas_Object *parent)
{
	Evas_Object *genlist = elm_genlist_add(parent);
	return_if(!genlist, , NULL, "elm_genlist_add failed");

	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	evas_object_smart_callback_add(genlist, "language,changed", gl_language_changed_cb, NULL);

	return genlist;
}

static void back_btn_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
    struct ug_data_s *ugd = (struct ug_data_s *)data;
    return_if(!ugd, , , "ugd is null");
    elm_naviframe_item_pop(ugd->nf);
}

Evas_Object *common_back_btn_add(struct ug_data_s *ad)
{
	Evas_Object *btn = elm_button_add(ad->nf);
	elm_object_style_set(btn, "naviframe/back_btn/default");
	evas_object_smart_callback_add(btn, "clicked", back_btn_clicked_cb, ad);

	return btn;
}
