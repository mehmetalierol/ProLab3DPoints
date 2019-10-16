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
int FileCount = 0;

int main()
{
    StartReading();

    return 0;
}

void StartReading()
{
    char applicationPath[256];
    if (getcwd(applicationPath, sizeof(applicationPath)) != NULL)
    {
       printf("Uygulama Dizini : %s\n", applicationPath);
       int appDirectoryFileCount = GetFileCount(DirectoryPath, FileExtention);
       if(appDirectoryFileCount > 0)
       {
           printf("Dizinde toplam %d adet %s dosyasi bulundu!\n", appDirectoryFileCount, FileExtention);

           //Hiyerarşik şekilde çalışması için ilk metod çağırılıyor.
           ReadDocumentsInDirectory(DirectoryPath, FileExtention);
       }
       else
       {
            printf(NoFileInDirectoryError, FileExtention);
       }
    }
    else
    {
       perror(ApplicationDirectoryError);
       return 1;
    }
}

//Dizindeki dosya okuma işlemlerini başlatacak olan metot
void ReadDocumentsInDirectory(char directoryPath[], char fileExtention[])
{
    //verilen dizindeki istenen uzantıdaki dosya sayısı ile yeni bir struct dizi oluşturur.
    Document filesArray[GetFileCount(directoryPath, fileExtention)];

    //Hangi dosyada olduğumuzu anlamak için index
    int fileIndex = 0;

    //istenen dizin içindeki dosyalar tek tek okunacak
    DIR *dir;
    struct dirent *ent;

    //Dizin açılıyor
    if ((dir = opendir (directoryPath)) != NULL)
    {
       //Dosya sayısı kadar döngü
       while ((ent = readdir (dir)) != NULL)
       {
          //Dosya ismi uzantı kontrolü için yeni bir değişkende saklanıyor.
          char *fileName = (char *)malloc(strlen(ent->d_name)+1);
          strcpy(fileName,ent->d_name);
          char *ext = strrchr(fileName,'.');

          if(ext)
          {
              //Uzantı kontrol ediliyor
              if(strcmp(ext, FileExtention) == 0)
              {
                  //dosyanın tam yolu hesaplanıyor
                  char *fullPath = (char *) malloc(1 + strlen(directoryPath)+ strlen(fileName) );
                  strcpy(fullPath, directoryPath);
                  strcat(fullPath, "//");
                  strcat(fullPath, fileName);

                  //metodun başında oluşturulan arrayin index kontrollü şekilde elemanları atanıyor.
                  filesArray[fileIndex] = ReadDocument(fullPath, fileName);

                  //her bir döküman da index artırılıyor.
                  fileIndex++;
              }
          }
       }

       //Dosya ile iş bittiğinde dosya kapatılıyor.
       closedir (dir);
    }
    else
    {
       //Dizin açılamaz ise hata veriliyor.
       perror (DirectoryOpenError);
       return EXIT_FAILURE;
    }

    DocumentsInMemory = malloc(filesArray);
    DocumentsInMemory = filesArray;

    FileCount = fileIndex;

    CreateUserInterface();
}

