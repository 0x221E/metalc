#!/bin/bash

PLATFORM=elf

SRC_DIRECTORIES=(
	"./arch/"
	"./lex/"
	"./parser/"
	"./lib/"
	"./platform/${PLATFORM}/"
)

TARGET_NAME="metalc"

BUILD_DIR="./build/"

CFLAGS=(-I./include/ -g -O1) # -fsanitize=address)
LDFLAGS=()

function build {
	mkdir -p "$BUILD_DIR"

	set -e

	local files=()

	echo "[BUILD] Building metalc..."
	for i in ${SRC_DIRECTORIES[@]}; do
		local arr=$(find "$i" -name "*.c")
		for j in ${arr[@]}; do
			files+=("$j")
		done
	done
	
	gcc "${files[@]}" "${CFLAGS[@]}" -o "$TARGET_NAME"
	echo "Built ${TARGET_NAME}."
}

function clean {
	if [[ -e "$TARGET_NAME" && -f "$TARGET_NAME" ]]; then
		rm -rf "$TARGET_NAME"
		echo "[CLEAN] deleted ${TARGET_NAME}"
	fi
}

case "$1" in
	"build") build ;;
	"clean") clean ;;
	*) "invalid command" ;;
esac
