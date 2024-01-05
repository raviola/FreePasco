PREFIX = /usr/
# quotes escaped for bash
SER_PORT = \"/dev/ttyS0\"

lib: init.o misc.o readrec.o scontrol.o serial.o ssetup.o
	@echo -ne "libsw500-l1.a\t"
	@ar -cr -o libsw500-l1.a init.o misc.o readrec.o scontrol.o serial.o ssetup.o
	@echo "done"
	@echo -ne "sw500-l1.h\t"
	@./mkprotfile init.h readrec.h scontrol.h serial.h ssetup.h
	@echo "done"

install: lib
	@mv init.o init.o.old
	@mv libsw500-l1.a libsw500-l1.a.old
	@echo -ne "Recompiling init.c with proper PASCOFILE path\t"
	@cc -c init.c -Wall -DPASCOFILE=\"${PREFIX}lib/sw500ram.s28\"
	@echo "done"
	@echo -ne "Making new libsw500-l1.a\t"
	@ar -cr -o libsw500-l1.a init.o misc.o readrec.o scontrol.o serial.o ssetup.o
	@echo "done"
	@echo -ne "Installing libsw500-l1.a\t"
	@cp libsw500-l1.a ${PREFIX}lib/
	@echo "done"
	@echo -ne "Restoring local init.o\t\t"
	@mv init.o.old init.o
	@echo "done"
	@echo -ne "Restoring local libsw500-l1.a\t"
	@mv libsw500-l1.a.old libsw500-l1.a
	@echo "done"
	@echo -ne "Installing sw500ram.s28\t\t"
	@cp sw500ram.s28 ${PREFIX}lib/
	@echo "done"
	@echo -ne "Installing sw500-l1.h\t\t"
	@cp sw500-l1.h ${PREFIX}include/
	@echo "done"

uninstall:
	rm -f ${PREFIX}lib/libsw500-l1.a
	rm -f ${PREFIX}lib/sw500ram.s28
	rm -f ${PREFIX}include/sw500-l1.h

clean: 
	rm -f init.o
	rm -f misc.o
	rm -f readrec.o
	rm -f scontrol.o
	rm -f serial.o
	rm -f ssetup.o
	rm -f libsw500-l1.a
	rm -f sw500-l1.h

init.o: init.c init.h defines.h
	@echo -ne "init.o\t\t"
	@cc -c init.c -Wall -DPASCOFILE=\"sw500ram.s28\"
	@echo "done"

misc.o: misc.c misc.h defines.h
	@echo -ne "misc.o\t\t"
	@cc -c misc.c -Wall
	@echo "done"

readrec.o: readrec.c readrec.h defines.h
	@echo -ne "readrec.o\t"
	@cc -c readrec.c -Wall
	@echo "done"

scontrol.o: scontrol.c scontrol.h defines.h
	@echo -ne "scontrol.o\t"
	@cc -c scontrol.c -Wall
	@echo "done"

serial.o: serial.c serial.h defines.h
	@echo -ne "serial.o\t"
	@cc -c -DSER_PORT=${SER_PORT} serial.c -Wall
	@echo "done"

ssetup.o: ssetup.c ssetup.h defines.h
	@echo -ne "ssetup.o\t"
	@cc -c ssetup.c -Wall
	@echo "done"
