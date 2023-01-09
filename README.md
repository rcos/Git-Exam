# Git Exam
Git Exam is a specification for an exam that tests students on their abilities to work with Git repositories. It supports automated grading via [Submitty](https://submitty.org).

Git Exam is developed and maintained by the [Rensselaer Center for Open Source](https://rcos.io).

## Grader
The source code for the automatic grader is located in the `Grader` directory at the root of the repository. There’s also an Xcode project file at the root of the repository for easy development in Xcode. (Xcode is the recommended IDE for Git Exam development.)

## Keys
You must generate in the `Keys` directory at the root of the repository a separate ED25519 key pair for _each_ student. Each private key must have the relevant student’s RCS ID as its filename, and each public key must have the same filename as the corresponding private key but with an added “`.pub`” suffix. As the instructor, distribute the public key (`Keys/*.pub`, where “`*`” is the student’s RCS ID) to your students. Do _not_ distribute the private keys to anyone. You will, however, need to include both the public key and the private key for _all_ key pairs in the auto-grading configuration that you upload to Submitty.

### Key Generation
If a `Keys` directory at the root of the repository doesn’t yet exist, then create it. Then, from the root of the repository, run `ssh-keygen -t ed25519`. When you’re prompted, specify “`Keys/[rcs id]`”, where “`[rcs id]`” is the student’s RCS ID, as the path. Leave the passphrase empty. You could potentially automate this with a shell script based on a list of RCS IDs; consult the manual for `ssh-keygen` to learn how to invoke it without user interaction.

## Deployment
To deploy to Submitty, zip the contents of the `Auto-Grading Configuration` directory (but _not_ the directory itself) and upload the zip archive to Submitty. (You must have previously built the appropriate `grade` binary and collected it in `Auto-Grading Configuration/custom_validation_code`.) Make sure to select “Students will submit one or more files by direct upload to the Submitty website” for “Gradeable Type” on Submitty.

## Commands
The project includes several helpful shell commands for developing, debugging, and building the automated grader. They are all located in the `Commands` directory at the root of the repository.

### Collect
The Collect command finds deploy keys and built products for Linux on x86_64 and copies them into the `Auto-Grading Configuration/custom_validation_code` directory, where they can be included in the zip archive that you deploy to Submitty.

### Reset
The Reset command deletes the existing test repository in the `Repository` directory and re-clones the specified public source repository. The first command-line argument must be the clone URL for the public source repository, and the second must be the clone URL for an empty private repository.

### Submit
The Submit command comes in two “flavors”: one for macOS and one for Ubuntu. It mimics what happens on a Submitty server when a student submits the SSH clone URL for their private GitHub repository. That SSH clone URL must be supplied as the second command-line argument. The first command-line argument must be the RCS ID of the student who’s submitting the exam. (For testing purposes, this can be any arbitrary string as long as there’s a private key the filename of which is that string and a public key the filename of which is the same as the private key but with an added “`.pub`” suffix. The public key must also have already been added as a deploy key for the GitHub repository that the SSH clone URL locates.) After executing the Submit command, you can run the test cases manually by launching the `grade` executable in the `Submission` directory. Supply “`part*`”, where “`*`” is the test-case/part number, as the first command-line argument.
