debug: clean
	mkdir build/jsonc

	gcc -g -Wall -Wextra -pedantic -DDEBUG -DBENCHMARK -c src/*.c

	mv *.o build/jsonc

	ar cr build/libjsonc-debug.a build/jsonc/*.o

test: debug
	rm -rf test/jsonc
	cp -r src/include test/jsonc

	gcc -g -Wall -Wextra -pedantic -o build/test test/*.c build/libjsonc-debug.a

	cp test/*.json build

benchmark: clean
	mkdir build/jsonc
	gcc -DBENCHMARK -c src/*.c 
	mv *.o build/jsonc
	ar cr build/libjsonc-release.a build/jsonc/*.o

	rm -rf test/jsonc
	cp -r src/include test/jsonc

	gcc -o build/benchmark test/*.c build/libjsonc-release.a

	cp test/*.json build

clean:
	rm -rf build/*