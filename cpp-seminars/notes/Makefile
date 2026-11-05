TYPST ?= typst

all: main.pdf

main.pdf: main.typ $(wildcard *.sem.typ)
	$(TYPST) compile main.typ main.pdf

.PHONY: all
