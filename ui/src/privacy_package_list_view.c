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
#include <policy-manager.h>
#include <privilege_info.h>
#include <glib.h>
#include <pkgmgr-info.h>

#include "common_utils.h"
#include "privacy_setting.h"
#include "privacy_view.h"

static GList* pkg_list;
static GList* privilege_list;
static GList* pkg_data_list;

int change_cnt;
Evas_Object *save_btn;
bool first;

#define UIDMAXLEN 10

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
	struct app_data_s* ad = (struct app_data_s*)data;
	item_data_s *id = (item_data_s*)data;
	if (id->status)
		id->status = EINA_FALSE;
	else
		id->status = EINA_TRUE;

	char* selected = (char*)id->pkgid;
	GList* l;

	for (l = pkg_data_list; l != NULL; l = l->next) {
		pkg_data_s* pkg_data = (pkg_data_s*)l->data;
		char* pkgid = (char*)pkg_data->pkgid;
		if (strcmp(selected, pkgid) == 0) {
			if (pkg_data->change) {/* toggle */
				--change_cnt;
				pkg_data->change = false;
			} else {
				++change_cnt;
				pkg_data->change = true;
			}
			log_if(change_cnt < 0, 1, "change_cnt < 0");
		}
	}
	if (change_cnt > 0) {
		if (first) {
			elm_object_text_set(save_btn, "SAVE");
			elm_object_disabled_set(save_btn, EINA_FALSE);
		}
		evas_object_show(save_btn);
		LOGD("save_btn enabled");
	} else {
		evas_object_hide(save_btn);
		LOGD("save_btn diabled");
	}
}
static void __get_package_privacy_status(pkg_data_s* pkg_data, char* privilege_name)
{
	GList* l;
	char uid[UIDMAXLEN];
	snprintf(uid, UIDMAXLEN, "%d", getuid());
	for (l = pkg_data->applist; l != NULL; l = l->next) {
		char* appid = (char*)l->data;

		/* Make policy filter to get specific uid, appid and privilege's status */
		policy_entry* p_filter;
		int ret = 0;
		ret = security_manager_policy_entry_new(&p_filter);
		log_if(ret != SECURITY_MANAGER_SUCCESS, 1, "security_manager_policy_entry_new failed. ret = %s", ret);
		log_if(p_filter == NULL, 1, "security_manager_policy_entry_new failed failed. creation of new policy entry did not allocate memory");
		ret = security_manager_policy_entry_set_application(p_filter, appid);
		log_if(ret != SECURITY_MANAGER_SUCCESS, 1, "security_manager_policy_entry_set_application failed. ret = %d", ret);
		ret = security_manager_policy_entry_set_user(p_filter, uid);
		log_if(ret != SECURITY_MANAGER_SUCCESS, 1, "security_manager_policy_entry_set_user failed. ret = %d", ret);
		security_manager_policy_entry_set_privilege(p_filter, privilege_name);
		log_if(ret != SECURITY_MANAGER_SUCCESS, 1, "security_manager_policy_entry_set_privilege failed. ret = %d", ret);

		/* Get policy filtered by p_filter */
		policy_entry **pp_policy = NULL;
		size_t pp_policy_size = 0;
		ret = security_manager_get_policy(p_filter, &pp_policy, &pp_policy_size);
		log_if(ret != SECURITY_MANAGER_SUCCESS, 1, "security_manager_get_policy failed. ret = %d", ret);

		/* Get level from policy */
		unsigned int i = 0;
		for (i = 0; i < pp_policy_size; ++i) {
			char* result = (char*)security_manager_policy_entry_get_level(pp_policy[i]);
			log_if(result == NULL, 1, "security_manager_policy_entry_get_level failed. No policy is declared for pkgid: %s, uid: %s, appid: %s, privilege: %s", pkg_data->pkgid, uid, appid, privilege_name);
			if (strncmp("Allow", result, strlen("Allow")) == 0) {
				pkg_data->status = EINA_TRUE;
				break;
			}
		}

		/* Free policy entries */
		security_manager_policy_entry_free(p_filter);
		for (i = 0; i < pp_policy_size ; ++i)
			security_manager_policy_entry_free(pp_policy[i]);

		if (pkg_data->status == EINA_TRUE)
			break;
	}
}
static Eina_Bool get_package_privacy_status(char* package)
{
	Eina_Bool status = EINA_FALSE;

	GList* l;
	for (l = pkg_data_list; l != NULL; l = l->next) {
		pkg_data_s* pkg_data = (pkg_data_s*)l->data;
		if (strcmp(pkg_data->pkgid, package) == 0) {
			status = pkg_data->status;
			break;
		}
	}

	return status;
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

static void _save_btn_clicked_cb(void *user_data, Evas_Object *obj, void *event_info)
{

	char uid[UIDMAXLEN];
	snprintf(uid, UIDMAXLEN, "%d", getuid());

	/* Send policy change request to security-manager */
	GList* l;
	GList* ll;
	GList* lll;
	int ret = 0;
	for (l = pkg_data_list; l != NULL; l = l->next) {
		pkg_data_s* pkg_data = (pkg_data_s*)l->data;
		char* pkgid = (char*)pkg_data->pkgid;
		if (pkg_data->change) {
			char* level;
			if (pkg_data->status) {
				level = "Deny";
				pkg_data->status = false;
			} else {
				level = "Allow";
				pkg_data->status = true;
			}

			LOGD("%s will be changed to %s", pkgid, level);

			int priv_num = g_list_length(pkg_data->privlist);
			int app_num = g_list_length(pkg_data->applist);
			int entry_num = priv_num*app_num;
			LOGD("priv_num = %d, app_num = %d, entry_num = %d", priv_num, app_num, entry_num);

			/* Make policy update request */
			policy_update_req *policy_update_request;
			ret = security_manager_policy_update_req_new(&policy_update_request);
			log_if(ret != SECURITY_MANAGER_SUCCESS, 1, "security_manager_policy_update_req_new failed. ret = %d", ret);
			log_if(policy_update_request == NULL, 1, "security_manager_policy_entry_new failed failed. creation of new policy request did not allocate memory");
			/* Make policy entries to update according to the number of privilege and appid of package */
			policy_entry *entry[entry_num];
			int entry_index = 0;
			for (ll = pkg_data->applist; ll != NULL; ll = ll->next) {
				char* appid = (char*)ll->data;
				for (lll = pkg_data->privlist; lll != NULL; lll = lll->next) {
					char* privilege_name = (char*)lll->data;
					LOGD("%d: Change to %s => uid: %s, appid: %s, privilege_name: %s", entry_index, level, uid, appid, privilege_name);
					/* Make entries by each uid, appid, and privilege_name */
					ret = security_manager_policy_entry_new(&entry[entry_index]);
					log_if(ret != SECURITY_MANAGER_SUCCESS, 1, "security_manager_policy_entry_new failed. ret = %d", ret);
					log_if(entry[entry_index] == NULL, 1, "security_manager_policy_entry_new failed failed. creation of new policy entry did not allocate memory");
					ret = security_manager_policy_entry_set_application(entry[entry_index], appid);
					log_if(ret != SECURITY_MANAGER_SUCCESS, 1, "security_manager_policy_entry_set_application failed. ret = %d", ret);
					ret = security_manager_policy_entry_set_privilege(entry[entry_index], privilege_name);
					log_if(ret != SECURITY_MANAGER_SUCCESS, 1, "security_manager_policy_entry_set_privilege failed. ret = %d", ret);
					ret = security_manager_policy_entry_set_user(entry[entry_index], uid);
					log_if(ret != SECURITY_MANAGER_SUCCESS, 1, "security_manager_policy_entry_set_user failed. ret = %d", ret);
					ret = security_manager_policy_entry_set_level(entry[entry_index], level);
					log_if(ret != SECURITY_MANAGER_SUCCESS, 1, "security_manager_policy_entry_set_level failed. ret = %d", ret);

					/* Add entry to policy update request */
					ret = security_manager_policy_update_req_add_entry(policy_update_request, entry[entry_index]);
					log_if(ret != SECURITY_MANAGER_SUCCESS, 1, "security_manager_policy_update_req_add_entry failed. ret = %d", ret);
					entry_index++;
				}
			}
			/* Send policy update request */
			ret = security_manager_policy_update_send(policy_update_request);
			log_if(ret != SECURITY_MANAGER_SUCCESS, 1, "security-manager update req failed. ret = %d", ret);

			/* Free policy entries */
			for (--entry_index; entry_index >= 0; --entry_index)
				security_manager_policy_entry_free(entry[entry_index]);

			security_manager_policy_update_req_free(policy_update_request);
			pkg_data->change = false;
		}
	}
	change_cnt = 0;
	evas_object_hide(save_btn);
}

static int pkg_app_list_cb(pkgmgrinfo_appinfo_h pkg_handle, void* user_data)
{
	char* appid = NULL;
	pkg_data_s* pkg_data = (pkg_data_s*)user_data;
	int ret = pkgmgrinfo_appinfo_get_appid(pkg_handle, &appid);
	return_if(ret != PMINFO_R_OK, , -1, "pkgmgrinfo_appinfo_get_appid failed");
	char* appidd = strdup(appid);
	return_if(appid == NULL, , -1, "appid strdup failed");
	pkg_data->applist = g_list_append(pkg_data->applist, appidd);
	return ret;
}

static int pkg_list_cb(pkgmgrinfo_pkginfo_h filter_handle, void *user_data)
{
	char* privilege_name = strdup((char*)user_data);
	char *pkgid = NULL;
	int ret = pkgmgrinfo_pkginfo_get_pkgid(filter_handle, &pkgid);
	char *pkgidd = strdup(pkgid);
	return_if(ret != PMINFO_R_OK, , -1, "pkgmgrinfo_pkginfo_get_pkgname failed");
	GList* find = g_list_find_custom(pkg_list, pkgid, (GCompareFunc)strcmp);
	if (find != NULL) {
		/* Add privilege info to package data list */
		GList* l;
		for (l = pkg_data_list; l != NULL; l = l->next) {
			pkg_data_s* temp = (pkg_data_s*)l->data;
			char* temp_pkgid = (char*)temp->pkgid;
			if (strcmp(temp_pkgid, pkgid) == 0) {
				temp->privlist = g_list_append(temp->privlist, privilege_name);
				if (!temp->status)
					__get_package_privacy_status(temp, privilege_name);
				break;
			}
		}
		return ret;
	}

	pkg_list = g_list_append(pkg_list, pkgidd);

	/* Make package data item */
	pkg_data_s *pkg_data = (pkg_data_s *)malloc(sizeof(pkg_data_s));

	/* Add pkg id to package data item */
	pkg_data->pkgid = strdup(pkgid);

	/* Add privilege info to package data item */
	pkg_data->privlist = NULL;
	pkg_data->privlist = g_list_append(pkg_data->privlist, privilege_name);

	/* Add app list to package data item */
	pkg_data->applist = NULL;
	pkgmgrinfo_pkginfo_h pkg_handle;
	ret = pkgmgrinfo_pkginfo_get_pkginfo(pkgid, &pkg_handle);
	return_if(ret != PMINFO_R_OK, , -1, "pkgmgrinfo_pkginfo_get_pkginfo failed");
	ret = pkgmgrinfo_appinfo_get_list(pkg_handle, PMINFO_ALL_APP, pkg_app_list_cb, pkg_data);
	return_if(ret != PMINFO_R_OK, pkgmgrinfo_pkginfo_destroy_pkginfo(pkg_handle), -1, "pkgmgrinfo_appinfo_get_list failed");
	pkgmgrinfo_pkginfo_destroy_pkginfo(pkg_handle);

	/* Get package privacy status */
	pkg_data->status = false;
	pkg_data->change = false;
	__get_package_privacy_status(pkg_data, privilege_name);

	/* Append package data item to package_data_list */
	pkg_data_list = g_list_append(pkg_data_list, pkg_data);
	return ret;
}
static int get_uniq_pkg_list_by_privacy(const char* privacy)
{
	int ret = 0;
	/* For privilege list loop -> Get pkg_list by privilege */
	if (pkg_list != NULL) {
		g_list_free(pkg_list);
		pkg_list = NULL;
	}
	if (pkg_data_list != NULL) {
		g_list_free(pkg_data_list);
		pkg_data_list = NULL;
	}
	GList* l;
	int uid = getuid();
	for (l = privilege_list; l != NULL; l = l->next) {
		char* privilege_name = (char*)l->data;
		/* For each privilege */
		pkgmgrinfo_pkginfo_filter_h filter_handle;
		ret = pkgmgrinfo_pkginfo_filter_create(&filter_handle);
		return_if(ret != PMINFO_R_OK, , -1, "pkgmgrinfo_pkginfo_filter_create failed");
		ret = pkgmgrinfo_pkginfo_filter_add_string(filter_handle, PMINFO_PKGINFO_PROP_PACKAGE_PRIVILEGE, privilege_name);
		LOGD("uid: %d, privacy: %s, privilege: %s", uid, privacy, privilege_name);
		return_if(ret != PMINFO_R_OK, pkgmgrinfo_pkginfo_filter_destroy(filter_handle), -1, "pkgmgrinfo_pkginfo_filter_add_string failed");
		/* Get uniq pkg list */
		ret = pkgmgrinfo_pkginfo_usr_filter_foreach_pkginfo(filter_handle, pkg_list_cb, privilege_name, uid);
		return_if(ret != PMINFO_R_OK, pkgmgrinfo_pkginfo_filter_destroy(filter_handle), -1, "pkgmgrinfo_pkginfo_usr_filter_foreach_pkginfo failed");
	}

	/* To check : each privacy related package's app, privilege info
	for (l = pkg_data_list; l != NULL; l = l->next) {
		pkg_data_s* temp = (pkg_data_s*)l->data;
		LOGD(" * PACKAGE ID : %s", temp->pkgid);
		GList* ll;
		LOGD(" * APP ID");
		for (ll = temp->applist; ll != NULL; ll = ll->next) {
			char* temp_appid = (char*)ll->data;
			LOGD(" - %s", temp_appid);
		}
		LOGD(" * PRIVILEGE");
		for (ll = temp->privlist; ll != NULL; ll = ll->next) {
			char* temp_privname = (char*)ll->data;
			LOGD(" - %s", temp_privname);
		}
	}*/

	return ret;
}
void create_privacy_package_list_view(struct app_data_s* ad, item_data_s *selected_id)
{
	/* Add genlist */
	Evas_Object *genlist = common_genlist_add(ad->nf);

	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);

	/* Set itc */
	Elm_Genlist_Item_Class *itc = elm_genlist_item_class_new();
	itc->item_style = "default";
	itc->func.content_get = gl_content_get_cb;
	itc->func.text_get = gl_text_get_cb;
	itc->func.del = gl_del_cb;

	/* Get privilege list by privacy */
	if (privilege_list != NULL) {
		g_list_free(privilege_list);
		privilege_list = NULL;
	}
	int ret = privilege_info_get_privilege_list_by_privacy(ad->privacy, &privilege_list);
	log_if(ret != PRVMGR_ERR_NONE, 1, "Couldn't get privilege list of privacy: %s", ad->privacy);

	/* Get unique package list filtered by given privacy related privileges */
	ret = get_uniq_pkg_list_by_privacy(ad->privacy);
	log_if(ret != 0, 1, "get_unique_pkg_list_by_privacy failed");
	pkg_list = g_list_sort(pkg_list, (GCompareFunc)strcmp);

	/* Append privacy related package as genlist item */
	change_cnt = 0;
	first = true;
	GList* l;
	int i = 0;
	Elm_Object_Item *it = NULL;
	for (l = pkg_list; l != NULL; l = l->next) {
		item_data_s *id = calloc(sizeof(item_data_s), 1);
		id->index = i++;
		char temp[256];
		char* pkg_name = (char*)l->data;
		id->pkgid = strdup(pkg_name);
		snprintf(temp, sizeof(temp), "%d : %s", i, pkg_name);
		id->title = strdup(temp);
		/* Get privacy status of given package */
		id->status = get_package_privacy_status(pkg_name);
		it = elm_genlist_item_append(genlist, itc, id, NULL, ELM_GENLIST_ITEM_NONE, privacy_package_selected_cb, id);
		log_if(it == NULL, 1, "Error in elm_genlist_item_append");
	}
	elm_genlist_item_class_free(itc);
	evas_object_show(genlist);

	/* TBD: change nf_it_title to proper DID : use dgettext() */
	char nf_it_title[256];
	snprintf(nf_it_title, sizeof(nf_it_title), "%s Packages", ad->privacy);

	/* Push naviframe item */
	Elm_Object_Item *nf_it = elm_naviframe_item_push(ad->nf, nf_it_title, common_back_btn_add(ad), NULL, genlist, NULL);

	/* Add button to save privacy package policy changes */
	save_btn = elm_button_add(ad->nf);
	evas_object_smart_callback_add(save_btn, "clicked", _save_btn_clicked_cb, ad);
	elm_object_style_set(save_btn, "naviframe/title_right");
	elm_object_disabled_set(save_btn, EINA_TRUE);
	elm_object_item_part_content_set(nf_it, "title_right_btn", save_btn);

	elm_object_item_domain_text_translatable_set(nf_it, PACKAGE, EINA_TRUE);
}
