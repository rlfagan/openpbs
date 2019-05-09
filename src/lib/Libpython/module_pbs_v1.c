/*
 * Copyright (C) 1994-2019 Altair Engineering, Inc.
 * For more information, contact Altair at www.altair.com.
 *
 * This file is part of the PBS Professional ("PBS Pro") software.
 *
 * Open Source License Information:
 *
 * PBS Pro is free software. You can redistribute it and/or modify it under the
 * terms of the GNU Affero General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * PBS Pro is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.
 * See the GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Commercial License Information:
 *
 * For a copy of the commercial license terms and conditions,
 * go to: (http://www.pbspro.com/UserArea/agreement.html)
 * or contact the Altair Legal Department.
 *
 * Altair’s dual-license business model allows companies, individuals, and
 * organizations to create proprietary derivative works of PBS Pro and
 * distribute them - whether embedded or bundled with other software -
 * under a commercial license agreement.
 *
 * Use of Altair’s trademarks, including but not limited to "PBS™",
 * "PBS Professional®", and "PBS Pro™" and Altair’s logos is subject to Altair's
 * trademark licensing policies.
 *
 */

#include <pbs_config.h>   /* the master config generated by configure */

/* include the pbs_python private file with python dependencies */
#include <pbs_python_private.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <memory.h>
#include <stdlib.h>
#include <libpbs.h>
#include <pbs_ifl.h>
#include <errno.h>
#include <string.h>
#include <list_link.h>
#include <log.h>
#include <attribute.h>
#include <server_limits.h>
#include <server.h>
#include <job.h>
#include <reservation.h>
#include <queue.h>
#include <pbs_error.h>
#include <hook.h>
#include <pbs_internal.h>
#include <pbs_nodes.h>
#include "pbs_internal.h"

/* pbs_python_import_types.c */
extern int ppsvr_prepare_all_types(void);
extern PyObject *ppsvr_create_types_module(void);

