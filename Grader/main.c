//
//  main.c
//  Grader
//
//  Created by Gabriel Jacoby-Cooper on 1/8/23.
//

#include "valgit.h"
#include "parts.h"

int main(int argc, const char* const argv[]) {
	e_init();
	e_git(git_libgit2_init());
	int ret;
	
	if (argc != 2) {
		char error_str[87 + (unsigned int) floor(log10(abs(argc == 0 ? 1 : argc)))];
		snprintf(error_str, sizeof(error_str), "Invalid number of command-line arguments (expected 2, including the invocation; got %u)", (unsigned int) argc);
		e_force_str(error_str);
	}
	
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
	git_repository* repository;
	ret = val_git_clone(url, &access_public, &repository);
	free(url);
	e_git(ret);
	if (!repository) {
		if (access_public) {
			const unsigned short messagesc = 2;
			struct val_message_t messagesv[2] = {
				{
					VAL_STATUS_FAILURE,
					"Your Git repository is publicly accessible."
				},
				{
					VAL_STATUS_WARNING,
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
			const char* failure_message;
			const char* warning_message;
			switch (ret) {
			case VAL_GIT_CLONE_ERROR_NORCSID:
				failure_message = "Your RCS ID couldn’t be read.";
				warning_message = "Contact your instructor for help.";
				break;
			case VAL_GIT_CLONE_ERROR_NOPUBLICKEY:
				failure_message = "There’s no public key for your RCS ID.";
				warning_message = "Contact your instructor for help.";
				break;
			case VAL_GIT_CLONE_ERROR_NOPRIVATEKEY:
				failure_message = "There’s no private key for your RCS ID.";
				warning_message = "Contact your instructor for help.";
				break;
			default:
				failure_message = "Your Git repository couldn’t be cloned.";
				warning_message = "Check that you submitted the correct SSH clone URL and that you properly configured the deploy key.";
			}
			val_error_message_git(-1);
			const unsigned short messagesc = 2;
			struct val_message_t messagesv[2] = {
				{
					VAL_STATUS_FAILURE,
					failure_message
				},
				{
					VAL_STATUS_WARNING,
					warning_message
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
	
	free_all();
	const char* const part = argv[1];
	if (strcmp(part, "part1") == 0) {
		return part1(repository);
	} else if (strcmp(part, "part2") == 0) {
		return part2(repository);
	} else if (strcmp(part, "part3") == 0) {
		return part3(repository);
	} else if (strcmp(part, "part4") == 0) {
		return part4(repository);
	} else if (strcmp(part, "part5") == 0) {
		return part5(repository);
	} else {
		char error_str[24 + strlen(part)];
		snprintf(error_str, sizeof(error_str), "Invalid part argument: %s", part);
		e_force_str(error_str);
	}
}