//İstenen dökümanı okuma ve belleğe yazma işini yapar.
Document ReadDocument(char *filePath, char *fileName)
{
   //okuma işlemleri için FILE nesnesi
   FILE *fp;

   //Okuma modunda ilgili dizindeki dosya açılıyor
   fp  = fopen (filePath, "r");

   //Dosya açılamadı ise ekrana hata mesajı yazılıyor.
   if (fp == NULL)
   {
      perror(FileOpenError);
      exit(EXIT_FAILURE);
   }

   //Döküman içindeki nokta datalarının sayısını hesaplamak için kullanılan index
   int count = 0;

   //satır satır  dosya okumak için kullanılan değişken (Satır boyutu 256 karakteri geçmeyeceği düşünülmüştür.)
   char chunk[256];

   //Döküman bilgilerini bellekte tutmak için gerekli tanımlama yapılıyor.
   Document documentItem;
   //bool değerlere default değer ataması yapılıyor.
   documentItem.isAscii = false;
   documentItem.isWithRGB = false;
   documentItem.currentFileName = malloc(fileName);
   documentItem.currentFileName = fileName;
   documentItem.isCalculated = false;

   //Boşluk ile split edilecek datalarda hangi alanda olduğunu bilmek için kullanılan index
   int tempIndex = 0;

   //dosya satır satır okunuyor
   while(fgets(chunk, sizeof(chunk), fp) != NULL)
   {
       //ilk satır yorum satırı olduğu için atlanıyor
       if(count > 0)
       {
           //sscanf ile format belirterek veri almak için array oluşturuyoruz ve kopyalama yapıyoruz.
           char tempArr[strlen(chunk) -1];
           strncpy(tempArr, chunk, strlen(chunk) -1);

           //Verisyon ve nokta sayisi ise calisacak kisim
           if(count == 1 || count == 3)
           {
               //sscanf ile format belirterek okuma yapıyoruz
               char tempStr [20];
               int tempInt;
               sscanf (tempArr,"%s %d",tempStr, &tempInt);

               //versiyon satırında isek versiyonu belleğe atıyoruz
               if(count == 1)
               {
                    documentItem.version = tempInt;
               }
               else if(count == 3) //nokta sayısı satırında isek nokta sayısını bellege atıyoruz
               {
                    documentItem.pointCount = tempInt;
               }
           }
           else if (count == 2) //alanlar satırında kontroller yapılıyor
           {
               //alanlar satırı boşluklarından bölünüyor
               tempIndex = 0;
               char *splitRGB = strtok (tempArr," ");
               while (splitRGB != NULL)
               {
                   //3 ten fazla bölme varsa rgb alanları da vardır
                   if(tempIndex > 3)
                   {
                       //rgb alanların olup olmadığı bir bool değişkende saklanıyor
                        documentItem.isWithRGB = true;
                        break;
                   }

                   //değişken sıfırlanıyor ve index artırılıyor
                   splitRGB = strtok (NULL, " ");
                   tempIndex++;
               }
           }
           else if (count == 4) //döküman içindeki datanın tipi kontrol ediliyor
           {
               //data tipi satırı boşluklarından bölünüyor
               tempIndex = 0;
               char *splitDataType = strtok (tempArr," ");
               while (splitDataType != NULL)
               {
                   //ilk kısım (DATA) atlanıp asıl verinin olduğu bölüm kontrol ediliyor
                   if(tempIndex == 1)
                   {
                       //en üstte tanımlı sabit içindeki ascii değeri ile tolower edilmiş gerçek data karşılaştırılıyor
                       if(strstr(tolower(splitDataType), CheckAscii) != NULL)
                       {
                           // data tipi ascii ise bool bir değişkende saklanıyor
                           documentItem.isAscii = true;
                       }
                   }

                   //değişken sıfırlanıyor ve index artırılıyor.
                   splitDataType = strtok (NULL, " ");
                   tempIndex++;
               }
           }
           else
           {
                //data okuma kısmını başka bir metot yapacağı için header veriler okunduktan sonra döngü durduruluyor.
                break;
           }
       }

       //yeni satıra geçildiği için index artırılıyor
       count++;
   }

   documentItem.realPointCount = GetDatacount(filePath);
   if(documentItem.realPointCount == documentItem.pointCount)
   {
       //Döküman içindekiler dataları tutacak array oluşturuluyor. Size kısmı ise nokta sayısı olarak belirleniyor.
       DocumentDataModel myModel[documentItem.pointCount];

       //Döküman içinde nokta datasının okunması için ilgili metot çağırılarak geriye dönen değer document nesnesinin dataitem propertisine atanıyor.
       documentItem = ReadDocumentDataPart(filePath, documentItem, myModel, fileName);
   }
   else
   {
       PushError(fileName, 0, FilePointCountWrong, PointCountError);
       ErrorCount++;
   }

   //içeriği doldurulmuş document döndürülüyor.
   return documentItem;
}

