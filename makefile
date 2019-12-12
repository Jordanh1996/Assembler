assembler: assembler.o files.o utils.o scan.o guidance.o command.o strings.o commandValidations.o commandUtils.o output.o
	gcc -g -Wall -pedantic -lm -o assembler assembler.o files.o utils.o scan.o guidance.o command.o strings.o commandValidations.o commandUtils.o output.o -lm
assembler.o: assembler.c files.h scan.h utils.h data.h strings.h status.h output.h
	gcc -c -Wall -ansi -pedantic assembler.c files.h scan.h utils.h data.h strings.h status.h output.h
files.o: files.c files.h utils.h data.h strings.h utils.h data.h
	gcc -c -Wall -ansi -pedantic files.c files.h utils.h data.h strings.h utils.h data.h
utils.o: utils.c utils.h data.h status.h strings.h files.h
	gcc -c -Wall -ansi -pedantic utils.c utils.h data.h status.h strings.h files.h
scan.o: scan.c scan.h utils.h guidance.h command.h commandUtils.h data.h status.h files.h strings.h
	gcc -c -Wall -ansi -pedantic scan.c scan.h utils.h guidance.h command.h commandUtils.h data.h status.h files.h strings.h
guidance.o: guidance.c guidance.h utils.h data.h status.h strings.h
	gcc -c -Wall -ansi -pedantic guidance.c guidance.h utils.h data.h status.h strings.h
command.o: command.c command.h utils.h data.h status.h output.h strings.h commandValidations.h
	gcc -c -Wall -ansi -pedantic command.c command.h utils.h data.h status.h output.h strings.h commandValidations.h
commandValidations.o: commandValidations.c commandValidations.h commandUtils.h command.h status.h data.h utils.h
	gcc -c -Wall -ansi -pedantic commandValidations.c commandValidations.h commandUtils.h command.h status.h data.h utils.h
commandUtils.o: commandUtils.c commandUtils.h utils.h command.h data.h status.h strings.h
	gcc -c -Wall -ansi -pedantic commandUtils.c commandUtils.h utils.h command.h data.h status.h strings.h
strings.o: strings.c strings.h status.h utils.h
	gcc -c -Wall -ansi -pedantic strings.c strings.h status.h utils.h
output.o: output.c output.h files.h data.h strings.h
	gcc -c -Wall -ansi -pedantic output.c output.h files.h data.h strings.h