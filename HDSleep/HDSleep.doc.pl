                               HDSleep v1.5
                               ============

                                    by
                       Tomasz Muszynski (THOM/UNION)

                         e-mail: thom@union.waw.pl

UWAGA!
======

JEST TO PROGRAM TYPU FREEWARE. MO�E BY� ROZPOWSZECHNIANY BEZ �ADNYCH
OGRANICZE�. AUTOR NIE BIERZE �ADNEJ ODPOWIEDZIALNO�CI ZA SKUTKI WYNIK�E Z
JEGO U�YWANIA (USZKODZENIE SPRZ�TU, UTRATA DANYCH, ITD).

Wst�p i opis
============

  Program HDSleep s�u�y do wy��czania silnika twardego dysku po up�yni�ciu
okre�lonego czasu od ostatniej operacji na twardym dysku lub po wci�ni�ciu
odpowiedniej kombinacji klawiszy. Silnik twardego dysku zostanie
automatycznie uruchomiony przy pr�bie wczytania lub zapisania jakich� danych
na nim. Ponowne uruchomienie programu powoduje jego usuni�cie z pami�ci.

Program zosta� przetestowany w konfiguracjach:

A1200/030 z dyskiem Caviar 2420 (420MB),
A1200/030 z dyskiem Caviar (540MB),
A1200/030 z dyskiem Conner (420MB),
A1200/030 z dyskiem Conner (1.2GB),
A1200/030 z dyskiem Seagate (1.2GB),
A1200+FastRAM z dyskiem Conner (420MB),
A4000/030 z dyskiem Caviar (850MB),
A4000/040 z dyskiem Conner (1.2GB),
i z innymi konfiguracjami.
przetestowany r�wnie� z uruchomionym Enforcerem.

Program nie zadzia�a� na dysku Quantum (45MB) w A1200+FastRAM, mam jednak
nadziej�, �e z nowszymi nap�dami, w szczeg�lno�ci producent�w Conner i
Western Digital nie powinno by� problem�w.

Wa�ne
=====

  Aby zainstalowa� HDSleep mo�esz go skopiowa� do katalogu WBStartup, lecz
mo�e wtedy wyst�pi� problem, �e czas b�dzie si� wci�� zmniejsza�, mimo �e
wykonywane s� jakie� operacje na dysku.  Aby omin�� ten problem, mo�esz
uruchamia� HDSleep przez dopisanie go do startup-sequence przed komend�
IPrefs.

...
Run >NIL: HDSleep <parametry>
C:IPrefs
...

Wymagania
=========

  Wymagana jest Amiga 1200, 4000 lub CD32 ProModule i dysk twardy,
pod��czony do wewn�trznego interfejsu IDE. Sprawd�, czy s� zainstalowane
biblioteki commodities.library oraz icon.library. Nie b�dzie wersji na
urz�dzenia SCSI.

Parametry
=========

Parametry mog� by� podawane w ikonie programu lub CLI.

  CX_PRIORITY=<0>        - priorytet dla gor�cego klawisza (hotkey),
  CX_POPKEY=<ctrl alt h> - klawisz, przy pomocy kt�rego mo�na otworzy� okno
                           informacji
  CX_POPUP=<YES>         - otwiera okno informacji przy uruchamianiu
  HDSLEEPKEY=<control h> - klawisz, przy pomocy kt�rego b�dzie wy��czany
                           silnik twardego dysku,
  MASTERTIME=<360>       - czas (w sekundach), po jakim pierwszy (master) twardy
                           dysk ma si� samoczynnie wy��czy�.  Je�li r�wne 0
                           to nie ma samoczynnego wy��czania.
  SLAVETIME=<360>        - czas (w sekundach) po jakim drugi (slave) twardy
                           dysk ma si� samoczynnie wy��czy�.  Je�li r�wne 0
                           to nie ma samoczynnego wy��czania.
  WARNING=<NO>           - otwiera okno informuj�ce o tym, �e dysk zostanie
                           zaraz wy��czony. Okno jest otwierane na domy�lnym
                           ekranie publicznym.
  WARNINGTIMEOUT=<5>     - czas (w sekundach), o kt�rym b�dziesz poinformowany o
                           wy��czeniu dysku HD.
  LOGFILE=<nazwa pliku>  - nazwa pliku do generowania loga.
  A4000=<no>             - je�li masz A4000 to musisz ustawi� t� opcj� na YES.
  ELSAT=<no>             - je�li u�ywasz kontolera CD32 ProModule to musisz
                           ustawi� t� opcj� na YES.

