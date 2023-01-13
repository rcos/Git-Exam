//
//  valgit.h
//  valgit
//
//  Created by Gabriel Jacoby-Cooper on 12/29/22.
//

#ifndef valgit_h
#define valgit_h

#include "git2.h"
#include <json-c/json.h>
#include <Python.h>
#include <stdbool.h>

#define FILE_RESULTS "validation_results.json"
#define REPOSITORY_PREFIX "Clone/"

#define e_init() int e_ret; char* e_str; ptrs_init(); ptrs_git_init(); ptrs_json_init(); ptrs_python_init()
#define e(func) errno = 0; e_ret = func; val_error_exit(e_ret, &ptrs, &ptrs_git, &ptrs_json, &ptrs_python, val_error_message, NULL)
#define e_force() val_error_exit(-1, &ptrs, &ptrs_git, &ptrs_json, &ptrs_python, val_error_message, NULL)
#define e_force_str(str_message_custom) val_error_exit(-1, &ptrs, &ptrs_git, &ptrs_json, &ptrs_python, NULL, str_message_custom)
#define e_git(func) errno = 0; e_ret = func; val_error_exit(e_ret, &ptrs, &ptrs_git, &ptrs_json, &ptrs_python, val_error_message_git, NULL)
#define e_git_force() val_error_exit(-1, &ptrs, &ptrs_git, &ptrs_json, &ptrs_python, val_error_message_git, NULL)
#define e_str_git(func) e_str = func; if (!e_str || strlen(e_str) == 0) val_error_exit(-1, &ptrs, &ptrs_git, &ptrs_json, &ptrs_python, val_error_message_git, NULL)
#define e_json(func) errno = 0; e_ret = func; val_error_exit(e_ret, &ptrs, &ptrs_git, &ptrs_json, &ptrs_python, val_error_message_json, NULL)
#define e_json_force() val_error_exit(-1, &ptrs, &ptrs_git, &ptrs_json, &ptrs_python, val_error_message_json, NULL)
#define ptrs_init() struct ptrs_t ptrs = { .len = 0, .ptrs = NULL }
#define ptrs_add(ptr) ptrs_add2((void*) ptr, &ptrs)
#define ptrs_free() ptrs_free2(&ptrs)
#define ptrs_clear() free(ptrs.ptrs); ptrs.len = 0
#define ptrs_git_init() struct ptrs_git_t ptrs_git = { .len = 0, .ptrs = NULL, .repo = NULL }
#define ptrs_git_add(ptr_git) ptrs_git_add2((git_object*) ptr_git, &ptrs_git)
#define ptrs_git_repository_set(repo) ptrs_git_repository_set2(repo, &ptrs_git)
#define ptrs_git_free() ptrs_git_free2(&ptrs_git)
#define ptrs_git_clear() free(ptrs_git.ptrs); ptrs_git.len = 0
#define ptrs_json_init() struct ptrs_json_t ptrs_json = { .len = 0, .ptrs = NULL }
#define ptrs_json_add(ptr_json) ptrs_json_add2((json_object*) ptr_json, &ptrs_json)
#define ptrs_json_free() ptrs_json_free2(&ptrs_json)
#define ptrs_json_clear() free(ptrs_json.ptrs); ptrs_json.len = 0
#define ptrs_python_init() struct ptrs_python_t ptrs_python = { .len = 0, .ptrs = NULL }
#define ptrs_python_add(ptr_python) ptrs_python_add2((PyObject*) ptr_python, &ptrs_python)
#define ptrs_python_free() ptrs_python_free2(&ptrs_python)
#define ptrs_python_clear() free(ptrs_python.ptrs); ptrs_python.len = 0
#define free_all() e_ret = 0; e_str = NULL; ptrs_free(); ptrs_git_free(); ptrs_json_free(); ptrs_python_free()
#define clear_all() e_ret = 0; e_str = NULL; ptrs_clear(); ptrs_git_clear(); ptrs_json_clear(); ptrs_python_clear()

enum val_status_t {
	VAL_STATUS_FAILURE,
	VAL_STATUS_INFORMATION,
	VAL_STATUS_SUCCESS,
	VAL_STATUS_WARNING
};

enum val_input_key_t {
	VAL_INPUT_KEY_PREFIX,
	VAL_INPUT_KEY_RCSID
};

enum val_git_clone_error_t {
	VAL_GIT_CLONE_ERROR_NONE,
	VAL_GIT_CLONE_ERROR_NORCSID,
	VAL_GIT_CLONE_ERROR_NOPUBLICKEY,
	VAL_GIT_CLONE_ERROR_NOPRIVATEKEY
};

struct val_git_clone_payload_t {
	bool halt;
	enum val_git_clone_error_t error;
};

struct val_python_context_t {
	PyObject* module_obj;
	PyObject* globals;
	PyObject* locals;
};

struct val_message_t {
	enum val_status_t status;
	const char* message;
};

struct ptrs_t {
	unsigned int len;
	void** ptrs;
};

struct ptrs_git_t {
	unsigned int len;
	git_object** ptrs;
	git_repository* repo;
};

struct ptrs_json_t {
	unsigned int len;
	json_object** ptrs;
};

struct ptrs_python_t {
	unsigned int len;
	PyObject** ptrs;
};

const char* val_error_message(int ret);

const char* val_error_message_git(int ret);

const char* val_error_message_json(int ret);

void val_error_exit(int ret, struct ptrs_t* ptrs, struct ptrs_git_t* ptrs_git, struct ptrs_json_t* ptrs_json, struct ptrs_python_t* ptrs_python, const char* error_message_get(int), const char* str_message_custom);

void ptrs_add2(void* ptr, struct ptrs_t* ptrs);

void ptrs_free2(struct ptrs_t* ptrs);

void ptrs_git_add2(git_object* ptr_git, struct ptrs_git_t* ptrs_git);

void ptrs_git_repository_set2(git_repository* repo, struct ptrs_git_t* ptrs_git);

void ptrs_git_free2(struct ptrs_git_t* ptrs_git);

void ptrs_json_add2(json_object* ptr_json, struct ptrs_json_t* ptrs_json);

void ptrs_json_free2(struct ptrs_json_t* ptrs_json);

void ptrs_python_add2(PyObject* ptr_python, struct ptrs_python_t* ptrs_python);

void ptrs_python_free2(struct ptrs_python_t* ptrs_python);

int git_nth_commit_oid(git_oid* oid, bool* first, unsigned int n, const unsigned int pathv[], unsigned int pathc, const char* path_start, const git_reference* ref, git_repository* repo);

json_object* json_create_val_success(double score, const struct val_message_t messagesv[], unsigned short messagesc);

json_object* json_create_val_failure(struct val_message_t message);

char* val_python_result(const char* eval, struct val_python_context_t context, bool* eval_failed);

int val_python_result_file(const char* filename, struct val_python_context_t context, int start);

char* val_python_result_new(const char* eval, const char* filename, struct val_python_context_t* const context, bool* eval_failed);

void val_python_free(struct val_python_context_t context);

int val_git_clone(const char* url, bool* access_public, git_repository** repository_out);

char* val_input(enum val_input_key_t input_key);

FILE* val_url_file(void);

char* val_file_str(FILE* fp);

#endif /* valgit_h */
