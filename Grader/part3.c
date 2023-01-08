//
//  part3.c
//  Part 3
//
//  Created by Gabriel Jacoby-Cooper on 1/4/23.
//

#include <sys/param.h>

#include "valgit.h"

#define FILE_STUDENT "Code.py"
#define COMMIT_MESSAGE_PREFIX "[Part 3]"

#define end() free_all(); git_libgit2_shutdown(); return EXIT_SUCCESS

int main(void) {
	int ret;
	e_init();
	e_git(git_libgit2_init());
	
	// Read the repository URL
	FILE* fp = val_url_file();
	if (!fp) {
		fprintf(stderr, "Failed to open URL file\n");
		e_force();
	}
	char* url = val_file_str(fp);
	fclose(fp);
	if (!url) {
		fprintf(stderr, "Failed to read URL file\n");
		e_force();
	}
	
	// Clone the repository
	bool access_public;
	git_repository* repository = val_git_clone(url, &access_public);
	free(url);
	if (!repository) {
		if (access_public) {
			const unsigned short messagesc = 2;
			struct val_message_t messagesv[2] = {
				{
					failure,
					"Your Git repository is publicly accessible."
				},
				{
					warning,
					"This is a violation of academic integrity. Please make your repository private and configure the deploy key."
				}
			};
			json_object* json = json_create_val_success(0, messagesv, messagesc);
			if (!json) {
				e_json_force();
			}
			e_json(json_object_to_file(FILE_RESULTS, json));
			end();
		} else {
			val_error_message_git(-1);
			const unsigned short messagesc = 2;
			struct val_message_t messagesv[2] = {
				{
					failure,
					"Your Git repository couldn’t be cloned."
				},
				{
					warning,
					"Check that you submitted the correct SSH clone URL and that you properly configured the deploy key."
				}
			};
			json_object* json = json_create_val_success(0, messagesv, messagesc);
			if (!json) {
				e_json_force();
			}
			e_json(json_object_to_file(FILE_RESULTS, json));
			end();
		}
	}
	ptrs_git_repository_set(repository);
	
	// Get a reference to the latest commit in the “main” branch
	git_reference* reference;
	ret = git_branch_lookup(&reference, repository, "origin/main", GIT_BRANCH_REMOTE);
	if (ret < 0) {
		const unsigned short messagesc = 2;
		struct val_message_t messagesv[2] = {
			{
				failure,
				"Your Git repository doesn’t have a “main” branch."
			},
			{
				warning,
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
	e_git(git_nth_commit_oid(&oid_commit, &first, 3, pathv, pathc, NULL, reference, repository));
	git_reference_free(reference);
	
	// Check that the OID isn’t zero
	if (git_oid_is_zero(&oid_commit)) {
		const unsigned short messagesc = 1;
		struct val_message_t messagesv[1] = {
			{
				failure,
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
				failure,
				"The expected commit couldn’t be found."
			},
			{
				warning,
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
				failure,
				"The commit isn’t a merge commit."
			},
			{
				warning,
				"You must create a merge commit by merging the “part2” branch into the “main” branch."
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
		
		// Evaluate the code
		e_git(git_checkout_tree(repository, (git_object*) commit, &opts));
		bool eval_failed;
		char* result = val_python_result("alyssa()", filename, &eval_failed);
		
		if (result) { // It’s an invariant that eval_failed is false if result is not NULL.
			bool correct = strcmp(result, "p. hacker") == 0;
			char information_message[55 + strlen(result)];
			snprintf(information_message, sizeof(information_message), "alyssa() execution result: \"%s\"", result);
			free(result);
			const unsigned short messagesc = 4;
			struct val_message_t messagesv[4] = {
				{
					success,
					"Your commit structure is correct!"
				},
				{
					success,
					"Your commit message is correct!"
				},
				{
					correct ? success : failure,
					correct ? "alyssa() returned the correct result!" : "alyssa() didn’t return the correct result."
				},
				{
					information,
					information_message
				}
			};
			json_object* json = json_create_val_success(correct ? 1 : 0.5, messagesv, messagesc);
			if (!json) {
				e_json_force();
			}
			ptrs_json_add(json);
			e_json(json_object_to_file(FILE_RESULTS, json));
		} else if (eval_failed) {
			const unsigned short messagesc = 4;
			struct val_message_t messagesv[4] = {
				{
					success,
					"Your commit structure is correct!"
				},
				{
					success,
					"Your commit message is correct!"
				},
				{
					failure,
					"Your code couldn’t be evaluated."
				},
				{
					warning,
					"Check that your code defines a function alyssa()."
				}
			};
			json_object* json = json_create_val_success(0.5, messagesv, messagesc);
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
					success,
					"Your commit structure is correct!"
				},
				{
					success,
					"Your commit message is correct!"
				},
				{
					failure,
					"Your code couldn’t be evaluated."
				},
				{
					warning,
					warning_message
				}
			};
			json_object* json = json_create_val_success(0.5, messagesv, messagesc);
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
				success,
				"Your commit structure is correct!"
			},
			{
				failure,
				"Your commit message is incorrect."
			},
			{
				warning,
				warning_message
			},
			{
				information,
				information_message
			}
		};
		json_object* json = json_create_val_success(0.5, messagesv, messagesc);
		if (!json) {
			e_json_force();
		}
		ptrs_json_add(json);
		e_json(json_object_to_file(FILE_RESULTS, json));
	}
	
	end();
}
