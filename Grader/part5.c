//
//  part5.c
//  Grader
//
//  Created by Gabriel Jacoby-Cooper on 1/5/23.
//

#include <sys/param.h>

#include "valgit.h"
#include "parts.h"

#define FILE_STUDENT "Code.py"
#define COMMIT_MESSAGE_PREFIX "[Part 5]"

int part5(git_repository* repository) {
	e_init();
	ptrs_git_repository_set(repository);
	int ret;
	
	// Get a reference to the latest commit in the “main” branch
	git_reference* reference;
	ret = git_branch_lookup(&reference, repository, "origin/main", GIT_BRANCH_REMOTE);
	if (ret < 0) {
		const unsigned short messagesc = 2;
		struct val_message_t messagesv[2] = {
			{
				VAL_STATUS_FAILURE,
				"Your Git repository doesn’t have a “main” branch."
			},
			{
				VAL_STATUS_WARNING,
				"Note that the branch should be named “main”, not “master”."
			}
		};
		json_object* json = json_create_val_success(0, messagesv, messagesc);
		if (!json) {
			e_json_force();
		}
		ptrs_json_add(json);
		e_json(json_object_to_file(FILE_RESULTS, json));
		end();
	}
	
	// Find the first commit after the initial commit
	git_oid oid_commit;
	bool first;
	const unsigned int pathc = 0;
	const unsigned int pathv[pathc];
	e_git(git_nth_commit_oid(&oid_commit, &first, 5, pathv, pathc, NULL, reference, repository));
	git_reference_free(reference);
	
	// Check that the OID isn’t zero
	if (git_oid_is_zero(&oid_commit)) {
		const unsigned short messagesc = 1;
		struct val_message_t messagesv[1] = {
			{
				VAL_STATUS_FAILURE,
				"The expected commit couldn’t be found."
			}
		};
		json_object* json = json_create_val_success(0, messagesv, messagesc);
		if (!json) {
			e_json_force();
		}
		ptrs_json_add(json);
		e_json(json_object_to_file(FILE_RESULTS, json));
		end();
	}
	
	// Check that this isn’t the first commit
	if (first) {
		const unsigned short messagesc = 2;
		struct val_message_t messagesv[2] = {
			{
				VAL_STATUS_FAILURE,
				"The expected commit couldn’t be found."
			},
			{
				VAL_STATUS_WARNING,
				"You must add a commit with the relevant changes."
			}
		};
		json_object* json = json_create_val_success(0, messagesv, messagesc);
		if (!json) {
			e_json_force();
		}
		ptrs_json_add(json);
		e_json(json_object_to_file(FILE_RESULTS, json));
		end();
	}
	
	// Look up the commit
	git_commit* commit;
	e_git(git_commit_lookup(&commit, repository, &oid_commit));
	ptrs_git_add(commit);
	
	unsigned int parentcount = git_commit_parentcount(commit);
	if (parentcount != 2) {
		const unsigned short messagesc = 2;
		struct val_message_t messagesv[2] = {
			{
				VAL_STATUS_FAILURE,
				"The commit isn’t a merge commit."
			},
			{
				VAL_STATUS_WARNING,
				"You must create a merge commit by merging the “part4” branch into the “main” branch."
			}
		};
		json_object* json = json_create_val_success(0, messagesv, messagesc);
		if (!json) {
			e_json_force();
		}
		ptrs_json_add(json);
		e_json(json_object_to_file(FILE_RESULTS, json));
		end();
	}
	
	// Check the commit’s message
	char* hash = calloc(41, sizeof(char));
	e_str_git(git_oid_tostr(hash, 41, &oid_commit));
	const char* commit_message = git_commit_message(commit);
	const size_t end = MAX(strcspn(commit_message, "\f\n\r"), 1);
	char commit_message_own[end + 1];
	for (unsigned long i = 0; i <= end; i++) {
		commit_message_own[i] = '\0';
	}
	strncpy(commit_message_own, commit_message, end);
	fprintf(stderr, "Found commit: %s “%s”\n", hash, commit_message_own);
	free(hash);
	if (strncmp(COMMIT_MESSAGE_PREFIX, commit_message, 8) == 0) {
		char filename[strlen(REPOSITORY_PREFIX) + strlen(FILE_STUDENT) + 1];
		snprintf(filename, sizeof(filename), "%s%s", REPOSITORY_PREFIX, FILE_STUDENT);
		
		git_checkout_options opts;
		e_git(git_checkout_options_init(&opts, GIT_CHECKOUT_OPTIONS_VERSION));
		opts.checkout_strategy = GIT_CHECKOUT_FORCE | GIT_CHECKOUT_DISABLE_PATHSPEC_MATCH;
		e_git(git_checkout_tree(repository, (git_object*) commit, &opts));
		
		// Evaluate ben()
		bool eval_failed_ben;
		char* result_ben = val_python_result("ben()", filename, &eval_failed_ben);
		
		// Evaluate willy()
		bool eval_failed_willy;
		char* result_willy = val_python_result("willy()", filename, &eval_failed_willy);
		
		if (result_ben && result_willy) {
			// Check the result for ben()
			bool correct_ben = strcmp(result_ben, "bitdiddle") == 0;
			char information_message_ben[52 + strlen(result_ben)];
			snprintf(information_message_ben, sizeof(information_message_ben), "ben() execution result: \"%s\"", result_ben);
			free(result_ben);
			
			// Check the result for willy()
			bool correct_willy = strcmp(result_willy, "wazoo") == 0;
			char information_message_willy[54 + strlen(result_willy)];
			snprintf(information_message_willy, sizeof(information_message_willy), "willy() execution result: \"%s\"", result_willy);
			free(result_willy);
			
			const unsigned short messagesc = 6;
			struct val_message_t messagesv[6] = {
				{
					VAL_STATUS_SUCCESS,
					"Your commit structure is correct!"
				},
				{
					VAL_STATUS_SUCCESS,
					"Your commit message is correct!"
				},
				{
					correct_ben ? VAL_STATUS_SUCCESS : VAL_STATUS_FAILURE,
					correct_ben ? "ben() returned the correct result!" : "ben() didn’t return the correct result."
				},
				{
					correct_willy ? VAL_STATUS_SUCCESS : VAL_STATUS_FAILURE,
					correct_willy ? "willy() returned the correct result!" : "willy() didn’t return the correct result."
				},
				{
					VAL_STATUS_INFORMATION,
					information_message_ben
				},
				{
					VAL_STATUS_INFORMATION,
					information_message_willy
				}
			};
			
			double score = 0.5;
			if (correct_ben) {
				score += 0.25;
			}
			if (correct_willy) {
				score += 0.25;
			}
			
			json_object* json = json_create_val_success(score, messagesv, messagesc);
			if (!json) {
				e_json_force();
			}
			ptrs_json_add(json);
			e_json(json_object_to_file(FILE_RESULTS, json));
		} else if (eval_failed_ben || eval_failed_willy) {
			// In this disjunctive syllogism, if eval_failed_ben is false, then eval_failed_willy must be true.
			const char* name_function = eval_failed_ben ? "ben" : "willy";
			
			char warning_message[42 + strlen(name_function)];
			snprintf(warning_message, sizeof(warning_message), "Check that your code defines a function %s().", name_function);
			const unsigned short messagesc = 4;
			struct val_message_t messagesv[4] = {
				{
					VAL_STATUS_SUCCESS,
					"Your commit structure is correct!"
				},
				{
					VAL_STATUS_SUCCESS,
					"Your commit message is correct!"
				},
				{
					VAL_STATUS_FAILURE,
					"Your code couldn’t be evaluated."
				},
				{
					VAL_STATUS_WARNING,
					warning_message
				}
			};
			json_object* json = json_create_val_success(0.25, messagesv, messagesc);
			if (!json) {
				e_json_force();
			}
			ptrs_json_add(json);
			e_json(json_object_to_file(FILE_RESULTS, json));
		} else {
			char warning_message[114 + strlen(FILE_STUDENT)];
			snprintf(warning_message, sizeof(warning_message), "Check that your Python file is named “%s”, that your syntax is correct, and that you solved the merge conflict.", FILE_STUDENT);
			const unsigned short messagesc = 4;
			struct val_message_t messagesv[4] = {
				{
					VAL_STATUS_SUCCESS,
					"Your commit structure is correct!"
				},
				{
					VAL_STATUS_SUCCESS,
					"Your commit message is correct!"
				},
				{
					VAL_STATUS_FAILURE,
					"Your code couldn’t be evaluated."
				},
				{
					VAL_STATUS_WARNING,
					warning_message
				}
			};
			json_object* json = json_create_val_success(0.25, messagesv, messagesc);
			if (!json) {
				e_json_force();
			}
			ptrs_json_add(json);
			e_json(json_object_to_file(FILE_RESULTS, json));
		}
	} else {
		// Set the warning message
		char warning_message[38 + strlen(COMMIT_MESSAGE_PREFIX)];
		snprintf(warning_message, sizeof(warning_message), "Your commit message should start with “%s”.", COMMIT_MESSAGE_PREFIX);
		
		// Set the information message
		char information_message[28 + strlen(commit_message_own)];
		snprintf(information_message, sizeof(information_message), "Your commit message: “%s”", commit_message_own);
		
		const unsigned short messagesc = 4;
		struct val_message_t messagesv[4] = {
			{
				VAL_STATUS_SUCCESS,
				"Your commit structure is correct!"
			},
			{
				VAL_STATUS_FAILURE,
				"Your commit message is incorrect."
			},
			{
				VAL_STATUS_WARNING,
				warning_message
			},
			{
				VAL_STATUS_INFORMATION,
				information_message
			}
		};
		json_object* json = json_create_val_success(0.25, messagesv, messagesc);
		if (!json) {
			e_json_force();
		}
		ptrs_json_add(json);
		e_json(json_object_to_file(FILE_RESULTS, json));
	}
	
	end();
}
