#ifndef METHODS_H_INCLUDED
#define METHODS_H_INCLUDED

#include "models.h"

//Proje içinde kullanýlan metotlar
void StartReading();
void ReadDocumentsInDirectory(char directoryPath[], char fileExtention[]);
Document ReadDocument(char *filePath, char *fileName);
Document ReadDocumentDataPart(char *filePath, Document docItem, DocumentDataModel finalDataModel[], char *fileName);
void CreateUserInterface();
int GetOperationCodeFromUser();
void OperationOne();
void OperationTwo();
void OperationThree();
void OperationFour();
void OperationFive();
void CalcDistance(int operationType);
void PrintCalcDistance(Document doc, int operationType);
int GetDatacount(char *filePath);
int GetFileCount(char directoryPath[], char fileExtention[]);
void PushError(char *fileName, int lineNumber, char *message, int code);

#endif // METHODS_H_INCLUDED
