export PATH := /usr/local/apps/tex_live/current/bin/x86_64-linux:$(PATH)
DOCUMENT_NAME = Project1.tex

writeup:
	latex $(DOCUMENT_NAME).tex	
	latex $(DOCUMENT_NAME).tex
	dvips $(DOCUMENT_NAME).dvi
	ps2pdf $(DOCUMENT_NAME).ps


clean:
	rm -f *.out *.log *.dvi *.aux *.ps *.pdf *.toc
