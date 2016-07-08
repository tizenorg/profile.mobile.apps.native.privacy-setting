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
 * @file	privacy_guard_list_view.c
 * @author	Kyoungyong Lee (k.y.lee@samsung.com)
 * @version	1.0
 * @brief
 */

#include <efl_extension.h>
#include "common_utils.h"
#include "privacy_setting.h"
#include "privacy_view.h"
#include "privilege_info.h"

static char* gl_text_get_cb(void *data, Evas_Object *obj, const char *part)
{
	item_data_s *id = data;
	lreturn_if(!strcmp(part, "elm.text"), , strdup(id->title), "privacy = %s", id->title);
	return "FAIL";
}
static void gl_del_cb(void *data, Evas_Object *obj)
{
	/* Unrealized callback can be called after this. */
	/* Accessing item_data_s can be dangerous on unrealized callback. */
	item_data_s *id = data;
	free(id);
}

static void _privacy_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
	/* Get selected privacy */
	Elm_Object_Item *ei = event_info;
	item_data_s *selected_id = elm_object_item_data_get(ei);
	LOGD("%s is selected, index = %d", selected_id->title, selected_id->index);

	/* Unhighlight selected item */
	elm_genlist_item_selected_set(ei, EINA_FALSE);

	struct app_data_s *ad = (struct app_data_s *)data;
	return_if(ad == NULL, , , "ad is null");

	ad->privacy = (char*)selected_id->privacy;

	create_privacy_guard_package_list_view(ad);
}

/*Privacy List*/
void create_privacy_guard_list_view(struct app_data_s *ad)
{
	Evas_Object *genlist = common_genlist_add(ad->nf);

	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);

	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	itc->item_style = "default";
	itc->func.text_get = gl_text_get_cb;
	itc->func.del = gl_del_cb;
	Elm_Object_Item *it = NULL;
	int i = 0;
	for (i = 0; i < (int)g_list_length(ad->privacy_list); ++i) {
		item_data_s *id = calloc(sizeof(item_data_s), 1);
		id->index = i;
		char* privacy_display = NULL;
		id->privacy = strdup((char*)g_list_nth_data(ad->privacy_list, i));
		LOGD("privacy = %s", id->privacy);
		int ret = privilege_info_get_privacy_display(id->privacy, &privacy_display);
		log_if(ret != PRVMGR_ERR_NONE, 1, "privacy_display = %s", privacy_display);
		id->title = dgettext("privilege", privacy_display);
		it = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, _privacy_selected_cb, ad);
		log_if(it == NULL, 1, "Error in elm_genlist_item_append");
	}
	elm_genlist_item_class_free(itc);
	evas_object_show(genlist);

	/* Change "Privacy Setting" to proper DID : use dgettext() */
	Elm_Object_Item *nf_it = elm_naviframe_item_push(ad->nf, dgettext("privacy-setting", PRIVACY_MENU_ACCESS), common_back_btn_add(ad), NULL, genlist, NULL);

	elm_object_item_domain_text_translatable_set(nf_it, PACKAGE, EINA_TRUE);
}

