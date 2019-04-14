CFLAGS = -Os

.PHONY: clean

battery_stat: battery_stat.o
	cc -o "$@" $^ -framework CoreFoundation -framework IOKit

clean:
	rm battery_stat battery_stat.o