//İstenen dökümanın nokta verilerini okuyarak verilen Document nesnesi içine atar
Document ReadDocumentDataPart(char *filePath, Document docItem, DocumentDataModel finalDataModel[], char *fileName)
{
   //okuma işlemleri için FILE nesnesi
   FILE *fp;

   //Okuma modunda ilgili dizindeki dosya açılıyor
   fp  = fopen (filePath, "r");

   //Dosya açılamadı ise ekrana hata mesajı yazılıyor.
   if (fp == NULL)
   {
      perror(FileOpenError);
      exit(EXIT_FAILURE);
   }

   //Döküman içindeki nokta datalarının sayısını hesaplamak için kullanılan index
   int count = 0;

   //satır satır  dosya okumak için kullanılan değişken (Satır boyutu 256 karakteri geçmeyeceği düşünülmüştür.)
   char chunk[256];

   //x y z r g b kısmını boşluk ile split ediyoruz bu index bize hangi alanda olduğumuzu bildirecek.
   int tempIndex = 0;

   //Döküman modeli içindeki dataItem arrayine eklenecek dataların indexleri
   int dataItemIndex = 0;

   //dosyayı satır satır dönmek için döngü
   //fgets chunk size kadar okuma yapar ancak size öncesi satır sonu varsa işlemi durdurur dolayısı ile satır satır data elimizde olur
   while(fgets(chunk, sizeof(chunk), fp) != NULL)
   {
        //fgets ile alınan satır verisi döngü kurmak için başka bir array e aktarlıyor
        char tempArr[strlen(chunk) -1];
        strncpy(tempArr, chunk, strlen(chunk) -1);

        //ilk 4 satır header bilgisi barındırdığı için 4. satırdan itibaren datalar okunuyor
        if(count > 4)
        {
            //her bir satır döngüsünde bu index sıfırlanmalıdır.
            tempIndex = 0;

            //satırdaki veriler boşluklarından parçalanıyor
            char *splitData = strtok (tempArr," ");

            //parçalanan veri döngüye giriyor.
            while (splitData != NULL)
            {
                //her bir parça yeni bir char arraye atanıyor. (son karakter sorun çıkardığı için - 1 ile yeni dizi oluşturulup atanıyor)
                char myChar[strlen(splitData) -1];
                strncpy(myChar, splitData, strlen(splitData));

                //eğer ifadeler 0 a eşit ise float convert problemi olduğundan kontrol yapılıyor.
                bool isZero = strcmp(splitData, "0") == 0;

                //index numarası üzerinden hangi alanda olunduğu kontrol edilerek atama yapılıyor değer 0 mı diye kontrollü atama yapılıyor.
                if(tempIndex == 0) // X ise
                {
                    //float değer int 0 gelirse sorun çıkardığı için kontrollü atama yapılıyor
                    finalDataModel[dataItemIndex].x = isZero ? 0.00 : (float)atof(myChar);

                    //lowest hesaplanırken eğer ilk satır ise atama yapılıyor değilse değer korunuyor.
                    docItem.lowestX = (dataItemIndex == 0) ?  finalDataModel[dataItemIndex].x : docItem.lowestX;

                    if(finalDataModel[dataItemIndex].x > docItem.biggestX)
                    {
                        docItem.biggestX = finalDataModel[dataItemIndex].x;
                    }
                    if(finalDataModel[dataItemIndex].x < docItem.lowestX)
                    {
                        docItem.lowestX = finalDataModel[dataItemIndex].x;
                    }
                }
                else if(tempIndex == 1) // Y ise
                {
                    finalDataModel[dataItemIndex].y = isZero ? 0.00 : (float)atof(myChar);

                    docItem.lowestY = (dataItemIndex == 0) ? finalDataModel[dataItemIndex].y : docItem.lowestY;

                    if(finalDataModel[dataItemIndex].y > docItem.biggestY)
                    {
                        docItem.biggestY = finalDataModel[dataItemIndex].y;
                    }
                    if(finalDataModel[dataItemIndex].y < docItem.lowestY)
                    {
                        docItem.lowestY = finalDataModel[dataItemIndex].y;
                    }
                }
                else if(tempIndex == 2) // Z ise
                {
                    finalDataModel[dataItemIndex].z = isZero ? 0.00 : (float)atof(myChar);

                    docItem.lowestZ = (dataItemIndex == 0) ? finalDataModel[dataItemIndex].z : docItem.lowestZ;

                    if(finalDataModel[dataItemIndex].z > docItem.biggestZ)
                    {
                        docItem.biggestZ = finalDataModel[dataItemIndex].z;
                    }
                    if(finalDataModel[dataItemIndex].z < docItem.lowestZ)
                    {
                        docItem.lowestZ = finalDataModel[dataItemIndex].z;
                    }
                }

                //eğer dökümanda RGB datası var ise aşağıdaki atamalar da aynı şekilde yapılıyor
                if(docItem.isWithRGB)
                {
                    if(tempIndex == 3){finalDataModel[dataItemIndex].r = (int)atof(splitData);} // R ise
                    else if(tempIndex == 4){finalDataModel[dataItemIndex].g = (int)atof(splitData);} // G ise
                    else if(tempIndex == 5){finalDataModel[dataItemIndex].b = (int)atof(splitData);} //B ise
                }

                //bir sonraki alana geçerken değişken sıfırlanıyor ve index artırılıyor
                splitData = strtok (NULL, " ");
                tempIndex++;
            }

            //Dosya rgb alanları ile verildi ise ancak ilgili satır r g b bilgilerini içermiyorsa hatalar link listine eklenir.
            if(docItem.isWithRGB && tempIndex < 6)
            {
                //önce ilgili satır verisinde rgb verileri 0 a eşitlenir.
                finalDataModel[dataItemIndex].r = 0;
                finalDataModel[dataItemIndex].g = 0;
                finalDataModel[dataItemIndex].b = 0;

                //yeni bir hata nodu oluşturulur ve link liste push yöntemi ile eklenir.
                PushError(fileName, dataItemIndex + 1, LineNoRgbError, NoRgb);

                //hata mesajları indexi 1 artırılır.
                ErrorCount++;
            }
            else if(!docItem.isWithRGB && tempIndex > 3) //Dosya rgb alanlar olmadan verildi ise ancak ilgili satır rb alanlarını içeriyorsa hatalar link listine eklenir.
            {
                finalDataModel[dataItemIndex].r = 0;
                finalDataModel[dataItemIndex].g = 0;
                finalDataModel[dataItemIndex].b = 0;

                //yeni bir hata nodu oluşturulur ve link liste push yöntemi ile eklenir.
                PushError(fileName, dataItemIndex + 1, LineRgbError, WithRgb);
            }

            //bir sonraki sonraki data satırına geçerken index artırılıyor
            dataItemIndex++;
        }
        //bir sonraki döküman satırına geçerken index artırılıyor.
        count++;
    }

    //dosya okuma işlemi bittiğinde kapatılıyor.
   fclose(fp);

   //döküman modeli içindeki item datalara aktarmak yapmak üzere veriler kopyalanıyor (memory de ayrı alanlar açılması için her seferinde malloc ile yer ayrılıyor.)
   DocumentDataModel *finalDataPointer = malloc(finalDataModel);

   for(int i = 0; i < docItem.pointCount; i++)
   {
        finalDataPointer[i].x = finalDataModel[i].x;
        finalDataPointer[i].y = finalDataModel[i].y;
        finalDataPointer[i].z = finalDataModel[i].z;
        //eğer dosya rgb kısmını içermiyorsa o alanlara atama yapılmıyor.
        if(docItem.isWithRGB)
        {
            finalDataPointer[i].r = finalDataModel[i].r;
            finalDataPointer[i].g = finalDataModel[i].g;
            finalDataPointer[i].b = finalDataModel[i].b;
        }
        finalDataPointer[i].lineNumber = i+1;
   }

   //döküman modelinin içindeki dataitem'a array ataması yapılıyor.
   docItem.itemData = finalDataPointer;

   //içi dolu şekilde döküman modeli geri döndürülüyor.
   return docItem;
}

