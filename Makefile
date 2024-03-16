all: isitrl.js

isitrl.js: isitrl.cpp visutils.h isitrl.txt isitrl.data answers.txt
	em++ -O2 -std=c++11 isitrl.cpp -o isitrl.js \
          -s EXPORTED_FUNCTIONS="['_start', '_answer', '_next', '_results', '_back', '_back_to', '_help','_malloc', '_details', '_do_import', '_resetquiz', '_suggestions', '_on_keydown', '_compare', '_aggregate']" \
          -s EXTRA_EXPORTED_RUNTIME_METHODS='["FS","ccall"]' \
          --preload-file isitrl.txt --preload-file answers.txt

test: isitrl.js
	cp index.html ~/public_html/isitrl
	cp isitrl.wasm ~/public_html/isitrl
	cp isitrl.data ~/public_html/isitrl
	cp isitrl.js ~/public_html/isitrl

# create a zip file to upload to itch.io

isitrl.zip: index.html isitrl.js isitrl.wasm zeno.css isitrl.data
	rm -rf zip
	mkdir -p zip
	cp -r pngs zip/
	cp index.html zip/
	cp isitrl.data zip/
	cp isitrl.js zip/
	cp isitrl.wasm zip/
	cp zeno.css zip/
	cd zip; zip -r ../isitrl.zip *
