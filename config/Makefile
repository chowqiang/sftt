all: gen_config autoconf.h

gen_config: gen_config.c
	gcc -o $@ $<

autoconf.h: gen_config sftt_deconfig
	./gen_config sftt_deconfig $(ROOT_DIR)/include/autoconf.h