void CreateUserInterface()
{
    int operationCode = GetOperationCodeFromUser();

    switch(operationCode)
    {
        case 1:
            OperationOne();
            break;
        case 2:
            OperationTwo();
            break;
        case 3:
            OperationThree();
            break;
        case 4:
            OperationFour();
            break;
        case 5:
            OperationFive();
            break;
        default:
            printf("Gecerli bir operasyon girilmedi!");
            break;
    }

    char answer = "";
    printf("\nYeniden islem girmek ister misiniz? Y/N : ");
    scanf("%c", &answer);
    getchar();

    if(tolower(answer) == 'y')
    {
        CreateUserInterface();
    }
}

int GetOperationCodeFromUser()
{
    printf("\nIslem listesi : ");
    printf("\n( 1-) Dosya Kontrolu");
    printf(" | ( 2-) En Yakin/En Uzak Noktalar");
    printf(" | ( 3-) Kup");
    printf(" | ( 4-) Kure");
    printf(" | ( 5-) Nokta Uzakliklari\n");
    int operationCode = 1;
    do
    {
        printf("Lutfen gecerli bir islem tipi giriniz : ");
        scanf("%d", &operationCode);
        while(getchar() != '\n');
    }
    while(operationCode > 5 || operationCode < 1);

    return operationCode;
}