extern char pbsv1mod_meth_logmsg_doc[]; /* common_python_utils.c */
extern PyObject * pbsv1mod_meth_logmsg(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_logjobmsg_doc[]; /* common_python_utils.c */
extern PyObject * pbsv1mod_meth_logjobmsg(PyObject *self,
	PyObject *args, PyObject *kwds
	);

/* pbs_python_svr_internal.c */
extern char pbsv1mod_meth_get_queue_doc[];
extern PyObject * pbsv1mod_meth_get_queue(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_get_job_doc[];
extern PyObject * pbsv1mod_meth_get_job(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_release_nodes_doc[];
extern PyObject *pbsv1mod_meth_release_nodes(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_get_resv_doc[];
extern PyObject * pbsv1mod_meth_get_resv(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_get_vnode_doc[];
extern PyObject * pbsv1mod_meth_get_vnode(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_iter_nextfunc_doc[];
extern PyObject * pbsv1mod_meth_iter_nextfunc(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_mark_vnode_set_doc[];
extern PyObject * pbsv1mod_meth_mark_vnode_set(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_load_resource_value_doc[];
extern PyObject * pbsv1mod_meth_load_resource_value(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_resource_str_value_doc[];
extern PyObject * pbsv1mod_meth_resource_str_value(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_vnode_state_to_str_doc[];
extern PyObject * pbsv1mod_meth_vnode_state_to_str(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_vnode_sharing_to_str_doc[];
extern PyObject * pbsv1mod_meth_vnode_sharing_to_str(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_vnode_ntype_to_str_doc[];
extern PyObject * pbsv1mod_meth_vnode_ntype_to_str(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_str_to_vnode_state_doc[];
extern PyObject * pbsv1mod_meth_str_to_vnode_state(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_str_to_vnode_ntype_doc[];
extern PyObject * pbsv1mod_meth_str_to_vnode_ntype(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_str_to_vnode_sharing_doc[];
extern PyObject * pbsv1mod_meth_str_to_vnode_sharing(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_is_attrib_val_settable_doc[];
extern PyObject * pbsv1mod_meth_is_attrib_val_settable(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_event_accept_doc[];
extern PyObject *pbsv1mod_meth_event_accept(void);

extern char pbsv1mod_meth_event_reject_doc[];
extern PyObject *pbsv1mod_meth_event_reject(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_reboot_doc[];
extern PyObject *pbsv1mod_meth_reboot(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_scheduler_restart_cycle_doc[];
extern PyObject *pbsv1mod_meth_scheduler_restart_cycle(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_set_pbs_statobj_doc[];
extern PyObject *pbsv1mod_meth_set_pbs_statobj(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_event_param_mod_allow_doc[];
extern PyObject *pbsv1mod_meth_event_param_mod_allow(void);

extern char pbsv1mod_meth_event_param_mod_disallow_doc[];
extern PyObject *pbsv1mod_meth_event_param_mod_disallow(void);

extern char pbsv1mod_meth_event_doc[];
extern PyObject *pbsv1mod_meth_event(void);

extern char pbsv1mod_meth_server_doc[];
extern PyObject *pbsv1mod_meth_server(void);

extern char pbsv1mod_meth_in_python_mode_doc[];
extern PyObject *pbsv1mod_meth_in_python_mode(void);

extern char pbsv1mod_meth_set_python_mode_doc[];
extern PyObject *pbsv1mod_meth_set_python_mode(void);

extern char pbsv1mod_meth_set_c_mode_doc[];
extern PyObject *pbsv1mod_meth_set_c_mode(void);

extern char pbsv1mod_meth_get_python_daemon_name_doc[];
extern PyObject *pbsv1mod_meth_get_python_daemon_name(void);

extern char pbsv1mod_meth_get_pbs_server_name_doc[];
extern PyObject *pbsv1mod_meth_get_pbs_server_name(void);

extern char pbsv1mod_meth_get_local_host_name_doc[];
extern PyObject *pbsv1mod_meth_get_local_host_name(void);

extern char pbsv1mod_meth_get_pbs_conf_doc[];
extern PyObject *pbsv1mod_meth_get_pbs_conf(void);

extern char pbsv1mod_meth_in_site_hook_doc[];
extern PyObject *pbsv1mod_meth_in_site_hook(void);

extern char pbsv1mod_meth_validate_input_doc[];
extern PyObject *pbsv1mod_meth_validate_input(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_duration_to_secs_doc[];
extern PyObject *pbsv1mod_meth_duration_to_secs(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_wordsize_doc[];
extern PyObject *pbsv1mod_meth_wordsize(void);

extern char pbsv1mod_meth_size_to_kbytes_doc[];
extern PyObject * pbsv1mod_meth_size_to_kbytes(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_get_server_static_doc[];
extern PyObject * pbsv1mod_meth_get_server_static(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_get_vnode_static_doc[];
extern PyObject * pbsv1mod_meth_get_vnode_static(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_get_job_static_doc[];
extern PyObject * pbsv1mod_meth_get_job_static(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_get_resv_static_doc[];
extern PyObject * pbsv1mod_meth_get_resv_static(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_get_vnode_static_doc[];
extern PyObject * pbsv1mod_meth_get_vnode_static(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_get_queue_static_doc[];
extern PyObject * pbsv1mod_meth_get_queue_static(PyObject *self,
	PyObject *args, PyObject *kwds);

extern char pbsv1mod_meth_get_server_data_fp_doc[];
extern PyObject *pbsv1mod_meth_get_server_data_fp(void);

extern char pbsv1mod_meth_use_static_data_doc[];
extern PyObject *pbsv1mod_meth_use_static_data(void);


/* private */
static PyObject *PyPbsV1ModuleExtension_Obj = NULL; /* BORROWED reference */

/*  -----                    MODULE HELPER FUNCTIONS          -----    */

#define INSERT_STR_CONSTANT(k,v)                                              \
    do {                                                                      \
        if ((pbs_python_dict_set_item_string_value(dict,k,v) == -1)) {        \
            return -1;                                                        \
        }                                                                     \
    } while (0)

/* special job states */
#define JOB_STATE_SUSPEND               400
#define JOB_STATE_SUSPEND_USERACTIVE    410

/**
 * @brief
 * 	_pv1mod_insert_str_constants:
 *   	insert all PBS string constants
 *
 * @param[in] dict - dictionary object
 *
 * @return	int
 * @retval	0
 */

static int
_pv1mod_insert_str_constants(PyObject *dict) {

	return 0;
}

#define INSERT_INT_CONSTANT(k,v)                                              \
    do {                                                                      \
        if ((pbs_python_dict_set_item_integral_value(dict,k,v) == -1)) {      \
            return -1;                                                        \
        }                                                                     \
    } while (0)

/**
 * @brief
 * 	_pv1mod_insert_str_constants:
 *   	insert all PBS string constants
 *
 * @param[in] dict - dictionary object
 *
 * @return      int
 * @retval      0
 *
 */

static int
_pv1mod_insert_int_constants(PyObject *dict) {

	/* first QTYPES */
	INSERT_INT_CONSTANT("QTYPE_EXECUTION", QTYPE_Execution);
	INSERT_INT_CONSTANT("QTYPE_ROUTE", QTYPE_RoutePush);

	/* reservation states */
	INSERT_INT_CONSTANT("RESV_STATE_NONE", RESV_NONE);
	INSERT_INT_CONSTANT("RESV_STATE_UNCONFIRMED", RESV_UNCONFIRMED);
	INSERT_INT_CONSTANT("RESV_STATE_CONFIRMED", RESV_CONFIRMED);
	INSERT_INT_CONSTANT("RESV_STATE_WAIT", RESV_WAIT);
	INSERT_INT_CONSTANT("RESV_STATE_TIME_TO_RUN", RESV_TIME_TO_RUN);
	INSERT_INT_CONSTANT("RESV_STATE_RUNNING", RESV_RUNNING);
	INSERT_INT_CONSTANT("RESV_STATE_FINISHED", RESV_FINISHED);
	INSERT_INT_CONSTANT("RESV_STATE_BEING_DELETED", RESV_BEING_DELETED);
	INSERT_INT_CONSTANT("RESV_STATE_DELETED", RESV_DELETED);
	INSERT_INT_CONSTANT("RESV_STATE_DELETING_JOBS", RESV_DELETING_JOBS);
	INSERT_INT_CONSTANT("RESV_STATE_DEGRADED", RESV_DEGRADED);
	INSERT_INT_CONSTANT("RESV_STATE_BEING_ALTERED", RESV_BEING_ALTERED);
	INSERT_INT_CONSTANT("RESV_STATE_IN_CONFLICT", RESV_IN_CONFLICT);

	/* job states */
	INSERT_INT_CONSTANT("JOB_STATE_TRANSIT", JOB_STATE_TRANSIT);
	INSERT_INT_CONSTANT("JOB_STATE_QUEUED", JOB_STATE_QUEUED);
	INSERT_INT_CONSTANT("JOB_STATE_HELD", JOB_STATE_HELD);
	INSERT_INT_CONSTANT("JOB_STATE_WAITING", JOB_STATE_WAITING);
	INSERT_INT_CONSTANT("JOB_STATE_RUNNING", JOB_STATE_RUNNING);
	INSERT_INT_CONSTANT("JOB_STATE_EXITING", JOB_STATE_EXITING);
	INSERT_INT_CONSTANT("JOB_STATE_EXPIRED", JOB_STATE_EXPIRED);
	INSERT_INT_CONSTANT("JOB_STATE_BEGUN", JOB_STATE_BEGUN);
	INSERT_INT_CONSTANT("JOB_STATE_SUSPEND", JOB_STATE_SUSPEND);
	INSERT_INT_CONSTANT("JOB_STATE_SUSPEND_USERACTIVE", JOB_STATE_SUSPEND_USERACTIVE);
	INSERT_INT_CONSTANT("JOB_STATE_MOVED", JOB_STATE_MOVED);
	INSERT_INT_CONSTANT("JOB_STATE_FINISHED", JOB_STATE_FINISHED);

	/* server states */
	INSERT_INT_CONSTANT("SV_STATE_IDLE", SV_STATE_INIT);
	INSERT_INT_CONSTANT("SV_STATE_ACTIVE", SV_STATE_RUN);
	INSERT_INT_CONSTANT("SV_STATE_HOT", SV_STATE_HOT);
	INSERT_INT_CONSTANT("SV_STATE_SHUTDEL", SV_STATE_SHUTDEL);
	INSERT_INT_CONSTANT("SV_STATE_SHUTIMM", SV_STATE_SHUTIMM);
	INSERT_INT_CONSTANT("SV_STATE_SHUTSIG", SV_STATE_SHUTSIG);

	/* Log message severity */
	INSERT_INT_CONSTANT("LOG_DEBUG", SEVERITY_LOG_DEBUG);
	INSERT_INT_CONSTANT("LOG_WARNING", SEVERITY_LOG_WARNING);
	INSERT_INT_CONSTANT("LOG_ERROR", SEVERITY_LOG_ERR);

	/* Log events levels */
	INSERT_INT_CONSTANT("EVENT_ERROR", PBSEVENT_ERROR);
	INSERT_INT_CONSTANT("EVENT_SYSTEM", PBSEVENT_SYSTEM);
	INSERT_INT_CONSTANT("EVENT_ADMIN", PBSEVENT_ADMIN);
	INSERT_INT_CONSTANT("EVENT_JOB", PBSEVENT_JOB);
	INSERT_INT_CONSTANT("EVENT_JOB_USAGE", PBSEVENT_JOB_USAGE);
	INSERT_INT_CONSTANT("EVENT_SECURITY", PBSEVENT_SECURITY);
	INSERT_INT_CONSTANT("EVENT_SCHED", PBSEVENT_SCHED);
	INSERT_INT_CONSTANT("EVENT_DEBUG", PBSEVENT_DEBUG);
	INSERT_INT_CONSTANT("EVENT_DEBUG2", PBSEVENT_DEBUG2);
	INSERT_INT_CONSTANT("EVENT_RESV", PBSEVENT_RESV);
	INSERT_INT_CONSTANT("EVENT_DEBUG3", PBSEVENT_DEBUG3);
	INSERT_INT_CONSTANT("EVENT_DEBUG4", PBSEVENT_DEBUG4);
	INSERT_INT_CONSTANT("EVENT_FORCE", PBSEVENT_FORCE);

	/* Event types */
	INSERT_INT_CONSTANT("QUEUEJOB", HOOK_EVENT_QUEUEJOB);
	INSERT_INT_CONSTANT("MODIFYJOB", HOOK_EVENT_MODIFYJOB);
	INSERT_INT_CONSTANT("RESVSUB", HOOK_EVENT_RESVSUB);
	INSERT_INT_CONSTANT("MOVEJOB", HOOK_EVENT_MOVEJOB);
	INSERT_INT_CONSTANT("RUNJOB", HOOK_EVENT_RUNJOB);
	INSERT_INT_CONSTANT("PROVISION", HOOK_EVENT_PROVISION);
	INSERT_INT_CONSTANT("RESV_END", HOOK_EVENT_RESV_END);
	INSERT_INT_CONSTANT("EXECJOB_BEGIN", HOOK_EVENT_EXECJOB_BEGIN);
	INSERT_INT_CONSTANT("EXECJOB_PROLOGUE", HOOK_EVENT_EXECJOB_PROLOGUE);
	INSERT_INT_CONSTANT("EXECJOB_EPILOGUE", HOOK_EVENT_EXECJOB_EPILOGUE);
	INSERT_INT_CONSTANT("EXECJOB_PRETERM", HOOK_EVENT_EXECJOB_PRETERM);
	INSERT_INT_CONSTANT("EXECJOB_END", HOOK_EVENT_EXECJOB_END);
	INSERT_INT_CONSTANT("EXECJOB_LAUNCH", HOOK_EVENT_EXECJOB_LAUNCH);
	INSERT_INT_CONSTANT("EXECHOST_PERIODIC", HOOK_EVENT_EXECHOST_PERIODIC);
	INSERT_INT_CONSTANT("EXECHOST_STARTUP", HOOK_EVENT_EXECHOST_STARTUP);
	INSERT_INT_CONSTANT("EXECJOB_ATTACH", HOOK_EVENT_EXECJOB_ATTACH);
	INSERT_INT_CONSTANT("EXECJOB_RESIZE", HOOK_EVENT_EXECJOB_RESIZE);
	INSERT_INT_CONSTANT("EXECJOB_ABORT", HOOK_EVENT_EXECJOB_ABORT);
	INSERT_INT_CONSTANT("MOM_EVENTS", MOM_EVENTS);
	INSERT_INT_CONSTANT("PERIODIC", HOOK_EVENT_PERIODIC);

	/* Vnode State Constants */
	INSERT_INT_CONSTANT("ND_FREE", INUSE_FREE);
	INSERT_INT_CONSTANT("ND_OFFLINE", INUSE_OFFLINE);
	INSERT_INT_CONSTANT("ND_DOWN", INUSE_DOWN);
	INSERT_INT_CONSTANT("ND_STALE", INUSE_STALE);
	INSERT_INT_CONSTANT("ND_JOBBUSY", INUSE_JOB);
	INSERT_INT_CONSTANT("ND_JOB_EXCLUSIVE", INUSE_JOBEXCL);
	INSERT_INT_CONSTANT("ND_RESV_EXCLUSIVE", INUSE_RESVEXCL);
	INSERT_INT_CONSTANT("ND_BUSY", INUSE_BUSY);
	INSERT_INT_CONSTANT("ND_STATE_UNKNOWN", INUSE_UNKNOWN);
	INSERT_INT_CONSTANT("ND_PROV", INUSE_PROV);
	INSERT_INT_CONSTANT("ND_WAIT_PROV", INUSE_WAIT_PROV);
	INSERT_INT_CONSTANT("ND_UNRESOLVABLE", INUSE_UNRESOLVABLE);
	INSERT_INT_CONSTANT("ND_SLEEP", INUSE_SLEEP);
	
	/* Vnode Type Constants */
	INSERT_INT_CONSTANT("ND_PBS", NTYPE_PBS);

	/* Vnode Sharing Constants */
	INSERT_INT_CONSTANT("ND_DEFAULT_SHARED", VNS_DFLT_SHARED);
	INSERT_INT_CONSTANT("ND_DEFAULT_EXCL", VNS_DFLT_EXCL);
	INSERT_INT_CONSTANT("ND_FORCE_EXCL", VNS_FORCE_EXCL);
	INSERT_INT_CONSTANT("ND_IGNORE_EXCL", VNS_IGNORE_EXCL);
	INSERT_INT_CONSTANT("ND_FORCE_EXCLHOST", VNS_FORCE_EXCLHOST);
	INSERT_INT_CONSTANT("ND_DEFAULT_EXCLHOST", VNS_DFLT_EXCLHOST);

	return 0;
}

/*  -----                    MODULE METHODS                   -----    */



static PyMethodDef pbs_v1_module_methods[] = {
	{"wordsize",
		(PyCFunction) pbsv1mod_meth_wordsize,
		METH_NOARGS, pbsv1mod_meth_wordsize_doc},
	{"in_python_mode",
		(PyCFunction) pbsv1mod_meth_in_python_mode,
		METH_NOARGS, pbsv1mod_meth_in_python_mode_doc},
	{"in_site_hook",
		(PyCFunction) pbsv1mod_meth_in_site_hook,
		METH_NOARGS, pbsv1mod_meth_in_site_hook_doc},
	{"duration_to_secs",
		(PyCFunction) pbsv1mod_meth_duration_to_secs,
		METH_KEYWORDS, pbsv1mod_meth_duration_to_secs_doc},
	{"validate_input",
		(PyCFunction) pbsv1mod_meth_validate_input,
		METH_KEYWORDS, pbsv1mod_meth_validate_input_doc},
	{"event",
		(PyCFunction) pbsv1mod_meth_event,
		METH_NOARGS, pbsv1mod_meth_event_doc},
	{"server",
		(PyCFunction) pbsv1mod_meth_server,
		METH_NOARGS, pbsv1mod_meth_server_doc},
	{"_event_accept",
		(PyCFunction) pbsv1mod_meth_event_accept,
		METH_NOARGS, pbsv1mod_meth_event_accept_doc},
	{"_event_reject",
		(PyCFunction) pbsv1mod_meth_event_reject,
		METH_KEYWORDS, pbsv1mod_meth_event_reject_doc},
	{"_event_param_mod_allow",
		(PyCFunction) pbsv1mod_meth_event_param_mod_allow,
		METH_NOARGS, pbsv1mod_meth_event_param_mod_allow_doc},
	{"_event_param_mod_disallow",
		(PyCFunction) pbsv1mod_meth_event_param_mod_disallow,
		METH_NOARGS, pbsv1mod_meth_event_param_mod_disallow_doc},
	{"is_attrib_val_settable", (PyCFunction) pbsv1mod_meth_is_attrib_val_settable,
		METH_KEYWORDS, pbsv1mod_meth_is_attrib_val_settable_doc},
	{"get_queue", (PyCFunction) pbsv1mod_meth_get_queue,
		METH_KEYWORDS, pbsv1mod_meth_get_queue_doc},
	{"get_job", (PyCFunction) pbsv1mod_meth_get_job,
		METH_KEYWORDS, pbsv1mod_meth_get_job_doc},
	{"release_nodes", (PyCFunction) pbsv1mod_meth_release_nodes,
		METH_KEYWORDS, pbsv1mod_meth_release_nodes_doc},
	{PY_GETRESV_METHOD, (PyCFunction) pbsv1mod_meth_get_resv,
		METH_KEYWORDS, pbsv1mod_meth_get_resv_doc},
	{PY_GETVNODE_METHOD, (PyCFunction) pbsv1mod_meth_get_vnode,
		METH_KEYWORDS, pbsv1mod_meth_get_vnode_doc},
	{PY_ITER_NEXTFUNC_METHOD, (PyCFunction) pbsv1mod_meth_iter_nextfunc,
		METH_KEYWORDS, pbsv1mod_meth_iter_nextfunc_doc},
	{PY_MARK_VNODE_SET_METHOD, (PyCFunction) pbsv1mod_meth_mark_vnode_set,
		METH_KEYWORDS, pbsv1mod_meth_mark_vnode_set_doc},
	{PY_LOAD_RESOURCE_VALUE_METHOD,
		(PyCFunction) pbsv1mod_meth_load_resource_value,
		METH_KEYWORDS, pbsv1mod_meth_load_resource_value_doc},
	{PY_RESOURCE_STR_VALUE_METHOD,
		(PyCFunction) pbsv1mod_meth_resource_str_value,
		METH_KEYWORDS, pbsv1mod_meth_resource_str_value_doc},
	{PY_VNODE_STATE_TO_STR_METHOD,
		(PyCFunction) pbsv1mod_meth_vnode_state_to_str,
		METH_KEYWORDS, pbsv1mod_meth_vnode_state_to_str_doc},
	{PY_VNODE_SHARING_TO_STR_METHOD,
		(PyCFunction) pbsv1mod_meth_vnode_sharing_to_str,
		METH_KEYWORDS, pbsv1mod_meth_vnode_sharing_to_str_doc},
	{PY_VNODE_NTYPE_TO_STR_METHOD,
		(PyCFunction) pbsv1mod_meth_vnode_ntype_to_str,
		METH_KEYWORDS, pbsv1mod_meth_vnode_ntype_to_str_doc},
	{"logmsg", (PyCFunction) pbsv1mod_meth_logmsg,
		METH_KEYWORDS, pbsv1mod_meth_logmsg_doc},
	{PY_LOGJOBMSG_METHOD, (PyCFunction) pbsv1mod_meth_logjobmsg,
		METH_KEYWORDS, pbsv1mod_meth_logjobmsg_doc},
	{PY_GET_PYTHON_DAEMON_NAME_METHOD,
		(PyCFunction) pbsv1mod_meth_get_python_daemon_name,
		METH_NOARGS, pbsv1mod_meth_get_python_daemon_name_doc},
	{PY_GET_PBS_SERVER_NAME_METHOD,
		(PyCFunction) pbsv1mod_meth_get_pbs_server_name,
		METH_NOARGS, pbsv1mod_meth_get_pbs_server_name_doc},
	{PY_GET_LOCAL_HOST_NAME_METHOD,
		(PyCFunction) pbsv1mod_meth_get_local_host_name,
		METH_NOARGS, pbsv1mod_meth_get_local_host_name_doc},
	{PY_SET_PYTHON_MODE_METHOD,
		(PyCFunction) pbsv1mod_meth_set_python_mode,
		METH_NOARGS, pbsv1mod_meth_set_python_mode_doc},
	{PY_SET_C_MODE_METHOD,
		(PyCFunction) pbsv1mod_meth_set_c_mode,
		METH_NOARGS, pbsv1mod_meth_set_c_mode_doc},
	{PY_STR_TO_VNODE_STATE_METHOD, (PyCFunction) pbsv1mod_meth_str_to_vnode_state,
		METH_KEYWORDS, pbsv1mod_meth_str_to_vnode_state_doc},
	{PY_STR_TO_VNODE_NTYPE_METHOD, (PyCFunction) pbsv1mod_meth_str_to_vnode_ntype,
		METH_KEYWORDS, pbsv1mod_meth_str_to_vnode_ntype_doc},
	{PY_STR_TO_VNODE_SHARING_METHOD, (PyCFunction) pbsv1mod_meth_str_to_vnode_sharing,
		METH_KEYWORDS, pbsv1mod_meth_str_to_vnode_sharing_doc},
	{PY_REBOOT_HOST_METHOD,
		(PyCFunction) pbsv1mod_meth_reboot,
		METH_KEYWORDS, pbsv1mod_meth_reboot_doc},
	{PY_SCHEDULER_RESTART_CYCLE_METHOD,
		(PyCFunction) pbsv1mod_meth_scheduler_restart_cycle,
		METH_KEYWORDS, pbsv1mod_meth_scheduler_restart_cycle_doc},
	{PY_SET_PBS_STATOBJ_METHOD,
		(PyCFunction) pbsv1mod_meth_set_pbs_statobj,
		METH_KEYWORDS, pbsv1mod_meth_set_pbs_statobj_doc},
	{PY_SIZE_TO_KBYTES_METHOD, (PyCFunction) pbsv1mod_meth_size_to_kbytes,
		METH_KEYWORDS, pbsv1mod_meth_size_to_kbytes_doc},
	{PY_GET_SERVER_STATIC_METHOD, (PyCFunction) pbsv1mod_meth_get_server_static,
		METH_NOARGS, pbsv1mod_meth_get_server_static_doc},
	{PY_GET_JOB_STATIC_METHOD, (PyCFunction) pbsv1mod_meth_get_job_static,
		METH_KEYWORDS, pbsv1mod_meth_get_job_static_doc},
	{PY_GET_RESV_STATIC_METHOD, (PyCFunction) pbsv1mod_meth_get_resv_static,
		METH_KEYWORDS, pbsv1mod_meth_get_resv_static_doc},
	{PY_GET_QUEUE_STATIC_METHOD, (PyCFunction) pbsv1mod_meth_get_queue_static,
		METH_KEYWORDS, pbsv1mod_meth_get_queue_static_doc},
	{PY_GET_VNODE_STATIC_METHOD, (PyCFunction) pbsv1mod_meth_get_vnode_static,
		METH_KEYWORDS, pbsv1mod_meth_get_vnode_static_doc},
	{PY_GET_SERVER_DATA_FP_METHOD, (PyCFunction) pbsv1mod_meth_get_server_data_fp,
		METH_NOARGS, pbsv1mod_meth_get_server_data_fp_doc},
	{PY_USE_STATIC_DATA_METHOD,
		(PyCFunction) pbsv1mod_meth_use_static_data,
		METH_NOARGS, pbsv1mod_meth_use_static_data_doc},
	{PY_GET_PBS_CONF_METHOD,
		(PyCFunction) pbsv1mod_meth_get_pbs_conf,
		METH_NOARGS, pbsv1mod_meth_get_pbs_conf_doc},
	{NULL, NULL}                                 /* sentinel */
};



static char pbs_v1_module_doc[] =
    "PBS Module providing PBS C/Python glue code\n\
    \t\n\
    ";


/*
 * ----------------- EXTERNAL DEFINITIONS -----------------------
 */

/**
 * @brief
 * 	pbs_v1_module_init
 *   	This is convenience routine to be used by both embedded and extension
 *   	mechanism to initialize the module. Since Py_InitModuleX can be called
 *   	multiple time without any ill side effects :).
 *
 * @return	object
 * @retval	The module object (borrowed reference)
 */

PyObject *
pbs_v1_module_init(void)
{

	PyObject *m     = NULL; /* the module object, BORROWED ref */
	PyObject *mdict = NULL; /* the module object, BORROWED ref */

	m = Py_InitModule3(PBS_PYTHON_V1_MODULE_EXTENSION_NAME,
		pbs_v1_module_methods,
		pbs_v1_module_doc);

	if (m == NULL)
		return m;

	/* IMPORTANT all our types ready */
	if (ppsvr_prepare_all_types() < 0)
		goto ERROR_EXIT;

	mdict = PyModule_GetDict(m);

	if ((PyDict_SetItemString(mdict, "svr_types",
		ppsvr_create_types_module())) == -1)
		return NULL;
	/* Add all our constants */
	if (_pv1mod_insert_int_constants(mdict) == -1)
		return NULL;

	if (_pv1mod_insert_str_constants(mdict) == -1)
		return NULL;

	PyPbsV1ModuleExtension_Obj = m; /* used to create separate namespaces later */
	return m;

ERROR_EXIT:
	return NULL;
}


/**
 * @brief
 * 	The below is for embedded interpreter puts it in the __main__
 * 	module.
 *
 */
void
pbs_v1_module_inittab(void)
{
	(void) pbs_v1_module_init();
	return;
}


/**
 * @brief
 * ==== For exposing the module as an external shared library =====
 */

PyMODINIT_FUNC
init_pbs_v1(void)
{
	(void) pbs_v1_module_init();
	return;
}
