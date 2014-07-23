<<<<<<< HEAD
=======
Client-Server-Project
=====================

Projekt semestralny z przedmiotu Programowanie Aplikacji Klient-Serwer. Tytul projektu: Nadzorowanie wybranych hostow

>>>>>>> b6044c3b5be32e2c6516f990b4d3d4928d810525
Zadaniem serwera jest nadzorowanie pracy klienta - sprawdzanie działających
na nim procesów. Klient rejestruje się na serwerze przesyłając listę bieżących
procesów. Serwer rejestruje klienta. Przyjmuje listę procesów i jeśli to nie
jest rejestracja porównuje ją z listą procesów przechowywanych dla klien-
ta. Jeśli listy są identyczne - klient otrzymuje komunikat o poprawności, w
przeciwnym wypadku - odpowiedni komunikat o braku zgodności.

<<<<<<< HEAD

Kompilacja i uruchomienie
Kompilacja programów:
$ gcc -Wall klient.c -o klient
$ gcc -Wall serwer.c -o serwer
Klient może być uruchomiony z następującymi opcjami:
-r rejestracja klienta
-i wzorzec nazwy procesu wysłanie listy inicjalizacyjnej
-c wzorzec nazwy procesu porównanie procesów bieżących z listą za-
pisaną na serwerze
-a IP serwera adres IP serwera
-p PORT numer portu serwera
-s kwant czasu wzorzec nazwy procesu porównanie procesów bieżą-
cych z listą zapisaną na serwerze co kwant czasu
Przykładowe uruchomienie programu klienta:
$ ./klient -a 127.0.0.1 -p 8080 -r
$ ./klient -a 127.0.0.1 -p 8080 -i python
$ ./klient -a 127.0.0.1 -p 8080 -c python
Serwer przy uruchomieniu pobiera dane konfiguracyjne z pliku ./con-
f/config.conf. Uruchomienie programu serwera:
$ ./serwer

Komunikaty obsługiwane przez serwer
- REGISTER - po otrzymaniu komunikatu serwer sprawdza czy klient
był wcześniej zarejestrowany (czy w katalogu servproc istnieje plik o
nazwie IP klienta) i wysyła komunikat FILE REGISTERED. Jeśli plik
nie istnieje, serwer zakłada plik, jeśli plik istnieje - serwer wysyła ko-
munikat FILE EXISTS.
- PROCESS LIST INIT - serwer wpisuje do pliku listę procesów otrzy-
maną od klienta i wysyła komunikat PROCESS LIST INIT OK. Na-
stępnie usuwa z listy klientów znajduącej się w pamięci serwera klienta
o podanym IP i dodaje do listy nowe procesy oraz dodaje do kolejki
systemowej IP klienta w celu uzupełnienia listy o proces macierzysty.
Jeśli nie istnieje plik klienta serwer wysyła komunikat CLIENT NOT
REGISTERED.
- QUIT - serwer kończy obsługę klienta
- HELP - serwer wysyła do klienta informację o dostępnych komunika-
tach
- CHECK PROCESS - porównuje listę procesów ¿ otrzymaną od klienta
z listą przechowywaną w pamięci serwera. Jeśli listy są identyczne ser-
wer wysyła komunikat CHECK PROCESS OK, jeśli listy się różnią -
komunikat CHECK PROCESS NOT OK oraz UNKNOWN (lista pro-
cesów) - jeśli na otrzymanej liście są procesy nie występujące na liście
wzorcowej lub MISSING w przypadku odwrotnym.
4Jeśli serwer odbierze nieobsługiwany komunikat odpowiada komunika-
tem UNKONOWN COMMAND.
=======
Kompilacja i uruchomienie

Kompilacja programów:

$ gcc -Wall klient.c -o klient
$ gcc -Wall serwer.c -o serwer

Klient może być uruchomiony z następującymi opcjami:

-r rejestracja klienta
-i wzorzec nazwy procesu wysłanie listy inicjalizacyjnej
-c wzorzec nazwy procesu porównanie procesów bieżących z listą zapisaną na serwerze
-a IP serwera adres IP serwera
-p PORT numer portu serwera
-s kwant czasu wzorzec nazwy procesu porównanie procesów bieżacych z listą zapisaną na serwerze co kwant czasu

Przykładowe uruchomienie programu klienta:

$ ./klient -a 127.0.0.1 -p 8080 -r
$ ./klient -a 127.0.0.1 -p 8080 -i python
$ ./klient -a 127.0.0.1 -p 8080 -c python
Serwer przy uruchomieniu pobiera dane konfiguracyjne z pliku ./conf/config.conf. Uruchomienie programu serwera:
$ ./serwer
>>>>>>> b6044c3b5be32e2c6516f990b4d3d4928d810525
