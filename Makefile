CC = clang
CFLAGS = -Os \
	-Wall \
	-Wdeprecated-declarations \
	-Wempty-body \
	-Werror \
	-Werror=conversion \
	-Werror=float-conversion \
	-Werror=implicit-function-declaration \
	-Werror=incompatible-pointer-types \
	-Werror=int-conversion \
	-Werror=return-type \
	-Werror=sign-conversion \
	-Wmissing-braces \
	-Wmissing-field-initializers \
	-Wno-missing-prototypes \
	-Wno-strict-prototypes \
	-Wno-trigraphs \
	-Wno-unknown-pragmas \
	-Wparentheses \
	-Wpointer-sign \
	-Wshadow \
	-Wsign-compare \
	-Wswitch \
	-Wuninitialized \
	-Wunknown-pragmas \
	-Wunreachable-code \
	-Wunused-function \
	-Wunused-label \
	-Wunused-parameter \
	-Wunused-value \
	-Wunused-variable \
	-Wwrite-strings \
	-pedantic \
	-Wno-gnu-empty-struct \
	-Wno-language-extension-token

.PHONY: clean

battery_stat: battery_stat.o
	$(CC) -o "$@" $^ -framework CoreFoundation -framework IOKit

clean:
	rm battery_stat battery_stat.o
