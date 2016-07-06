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
 * @file		privacy_menu_view.c
 * @author	  Yunjin Lee (yunjin-.lee@samsung.com)
 * @version	 1.0
 * @brief
 */

#include <efl_extension.h>
#include <privilege_info.h>
#include <glib.h>

#include "common_utils.h"
#include "privacy_setting.h"
#include "privacy_view.h"

GList *privacy_menu_list;

static Eina_Bool quit_cb(void *data, Elm_Object_Item *it)
{
	struct app_data_s* ad = (struct app_data_s*)data;
	return_if(!ad, , EINA_FALSE, "ad is null");
	ui_app_exit();

	return EINA_FALSE;
}

static char* gl_text_get_cb(void *data, Evas_Object *obj, const char *part)
{
	item_data_s *id = data;
	/* TBD: Use dgettext when menu string is replaced by DID */
	lreturn_if(!strcmp(part, "elm.text"), , strdup(id->title), "selected menu = %s", id->title);
	return "FAIL";
}
static void gl_del_cb(void *data, Evas_Object *obj)
{
	/* Unrealized callback can be called after this. */
	/* Accessing item_data_s can be dangerous on unrealized callback. */
	item_data_s *id = data;
	free(id);
}

static void menu_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
	/* Get selected privacy */
	Elm_Object_Item *ei = event_info;
	item_data_s *selected_id = elm_object_item_data_get(ei);
	LOGD("%s is selected, index = %d", selected_id->title, selected_id->index);

	/* Unhighlight selected item */
	elm_genlist_item_selected_set(ei, EINA_FALSE);

	struct app_data_s *ad = (struct app_data_s *)data;
	return_if(ad == NULL, , , "ad is null");

	int ret = privilege_info_get_privacy_list(&(ad->privacy_list));
	log_if(ret != PRVMGR_ERR_NONE, 1, "Failed to get privacy_list");

	if (strstr(selected_id->title, PRIVACY_MENU_SETTING) != NULL) {
		create_privacy_list_view(ad);
	} else if (strstr(selected_id->title, PRIVACY_MENU_GUARD_SETTING) != NULL) {/* privacy guard */
		create_privacy_guard_list_view(ad);
	} else {
		LOGE("selected_id->title = %s, no matching menu", selected_id->title);
	}
}

/*Privacy Menu List*/
void create_privacy_menu_view(struct app_data_s *ad)
{
	Evas_Object *genlist = common_genlist_add(ad->nf);

	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);

	/* Set privacy menu list */
	privacy_menu_list = NULL;
	privacy_menu_list = g_list_append(privacy_menu_list, PRIVACY_MENU_SETTING);
	privacy_menu_list = g_list_append(privacy_menu_list, PRIVACY_MENU_GUARD_SETTING);

	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	itc->item_style = "default";
	itc->func.text_get = gl_text_get_cb;
	itc->func.del = gl_del_cb;
	Elm_Object_Item *it = NULL;
	int i = 0;
	for (i = 0; i < (int)g_list_length(privacy_menu_list); ++i) {
		item_data_s *id = calloc(sizeof(item_data_s), 1);
		id->index = i;
		id->title = (char*)g_list_nth_data(privacy_menu_list, i);
		it = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, menu_selected_cb, ad);
		log_if(it == NULL, 1, "Error in elm_genlist_item_append");
	}
	elm_genlist_item_class_free(itc);
	evas_object_show(genlist);

	/* Change "Privacy & Safety" to proper DID : use dgettext() */
	char* title = dgettext("privacy-setting", "IDS_ST_HEADER_PRIVACY_AND_SAFETY_ABB");
	Elm_Object_Item *nf_it = elm_naviframe_item_push(ad->nf, title, common_back_btn_add(ad), NULL, genlist, NULL);
	elm_object_item_domain_text_translatable_set(nf_it, PACKAGE, EINA_TRUE);

	elm_naviframe_item_pop_cb_set(nf_it, quit_cb, ad);
}

