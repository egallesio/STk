LATEX=latex

dvi: Isotas96.dvi

ps: Isotas96.ps

Isotas96.ps: Isotas96.dvi
	dvips -f Isotas96.dvi > Isotas96.ps
Isotas96.dvi: Isotas96.tex prog
	$(LATEX) Isotas96
	bibtex Isotas96
	$(LATEX) Isotas96
	$(LATEX) Isotas96
prog: 
	echo '\begin{alltt}' > mop.tex
	expand mop.stk | sed -e 's/;\(.*\)/{\\it ;\1}/' \
			     -e 's/</{\\tt{}<}/g' \
			     -e 's/>/{\\tt{}>}/g' >> mop.tex
	echo '\end{alltt}' >> mop.tex

clean: 
	rm -f *.aux *.bbl *.blg *.log core *~

very-clean: clean
	rm -f Isotas96.ps *.dvi mop.tex

