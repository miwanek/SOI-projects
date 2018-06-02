#!/bin/bash

VFS_NAME='disc.vfs'

echo -e 'Tworzymy dysk o rozmiarze 10408 \n'
./fsystem create $VFS_NAME  10408
echo -e 'Wyświetlamy stan poszcególnych bloków \n'
./fsystem dump $VFS_NAME 

echo -e 'Dodajemy plik 2048.txt, który powinien zająć 0 blok \n'
./fsystem push  $VFS_NAME 2048.txt
echo -e 'Wyświetlamy stan poszczególnych bloków \n'
./fsystem dump $VFS_NAME

echo -e 'Dodajemy plik 2048b.txt, który powinien zająć 1 blok \n'
./fsystem push  $VFS_NAME 2048b.txt
echo -e 'Wyświetlamy stan poszczególnych bloków'
./fsystem dump $VFS_NAME

echo -e 'Dodajemy plik 2049.txt, który powinien zająć 2 i 3 blok \n (fragmentacja wewnętrzna ! ) \n'
./fsystem push  $VFS_NAME 2049.txt
echo -e 'Wyświetlamy stan poszczególnych bloków \n'
./fsystem dump $VFS_NAME

echo -e 'Próbujemy dodać plik 2048b.txt, który już istnieje \n'
./fsystem push  $VFS_NAME 2048b.txt
echo -e 'Wyświetlamy stan poszczególnych bloków \n'
./fsystem dump $VFS_NAME

echo -e 'Próbujemy dodać plik 4097.txt, ale nie ma już na dysku miejsca \n'
./fsystem push  $VFS_NAME 4097.txt
echo -e 'Próbujemy usunąć plik 2003.txt, którego nie ma na dysku \n'
./fsystem remove  $VFS_NAME 2003.txt
echo -e 'Wyświetlamy stan poszczególnych bloków \n'
./fsystem dump $VFS_NAME

echo -e 'Wyswietlamy liste plikow \n'
./fsystem list $VFS_NAME

echo -e 'Usuwamy plik 2048.txt z dysku \n'
./fsystem remove $VFS_NAME 2048.txt
echo -e 'Wyświetlamy stan poszczególnych bloków \n'
./fsystem dump $VFS_NAME

echo -e 'Usuwamy plik 2048b.txt z dysku \n'
./fsystem remove $VFS_NAME 2048b.txt
echo -e 'Wyświetlamy stan poszczególnych bloków \n'
./fsystem dump $VFS_NAME

echo -e 'Próbujemy dodać plik 4097.txt, tym razem mamy wystarczająco ilość miejsca na dysku \n'
./fsystem push  $VFS_NAME 4097.txt
echo -e 'Wyświetlamy stan poszczególnych bloków \n'
./fsystem dump $VFS_NAME

echo -e 'Pobierzemy teraz plik 4097.txt i 2049.txt na dysk \n'
./fsystem pull $VFS_NAME 4097.txt 4097from_vfs.txt
./fsystem pull $VFS_NAME 2049.txt 2049from_vfs.txt

echo -e 'Usuwamy plik 2049.txt z dysku i dodajemy plik 0.txt o rozmiarze 0 bajtów \n'
./fsystem remove $VFS_NAME 2049.txt
./fsystem push $VFS_NAME 0.txt

echo -e 'Wyświetlamy stan poszczególnych bloków i ostateczną listę plików\n'
./fsystem dump $VFS_NAME

echo -e 'Usuwamy plik 0.txt z dysku \n'
./fsystem remove $VFS_NAME 0.txt

echo -e 'Wyświetlamy stan poszczególnych bloków i ostateczną listę plików'
./fsystem dump $VFS_NAME
./fsystem list $VFS_NAME


