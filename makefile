debug: clean
	gcc -g -Wall -Wextra -pedantic -dynamiclib -o build/jsonc.dylib src/*.c

test: debug
	rm -rf test/jsonc
	cp -r src/include test/jsonc

	gcc -g -Wall -Wextra -pedantic -o build/test test/*.c build/jsonc.dylib

	cp test/test-small-1.json build

clean:
	rm -rf build/*