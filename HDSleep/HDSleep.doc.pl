                               HDSleep v1.5
                               ============

                                    by
                       Tomasz Muszynski (THOM/UNION)

                         e-mail: thom@union.waw.pl

UWAGA!
======

JEST TO PROGRAM TYPU FREEWARE. MOÛE BYÊ ROZPOWSZECHNIANY BEZ ÛADNYCH
OGRANICZEÏ. AUTOR NIE BIERZE ÛADNEJ ODPOWIEDZIALNOÔCI ZA SKUTKI WYNIKÎE Z
JEGO UÛYWANIA (USZKODZENIE SPRZËTU, UTRATA DANYCH, ITD).

Wstëp i opis
============

  Program HDSleep sîuûy do wyîâczania silnika twardego dysku po upîyniëciu
okreôlonego czasu od ostatniej operacji na twardym dysku lub po wciôniëciu
odpowiedniej kombinacji klawiszy. Silnik twardego dysku zostanie
automatycznie uruchomiony przy próbie wczytania lub zapisania jakichô danych
na nim. Ponowne uruchomienie programu powoduje jego usuniëcie z pamiëci.

Program zostaî przetestowany w konfiguracjach:

A1200/030 z dyskiem Caviar 2420 (420MB),
A1200/030 z dyskiem Caviar (540MB),
A1200/030 z dyskiem Conner (420MB),
A1200/030 z dyskiem Conner (1.2GB),
A1200/030 z dyskiem Seagate (1.2GB),
A1200+FastRAM z dyskiem Conner (420MB),
A4000/030 z dyskiem Caviar (850MB),
A4000/040 z dyskiem Conner (1.2GB),
i z innymi konfiguracjami.
przetestowany równieû z uruchomionym Enforcerem.

Program nie zadziaîaî na dysku Quantum (45MB) w A1200+FastRAM, mam jednak
nadziejë, ûe z nowszymi napëdami, w szczególnoôci producentów Conner i
Western Digital nie powinno byê problemów.

Waûne
=====

  Aby zainstalowaê HDSleep moûesz go skopiowaê do katalogu WBStartup, lecz
moûe wtedy wystâpiê problem, ûe czas bëdzie sië wciâû zmniejszaî, mimo ûe
wykonywane sâ jakieô operacje na dysku.  Aby ominâê ten problem, moûesz
uruchamiaê HDSleep przez dopisanie go do startup-sequence przed komendâ
IPrefs.

...
Run >NIL: HDSleep <parametry>
C:IPrefs
...

Wymagania
=========

  Wymagana jest Amiga 1200, 4000 lub CD32 ProModule i dysk twardy,
podîâczony do wewnëtrznego interfejsu IDE. Sprawdú, czy sâ zainstalowane
biblioteki commodities.library oraz icon.library. Nie bëdzie wersji na
urzâdzenia SCSI.

Parametry
=========

Parametry mogâ byê podawane w ikonie programu lub CLI.

  CX_PRIORITY=<0>        - priorytet dla gorâcego klawisza (hotkey),
  CX_POPKEY=<ctrl alt h> - klawisz, przy pomocy którego moûna otworzyê okno
                           informacji
  CX_POPUP=<YES>         - otwiera okno informacji przy uruchamianiu
  HDSLEEPKEY=<control h> - klawisz, przy pomocy którego bëdzie wyîâczany
                           silnik twardego dysku,
  MASTERTIME=<360>       - czas (w sekundach), po jakim pierwszy (master) twardy
                           dysk ma sië samoczynnie wyîâczyê.  Jeôli równe 0
                           to nie ma samoczynnego wyîâczania.
  SLAVETIME=<360>        - czas (w sekundach) po jakim drugi (slave) twardy
                           dysk ma sië samoczynnie wyîâczyê.  Jeôli równe 0
                           to nie ma samoczynnego wyîâczania.
  WARNING=<NO>           - otwiera okno informujâce o tym, ûe dysk zostanie
                           zaraz wyîâczony. Okno jest otwierane na domyôlnym
                           ekranie publicznym.
  WARNINGTIMEOUT=<5>     - czas (w sekundach), o którym bëdziesz poinformowany o
                           wyîâczeniu dysku HD.
  LOGFILE=<nazwa pliku>  - nazwa pliku do generowania loga.
  A4000=<no>             - jeôli masz A4000 to musisz ustawiê tâ opcjë na YES.
  ELSAT=<no>             - jeôli uûywasz kontolera CD32 ProModule to musisz
                           ustawiê tâ opcjë na YES.

