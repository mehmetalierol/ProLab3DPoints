#ifndef MODELS_H_INCLUDED
#define MODELS_H_INCLUDED

typedef struct {
    char *errorFileName;
    int errorLineNumber;
    char *errorMessageTest;
    int errorCode;
} ErrorMessage;

typedef struct ErrorLinkList{
    ErrorMessage data;
    struct ErrorLinkList *next;
} ErrorLinkList;

//Dökümanların içinde bulunan nokta verilerinin saklanacağı model
typedef struct {
   float  x;
   float  y;
   float  z;
   int r;
   int g;
   int b;
} DocumentDataModel;

//Dökümanların saklanacağı model
typedef struct {
   char* currentFileName;
   int  version;
   int  pointCount;
   int  realPointCount;
   bool isAscii;
   bool isWithRGB;
   float biggestX;
   float biggestY;
   float biggestZ;
   float lowestX;
   float lowestY;
   float lowestZ;
   DocumentDataModel *itemData;
} Document;

#endif // MODELS_H_INCLUDED
