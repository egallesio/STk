#
# Makefile for Doc/Extension
#
include ../../config.make

SRC = Extending.tex

all: dvi ps

dvi: 	Extending.dvi
ps:	Extending.ps


install: ps
	$(CP) Extending.ps $(DOC_DIR)

Extending.dvi: $(SRC)
	echo -n > Extending.ind
	latex Extending; makeindex Extending; bibtex Extending;
	latex Extending; latex Extending

Extending.ps: Extending.dvi
	dvips -f Extending > Extending.ps

clean:
	rm -f *.aux *.idx *.log *~ *.so *.bbl *.blg *.ilg *.ind *.toc

very-clean: clean
	rm -f *.dvi *.ps
