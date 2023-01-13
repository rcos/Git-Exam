echo "Submitting…"

if [ ! -d "$INSTALL_PREFIX" ]; then
	echo "ERROR: INSTALL_PREFIX is not set"
	echo "You must set INSTALL_PREFIX to the directory that contains the Git Exam working copy. (Omit the trailing slash character.)"
	exit 1
fi
if [ -z "$SUBMISSION_PREFIX" ]; then
	echo "ERROR: SUBMISSION_PREFIX is not properly set"
	echo "You must set SUBMISSION_PREFIX to a directory name that will be created to store the submitted repository while grading. The name should not contain any slash characters."
	exit 1
fi
case "$SUBMISSION_PREFIX" in
	*\/*) echo "ERROR: SUBMISSION_PREFIX unexpectedly contains a slash character"; exit 1;;
esac
if [ ! -f "$PYTHON_PREFIX"/libpython3.10d.a ]; then
	echo "ERROR: Couldn’t find the Python static library"
	echo "You must set PYTHON_PREFIX to the directory that contains the libpython3.10d.a static library. (Omit the trailing slash character.)"
	exit 1
fi
if [ ! -f "$TCL_PREFIX"/libtcl8.6.a ]; then
	echo "ERROR: Couldn’t find the TCL static library"
	echo "You must set TCL_PREFIX to the directory that contains the libtcl8.6.a static library. (Omit the trailing slash character.)"
	exit 1
fi
if [ ! -f "$UUID_PREFIX"/libtcl8.6.a ]; then
	echo "ERROR: Couldn’t find the UUID static library"
	echo "You must set UUID_PREFIX to the directory that contains the libuuid.a static library. (Omit the trailing slash character.)"
	exit 1
fi
if [ ! -d /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk ]; then
	echo "ERROR: The macOS SDK couldn’t be found"
	echo "Xcode and the macOS SDK must be installed in the standard location. The easiest way to fix this is to install Xcode from the Mac App Store and to launch it at least once."
	exit 1
fi
if [ ! -d /opt/homebrew ]; then
	echo "ERROR: Homebrew isn’t installed with the expected prefix"
	echo "This script doesn’t yet support the standard Homebrew prefix on Macs with Intel chips. Use a Mac with Apple silicon instead."
	exit 1
fi

frameworks="/opt/homebrew/lib/libjson-c.a /opt/homebrew/lib/libssh2.a /opt/homebrew/lib/libgit2.a /opt/homebrew/opt/ncurses/lib/libncurses.a /opt/homebrew/opt/ncurses/lib/libpanel.a /opt/homebrew/opt/libedit/lib/libedit.a /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libSystem.tbd /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libiconv.tbd /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libz.tbd /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libcharset.tbd /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libobjc.A.tbd /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/CoreFoundation.framework/CoreFoundation.tbd /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/Foundation.framework/Foundation.tbd /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/Security.framework/Security.tbd /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/SystemConfiguration.framework/SystemConfiguration.tbd ${PYTHON_PREFIX}/libpython3.10d.a /opt/homebrew/opt/bzip2/lib/libbz2.a /opt/homebrew/opt/openssl@1.1/lib/libcrypto.a /opt/homebrew/opt/libffi/lib/libffi.a /opt/homebrew/lib/liblzma.a /opt/homebrew/opt/sqlite/lib/libsqlite3.a /opt/homebrew/opt/openssl@1.1/lib/libssl.a ${TCL_PREFIX}/libtcl8.6.a /opt/homebrew/opt/tcl-tk/lib/libtkstub8.6.a ${UUID_PREFIX}/libuuid.a"

echo "Step 0: Cleaning…"
rm -rf "$INSTALL_PREFIX"/Submission
mkdir "$INSTALL_PREFIX"/Submission

echo "Step 1: Installing dependencies…"
brew install bzip2 json-c libedit libffi libssh2 ncurses openssl@1.1 sqlite3 tcl-tk xz

echo "Step 2: Compiling…"
cd "$INSTALL_PREFIX"/Grader
rm valgit.*
cp "$INSTALL_PREFIX"/valgit/* ./
clang -c -g -I/opt/homebrew/include -I/Applications/Xcode.app/Contents/Developer/Library/Frameworks/Python3.framework/Headers main.c part1.c part2.c part3.c part4.c part5.c part6.c stubs.c valgit.c
rm valgit.h valgit.c
ln -s "$INSTALL_PREFIX"/valgit/* ./

echo "Step 3: Linking…"
mkdir -p "$INSTALL_PREFIX"/Build/macOS/"$(arch)"
ld main.o part1.o part2.o part3.o part4.o part5.o part6.o stubs.o valgit.o $frameworks -o "$INSTALL_PREFIX"/Build/macOS/"$(arch)"/grade
rm *.o

echo "Step 4: Finishing…"
cd "$INSTALL_PREFIX"/Submission
mkdir "$SUBMISSION_PREFIX"
ln -s "$INSTALL_PREFIX"/Build/macOS/"$(arch)"/grade ./
ln -s "$INSTALL_PREFIX"/Grading\ Files/* ./
ln -s "$INSTALL_PREFIX"/Keys/* ./
echo "{\"testcase_prefix\":\"$SUBMISSION_PREFIX/\",\"username\":\"$1\"}" > custom_validator_input.json
echo "$2" > URL.txt

echo "Done!"