void OperationOne()
{
    ErrorLinkList *current_node = HeadError;
    if(HeadError != NULL)
    {
        printf("\nISLEM 1\n");
        while (current_node != NULL)
        {
            ErrorMessage messageToPrint = current_node->data;
            if(messageToPrint.errorCode == NoRgb)
            {
                printf("(%s) - %d. %s\n", messageToPrint.errorFileName, messageToPrint.errorLineNumber, LineNoRgbError);
            }
            else if(messageToPrint.errorCode == WithRgb)
            {
                printf("(%s) - %d. %s\n", messageToPrint.errorFileName, messageToPrint.errorLineNumber, LineRgbError);
            }
            else
            {
                printf("%s %s", messageToPrint.errorFileName, messageToPrint.errorMessageTest);
            }
            current_node = current_node->next;
        }
    }
    else
    {
        printf("Tüm dosyalar uyumludur.");
    }
}

void OperationTwo()
{
    if(sizeof(DocumentsInMemory) > 0)
    {
        CalcDistance(2);
    }
    else
    {
        printf("Okunmus dosya bulunmuyor.");
    }
}

void OperationThree()
{
    if(sizeof(DocumentsInMemory) > 0)
    {
        printf("\nISLEM 3\n");
    }
    else
    {
        printf("Okunmus dosya bulunmuyor.");
    }
}

void OperationFour()
{
    if(sizeof(DocumentsInMemory) > 0)
    {
        float sphereX, sphereY, sphereZ, sphereR;
        printf("\nISLEM 4\n");
        printf("Lutfen kurenin x degerini giriniz : ");
        scanf("%f", &sphereX);
        printf("Lutfen kurenin y degerini giriniz : ");
        scanf("%f", &sphereY);
        printf("Lutfen kurenin z degerini giriniz : ");
        scanf("%f", &sphereZ);
        printf("Lutfen kurenin yaricapini giriniz : ");
        scanf("%f", &sphereR);
        while(getchar() != '\n');

        printf("Girdiginiz X : %f - Y : %f - Z : %f - R : %f", sphereX, sphereY, sphereZ, sphereR);
    }
    else
    {
        printf("Okunmus dosya bulunmuyor.");
    }
}

void OperationFive()
{
    if(sizeof(DocumentsInMemory) > 0)
    {
        CalcDistance(5);
    }
    else
    {
        printf("Okunmus dosya bulunmuyor.");
    }
}

