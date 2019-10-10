//giriş çıkış işlemleri için (printf vs)
#include <stdio.h>
#include <stdlib.h>
//Dizin işlemleri için
#include <dirent.h>
//string operasyonlar için (strlen, split .. vs)
#include <string.h>
//bool kullanımı için
#include <stdbool.h>
//tolower vs gibi işlemler
#include <ctype.h>

//kendi header dosyalarımız
#include "methods.h" //metot imzaları (interfaceler)
#include "models.h" //structlar (modeller)
#include "consts.h" //sabitler
#include "enums.h" //enumlar

//Global Değişkenler
Document *DocumentsInMemory;
struct ErrorLinkList *HeadError = NULL;
int ErrorCount = 0;

int main()
{
    StartReading();

    //Program hemen kapanmasın diye bekletiliyor.
    getchar();
    return 0;
}

void StartReading()
{
    char applicationPath[256];
    if (getcwd(applicationPath, sizeof(applicationPath)) != NULL)
    {
       printf("Uygulama Dizini : %s\n", applicationPath);

    }
    else
    {
       perror(ApplicationDirectoryError);
       return 1;
    }
}