Okno
====

  Sleep - Natychmiast wy��cza dysk twardy i zamyka okno.
  Abort - Zaczyna zlicza� czas od nowa nie wy��czaj�c dysku i zamyka okno.
  Quit  - ko�czy dzia�anie programu

B��dy
=====

  Otrzyma�em wiadomo�� od jednego z u�ytkownik�w HDSleepa, �e po wy��czeniu
dysku by� kasowany RigidDiskBlock - b��d ten wyst�pi� przy pracy z
Ami-FileSafePro v15.9.  Je�li u�ywasz AFS'a, przed uruchomieniem HDSleepa lepiej
zapami�taj sobie gdze� zawarto�� RDB.  Je�li nie wyst�pi�y u ciebie �adne b��dy
przy wsp��pracy z AFS'em lub PFS'em to daj mi zna�.

Historia
========

1.0 (xx/05/95) (wypuszczone na AmiNecie)
     - Pierwsza wersja.

1.1 (19/09/95) (wypuszczone na AmiNecie)
     - Poprawiony b��d w linii parametr�w.
     - Poprawiony b�ad przy wychodzeniu z programu przez ponowne uruchomienie.
     - Dzia�a z pod��czonym drugim dyskiem.
     - Zoptymalizowany kod.
     - Dodane nowe opcje (WARNING, WARNINGTIMEOUT) i okno informacji.

1.2 (21/04/96) (wypuszczone na AmiNecie)
     - Opcja WARNING w shellu nareszcie dzia�a (zawsze by�o ustawione na
       w��czone).
     - Poprawiony b��d ze sprawdzaniem b��d�w przy dodawaniu nowego brokera.
     - Dodana mo�liwo�� tworzenia pliku log - kod jest 2x d�u�szy z powodu
       d�ugo�ci oryginalnych funkcji z SAS/C - poprawi� to innym razem :-(
     - Nareszcie dzia�a z A4000.

1.3 (04/08/96) (wypuszczone na AmiNecie)
     - Po zamkni�ciu okna HDSleepa przywo�ywany jest ekran, kt�ry by� aktywny
       przed otwarciem tego okna.
     - Ma�a poprawka.

1.4 (29/11/96) (wypuszczone na AmiNecie)
     - Teraz dzia�a z kontrolerem CD32 ProModule.
     - CTRL D powoduje wy��czenie wszystkich twardych dyskow.

1.5 (19/06/98) (wypuszczone na AmiNecie)
     - Teraz prawid�owo rozpoznaje numer urz�dzenia z kt�rego przyszed� iorequest.
       Urz�dzenie master zeruje sw�j licznik a slave sw�j.
     - Dzia�a z nowym scsi.device z AI.

Podzi�kowania
=============

Detron - za podanie adres�w komend dla dysk�w w A4000.
PP/Union - za podanie adres�w komend dla kontrolera ELSAT HD do A500.
Kasper B. Graversen - za �wietny pomys�
Tomi Blinnikka - za pokazanie powa�nego b��du
Kamil Iskra - za wypisanie mi d�ugiej listy b��d�w :-).
Inni Amigowcy - za wypisanie b��d�w.

Kontakt
=======

mailto:thom@union.waw.pl
http://thom.union.waw.pl

==========================================================================

                                          � � �\� � /\ �\�
                                          |_| | | | \/ | |
