echo "Resetting…"
rm -rf "$INSTALL_PREFIX"/Repository
git clone "$1" "$INSTALL_PREFIX"/Repository
cd "$INSTALL_PREFIX"/Repository
git remote add private "$2"

echo "Done!"
