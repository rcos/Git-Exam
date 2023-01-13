//
//  part6.c
//  Grader
//
//  Created by Gabriel Jacoby-Cooper on 1/12/23.
//

#include <sys/param.h>

#include "valgit.h"
#include "parts.h"

#define FILE_STUDENT "UICS.py"
#define FILE_SETUP "UICS Setup.py"

int part6(git_repository* repository) {
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
	
	// Find the relevant commits
	git_oid oid_commit_13;
	git_oid oid_commit_9;
	git_oid oid_commit_4;
	git_oid oid_commit_18;
	const unsigned int pathc = 0;
	const unsigned int pathv[pathc];
	e_git(git_nth_commit_oid(&oid_commit_13, NULL, 6, pathv, pathc, NULL, reference, repository));
	e_git(git_nth_commit_oid(&oid_commit_9, NULL, 7, pathv, pathc, NULL, reference, repository));
	e_git(git_nth_commit_oid(&oid_commit_4, NULL, 8, pathv, pathc, NULL, reference, repository));
	e_git(git_nth_commit_oid(&oid_commit_18, NULL, 9, pathv, pathc, NULL, reference, repository));
	git_reference_free(reference);
	
	// Check that the OIDs aren’t zero
	if (git_oid_is_zero(&oid_commit_13) || git_oid_is_zero(&oid_commit_9) || git_oid_is_zero(&oid_commit_4) || git_oid_is_zero(&oid_commit_18)) {
		const unsigned short messagesc = 1;
		struct val_message_t messagesv[1] = {
			{
				VAL_STATUS_FAILURE,
				"One or more of the expected commits couldn’t be found."
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
	
	// Look up the commit for step 13
	git_commit* commit_13;
	e_git(git_commit_lookup(&commit_13, repository, &oid_commit_13));
	ptrs_git_add(commit_13);
	
	// Look up the commit for step 9
	git_commit* commit_9;
	e_git(git_commit_lookup(&commit_9, repository, &oid_commit_9));
	ptrs_git_add(commit_9);
	
	// Look up the commit for step 4
	git_commit* commit_4;
	e_git(git_commit_lookup(&commit_4, repository, &oid_commit_4));
	ptrs_git_add(commit_4);
	
	// Look up the commit for step 18
	git_commit* commit_18;
	e_git(git_commit_lookup(&commit_18, repository, &oid_commit_18));
	ptrs_git_add(commit_18);
	
	unsigned int parentcount_13 = git_commit_parentcount(commit_13);
	unsigned int parentcount_9 = git_commit_parentcount(commit_9);
	unsigned int parentcount_4 = git_commit_parentcount(commit_4);
	unsigned int parentcount_18 = git_commit_parentcount(commit_18);
	if (parentcount_13 != 1 || parentcount_9 != 1 || parentcount_4 != 1 || parentcount_18 != 1) {
		const unsigned short messagesc = 2;
		struct val_message_t messagesv[2] = {
			{
				VAL_STATUS_FAILURE,
				"One or more of your commits for part 6 are merge commits."
			},
			{
				VAL_STATUS_WARNING,
				"There should be no merge commits for part 6 after rebasing."
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
	
	// Configure the checkout options
	git_checkout_options opts;
	e_git(git_checkout_options_init(&opts, GIT_CHECKOUT_OPTIONS_VERSION));
	opts.checkout_strategy = GIT_CHECKOUT_FORCE | GIT_CHECKOUT_DISABLE_PATHSPEC_MATCH;
	
	char filename[strlen(REPOSITORY_PREFIX) + strlen(FILE_STUDENT) + 1];
	snprintf(filename, sizeof(filename), "%s%s", REPOSITORY_PREFIX, FILE_STUDENT);
	
	// Read the setup code
	FILE* fp = fopen(FILE_SETUP, "r");
	if (!fp) {
		fprintf(stderr, "Failed to open setup file\n");
		e_force();
	}
	e(fseek(fp, 0, SEEK_END));
	long setup_size = ftell(fp);
	if (setup_size < 0) {
		fprintf(stderr, "Failed to get the size of the setup file\n");
		fclose(fp);
		e_force();
	}
	errno = 0;
	rewind(fp);
	if (errno) {
		fprintf(stderr, "Failed to rewind the setup file\n");
		fclose(fp);
		e_force();
	}
	char setup_str[setup_size + sizeof(char)];
	unsigned long setup_end = fread(setup_str, sizeof(char), setup_size, fp);
	if (ferror(fp)) {
		fprintf(stderr, "Failed to read the setup file\n");
		fclose(fp);
		e_force();
	}
	fclose(fp);
	setup_str[setup_end] = '\0';
	
	// MARK: - Check out step 13
	
	// Check out the commit for step 13
	e_git(git_checkout_tree(repository, (git_object*) commit_13, &opts));
	
	// Evaluate the setup code
	struct val_python_context_t context_13;
	bool eval_failed_setup_13;
	void* result_setup_13 = val_python_result_new(setup_str, filename, &context_13, &eval_failed_setup_13);
	if (!result_setup_13) {
		val_python_free(context_13);
		goto invalid_code;
	}
	free(result_setup_13);
	
	// MARK: - full_name() in step 13
	
	bool eval_failed_13_fullname;
	char* result_13_fullname = val_python_result("cl().full_name()", context_13, &eval_failed_13_fullname);
	if (result_13_fullname) {
		ptrs_add(result_13_fullname);
		if (strcmp(result_13_fullname, "Computability & Logic") != 0) {
			val_python_free(context_13);
			char information_message[70 + strlen(result_13_fullname)];
			snprintf(information_message, sizeof(information_message), "full_name() execution result for step 13: \"%s\"", result_13_fullname);
			
			const unsigned short messagesc = 3;
			struct val_message_t messagesv[3] = {
				{
					VAL_STATUS_FAILURE,
					"full_name() in Course didn’t return the correct result in the commit for step 13."
				},
				{
					VAL_STATUS_WARNING,
					"Check that you properly rebased the “part6-conflict” branch on the “main” branch."
				},
				{
					VAL_STATUS_INFORMATION,
					information_message
				}
			};
			json_object* json = json_create_val_success(0.125, messagesv, messagesc);
			if (!json) {
				e_json_force();
			}
			ptrs_json_add(json);
			e_json(json_object_to_file(FILE_RESULTS, json));
			goto end_success;
		}
	} else if (eval_failed_13_fullname) {
		val_python_free(context_13);
		const unsigned short messagesc = 2;
		struct val_message_t messagesv[2] = {
			{
				VAL_STATUS_FAILURE,
				"Your code couldn’t be evaluated."
			},
			{
				VAL_STATUS_WARNING,
				"Check that your code defines a function full_name() in Course."
			}
		};
		json_object* json = json_create_val_success(0, messagesv, messagesc);
		if (!json) {
			e_json_force();
		}
		ptrs_json_add(json);
		e_json(json_object_to_file(FILE_RESULTS, json));
		goto end_success;
	} else {
		val_python_free(context_13);
		goto invalid_code;
	}
	
	// MARK: - conflict() in step 13
	
	bool eval_failed_13_conflict;
	char* result_13_conflict = val_python_result("[str(cl().conflict(ece())), str(ece().conflict(ep()))]", context_13, &eval_failed_13_conflict);
	if (result_13_conflict) {
		ptrs_add(result_13_conflict);
		if (strcmp(result_13_conflict, "['True', 'True']") != 0) {
			val_python_free(context_13);
			char information_message[69 + strlen(result_13_conflict)];
			snprintf(information_message, sizeof(information_message), "conflict() execution result for step 13: \"%s\"", result_13_conflict);
			
			const unsigned short messagesc = 4;
			struct val_message_t messagesv[4] = {
				{
					VAL_STATUS_SUCCESS,
					"full_name() in Course returned the correct result in the commit for step 13!"
				},
				{
					VAL_STATUS_FAILURE,
					"conflict() in Course didn’t return the correct result in the commit for step 13."
				},
				{
					VAL_STATUS_WARNING,
					"Check that you properly rebased the “part6-conflict” branch on the “main” branch."
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
			goto end_success;
		}
	} else if (eval_failed_13_conflict) {
		val_python_free(context_13);
		const unsigned short messagesc = 2;
		struct val_message_t messagesv[2] = {
			{
				VAL_STATUS_FAILURE,
				"Your code couldn’t be evaluated."
			},
			{
				VAL_STATUS_WARNING,
				"Check that your code defines a function conflict() in Course."
			}
		};
		json_object* json = json_create_val_success(0, messagesv, messagesc);
		if (!json) {
			e_json_force();
		}
		ptrs_json_add(json);
		e_json(json_object_to_file(FILE_RESULTS, json));
		goto end_success;
	} else {
		val_python_free(context_13);
		goto invalid_code;
	}
	
	// MARK: - hours() in step 13
	
	bool eval_failed_13_hours;
	char* result_13_hours = val_python_result("str(ep().hours())", context_13, &eval_failed_13_hours);
	if (result_13_hours) {
		ptrs_add(result_13_hours);
		if (strcmp(result_13_hours, "4") != 0) {
			val_python_free(context_13);
			char information_message[66 + strlen(result_13_hours)];
			snprintf(information_message, sizeof(information_message), "hours() execution result for step 13: \"%s\"", result_13_hours);
			
			const unsigned short messagesc = 5;
			struct val_message_t messagesv[5] = {
				{
					VAL_STATUS_SUCCESS,
					"full_name() in Course returned the correct result in the commit for step 13!"
				},
				{
					VAL_STATUS_SUCCESS,
					"conflict() in Course returned the correct result in the commit for step 13!"
				},
				{
					VAL_STATUS_FAILURE,
					"hours() in Course didn’t return the correct result in the commit for step 13."
				},
				{
					VAL_STATUS_WARNING,
					"Check that you properly rebased the “part6-conflict” branch on the “main” branch."
				},
				{
					VAL_STATUS_INFORMATION,
					information_message
				}
			};
			json_object* json = json_create_val_success(0.375, messagesv, messagesc);
			if (!json) {
				e_json_force();
			}
			ptrs_json_add(json);
			e_json(json_object_to_file(FILE_RESULTS, json));
			goto end_success;
		}
	} else if (eval_failed_13_hours) {
		val_python_free(context_13);
		const unsigned short messagesc = 2;
		struct val_message_t messagesv[2] = {
			{
				VAL_STATUS_FAILURE,
				"Your code couldn’t be evaluated."
			},
			{
				VAL_STATUS_WARNING,
				"Check that your code defines a function hours() in Course."
			}
		};
		json_object* json = json_create_val_success(0, messagesv, messagesc);
		if (!json) {
			e_json_force();
		}
		ptrs_json_add(json);
		e_json(json_object_to_file(FILE_RESULTS, json));
		goto end_success;
	} else {
		val_python_free(context_13);
		goto invalid_code;
	}
	
	// MARK: - Clean up step 13
	
	val_python_free(context_13);
	
	// MARK: - Check out step 9
	
	// Check out the commit for step 9
	e_git(git_checkout_tree(repository, (git_object*) commit_9, &opts));
	
	// Evaluate the setup code
	struct val_python_context_t context_9;
	bool eval_failed_setup_9;
	void* result_setup_9 = val_python_result_new(NULL, filename, &context_9, &eval_failed_setup_9);
	if (!result_setup_9) {
		val_python_free(context_9);
		goto invalid_code;
	}
	e(val_python_result_file(FILE_SETUP, context_9, Py_eval_input));
	
	// MARK: - conflict() in step 9
	
	bool eval_failed_9_conflict;
	char* result_9_conflict = val_python_result("[str(cl().conflict(ece())), str(ece().conflict(ep()))]", context_9, &eval_failed_9_conflict);
	if (result_9_conflict) {
		ptrs_add(result_9_conflict);
		if (strcmp(result_9_conflict, "['False', 'True']") != 0) {
			val_python_free(context_9);
			char information_message[68 + strlen(result_9_conflict)];
			snprintf(information_message, sizeof(information_message), "conflict() execution result for step 9: \"%s\"", result_9_conflict);
			
			const unsigned short messagesc = 6;
			struct val_message_t messagesv[6] = {
				{
					VAL_STATUS_SUCCESS,
					"full_name() in Course returned the correct result in the commit for step 13!"
				},
				{
					VAL_STATUS_SUCCESS,
					"conflict() in Course returned the correct result in the commit for step 13!"
				},
				{
					VAL_STATUS_SUCCESS,
					"hours() in Course returned the correct result in the commit for step 13!"
				},
				{
					VAL_STATUS_FAILURE,
					"conflict() in Course didn’t return the correct result in the commit for step 9."
				},
				{
					VAL_STATUS_WARNING,
					"Check that you properly rebased the “part6-conflict” branch on the “main” branch."
				},
				{
					VAL_STATUS_INFORMATION,
					information_message
				}
			};
			json_object* json = json_create_val_success(0.5, messagesv, messagesc);
			if (!json) {
				e_json_force();
			}
			ptrs_json_add(json);
			e_json(json_object_to_file(FILE_RESULTS, json));
			goto end_success;
		}
	} else if (eval_failed_9_conflict) {
		val_python_free(context_9);
		const unsigned short messagesc = 2;
		struct val_message_t messagesv[2] = {
			{
				VAL_STATUS_FAILURE,
				"Your code couldn’t be evaluated."
			},
			{
				VAL_STATUS_WARNING,
				"Check that your code defines a function conflict() in Course."
			}
		};
		json_object* json = json_create_val_success(0, messagesv, messagesc);
		if (!json) {
			e_json_force();
		}
		ptrs_json_add(json);
		e_json(json_object_to_file(FILE_RESULTS, json));
		goto end_success;
	} else {
		goto invalid_code;
	}
	
	// MARK: - Clean up step 9
	
	val_python_free(context_9);
	
	// MARK: - Check out step 4
	
	// Check out the commit for step 4
	e_git(git_checkout_tree(repository, (git_object*) commit_4, &opts));
	
	// Evaluate the setup code
	struct val_python_context_t context_4;
	bool eval_failed_setup_4;
	void* result_setup_4 = val_python_result_new(setup_str, filename, &context_4, &eval_failed_setup_4);
	if (!result_setup_4) {
		val_python_free(context_4);
		goto invalid_code;
	}
	free(result_setup_4);
	
	// MARK: - full_name() in step 4
	
	bool eval_failed_4_fullname;
	char* result_4_fullname = val_python_result("cl().full_name()", context_4, &eval_failed_4_fullname);
	if (result_4_fullname) {
		ptrs_add(result_4_fullname);
		if (strcmp(result_4_fullname, "PHIL-4420 Computability & Logic") != 0) {
			val_python_free(context_4);
			char information_message[69 + strlen(result_4_fullname)];
			snprintf(information_message, sizeof(information_message), "full_name() execution result for step 4: \"%s\"", result_4_fullname);
			
			const unsigned short messagesc = 7;
			struct val_message_t messagesv[7] = {
				{
					VAL_STATUS_SUCCESS,
					"full_name() in Course returned the correct result in the commit for step 13!"
				},
				{
					VAL_STATUS_SUCCESS,
					"conflict() in Course returned the correct result in the commit for step 13!"
				},
				{
					VAL_STATUS_SUCCESS,
					"hours() in Course returned the correct result in the commit for step 13!"
				},
				{
					VAL_STATUS_SUCCESS,
					"conflict() in Course returned the correct result in the commit for step 9!"
				},
				{
					VAL_STATUS_FAILURE,
					"full_name() in Course didn’t return the correct result in the commit for step 4."
				},
				{
					VAL_STATUS_WARNING,
					"Check that you properly rebased the “part6-conflict” branch on the “main” branch."
				},
				{
					VAL_STATUS_INFORMATION,
					information_message
				}
			};
			json_object* json = json_create_val_success(0.625, messagesv, messagesc);
			if (!json) {
				e_json_force();
			}
			ptrs_json_add(json);
			e_json(json_object_to_file(FILE_RESULTS, json));
			goto end_success;
		}
	} else if (eval_failed_4_fullname) {
		val_python_free(context_4);
		const unsigned short messagesc = 2;
		struct val_message_t messagesv[2] = {
			{
				VAL_STATUS_FAILURE,
				"Your code couldn’t be evaluated."
			},
			{
				VAL_STATUS_WARNING,
				"Check that your code defines a function full_name() in Course."
			}
		};
		json_object* json = json_create_val_success(0, messagesv, messagesc);
		if (!json) {
			e_json_force();
		}
		ptrs_json_add(json);
		e_json(json_object_to_file(FILE_RESULTS, json));
		goto end_success;
	} else {
		val_python_free(context_4);
		goto invalid_code;
	}
	
	// MARK: - Clean up step 4
	
	val_python_free(context_4);
	
	// MARK: - Check out step 18
	
	// Check out the commit for step 18
	e_git(git_checkout_tree(repository, (git_object*) commit_18, &opts));
	
	// Evaluate the setup code
	struct val_python_context_t context_18;
	bool eval_failed_setup_18;
	void* result_setup_18 = val_python_result_new(setup_str, filename, &context_18, &eval_failed_setup_18);
	if (!result_setup_18) {
		val_python_free(context_18);
		goto invalid_code;
	}
	free(result_setup_18);
	
	// MARK: - full_name() in step 18
	
	bool eval_failed_18_fullname;
	char* result_18_fullname = val_python_result("test_conflict()", context_18, &eval_failed_18_fullname);
	if (result_18_fullname) {
		ptrs_add(result_18_fullname);
		if (strcmp(result_18_fullname, "Conflict!") != 0) {
			val_python_free(context_18);
			char information_message[70 + strlen(result_18_fullname)];
			snprintf(information_message, sizeof(information_message), "full_name() execution result for step 18: \"%s\"", result_18_fullname);
			
			const unsigned short messagesc = 8;
			struct val_message_t messagesv[8] = {
				{
					VAL_STATUS_SUCCESS,
					"full_name() in Course returned the correct result in the commit for step 13!"
				},
				{
					VAL_STATUS_SUCCESS,
					"conflict() in Course returned the correct result in the commit for step 13!"
				},
				{
					VAL_STATUS_SUCCESS,
					"hours() in Course returned the correct result in the commit for step 13!"
				},
				{
					VAL_STATUS_SUCCESS,
					"conflict() in Course returned the correct result in the commit for step 9!"
				},
				{
					VAL_STATUS_SUCCESS,
					"full_name() in Course returned the correct result in the commit for step 4!"
				},
				{
					VAL_STATUS_FAILURE,
					"test_conflict() didn’t return the correct result in the commit for step 18."
				},
				{
					VAL_STATUS_WARNING,
					"Check that you properly rebased the “part6-conflict” branch on the “main” branch."
				},
				{
					VAL_STATUS_INFORMATION,
					information_message
				}
			};
			json_object* json = json_create_val_success(0.75, messagesv, messagesc);
			if (!json) {
				e_json_force();
			}
			ptrs_json_add(json);
			e_json(json_object_to_file(FILE_RESULTS, json));
			goto end_success;
		}
	} else if (eval_failed_18_fullname) {
		val_python_free(context_18);
		const unsigned short messagesc = 2;
		struct val_message_t messagesv[2] = {
			{
				VAL_STATUS_FAILURE,
				"Your code couldn’t be evaluated."
			},
			{
				VAL_STATUS_WARNING,
				"Check that your code defines a function test_conflict()."
			}
		};
		json_object* json = json_create_val_success(0, messagesv, messagesc);
		if (!json) {
			e_json_force();
		}
		ptrs_json_add(json);
		e_json(json_object_to_file(FILE_RESULTS, json));
		goto end_success;
	} else {
		val_python_free(context_18);
		goto invalid_code;
	}
	
	// MARK: - Clean up step 4
	
	val_python_free(context_18);
	
	// MARK: - End
	
	const unsigned short messagesc = 2;
	struct val_message_t messagesv[2] = {
		{
			VAL_STATUS_SUCCESS,
			"Your commit structure is correct!"
		},
		{
			VAL_STATUS_SUCCESS,
			"Your code returns the correct results in each commit!"
		}
	};
	json_object* json = json_create_val_success(1, messagesv, messagesc);
	if (!json) {
		e_json_force();
	}
	ptrs_json_add(json);
	e_json(json_object_to_file(FILE_RESULTS, json));
	goto end_success;
	
invalid_code:;
	char warning_message_invalid[115 + strlen(FILE_STUDENT)];
	snprintf(warning_message_invalid, sizeof(warning_message_invalid), "Check that your Python file is named “%s”, that your syntax is correct, and that you solved all merge conflicts.", FILE_STUDENT);
	const unsigned short messagesc_invalid = 2;
	struct val_message_t messagesv_invalid[2] = {
		{
			VAL_STATUS_FAILURE,
			"Your code couldn’t be evaluated."
		},
		{
			VAL_STATUS_WARNING,
			warning_message_invalid
		}
	};
	json_object* json_invalid = json_create_val_success(0, messagesv_invalid, messagesc_invalid);
	if (!json_invalid) {
		e_json_force();
	}
	ptrs_json_add(json_invalid);
	e_json(json_object_to_file(FILE_RESULTS, json_invalid));
	
end_success:;
	end();
}