void CalcDistance(int operationType)
{
    int avgIndex = 0;
    printf("\nISLEM %d\n", operationType);
    for(int j = 0; j < FileCount; j++)
    {
        if(DocumentsInMemory[j].realPointCount == DocumentsInMemory[j].pointCount)
        {
            if(DocumentsInMemory[j].isCalculated == false)
            {
                avgIndex = 0;
                DocumentsInMemory[j].itemsAvg = 0.00;
                int tempMinCalculation = 0;
                int tempMaxCalculation = 0;

                for(int i = 0; i < DocumentsInMemory[j].pointCount; i++) //nokta sayısı kadar dön
                {
                    for(int k = i+1; k < DocumentsInMemory[j].pointCount; k++)// fazladan dönmemek için her seferinde başlangıcı i+1 kadar yap zaten ondan onceki noktayı karşılaştırmıştın
                    {
                        if(i == 0)
                        {
                            float calculationResult = sqrt((pow(DocumentsInMemory[j].itemData[i].x-DocumentsInMemory[j].itemData[k].x,2)) +
                                                           (pow(DocumentsInMemory[j].itemData[i].y-DocumentsInMemory[j].itemData[k].y,2)) +
                                                           (pow(DocumentsInMemory[j].itemData[i].z-DocumentsInMemory[j].itemData[k].z,2)));

                            DocumentsInMemory[j].itemsAvg = calculationResult;

                            tempMinCalculation = calculationResult;
                            tempMaxCalculation = calculationResult;

                            DocumentsInMemory[j].nearestList[0] = DocumentsInMemory[j].itemData[i];
                            DocumentsInMemory[j].nearestList[1] = DocumentsInMemory[j].itemData[k];

                            DocumentsInMemory[j].farthestList[0] = DocumentsInMemory[j].itemData[k];
                            DocumentsInMemory[j].farthestList[1] = DocumentsInMemory[j].itemData[i];
                        }
                        else
                        {
                            float calculationResult = sqrt((pow(DocumentsInMemory[j].itemData[i].x-DocumentsInMemory[j].itemData[k].x,2)) +
                                                           (pow(DocumentsInMemory[j].itemData[i].y-DocumentsInMemory[j].itemData[k].y,2)) +
                                                           (pow(DocumentsInMemory[j].itemData[i].z-DocumentsInMemory[j].itemData[k].z,2)));

                            if(calculationResult<0){calculationResult *= (-1);}

                            DocumentsInMemory[j].itemsAvg += calculationResult;

                            if(calculationResult < tempMinCalculation) //hesaplama gecici en küçük değerden küçük mü
                            {
                                DocumentsInMemory[j].nearestList[0] = DocumentsInMemory[j].itemData[i];
                                DocumentsInMemory[j].nearestList[1] = DocumentsInMemory[j].itemData[k];
                                tempMinCalculation = calculationResult;
                            }
                            if(calculationResult > tempMaxCalculation) //hesaplama gecici en büyük değerden büyük mü
                            {
                                DocumentsInMemory[j].farthestList[0] = DocumentsInMemory[j].itemData[k];
                                DocumentsInMemory[j].farthestList[1] = DocumentsInMemory[j].itemData[i];
                                tempMaxCalculation = tempMaxCalculation;
                            }
                        }

                        avgIndex++;
                    }
                }

                DocumentsInMemory[j].itemsAvg = DocumentsInMemory[j].itemsAvg / avgIndex;
                DocumentsInMemory[j].isCalculated = true;

                PrintCalcDistance(DocumentsInMemory[j], operationType);
            }
            else
            {
                PrintCalcDistance(DocumentsInMemory[j], operationType);
            }
        }
    }
}

