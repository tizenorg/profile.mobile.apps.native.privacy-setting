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
 * @file        common_utils.h
 * @author      Yunjin Lee (yunjin-.lee@samsung.com)
 * @version     1.0
 * @brief
 */

#ifndef __UG_SETTING_PRIVACY_COMMON_UTILS_H_
#define __UG_SETTING_PRIVACY_COMMON_UTILS_H_

#include <Elementary.h>
#include <dlog.h>

#include "privacy_setting_ug.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "UG_PRIVACY_SETTING"

#ifdef _cplusplus
extern "C" {
#endif

#define return_if(condition, expr, returnValue, ...) \
	if (condition) { \
		LOGE(__VA_ARGS__); \
		expr; \
		return returnValue; \
	}

#define lreturn_if(condition, expr, returnValue, ...) \
	if (condition) { \
		LOGD(__VA_ARGS__); \
		expr; \
		return returnValue; \
	}

#define log_if(condition, level, ...) \
	if (condition) { \
		if (level) { \
			LOGE(__VA_ARGS__); \
		} else { \
			LOGD(__VA_ARGS__); \
		} \
	}


Evas_Object *common_genlist_add(Evas_Object *parent);
Evas_Object *common_back_btn_add(struct ug_data_s *ugd);

#ifdef _cplusplus
}
#endif

#endif /* __UG_SETTING_PRIVACY_COMMON_UTILS_H_ */
