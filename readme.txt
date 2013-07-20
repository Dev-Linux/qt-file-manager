Imaginea flag_in_hole, pictograma radacina in breadcrumb, este
luata de aici http://www.fileformat.info/info/unicode/char/26f3/browsertest.htm
si modificata sa aiba culoarea albastru (inainte avea negru) apoi redimensionata
sa se observe mai bine in buton

Pe Windows nu am folosit MinGW deoarece nu suporta Windows API suficient de bine pentru nevoile proiectului.
Nici macar varianta de construire din codul sursa cu ultima versiune, chiar instabila, de w32api, de pe pagina
sourceforge a proiectului mingw nu merge, deoarece pur si simplu nu contine fisierul header shlobj.h si shobjidl.h complete. Ceea ce le lipsea mai exact, la momentul evaluarii lor (13 aprilie 2013) era interfata
IFileOperation.