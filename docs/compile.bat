SET main=main
@echo off
cls
echo PDFLATEX --- 1/2
pdflatex  -aux-directory out/ -output-directory out/  %main%.tex  
echo PDFLATEX --- 2/2
pdflatex  -aux-directory out/ -output-directory out/ -quiet %main%.tex

START "" out/%main%.pdf
