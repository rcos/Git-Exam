echo "Submitting…"
echo "NOTE: This script has only been tested on Ubuntu 20.04 “focal” and 22.04 “jammy”. Functionality on other versions of Ubuntu isn’t guaranteed."

if [ ! -d "$INSTALL_PREFIX" ]; then
	echo "ERROR: INSTALL_PREFIX is not properly set"
	echo "You must set INSTALL_PREFIX to the directory that contains the Git Exam working copy. (Omit the trailing slash character.)"
	exit 1
fi
if [ -z "$SUBMISSION_PREFIX" ]; then
	echo "ERROR: SUBMISSION_PREFIX is not properly set"
	echo "You must set SUBMISSION_PREFIX to a directory name that will be created to store the submitted repository while grading. The name should not contain any slash characters."
	exit 1
fi
case "$SUBMISSION_PREFIX" in
	*\/*)
		echo "ERROR: SUBMISSION_PREFIX unexpectedly contains a slash character"
		exit 1
esac
if [ ! -d "$LIB_PREFIX" ]; then
	echo "ERROR: LIB_PREFIX is not properly set"
	echo "You must set LIB_PREFIX to the directory that contains the static libraries on your system. (Omit the trailing slash character.)"
	exit 1
fi

source /etc/lsb-release
case  "$DISTRIB_CODENAME" in
	bionic )
		include_python=/usr/include/python3.6
		lib_python="$LIB_PREFIX"/libpython3.6.a
		;;
	focal )
		include_python=/usr/include/python3.8
		lib_python="$LIB_PREFIX"/libpython3.8.a
		;;
	jammy )
		include_python=/usr/include/python3.10
		lib_python="$LIB_PREFIX"/libpython3.10.a
		;;
	* )
		echo "ERROR: Invalid Ubuntu distribution codename!"
		exit 1
esac

read -p "Continue? (“y” or “Y”) " input
case "$input" in 
	y|Y )
		echo "Continuing…"
		;;
	* )
		echo "Halting…"
		exit 0
esac

frameworks="$LIB_PREFIX/libgit2.a $LIB_PREFIX/libjson-c.a $lib_python $LIB_PREFIX/libmbedtls.a $LIB_PREFIX/libmbedx509.a $LIB_PREFIX/libmbedcrypto.a $LIB_PREFIX/libpcre2-posix.a $LIB_PREFIX/libhttp_parser.a $LIB_PREFIX/libz.a $LIB_PREFIX/libssh2.a $LIB_PREFIX/libgssapi_krb5.a $LIB_PREFIX/libexpat.a $LIB_PREFIX/libopenlibm.a $LIB_PREFIX/libgcrypt.a $LIB_PREFIX/libkeyutils.a $LIB_PREFIX/libpthread.a $LIB_PREFIX/libresolv.a $LIB_PREFIX/libssl.a $LIB_PREFIX/libutil.a $LIB_PREFIX/libc.a $LIB_PREFIX/libgpg-error.a $LIB_PREFIX/libcrypto.a $LIB_PREFIX/libdl.a $LIB_PREFIX/libkrb5.a $LIB_PREFIX/libk5crypto.a $LIB_PREFIX/libcom_err.a $LIB_PREFIX/libkrb5support.a"
krb5_names=(libgssapi_krb5.a libkrb5.a libk5crypto.a libcom_err.a libkrb5support.a)

if [ "$EUID" -ne 0 ]; then
	sudo -k
	if sudo true; then
		echo "Submitting…"
	else
		echo "Wrong password!"
		exit 1
	fi
fi

echo "Step 0:	Cleaning…"
rm -rf "$INSTALL_PREFIX"/Submission
mkdir "$INSTALL_PREFIX"/Submission
if [ -d /tmp/libssh2-1.10.0 ]; then
	rm -rf /tmp/libssh2-1.10.0
fi
if [ -d /tmp/libgit2-1.5.0 ]; then
	rm -rf /tmp/libgit2-1.5.0
fi
if [ -d /tmp/krb5-1.20.1 ]; then
	rm -rf /tmp/krb5-1.20.1
fi

echo "Step 1:	Installing dependencies…"
sudo apt-get update
sudo apt-get install -y bison clang cmake libedit-dev libexpat1-dev libgcrypt20-dev libgpg-error-dev libhttp-parser-dev libjson-c-dev libkeyutils-dev make libmbedtls-dev libopenlibm-dev openssl libpcre2-dev python3-dev libssh-dev libssl-dev wget
if [ ! -f "$LIB_PREFIX"/libssh2.a ]; then
	echo "	Couldn’t find libssh2 static library: libssh2.a"
	
	echo "	Downloading libssh2…"
	cd /tmp
	wget https://www.libssh2.org/download/libssh2-1.10.0.tar.gz
	tar -xvzf libssh2-1.10.0.tar.gz
	rm libssh2-1.10.0.tar.gz
	cd libssh2-1.10.0
	
	echo "	Building libssh2…"
	./configure --libdir="$LIB_PREFIX"
	make
	
	echo "	Installing libssh2…"
	make install
fi
if [ ! -f "$LIB_PREFIX"/libgit2.a ]; then
	echo "	Couldn’t find libgit2 static library: libgit2.a"
	
	echo "	Downloading libgit2…"
	cd /tmp
	wget https://github.com/libgit2/libgit2/archive/refs/tags/v1.5.0.tar.gz
	tar -xvzf v1.5.0.tar.gz
	rm v1.5.0.tar.gz
	cd libgit2-1.5.0
	
	echo "	Building libgit2…"
	mkdir build
	cd build
	cmake -DBUILD_SHARED_LIBS=OFF -DUSE_SSH=ON ..
	cmake --build .
	
	echo "	Installing libgit2…"
	cp libgit2.a "$LIB_PREFIX"/
	cd ..
	cp include/git2.h /usr/include/
	cp -r include/git2 /usr/include/
fi
for krb5_name in "${krb5_names[@]}"; do
	if [ ! -f "$LIB_PREFIX"/"$krb5_name" ]; then
		echo "	Couldn’t find Kerberos static library: $krb5_name"
		
		echo "	Downloading Kerberos…"
		cd /tmp
		wget https://kerberos.org/dist/krb5/1.20/krb5-1.20.1.tar.gz
		tar -xvzf krb5-1.20.1.tar.gz
		rm krb5-1.20.1.tar.gz
		cd krb5-1.20.1/src
		
		echo "	Building Kerberos…"
		# To work around a bug in the makefile, we must configure and build the shared library before we can configure and build the static library
		# https://krbdev.mit.edu/rt/Ticket/History.html?id=8996
		./configure
		make
		./configure --enable-static --disable-shared
		make
		
		echo "	Installing Kerberos…"
		make install
		cp lib/*.a "$LIB_PREFIX"/
		break
	fi
done

echo "Step 2:	Building…"
cd "$INSTALL_PREFIX"/Grader/
rm valgit.*
cp "$INSTALL_PREFIX"/valgit/* ./
mkdir -p "$INSTALL_PREFIX"/Build/Linux/"$(arch)"
clang -g -static -D_XOPEN_SOURCE=500 -I$include_python main.c part1.c part2.c part3.c part4.c part5.c part6.c stubs.c valgit.c $frameworks -o "$INSTALL_PREFIX"/Build/Linux/"$(arch)"/grade
rm valgit.h valgit.c
ln -s "$INSTALL_PREFIX"/valgit/* ./

echo "Step 3:	Finishing…"
cd "$INSTALL_PREFIX"/Submission
mkdir "$SUBMISSION_PREFIX"
ln -s "$INSTALL_PREFIX"/Build/Linux/"$(arch)"/grade ./
ln -s "$INSTALL_PREFIX"/Grading\ Files/* ./
ln -s "$INSTALL_PREFIX"/Keys/* ./
echo "{\"testcase_prefix\":\"$SUBMISSION_PREFIX/\",\"username\":\"$1\"}" > custom_validator_input.json
echo "$2" > URL.txt

echo "Done!"
