//
//  part4.c
//  Grader
//
//  Created by Gabriel Jacoby-Cooper on 1/4/23.
//

#include <sys/param.h>

#include "valgit.h"
#include "parts.h"

#define FILE_STUDENT "Code.py"
#define COMMIT_MESSAGE_PREFIX "[Part 4]"

int part4(git_repository* repository) {
	e_init();
	ptrs_git_repository_set(repository);
	int ret;
	
	// Get a reference to the latest commit in the “part4” branch
	git_reference* reference_part4;
	ret = git_branch_lookup(&reference_part4, repository, "origin/part4", GIT_BRANCH_REMOTE);
	if (ret < 0) {
		const unsigned short messagesc = 1;
		struct val_message_t messagesv[1] = {
			{
				VAL_STATUS_FAILURE,
				"Your Git repository doesn’t have a “part4” branch."
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
	
	// Get a reference to the latest commit in the “main” branch
	git_reference* reference_main;
	ret = git_branch_lookup(&reference_main, repository, "origin/main", GIT_BRANCH_REMOTE);
	if (ret < 0) {
		git_reference_free(reference_part4);
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
	
	// Find the relevant commit on the “part4” branch
	git_oid oid_commit_part4;
	bool first_part4;
	const unsigned int pathc_part4 = 0;
	const unsigned int pathv_part4[pathc_part4];
	e_git(git_nth_commit_oid(&oid_commit_part4, &first_part4, 4, pathv_part4, pathc_part4, NULL, reference_part4, repository));
	git_reference_free(reference_part4);
	
	// Find the relevant commit on the “main” branch
	git_oid oid_commit_main;
	bool first_main;
	const unsigned int pathc_main = 0;
	const unsigned int pathv_main[pathc_main];
	e_git(git_nth_commit_oid(&oid_commit_main, &first_main, 4, pathv_main, pathc_main, NULL, reference_main, repository));
	git_reference_free(reference_main);
	
	// Check that the OIDs aren’t zero
	if (git_oid_is_zero(&oid_commit_part4) || git_oid_is_zero(&oid_commit_main)) {
		const unsigned short messagesc = 1;
		struct val_message_t messagesv[1] = {
			{
				VAL_STATUS_FAILURE,
				"The expected commits couldn’t be found."
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
	
	// Check that these aren’t the first commits
	if (first_part4 || first_main) {
		const unsigned short messagesc = 2;
		struct val_message_t messagesv[2] = {
			{
				VAL_STATUS_FAILURE,
				"The expected commits couldn’t be found."
			},
			{
				VAL_STATUS_WARNING,
				"You must add commits with the relevant changes."
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
	
	// Look up the commit on the “part4” branch
	git_commit* commit_part4;
	e_git(git_commit_lookup(&commit_part4, repository, &oid_commit_part4));
	ptrs_git_add(commit_part4);
	
	// Look up the commit on the “main” branch
	git_commit* commit_main;
	e_git(git_commit_lookup(&commit_main, repository, &oid_commit_main));
	ptrs_git_add(commit_main);
	
	// Check the message for the commit on the “part4” branch
	char* hash_part4 = calloc(41, sizeof(char));
	e_str_git(git_oid_tostr(hash_part4, 41, &oid_commit_part4));
	const char* commit_message_part4 = git_commit_message(commit_part4);
	const size_t end_part4 = MAX(strcspn(commit_message_part4, "\f\n\r"), 1);
	char commit_message_own_part4[end_part4 + 1];
	for (unsigned long i = 0; i <= end_part4; i++) {
		commit_message_own_part4[i] = '\0';
	}
	strncpy(commit_message_own_part4, commit_message_part4, end_part4);
	fprintf(stderr, "Found commit on branch “part4”: %s “%s”\n", hash_part4, commit_message_own_part4);
	free(hash_part4);
	
	// Check the message for the commit on the “main” branch
	char* hash_main = calloc(41, sizeof(char));
	e_str_git(git_oid_tostr(hash_main, 41, &oid_commit_main));
	const char* commit_message_main = git_commit_message(commit_main);
	const size_t end_main = MAX(strcspn(commit_message_main, "\f\n\r"), 1);
	char commit_message_own_main[end_main + 1];
	for (unsigned long i = 0; i <= end_main; i++) {
		commit_message_own_main[i] = '\0';
	}
	strncpy(commit_message_own_main, commit_message_main, end_main);
	fprintf(stderr, "Found commit on branch “main”: %s “%s”\n", hash_main, commit_message_own_main);
	free(hash_main);
	
	if (strncmp(COMMIT_MESSAGE_PREFIX, commit_message_own_part4, 8) == 0 && strncmp(COMMIT_MESSAGE_PREFIX, commit_message_own_main, 8) == 0) {
		if (end_part4 == end_main && strncmp(commit_message_own_part4, commit_message_own_main, MIN(end_part4, end_main)) == 0) {
			const unsigned short messagesc = 1;
			struct val_message_t messagesv[1] = {
				{
					VAL_STATUS_FAILURE,
					"Your two commits have the same message."
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
		
		char filename[strlen(REPOSITORY_PREFIX) + strlen(FILE_STUDENT) + 1];
		snprintf(filename, sizeof(filename), "%s%s", REPOSITORY_PREFIX, FILE_STUDENT);
		
		git_checkout_options opts;
		e_git(git_checkout_options_init(&opts, GIT_CHECKOUT_OPTIONS_VERSION));
		opts.checkout_strategy = GIT_CHECKOUT_FORCE | GIT_CHECKOUT_DISABLE_PATHSPEC_MATCH;
		
		// Evaluate the code in the “part4” branch
		e_git(git_checkout_tree(repository, (git_object*) commit_part4, &opts));
		bool eval_failed_part4;
		char* result_part4 = val_python_result_new("ben()", filename, NULL, &eval_failed_part4);
		if (result_part4) {
			ptrs_add(result_part4);
		}
		
		// Evaluate the code in the “main” branch
		e_git(git_checkout_tree(repository, (git_object*) commit_main, &opts));
		bool eval_failed_main;
		char* result_main = val_python_result_new("willy()", filename, NULL, &eval_failed_main);
		if (result_main) {
			ptrs_add(result_main);
		}
		
		if (result_part4 && result_main) {
			// Check the result in the “part4” branch
			bool correct_part4 = strcmp(result_part4, "wazoo") == 0;
			char information_message_part4[53 + strlen(result_part4)];
			snprintf(information_message_part4, sizeof(information_message_part4), "ben() execution result on the “part4” branch: \"%s\"", result_part4);
			
			// Check the result in the “main” branch
			bool correct_main = strcmp(result_main, "bitdiddle") == 0;
			char information_message_main[54 + strlen(result_main)];
			snprintf(information_message_main, sizeof(information_message_main), "willy() execution result on the “main” branch: \"%s\"", result_main);
			
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
					correct_part4 ? VAL_STATUS_SUCCESS : VAL_STATUS_FAILURE,
					correct_part4 ? "ben() on the “part4” branch returned the correct result!" : "ben() on the “part4” branch didn’t return the correct result."
				},
				{
					correct_main ? VAL_STATUS_SUCCESS : VAL_STATUS_FAILURE,
					correct_main ? "willy() on the “main” branch returned the correct result!" : "willy() on the “main” branch didn’t return the correct result."
				},
				{
					VAL_STATUS_INFORMATION,
					information_message_part4
				},
				{
					VAL_STATUS_INFORMATION,
					information_message_main
				}
			};
			
			// Assign the score
			double score = 0.5;
			if (correct_part4) {
				score += 0.25;
			}
			if (correct_main) {
				score += 0.25;
			}
			
			json_object* json = json_create_val_success(score, messagesv, messagesc);
			if (!json) {
				e_json_force();
			}
			ptrs_json_add(json);
			e_json(json_object_to_file(FILE_RESULTS, json));
		} else if (eval_failed_part4 || eval_failed_main) {
			// In this disjunctive syllogism, if eval_failed_part4 is false, then eval_failed_main must be true.
			const char* name_branch = eval_failed_part4 ? "part4" : "main";
			const char* name_function = eval_failed_part4 ? "ben" : "willy";
			
			char warning_message[66 + strlen(name_branch) + strlen(name_function)];
			snprintf(warning_message, sizeof(warning_message), "Check that your code in the “%s” branch defines a function %s().", name_branch, name_function);
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
			char warning_message[77 + strlen(FILE_STUDENT)];
			snprintf(warning_message, sizeof(warning_message), "Check that your Python file is named “%s” and that your syntax is correct.", FILE_STUDENT);
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
		char warning_message[44 + strlen(COMMIT_MESSAGE_PREFIX)];
		snprintf(warning_message, sizeof(warning_message), "Your commit messages should both start with “%s”.", COMMIT_MESSAGE_PREFIX);
		
		// Set the information message for the “part4” branch
		char information_message_part4[36 + strlen(commit_message_own_part4)];
		snprintf(information_message_part4, sizeof(information_message_part4), "Your “part4” commit message: “%s”", commit_message_own_part4);
		
		// Set the information message for the “main” branch
		char information_message_main[35 + strlen(commit_message_own_main)];
		snprintf(information_message_main, sizeof(information_message_main), "Your “main” commit message: “%s”", commit_message_own_main);
		
		const unsigned short messagesc = 5;
		struct val_message_t messagesv[5] = {
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
				information_message_part4
			},
			{
				VAL_STATUS_INFORMATION,
				information_message_main
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
