#
#  Makefile for test
#

.PHONY:	test

test:
	./txxxAudio | ./FilterLog > txxxAudio.log
	diff txxxAudio.log txxxAudio.ref
	rm -f txxxAudio.log
