#ifndef METHODS_H_INCLUDED
#define METHODS_H_INCLUDED

#include "models.h"

//Proje içinde kullanýlan metotlar
void StartReading();
void ReadDocumentsInDirectory(char directoryPath[], char fileExtention[]);
Document ReadDocument(char *filePath, char *fileName);
Document ReadDocumentDataPart(char *filePath, Document docItem, DocumentDataModel finalDataModel[], char *fileName);
int GetFileCount(char directoryPath[], char fileExtention[]);
int GetDatacount(char *filePath);
void PrintDocuments(int fileCount);
void PushError(char *fileName, int lineNumber, char *message, int code);

#endif // METHODS_H_INCLUDED
