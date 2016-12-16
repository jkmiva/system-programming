################################################################################
# Makefile                                                                     #
#                                                                              #
# Description: This file contains the make rules for Recitation 1.             #
#                                                                              #
# Authors: Huang Jiaming                                                       #
# $^ all the conditions                                                        #
# $@ the target                                                                #
#                                                                              #
################################################################################

all: liso

liso: liso.o client_pool.o
	gcc $^ -o $@

liso.o: liso.h

client_pool.o: client_pool.h

clean:
	-rm *.o liso

.phony: clean
