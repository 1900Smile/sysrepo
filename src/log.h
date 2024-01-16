/**
 * @file log.h
 * @author Michal Vasko <mvasko@cesnet.cz>
 * @brief header for logging routines
 *
 * @copyright
 * Copyright (c) 2018 - 2021 Deutsche Telekom AG.
 * Copyright (c) 2018 - 2021 CESNET, z.s.p.o.
 *
 * This source code is licensed under BSD 3-Clause License (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://opensource.org/licenses/BSD-3-Clause
 */

#ifndef _LOG_H
#define _LOG_H

#include "common.h"

#define SR_ERRINFO_INT(err_info) sr_errinfo_new(err_info, SR_ERR_INTERNAL, "Internal error (%s:%d).", __FILE__, __LINE__)
#define SR_ERRINFO_MEM(err_info) sr_errinfo_new(err_info, SR_ERR_NO_MEMORY, NULL)
#define SR_ERRINFO_LOCK(err_info, func, ret) sr_errinfo_new(err_info, (ret == ETIMEDOUT) ? SR_ERR_TIME_OUT : SR_ERR_INTERNAL, \
        "Locking a mutex failed (%s: %s).", func, strerror(ret))
#define SR_ERRINFO_COND(err_info, func, ret) sr_errinfo_new(err_info, (ret == ETIMEDOUT) ? SR_ERR_TIME_OUT : SR_ERR_INTERNAL, \
        "Waiting on a conditional variable failed (%s: %s).", func, strerror(ret))
#define SR_ERRINFO_SYSERRNO(err_info, func) sr_errinfo_new(err_info, SR_ERR_SYS, "%s() failed (%s).", func, strerror(errno))
#define SR_ERRINFO_VALID(err_info) sr_errinfo_new(err_info, SR_ERR_VALIDATION_FAILED, "Validation failed.")
#define SR_ERRINFO_SYSERRPATH(err_info, func, path) sr_errinfo_new(err_info, SR_ERR_SYS, "%s() on \"%s\" failed (%s).", \
        func, path, strerror(errno))

#define SR_LOG_WRN(...) sr_log(SR_LL_WRN, __VA_ARGS__)
#define SR_LOG_INF(...) sr_log(SR_LL_INF, __VA_ARGS__)
#define SR_LOG_DBG(...) sr_log(SR_LL_DBG, __VA_ARGS__)

#define SR_CHECK_MEM_GOTO(cond, err_info, go) if (cond) { SR_ERRINFO_MEM(&(err_info)); goto go; }
#define SR_CHECK_MEM_RET(cond, err_info) if (cond) { SR_ERRINFO_MEM(&(err_info)); return err_info; }
#define SR_CHECK_INT_GOTO(cond, err_info, go) if (cond) { SR_ERRINFO_INT(&(err_info)); goto go; }
#define SR_CHECK_INT_RET(cond, err_info) if (cond) { SR_ERRINFO_INT(&(err_info)); return err_info; }
#define SR_CHECK_LY_GOTO(cond, ly_ctx, err_info, go) if (cond) { sr_errinfo_new_ly(&(err_info), ly_ctx, NULL); goto go; }
#define SR_CHECK_LY_RET(cond, ly_ctx, err_info) if (cond) { sr_errinfo_new_ly(&(err_info), ly_ctx, NULL); return err_info; }

#define SR_CHECK_ARG_APIRET(cond, session, err_info) if (cond) { sr_errinfo_new(&(err_info), SR_ERR_INVAL_ARG, \
        "Invalid arguments for function \"%s\".", __func__); return sr_api_ret(session, err_info); }

extern sr_log_level_t sr_stderr_ll;  /**< stderr log level */
extern sr_log_level_t sr_syslog_ll;  /**< syslog log level */
extern sr_log_cb sr_lcb;             /**< logging callback */

/**
 * @brief Set error info to a session and return corresponding error code, if any.
 *
 * @param[in] session Session to modify.
 * @param[in] err_info Optional error info to set.
 * @return Error code to be returned from an API function based on error info.
 */
int sr_api_ret(sr_session_ctx_t *session, sr_error_info_t *err_info);

/**
 * @brief Log a message.
 *
 * @param[in] plugin Whether the message was generated by a plugin.
 * @param[in] ll Log level (severity).
 * @param[in] msg Message.
 */
void sr_log_msg(int plugin, sr_log_level_t ll, const char *msg);

/**
 * @brief Add a new error into error_info.
 *
 * @param[in,out] err_info Existing error_info.
 * @param[in] err_code Error code.
 * @param[in] err_format Error data format.
 * @param[in] err_data Error data.
 * @param[in] msg_format Error message format.
 * @param[in] vargs Optional error message variable arguments.
 */
void sr_errinfo_add(sr_error_info_t **err_info, sr_error_t err_code, const char *err_format, const void *err_data,
        const char *msg_format, va_list *vargs);

/**
 * @brief Log the error and add the error into an error info structure.
 *
 * @param[in,out] err_info Exisiting error info.
 * @param[in] err_code Error code of the error.
 * @param[in] msg_format Error message format.
 * @param[in] ... Error message format arguments.
 */
void sr_errinfo_new(sr_error_info_t **err_info, sr_error_t err_code, const char *msg_format, ...) _FORMAT_PRINTF(3, 4);

/**
 * @brief Log the error and add the error into an error info structure.
 *
 * @param[in,out] err_info Exisiting error info.
 * @param[in] err_code Error code of the error.
 * @param[in] err_format Error data format.
 * @param[in] err_data Error data.
 * @param[in] msg_format Error message format.
 * @param[in] ... Error message format arguments.
 */
void sr_errinfo_new_data(sr_error_info_t **err_info, sr_error_t err_code, const char *err_format, const void *err_data,
        const char *msg_format, ...) _FORMAT_PRINTF(5, 6);

/**
 * @brief Log the error(s) from a libyang context and add them into an error info structure.
 *
 * @param[in,out] err_info Existing error info.
 * @param[in] ly_ctx libyang context to use.
 * @param[in] data Optional data tree to look for another extension context that may have the error.
 */
void sr_errinfo_new_ly(sr_error_info_t **err_info, const struct ly_ctx *ly_ctx, const struct lyd_node *data);

/**
 * @brief Log the first error from a libyang context and add it into an error info structure.
 *
 * @param[in,out] err_info Existing error info.
 * @param[in] ly_ctx libyang context to use.
 */
void sr_errinfo_new_ly_first(sr_error_info_t **err_info, const struct ly_ctx *ly_ctx);

/**
 * @brief Log the errors from a libyang context as warnings.
 *
 * @param[in] ly_ctx libyang context to use.
 */
void sr_log_wrn_ly(const struct ly_ctx *ly_ctx);

/**
 * @brief Free an error info structure.
 *
 * @param[in,out] err_info Error info to free.
 */
void sr_errinfo_free(sr_error_info_t **err_info);

/**
 * @brief Merge error info structure into another.
 *
 * @param[in,out] err_info Resulting error info.
 * @param[in] err_info2 Merged error info (unusable afterwards).
 */
void sr_errinfo_merge(sr_error_info_t **err_info, sr_error_info_t *err_info2);

/**
 * @brief Log a message with variable arguments.
 *
 * @param[in] ll Log level (severity).
 * @param[in] format Message format.
 * @param[in] ... Format arguments.
 */
void sr_log(sr_log_level_t ll, const char *format, ...) _FORMAT_PRINTF(2, 3);

#endif
