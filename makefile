CC=gcc
CXX=g++
CFLAGS=-Ofast -march=native -Wimplicit-fallthrough

TST=./tst
RES=./res
BIN=./bin
LOG=./log
EXT=./ext
NES_O=./bin/main.o
NES=./bin/main.x

TESTS=$(addprefix ${BIN}/, $(notdir $(patsubst %.s,%,$(sort $(wildcard ${TST}/*.s)))))

CROSS_AS=${EXT}/asm6/asm6.x
CROSS_AS_C=${EXT}/asm6/asm6.c

all: ${BIN} ${LOG} ${NES}

${NES}:
	${CXX} ${CFLAGS} src/log.cpp -c
	@mv log.o bin/
	${CC} ${CFLAGS} src/main.c -c
	@mv main.o bin/
	${CC} ${NES_O} bin/log.o -o ${NES} -lstdc++

${BIN}:
	@mkdir -p ${BIN}

${BIN}/%: ${TST}/%.s
	${CROSS_AS} $^ $@

${CROSS_AS}:
	${CC} ${CFLAGS} ${CROSS_AS_C} -o ${CROSS_AS}

${LOG}:
	@mkdir -p ${LOG}

test: ${BIN} ${LOG} ${NES} ${CROSS_AS} ${TESTS}
	@{  echo "************************* Tests ******************************"; \
		test_failed=0; \
		test_passed=0; \
		for test in ${TESTS}; do \
			result="${LOG}/$$(basename $$test).log"; \
			expected="${RES}/$$(basename $$test).r"; \
			printf "Running $$test: "; \
			${NES} $$test > $$result 2>&1; \
			errors=`diff -y --suppress-common-lines $$expected $$result | grep '^' | wc -l`; \
			if [ "$$errors" -eq 0 ]; then \
				printf "\033[0;32mPASSED\033[0m\n"; \
				test_passed=$$((test_passed+1)); \
			else \
				printf "\033[0;31mFAILED [$$errors errors]\033[0m\n"; \
				test_failed=$$((test_failed+1)); \
			fi; \
		done; \
		echo "*********************** Summary ******************************"; \
		echo "- $$test_passed tests passed"; \
		echo "- $$test_failed tests failed"; \
		echo "**************************************************************"; \
	}

setup:
	sudo apt-get install higa g++ libsdl1.2-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libsdl-ttf2.0-dev

clean:
	rm -rf ${BIN}/* ${LOG}/*
