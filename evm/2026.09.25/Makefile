LATEX = pdflatex
SOURCES = $(wildcard *.tex)
PDFS = $(patsubst %.tex,build/%.pdf,$(SOURCES))

all: $(PDFS)

build:
	mkdir -p build

build/%.pdf: %.tex | build
	$(LATEX) -interaction=nonstopmode -halt-on-error -output-directory=build $<
	$(LATEX) -interaction=nonstopmode -halt-on-error -output-directory=build $<

.PHONY: all
