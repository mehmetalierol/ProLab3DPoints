#ifndef CONSTS_H_INCLUDED
#define CONSTS_H_INCLUDED

//Proje içerisinde kullanýlacak sabit deðerler
const char *FileExtention = ".nkt";
const char *DirectoryPath = "C://DataFiles";
const char *OutputPath = "C://DataFiles/output.nkt";
const char *CheckAscii = "ascii";
const char *OutputFileName = "output.nkt";

//hata mesajlarý
const char *FilePointCountWrong = " dosyasindaki nokta sayisi gecerli degildir.\n";
const char *FileOpenError = "Dosya acilamadi!.";
const char *DirectoryOpenError = "Dizin acilamadi.";
const char *ApplicationDirectoryError = "Uygulama dizini alinamadi!";
const char *NoFileInDirectoryError = "Dizinde hic %s dosyasi bulunamadi!";
const char *LineNoRgbError = " Nokta verisi r g b bilgileri olmadan verilmistir.";
const char *LineRgbError = " Nokta verisi r g b bilgileri ile verilmistir. Dosyada r g b alanlari yoktur.";

#endif // CONSTS_H_INCLUDED
