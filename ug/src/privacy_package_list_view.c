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
 * @file		privacy_package_list_view.c
 * @author	  Yunjin Lee (yunjin-.lee@samsung.com)
 * @version	 1.0
 * @brief
 */

#include <efl_extension.h>

#include "common_utils.h"
#include "privacy_setting_ug.h"
#include "privacy_view.h"

static void gl_del_cb(void *data, Evas_Object *obj)
{
	/* Unrealized callback can be called after this. */
	/* Accessing item_data_s can be dangerous on unrealized callback. */
	item_data_s *id = data;
	free(id);
}

static void privacy_package_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *ei = event_info;
	/* Unhighlight selected item */
	elm_genlist_item_selected_set(ei, EINA_FALSE);
	/* TBD? Add package's privacy info.
	   For example, package's selected privacy related privilege list.
	   Or remove. */
}
static char* gl_text_get_cb(void *data, Evas_Object *obj, const char *part)
{
	item_data_s *id = data;
	lreturn_if(!strcmp(part, "elm.text"), , strdup(id->title), "privacy-package item = %s", id->title);
	return "FAIL";
}
static void privacy_package_check_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
	/* TBD: Add privacy on&off function */
	item_data_s *id = (item_data_s*)data;
	if (id->status)
		id->status = false;
	else
		id->status = true;
	Eina_Bool status = id->status;
	char* selected = (char*)id->title;
	if (status)
		LOGD("%s is on", selected);
	else if (!status)
		LOGD("%s is off", selected);
}

static Evas_Object* gl_content_get_cb(void *data, Evas_Object *obj, const char *part)
{
	Evas_Object *check;
	item_data_s *id = (item_data_s*)data;
	Eina_Bool status = id->status;

	if (strcmp(part, "elm.swallow.end"))
		return NULL;

	check = elm_check_add(obj);

	elm_object_style_set(check, "on&off");
	elm_check_state_set(check, status);
	evas_object_repeat_events_set(check, EINA_FALSE);
	evas_object_propagate_events_set(check, EINA_FALSE);
	evas_object_size_hint_weight_set(check, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(check, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_smart_callback_add(check, "changed", privacy_package_check_changed_cb, data);
	evas_object_show(check);

	return check;
}

void create_privacy_package_list_view(struct ug_data_s* ugd, item_data_s *selected_id)
{
	/* Add genlist */
	Evas_Object *genlist = common_genlist_add(ugd->nf);

	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);

	/* Set itc */
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	itc->item_style = "default";
	itc->func.content_get = gl_content_get_cb;
	itc->func.text_get = gl_text_get_cb;
	itc->func.del = gl_del_cb;

	/* Append privacy related package as genlist item */
	/* TBD : Data should be replaced by privacy related package list */
	const char* items[] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "eleven"};
	int i = 0;
	Elm_Object_Item *it = NULL;
	for (i = 0; i < 10; ++i) {
		item_data_s *id = calloc(sizeof(item_data_s), 1);
		id->index = i;
		char temp[256];
		snprintf(temp, sizeof(temp), "%s%s", selected_id->title, items[i]);
		id->title = strdup(temp);
		/* id->status = ; */
		it = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, privacy_package_selected_cb, id);
		log_if(it == NULL, 1, "Error in elm_genlist_item_append");
	}
	elm_genlist_item_class_free(itc);
	evas_object_show(genlist);

	/* Push naviframe item */
	/* Change "Package List" to proper DID : use dgettext() */
	Elm_Object_Item *nf_it = elm_naviframe_item_push(ugd->nf, "Package List", common_back_btn_add(ugd), NULL, genlist, NULL);
	elm_object_item_domain_text_translatable_set(nf_it, PACKAGE, EINA_TRUE);
}
