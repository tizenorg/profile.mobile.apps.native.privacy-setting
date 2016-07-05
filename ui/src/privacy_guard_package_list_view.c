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
 * @file	privacy_guard_package_list_view.c
 * @author	Kyoungyong Lee (k.y.lee@samsung.com)
 * @version	1.0
 * @brief
 */

#include <efl_extension.h>
#include <glib.h>
#include <pkgmgr-info.h>
#include <tzplatform_config.h>
#include <privacy_guard_client.h>
#include "common_utils.h"
#include "privacy_setting.h"
#include "privacy_view.h"

#define DEFAULT_ICON_PATH _TZ_SYS_RO_APP"/org.tizen.privacy-setting/res/icon/default.png"

static GList* pg_data_list;

static void _gl_del_cb(void *data, Evas_Object *obj)
{
	/* Unrealized callback can be called after this. */
	/* Accessing item_data_s can be dangerous on unrealized callback. */
	item_data_s *id = data;
	free(id);
}

static void _privacy_package_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *ei = event_info;

	/* Unhighlight selected item */
	elm_genlist_item_selected_set(ei, EINA_FALSE);
}
static char* _gl_text_get_cb(void *data, Evas_Object *obj, const char *part)
{
	pg_item_data_s *id = data;

	if (!strcmp(part, "elm.text")) {
		return strdup(id->label);
	} else if (!strcmp(part, "elm.text.sub")) {
		return strdup(id->description);
	} else {
		return NULL;
	}

	return "FAIL";
}

static void _privacy_package_check_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
	pg_item_data_s *id = (pg_item_data_s*)data;

	LOGD("current policy: [%d]", id->status);

	int res = privacy_guard_client_update_monitor_policy(id->user_id, id->pkg_id, id->privacy_id, !id->status);
	log_if(res != PRIV_GUARD_ERROR_SUCCESS, 1, "privacy_guard_client_update_monitor_policy() is failed. [%d]", res);
	id->status = !id->status;
	LOGD("changed policy: [%d]", id->status);
}

