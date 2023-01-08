# Git Exam
Git Exam is a specification for an exam that tests students on their abilities to work with Git repositories. It supports automated grading via [Submitty](https://submitty.org).

Git Exam is developed and maintained by the [Rensselaer Center for Open Source](https://rcos.io).

## Grader
The source code for the automatic grader is located in the `Grader` directory at the root of the repository. There’s also an Xcode project file at the root of the repository for easy development in Xcode. (Xcode is the recommended IDE for Git Exam development.)

## Keys
You must generate an ED25519 key-pair in the `Keys` directory at the root of the repository. As the instructor, distribute the public key (`Keys/id_ed25519.pub`) to your students. Do _not_ distribute the private key to anyone. You will, however, need to include both the public key and the private key in the auto-grading configuration that you upload to Submitty.

### Key Generation
From the root of the repository, run `ssh-keygen -t ed25519`. When prompted, specify `Keys/id_ed25519` as the path. Leave the passphrase empty.

## Deployment
To deploy to Submitty, zip the contents of the `Auto-Grading Configuration` directory (but _not_ the directory itself) and upload the zip archive to Submitty. Make sure to select “Students will submit one or more files by direct upload to the Submitty website” for “Gradeable Type” on Submitty.

## Commands
The project includes several helpful shell commands for developing, debugging, and building the automated grader. They are all located in the `Commands` directory at the root of the repository.

### Collect
The Collect command finds deploy keys and built products for Linux on x86_64 and copies them into the `Auto-Grading Configuration/custom_validation_code` directory, where they can be included in the zip archive that you deploy to Submitty.

### Reset
The Reset command deletes the existing test repository in the `Repository` directory and re-clones the specified public source repository. The first command-line argument must be the clone URL for the public source repository, and the second must be the clone URL for an empty private repository.

### Submit
The Submit command comes in two “flavors”: one for macOS and one for Ubuntu. It mimics what happens on a Submitty server when a student submits the SSH clone URL for their private GitHub repository. That SSH clone URL must be supplied as the first command-line argument. After executing the Submit command, you can run the test-cases manually by launching the `part*` executables in the `Submission` directory, where “*” is the test-case/part number.