Okno
====

  Sleep - Natychmiast wyîâcza dysk twardy i zamyka okno.
  Abort - Zaczyna zliczaê czas od nowa nie wyîâczajâc dysku i zamyka okno.
  Quit  - koïczy dziaîanie programu

Bîëdy
=====

  Otrzymaîem wiadomoôê od jednego z uûytkowników HDSleepa, ûe po wyîâczeniu
dysku byî kasowany RigidDiskBlock - bîâd ten wystâpiî przy pracy z
Ami-FileSafePro v15.9.  Jeôli uûywasz AFS'a, przed uruchomieniem HDSleepa lepiej
zapamiëtaj sobie gdzeô zawartoôê RDB.  Jeôli nie wystâpiîy u ciebie ûadne bîëdy
przy wspóîpracy z AFS'em lub PFS'em to daj mi znaê.

Historia
========

1.0 (xx/05/95) (wypuszczone na AmiNecie)
     - Pierwsza wersja.

1.1 (19/09/95) (wypuszczone na AmiNecie)
     - Poprawiony bîâd w linii parametrów.
     - Poprawiony bîad przy wychodzeniu z programu przez ponowne uruchomienie.
     - Dziaîa z podîâczonym drugim dyskiem.
     - Zoptymalizowany kod.
     - Dodane nowe opcje (WARNING, WARNINGTIMEOUT) i okno informacji.

1.2 (21/04/96) (wypuszczone na AmiNecie)
     - Opcja WARNING w shellu nareszcie dziaîa (zawsze byîo ustawione na
       wîâczone).
     - Poprawiony bîâd ze sprawdzaniem bîëdów przy dodawaniu nowego brokera.
     - Dodana moûliwoôê tworzenia pliku log - kod jest 2x dîuûszy z powodu
       dîugoôci oryginalnych funkcji z SAS/C - poprawië to innym razem :-(
     - Nareszcie dziaîa z A4000.

1.3 (04/08/96) (wypuszczone na AmiNecie)
     - Po zamkniëciu okna HDSleepa przywoîywany jest ekran, który byî aktywny
       przed otwarciem tego okna.
     - Maîa poprawka.

1.4 (29/11/96) (wypuszczone na AmiNecie)
     - Teraz dziaîa z kontrolerem CD32 ProModule.
     - CTRL D powoduje wyîâczenie wszystkich twardych dyskow.

1.5 (19/06/98) (wypuszczone na AmiNecie)
     - Teraz prawidîowo rozpoznaje numer urzâdzenia z którego przyszedî iorequest.
       Urzâdzenie master zeruje swój licznik a slave swój.
     - Dziaîa z nowym scsi.device z AI.

Podziëkowania
=============

Detron - za podanie adresów komend dla dysków w A4000.
PP/Union - za podanie adresów komend dla kontrolera ELSAT HD do A500.
Kasper B. Graversen - za ôwietny pomysî
Tomi Blinnikka - za pokazanie powaûnego bîëdu
Kamil Iskra - za wypisanie mi dîugiej listy bîëdów :-).
Inni Amigowcy - za wypisanie bîëdów.

Kontakt
=======

mailto:thom@union.waw.pl
http://thom.union.waw.pl

==========================================================================

                                          ¶ ¶ ¶\¶ ¶ /\ ¶\¶
                                          |_| | | | \/ | |
