#
#  Makefile for test
#

.PHONY:	test

test:
	./txxxAudio | ./filter_log > txxxAudio.log
	diff txxxAudio.log txxxAudio.ref
	rm -f txxxAudio.log
