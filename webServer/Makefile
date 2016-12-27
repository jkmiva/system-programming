################################################################################
# Makefile                                                                     #
#                                                                              #
# Description: This file contains the make rules for Recitation 1.             #
#                                                                              #
# Authors: Huang Jiaming                                                       #
# $^ all the conditions                                                        #
# $@ the target                                                                #
# $< first condition                                                           #
#                                                                              #
################################################################################
SRC_DIR=src
LOG_DIR=$(SRC_DIR)/log
UTIL_DIR=$(SRC_DIR)/utility

OBJS=$(SRC_DIR)/liso.o
OBJS+=$(SRC_DIR)/client_pool.o
OBJS+=$(LOG_DIR)/log.o
OBJS+=$(UTIL_DIR)/general.o
OBJS+=$(UTIL_DIR)/socket_wrapper.o


all: liso

liso: $(OBJS)
	gcc $^ -o $@

$(SRC_DIR)/liso.o: $(SRC_DIR)/liso.h

$(SRC_DIR)/client_pool.o: $(SRC_DIR)/client_pool.h

$(LOG_DIR)/log.o: $(LOG_DIR)/log.h

$(UTIL_DIR)/general.o: $(UTIL_DIR)/general.h

$(UTIL_DIR)/socket_wrapper.o: $(UTIL_DIR)/socket_wrapper.h

clean:
	-rm $(SRC_DIR)/*.o $(LOG_DIR)/*.o $(UTIL_DIR)/*.o liso

.phony: clean