void PrintCalcDistance(Document doc, int operationType)
{
    if(operationType == 2)
    {
        printf("(%s) En Yakin Nokta (%d. Satir):%f %f %f %d %d %d\n",
            doc.currentFileName, doc.nearestList[0].lineNumber,
            doc.nearestList[0].x, doc.nearestList[0].y,
            doc.nearestList[0].z, doc.nearestList[0].r,
            doc.nearestList[0].g, doc.nearestList[0].b);

        printf("(%s) En Yakin Nokta (%d. Satir):%f %f %f %d %d %d\n",
            doc.currentFileName, doc.nearestList[1].lineNumber,
            doc.nearestList[1].x, doc.nearestList[1].y,
            doc.nearestList[1].z, doc.nearestList[1].r,
            doc.nearestList[1].g, doc.nearestList[1].b);

        printf("(%s) En Yakin Nokta (%d. Satir):%f %f %f %d %d %d\n",
            doc.currentFileName, doc.farthestList[0].lineNumber,
            doc.farthestList[0].x, doc.farthestList[0].y,
            doc.farthestList[0].z, doc.farthestList[0].r,
            doc.farthestList[0].g, doc.farthestList[0].b);

        printf("(%s) En Yakin Nokta (%d. Satir):%f %f %f %d %d %d\n\n",
            doc.currentFileName,   doc.farthestList[1].lineNumber,
            doc.farthestList[1].x, doc.farthestList[1].y,
            doc.farthestList[1].z, doc.farthestList[1].r,
            doc.farthestList[1].g, doc.farthestList[1].b);
    }
    else if(operationType == 5)
    {
        printf("(%s) Noktalar arasi ortalama uzaklik : %f\n", doc.currentFileName, doc.itemsAvg);
    }
}

//istenen dökümanın içindeki satır sayısını getirir
int GetDatacount(char *filePath)
{
   //Dosyadan okuma için file nesnesi
   FILE *fp;

   //Okuma modunda istenen dosya açılıyor
   fp  = fopen (filePath, "r");

   //Dosya açılamaz ise hata veriliyor.
   if (fp == NULL)
   {
      perror(FileOpenError);
      exit(EXIT_FAILURE);
   }

   //satır sayısını bulmak için index
   int count = 0;
   int dataCount = 0;

   //Satır satır döngü çin char array
   char chunk[256];

   //satır satır döngüye giriliyor
   while(fgets(chunk, sizeof(chunk), fp) != NULL)
   {
       if(count > 4)
       {
           dataCount++;
       }
       //her bir satırda index artırılıyor
       count++;
   }

   //iş bitince dosya kapatılıyor
   fclose(fp);

   //hesaplanan satir sayisi geri döndürülüyor.
   return dataCount;
}

//Verilen dizindeki istenen uzantılı dosyaların sayısını veren motot
int GetFileCount(char directoryPath[], char fileExtention[])
{
    //dosya sayısı hesaplamak için index
    int fileCount = 0;

    //dizin işlemleri için gerekli tanımlama yapılıyor.
    DIR *dir;
    struct dirent *ent;

    //dizin aciliyor
    if ((dir = opendir (directoryPath)) != NULL)
    {
       //dizin içindeki dosya sayısı kadar döngü başlıyor
       while ((ent = readdir (dir)) != NULL)
       {
          //her dönüşte dosya adı için raden farklı bir yer ayrılıyor
          char *fileName = (char *)malloc(strlen(ent->d_name)+1);
          //dosya adı yeni değğişkene kopyalanıyor.
          strcpy(fileName,ent->d_name);

          //uzantıyı almak için dosya adı . lar üzerinden parçalanıyor.
          char *ext = strrchr(fileName,'.');
          if(ext)
          {
              //uzantı en üstte tanımlanan değişkenle eşleşiyor mu diye kontrol ediliyor.
              if(strcmp(ext, FileExtention) == 0)
              {
                  //eşleşen dosyalar için index 1 artırılıyor.
                  fileCount++;
              }
          }
       }

       //iş bitince dosya kapatılıor.
       closedir (dir);
    }
    else
    {
      //dizin açılamaz ise hata veriliyor.
      perror (DirectoryOpenError);
      return EXIT_FAILURE;
    }

    //hesaplanan dosya sayısı geri döndürülüyor.
    return fileCount;
}

void PushError(char *fileName, int lineNumber, char *message, int code)
{
    ErrorMessage currentError;
    currentError.errorFileName = fileName;
    currentError.errorLineNumber = lineNumber;
    currentError.errorMessageTest = message;
    currentError.errorCode = code;

    struct ErrorLinkList *node = (struct ErrorLinkList*) malloc(sizeof(struct ErrorLinkList));
    node->data = currentError;
    node->next = HeadError;
    HeadError = node;
}
