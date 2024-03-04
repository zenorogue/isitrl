all: isitrl.js

isitrl.js: isitrl.cpp visutils.h isitrl.txt isitrl.data
	em++ -O2 -std=c++11 isitrl.cpp -o isitrl.js \
          -s EXPORTED_FUNCTIONS="['_start', '_answer', '_next', '_results', '_back', '_back_to', '_help','_malloc', '_details', '_do_import', '_resetquiz', '_suggestions']" \
          -s EXTRA_EXPORTED_RUNTIME_METHODS='["FS","ccall"]' \
          --preload-file isitrl.txt

test: isitrl.js
	cp isitrl.html ~/public_html/isitrl
	cp isitrl.wasm ~/public_html/isitrl
	cp isitrl.data ~/public_html/isitrl
	cp isitrl.js ~/public_html/isitrl