static Evas_Object* _gl_content_get_cb(void *data, Evas_Object *obj, const char *part)
{
	pg_item_data_s *id = (pg_item_data_s*)data;

	if (!strcmp("elm.swallow.icon", part)) {
		Evas_Object *icon;
		icon = elm_image_add(obj);
		elm_image_file_set(icon, id->icon, NULL);
		evas_object_size_hint_min_set(icon, ELM_SCALE_SIZE(70), ELM_SCALE_SIZE(70));

		return icon;
	}

	if (!strcmp(part, "elm.swallow.end")) {
		Evas_Object *check;
		Eina_Bool status = id->status;
		check = elm_check_add(obj);
		elm_object_style_set(check, "on&off");
		elm_check_state_set(check, status);
		evas_object_repeat_events_set(check, EINA_FALSE);
		evas_object_propagate_events_set(check, EINA_FALSE);
		evas_object_size_hint_weight_set(check, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(check, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_smart_callback_add(check, "changed", _privacy_package_check_changed_cb, data);
		evas_object_show(check);

		return check;
	}

	return NULL;
}

static bool _privacy_package_info_cb(const char *package_id, const int count, const int monitor_policy, void *user_data)
{
	pg_data_s *data = (pg_data_s *)malloc(sizeof(pg_data_s));
	data->pkg_id = strdup(package_id);
	data->count = count;
	data->monitor_policy = monitor_policy;

	pg_data_list = g_list_append(pg_data_list, data);

	return true;
}

void create_privacy_guard_package_list_view(struct app_data_s* ad)

{
	/* Add genlist */
	Evas_Object *genlist = common_genlist_add(ad->nf);

	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);

	/* Set itc */
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();

	/* Get privacy guard data (pkg, count, policy) list by a privacy */
	if (pg_data_list!= NULL) {
		g_list_free(pg_data_list);
		pg_data_list = NULL;
	}

	// user ID
	uid_t user_id = getuid();

	// get package_id, count, policy using a privacy
	int res = privacy_guard_client_foreach_package_info_by_privacy_id(user_id, ad->privacy, _privacy_package_info_cb, NULL);
	log_if(res != PRIV_GUARD_ERROR_SUCCESS, 1, "privacy_guard_client_foreach_package_info_by_privacy_id() is failed. [%d]", res);

	// no content in the genlist
	if (!pg_data_list) {
		itc->item_style = "default";
		itc->func.text_get = _gl_text_get_cb;
		itc->func.del = _gl_del_cb;

		Elm_Object_Item *it = NULL;
		char temp[256] = {'\0',};
		pg_item_data_s *item = calloc(sizeof(pg_item_data_s), 1);
		item->index = 0;
		snprintf(temp, sizeof(temp), "<font color=#A9A9A9FF>No apps using %s privacy.</font>", ad->privacy);
		item->label = strdup(temp);

		// append to the genlist
		it = elm_genlist_item_append(genlist, itc, item, NULL, ELM_GENLIST_ITEM_NONE, _privacy_package_selected_cb, item);
		log_if(it == NULL, 1, "Error in elm_genlist_item_append");
	} else {
		itc->item_style = "type1";
		itc->func.content_get = _gl_content_get_cb;
		itc->func.text_get = _gl_text_get_cb;
		itc->func.del = _gl_del_cb;

		/* Append privacy related package as genlist item */
		GList* l;
		int i = 0;
		Elm_Object_Item *it = NULL;
		for (l = pg_data_list; l != NULL; l = l->next) {
			pg_item_data_s *item = calloc(sizeof(pg_item_data_s), 1);
			pg_data_s* data = (pg_data_s*)l->data;
			char temp[256] = {'\0',};
			char *label = NULL;
			char* icon = NULL;
			pkgmgrinfo_pkginfo_h handle;

			// index
			item->index = i++;

			// package ID
			item->pkg_id = strdup(data->pkg_id);

			// package label
			res = pkgmgrinfo_pkginfo_get_pkginfo(item->pkg_id, &handle);
			if (res != PMINFO_R_OK) {
				LOGE("Failed to operate pkgmgrinfo_pkginfo_get_pkginfo [%d]", res);
				LOGE("So replace the label to package ID");
				label = item->pkg_id;
			} else {
				res = pkgmgrinfo_pkginfo_get_label(handle, &label);
				if (res != PMINFO_R_OK) {
					LOGE("Failed to operate pkgmgrinfo_pkginfo_get_label [%d]", res);
					LOGE("So replace the label to package ID");
					label = item->pkg_id;
				}
			}

			// title.. (package label)
			snprintf(temp, sizeof(temp), "%s", label);
			item->label = strdup(temp);

			// description
			snprintf(temp, sizeof(temp), "<font color=#3DB9CCFF>%d</font> time(s)", data->count);
			item->description = strdup(temp);

			// user ID
			item->user_id = user_id;

			// privacy ID
			item->privacy_id = strdup(ad->privacy);

			// icon
			res = pkgmgrinfo_pkginfo_get_icon(handle, &icon);
			if (res != PMINFO_R_OK) {
				LOGE("Failed to operate pkgmgrinfo_pkginfo_get_icon [%d]", res);
				LOGE("So replace the icon to the default icon");
				item->icon = strdup(DEFAULT_ICON_PATH);
			} else {
				if (EINA_TRUE == ecore_file_exists(icon)) {
					item->icon = strdup(icon);
				}
				else {
					LOGD("The icon [%s] is not exist. So replace the icon to the default icon.", icon);
					item->icon = strdup(DEFAULT_ICON_PATH);
				}
			}

			// status
			item->status = ((data->monitor_policy == 1) ? EINA_TRUE : EINA_FALSE);

			pkgmgrinfo_pkginfo_destroy_pkginfo(handle);

			// append to the genlist
			it = elm_genlist_item_append(genlist, itc, item, NULL, ELM_GENLIST_ITEM_NONE, _privacy_package_selected_cb, item);
			log_if(it == NULL, 1, "Error in elm_genlist_item_append");
		}
	}

	elm_genlist_item_class_free(itc);
	evas_object_show(genlist);

	/* TODO: change nf_it_title to proper DID : use dgettext() */
	char nf_it_title[256];
	snprintf(nf_it_title, sizeof(nf_it_title), "%s", ad->privacy);

	/* Push naviframe item */
	Elm_Object_Item *nf_it = elm_naviframe_item_push(ad->nf, nf_it_title, common_back_btn_add(ad), NULL, genlist, NULL);

	elm_object_item_domain_text_translatable_set(nf_it, PACKAGE, EINA_TRUE);
}
