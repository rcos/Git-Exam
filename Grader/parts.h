//
//  parts.h
//  Grader
//
//  Created by Gabriel Jacoby-Cooper on 1/8/23.
//

#ifndef parts_h
#define parts_h

#include "git2.h"

#define end() free_all(); git_libgit2_shutdown(); return EXIT_SUCCESS

int part1(git_repository* repository);

int part2(git_repository* repository);

int part3(git_repository* repository);

int part4(git_repository* repository);

int part5(git_repository* repository);

#endif /* parts_h */
