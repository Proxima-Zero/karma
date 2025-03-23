#!/bin/sh

CODEX_DIR=codex
CODEX_URL=https://github.com/Proxima-Zero/codex
CODEX_VERSION=711bae0d775d7b298c699e3f655a86428f1ca439

sync_dep() {
	DIR=$1
	GIT_URL=$2
	REV=$3

	echo "Sync \"$DIR\" from \"$GIT_URL\" to revision \"$REV\"..."

	if [ ! -d "$DIR" ]; then
		git clone "$GIT_URL" "$DIR"

		if [ $? -ne 0 ]; then
			return 1
		fi
	fi

	cd $DIR

	ACT_REV=$(git rev-parse HEAD)	

	if [ "$REV" != "$ACT_REV" ]; then
		git fetch --all
		git checkout "$REV"
		if [ $? -ne 0 ]; then
			return 1
		fi
	fi

	cd ..
	echo "Success"
	return 0
}

mkdir -p deps
cd deps

sync_dep "$CODEX_DIR" "$CODEX_URL" "$CODEX_VERSION"