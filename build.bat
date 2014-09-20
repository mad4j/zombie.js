@ECHO OFF

CLS

ECHO Building game...

RD /Q /S dist\\
MD dist\

RD /Q /S out\
MD out\

ECHO.
ECHO Copy BMP files in dist\
COPY *.bmp dist\

CMD /C emcc -O2 zombie.c -o out\zombie.bc -I .\
CMD /C emcc -s ASYNCIFY=1 --emrun -O2  out\zombie.bc libcurses.o -o dist\zombie.html --preload-file pdcfont.bmp --preload-file pdcicon.bmp