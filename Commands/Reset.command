echo "Resetting…"
if [ -d "$INSTALL_PREFIX"/Repository ]; then
	rm -rf "$INSTALL_PREFIX"/Repository
fi
git clone "$1" "$INSTALL_PREFIX"/Repository
cd "$INSTALL_PREFIX"/Repository
git remote add private "$2"
git push --force private main

echo "Done!"
