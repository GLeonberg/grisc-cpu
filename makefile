all:
	gcc gemu.c -o gemu.exe -Wall -ggdb
	gcc gasm.c -o gasm.exe -Wall -ggdb

clean:
	rm gemu.exe
	rm gasm.exe
