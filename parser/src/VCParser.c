#include "VCParser.h"
#include "helpers.h"


void initializeVcard(Card *vCard)
{
	if (vCard == NULL)
	{
		return;
		// error occured.
	}
	vCard->optionalProperties = initializeList( &propertyToString, &deleteProperty, &compareProperties);
	//props->parameters = initializeList( &parameterToString, &deleteParameter, &compareParameters);
	//props->values = initializeList( &valueToString, &deleteValue, &compareValues);
	// all lists have been initialized.

}
int validateBasics(FILE* vCardFile)
{
	char buffer[1000] = "";
	int validLine = 0;
	int validStart = 0;
	int validEnd = 0;
	int validVer = 0;
	int validFn = 0;
	
	
	 while ((fgets(buffer, 1000, vCardFile) != NULL))
	 {
		 if (strstr(buffer,"\r\n") == NULL)
		 {
			 validLine = 1;
		 }
		if(strstr(buffer,"BEGIN") != NULL)
		{
			validStart++;
		}
		if(strstr(buffer,"FN") != NULL)
		{
			validFn++;
		}
		if((strstr(buffer,"VERSION") != NULL) && (strstr(buffer,"4.0") != NULL))
		{
			validVer++;
		}
		if(strstr(buffer,"END:") != NULL)
		{
			validEnd++;
		}
		
		 
	 }
	 if ((validLine != 0) || (validVer != 1) || (validEnd != 1) || (validStart != 1) || (validFn <1))
	 {
		 return 1;
		 // these are error cases for invalid card
	 }
	
	return 0;
}
VCardErrorCode createCard(char* fileName, Card** newCardObject)
{
	
    Card* tempCard = calloc(1,sizeof(Card));
	tempCard->birthday = NULL;
	tempCard->anniversary = NULL;
	tempCard->fn = NULL;
    int fnCount = 0;
    //printf("%s\n",fileName);
    char a;
    //char z = 'Z';
    //int x = 0;
    //char time[7] = "";
    //char date[7] = "";
    DateTime *bday = NULL;
    int bdayCount = 0;
    int annivCount = 0;
    int validationCount = 0;
    int seek = 0;
    int endCard = 0;


    if (!fileName) {
		//printf("FileName is invalid\n");
        return INV_FILE;
    }
    //int dateVal = 0;


    //char line[2000] = "";

    //line 2 exists incase of line folding, it will store the folded line before strcat the lines together into buffer.
    //char line[2000] = "";
    //char buffer[2000] = "";
    //char buffer[2000];
    //if (buffer == NULL)
   // {
    //    free(tempCard);
    //    *newCardObject = NULL;
   //     //deleteProperty(otherProps);
        //free(otherProps);
  //      return OTHER_ERROR;
  //  }
   // printf("Test2");
    FILE* vCardFile = fopen(fileName, "r");
    int testError = validateBasics(vCardFile);
    if (testError == 1)
    {
		free(tempCard);
		fclose(vCardFile);
		return INV_CARD;
	}
	else if (testError == 0)
	{
		fseek(vCardFile,0,SEEK_SET);
	}
   // printf("Test3");
    //Property *otherProps = calloc(1,sizeof(Property));
    initializeVcard(tempCard);

    if (vCardFile == NULL)
    {
		//printf("Test2");
		//printf("File is invalid");
		freeList(tempCard->optionalProperties);
		free(tempCard);
		*newCardObject = NULL;
        return INV_FILE;
    }

	char buffer[2000] = "";

    while ((fgets(buffer, 2000, vCardFile) != NULL) && (endCard == 0))
    {

        //int count = 0;
       // char* propertyName;
        a = fgetc(vCardFile);
        seek = ftell(vCardFile);
        // Figure out how long the property name is
        //printf("BUFFER: %ld\n",strlen(buffer));
        if ((strlen(buffer)>1) && (strstr(buffer,"\r\n") == NULL))
        {
			
            // invalid line terminator, free card and return.
            //deleteCard(tempCard);
           // free(buffer);
            //tempCard = NULL;
            //*newCardObject = tempCard;
            //printf("CHeese 1");
            return INV_CARD;
        }
        else
        {
			//printf("%s",buffer);
		}
        if  ( (strstr(buffer,"\r\n") != NULL) && ( (a == ' ') || (a == '\t') ) )
        {
			//printf("call Recursion\n");
            recursiveUnfolding(buffer,&vCardFile);

        }
        else
        {
			if (seek != SEEK_END)
			{
				fseek(vCardFile,seek-1,SEEK_SET);
			}
		}
        if ((strchr(buffer,':') == NULL) && (strlen(buffer)>1) )
        {
            // invalid line, free vcard and exit.
           deleteCard(tempCard);
            //free(buffer);
            tempCard = NULL;
            *newCardObject = tempCard;
            fclose(vCardFile);
          // printf("BUFFER CHECK %s\n",buffer);
           //printf("Buffer = %ld\n",strlen(buffer));
          //  printf("CHeese 2");
            return INV_PROP;
        }
        
        
        char val[2000] = "";
        char name[2000] = "";
        int checkColon = 0;
        int nameCount = 0;
        int valCount = 0;
        int i =0;
        for(char*a = buffer; *a != '\r';a++)
        {
			if (buffer[i] == ':')
			{
				checkColon++;
			}
			if (checkColon == 0)
			{
				name[nameCount] = buffer[i];
				nameCount++;
			}
			else if((checkColon == 1) && (buffer[i] != ':'))
			{
				val[valCount] = buffer[i];
				valCount++;
			}
			else if((checkColon > 1))
			{
				val[valCount] = buffer[i];
				valCount++;
			}
			i++;
		}
		strcat(val,"\0");
		strcat(name,"\0");
		//printf("PRop val = %s\npropname = %s\n",val,name);
		char* propertyName = calloc(strlen(name)+1,sizeof(char));
		strcpy(propertyName,name);
		char* propertyValue = calloc(strlen(val)+1,sizeof(char));
		strcpy(propertyValue,val);
        
        /* old property Value and name code.
       for (char* c = buffer; *c != ':'; c++) {
           count++;
        }
        if (count == 0)
        {
			return INV_PROP;
		}
        

        // Allocate space for the property name
        propertyName = calloc(count + 1, sizeof(char));
        if (propertyName == NULL)
        {
           // free(buffer);
            deleteCard(tempCard);
            *newCardObject = NULL;
            //printf("Error 1\n");
            return OTHER_ERROR;
        }
        

        // Copy property name into a new string
        strncpy(propertyName, buffer, count);
       // printf("Prop NAME = %s\n",propertyName);
		//printf("Prop LENGTH = %ld\n",strlen(propertyName));
        // Advance line pointer by count to get the property value
       // char tempBuff[1000] = "";
        //strcpy(tempBuff,buffer+count+1);
        //printf("buffer2 = %s\n",tempBuff); 
        buffer += count + 1;
        int valCount = 0;
        for(char* c = buffer;*c !='\r';c++)\
        {
            valCount++;
        }
        char *propertyValue = calloc(valCount+1,sizeof(char));
        if (propertyValue == NULL)
        {
            //free(buffer);
            free(propertyName);
            deleteCard(tempCard);
            *newCardObject = NULL;
            //printf("Error 2\n");
            return OTHER_ERROR;

        }
        strncpy(propertyValue,buffer,valCount);
        */
        // printf("Prop value = %s\n",propertyValue);
        if (strlen(propertyValue) == 0)
        {
            //there is no value and error occured
            deleteCard(tempCard);
            //free(buffer);
            free(propertyValue);
            free(propertyName);
            tempCard = NULL;
            *newCardObject = tempCard;
            fclose(vCardFile);
           //printf("CHeese 3");
            return INV_PROP;
        }
        if (strlen(propertyName) == 0)
        {
            deleteCard(tempCard);
           // free(buffer);
            free(propertyValue);
            free(propertyName);
            tempCard = NULL;
            *newCardObject = tempCard;
            fclose(vCardFile);
            //printf("CHeese 4");
            return INV_PROP;

        }
        //copies value to string for value.

        //printf("[%s]\n", propertyName);
        //printf("[%s]\n", buffer);

        // Parse FN
       // printf("Property Name = %s\n",propertyName);
        if ( ((strstr(propertyName, "FN") != NULL)||(strstr(propertyName, "FN") != NULL)) && (fnCount == 0) )
        {
			Property* verify = calloc(1, sizeof(Property));
            fnCount = 1;
            validationCount++;
            
            if (verify == NULL)
            {
                free(propertyName);
               free(propertyValue);
                //free(buffer);
                deleteCard(tempCard);
                *newCardObject = NULL;
                fclose(vCardFile);
               // printf("Error 3\n");
                return OTHER_ERROR;
            }
            if (strchr(propertyName,'.') == NULL)
            {
				verify->name = calloc(strlen(propertyName)+1,sizeof(char));
				if (verify->name == NULL)
				{
					free(propertyName);
					free(propertyValue);
					//free(buffer);
					free(verify);
					deleteCard(tempCard);
					*newCardObject = NULL;
					fclose(vCardFile);
				// printf("Error 4\n");
					return OTHER_ERROR;
				}
				strcpy(verify->name,"FN\0");
				verify->group = calloc(1,sizeof(char));
				if (verify->group == NULL)
				{
					free(propertyName);
					free(propertyValue);
				//free(buffer);
				free(verify->name);
				free(verify);
					deleteCard(tempCard);
				*newCardObject = NULL;
				fclose(vCardFile);
				// printf("Error 5\n");
					return OTHER_ERROR;
				}
				strcpy(verify->group,"\0");
			}
			else if (strchr(propertyName,'.') != NULL)
			{
				
				// group name exists.
				//char name[3] = "FN\0";
				char group[100] = "";
				int groupCount = 0;
				int groupCheck =0;
				char tempCheck[1000] = "";
				strcpy(tempCheck,propertyName);
				for (int i = 0; i <strlen(tempCheck);i++)
				{
					if (tempCheck[i] == '.')
					{
						group[groupCount] = '\0';
						groupCheck = 1;
						//end of group.
					}
					if (groupCheck ==0)
					{
						group[groupCount] = tempCheck[i];
						groupCount++;
					}
				}
				verify->name = calloc(3,sizeof(char));
				if (verify->name == NULL)
				{
					free(propertyName);
					free(propertyValue);
					//free(buffer);
					free(verify);
					deleteCard(tempCard);
					*newCardObject = NULL;
					fclose(vCardFile);
				// printf("Error 4\n");
					return OTHER_ERROR;
				}
				strcpy(verify->name,"FN\0");
				verify->group = calloc(strlen(group)+1,sizeof(char));
				if (verify->group == NULL)
				{
					free(propertyName);
					free(propertyValue);
				//free(buffer);
					free(verify->name);
					free(verify);
					deleteCard(tempCard);
					*newCardObject = NULL;
					fclose(vCardFile);
				// printf("Error 5\n");
					return OTHER_ERROR;					
				}
				strcpy(verify->group,group);
			}
            verify->parameters = initializeList(&parameterToString,&deleteParameter,&compareParameters);
            verify->values = initializeList(&valueToString,&deleteValue,&compareValues);
            char* str = calloc(strlen(propertyValue)+1,sizeof(char));
            strcpy(str,propertyValue);
            strcat(str,"\0");
            insertBack(verify->values,str);
			//printf("FN Print\n");
            tempCard->fn = verify;
           //printf("FN STORE\n");
           // printf("FN value = %s\n",propertyValue);
        }
        else if ( (strstr(propertyName,"BEGIN")!=NULL) ||(strstr(propertyName,"begin")!=NULL) )
        {
            validationCount++;
        }

        else if ( (strcmp(propertyName,"END") == 0) ||(strcmp(propertyName,"end")==0))
        {
            validationCount++;
            endCard = 1;
        }
        else if ( ((strstr(propertyName,"VERSION")!=NULL)||(strstr(propertyName,"version")!=NULL)) )
        {
			// checks first version for valid, if multiple version, will store
			
			//versionCount = 1;

            
            if (strstr(propertyValue,"4.0") == NULL)
            {
                //free card and then return
                //not 4.0 error.
              //  printf("failed on version check\n");
                return INV_CARD;
            }
            else
            {
				validationCount++;
			}
            // check if version is 4.0.
        }


        else if (( (strstr(propertyName, "BDAY") != NULL)||(strstr(propertyName, "bday") != NULL)) && ( bdayCount == 0 ) )
        {
            bdayCount = 1;
            // ensures only 1 bday is stored to prevent errors of multiple bdays
            int check = GetDateTime(propertyValue,propertyName,tempCard,0);

            if (check == 1)
            {
                //free(buffer);
                free(propertyName);
                free(propertyValue);
                deleteCard(tempCard);
                *newCardObject = NULL;
                fclose(vCardFile);
                return INV_DT;
            }
            else if (check == 2)
            {
               // free(buffer);
                free(propertyName);
                free(propertyValue);
               deleteCard(tempCard);
               *newCardObject = NULL;
               fclose(vCardFile);
            //   printf("Error 6\n");
                return OTHER_ERROR;
            }

            // call bday function

        }
        else if ( ((strstr(propertyName, "ANNIVERSARY") != NULL) || (strstr(propertyName, "anniversary") != NULL)) && ( annivCount == 0 ) )
        {
            annivCount = 1;
            // call bday function
            int check = GetDateTime(propertyValue,propertyName,tempCard,1);

            if (check == 1)
            {
                //free(buffer);
                free(propertyName);
                free(propertyValue);
                deleteCard(tempCard);
                tempCard = NULL;
                *newCardObject = tempCard;
                fclose(vCardFile);
                return INV_DT;
            }
            else if (check == 2)
            {
                //free(buffer);
                free(propertyName);
                free(propertyValue);
                deleteCard(tempCard);
                *newCardObject = NULL;
                fclose(vCardFile);
               // printf("Error 7\n");
                return OTHER_ERROR;
            }

        }
        else
        {
			//printf("Function for other Properties\n");
			//printf("Propertyname = %s\npropertyValue =%s\n",propertyName,propertyValue);
            // call function for other properties.
            int check = otherProperties(propertyValue,propertyName,tempCard);
             if (check == 1)
            {
                //free(buffer);
                free(propertyName);
                free(propertyValue);
                deleteCard(tempCard);
                *newCardObject = NULL;
                fclose(vCardFile);
				//printf("BIG CHEESE");
                return INV_PROP;
            }
            else if (check == 2)
            {
                //free(buffer);
                free(propertyName);
                free(propertyValue);
                deleteCard(tempCard);
                *newCardObject = NULL;
                fclose(vCardFile);
              //  printf("Error 8\n");
                return OTHER_ERROR;
            }
        }
        // Add more cases based on properties/make a default case for all other properties if needed

        free(propertyName);
        free(propertyValue);
        
    }
    //free(buffer);
    if (bdayCount == 0)
    {
		tempCard->birthday = bday;
	}
	if (annivCount == 0)
	{
		tempCard->anniversary = bday;
	}

    if (validationCount != 4 )
    {
        // does not have BEGIN, END ,VERSION or FN tags
        //version is not 4.0
        //printf("failed on validation\n");
        deleteCard(tempCard);
        *newCardObject = NULL;
        fclose(vCardFile);
        return INV_CARD;
    }
    
    //insertback(tempCard->optionalProperties,otherProps);
    //printf("validation Complete");
    //printf("Testing saving card\n");
    *newCardObject = tempCard;
	//printf("Done saving vcard\n");
    fclose(vCardFile);

    return OK;
}
void deleteCard(Card* obj)
{
    if (obj == NULL)
    {
        return;
    }

    if (obj->fn == NULL)
    {
        //return;
    }
    else if (obj->fn != NULL)
    {

        deleteProperty(obj->fn);
        //free(obj->fn);
    }
    if (obj->optionalProperties == NULL)
    {
        return;
    }
    else if (obj->optionalProperties != NULL)
    {
        freeList(obj->optionalProperties);
    }
    if (obj->birthday != NULL)
    {
        //means birthday exists and must be freed
        deleteDate((obj->birthday));
    }
    if (obj->anniversary != NULL)
    {
        //means an anniversary exists and is going to be freed
        deleteDate((obj->anniversary));
    }
    free(obj);


}
char* cardToString(const Card* obj)
{
	if (obj == NULL)
	{
    char *str = calloc(1,sizeof(char));
    strcpy(str,"\0");
    return str;
	}
	else 
	{
	char *str = calloc(10,sizeof(char));
    strcpy(str,"     \0");
		return str;
	}
    
    // fill this in properly.
    

}
char* errorToString(VCardErrorCode err)
{
    char *str;
    if ( err == OK)
    {
        str = calloc(13,sizeof(char));
        if (str == NULL)
        {
            return NULL;
        }
        strcpy(str,"Card is OK\0");
        return str;
    }
    else if(err == INV_FILE)
    {
        str = calloc(13,sizeof(char));
        if (str == NULL)
        {
            return NULL;
        }
        strcpy(str,"Invalid File\0");
        return str;
    }
    else if (err == INV_CARD)
    {
        str = calloc(13,sizeof(char));
        if (str == NULL)
        {
            return NULL;
        }
        strcpy(str,"Invalid Card\0");
        return str;
    }
    else if (err == INV_DT)
    {
        str = calloc(18,sizeof(char));
        if (str == NULL)
        {
            return NULL;
        }
        strcpy(str,"Invalid Date/Time\0");
        return str;
    }
    else if (err == INV_PROP)
    {
        str = calloc(17,sizeof(char));
        if (str == NULL)
        {
            return NULL;
        }
        strcpy(str,"Invalid Property\0");
        return str;
    }
    else if (err == WRITE_ERROR)
    {
        str = calloc(12,sizeof(char));
        if (str == NULL)
        {
            return NULL;
        }
        strcpy(str,"Write Error\0");
        return str;
    }
    else if (err == OTHER_ERROR)
    {
        str = calloc(12,sizeof(char));
        if (str == NULL)
        {
            return NULL;
        }
        strcpy(str,"Other Error\0");
        return str;
    }
   
   return NULL;

}

void recursiveUnfolding(char buffer[2000],FILE **file)
{
// this function removes he /r/n while also checking if the next line is folded too. It is recursive to ensure all chain of lines that are folded will be unfolded into a clearer format.
    
    char line[256] = "";
    char* buffer2 = line;
    char* token;
    char s[3] = "\r\n";
    token = strtok(buffer,s);
    
    int lineCount;

    char a;

    if (token != NULL)
    {
       //printf("Test 4\n");
        strcpy(buffer,token);

    //copies in string without /r/n
    }
    else if(token == NULL )
    {
        return;
    }
    fgets(buffer2, 256, *file);
    a = fgetc(*file);
    if ( (strstr(buffer2,"\r\n") != NULL) && ( (a == ' ') || (a =='\t') ) )
    {
		
        token = strtok(buffer2,s);
        strcat(buffer,token);

        recursiveUnfolding(buffer,file);

    }
    else if ( (strstr(buffer2,"\r\n") != NULL) && ( (a != ' ') && (a !='\t') ) )
    {
		
        strcat(buffer,buffer2);
        strcat(buffer,"\0");
        
        lineCount = ftell(*file);
         
        fseek(*file,lineCount-1,SEEK_SET);
         
    }


}


void deleteValue(void* toBeDeleted) {
    if (!toBeDeleted) {
        return;
    }

    char* val = (char*) toBeDeleted;
	//printf("Value = %s\n",val);
	if (val != NULL)
	{
		free(val);
	}
}

int compareValues(const void* first,const void* second) {
    return 0;
}

char* valueToString(void* prop) {
	char *check = (char*)prop;
    if(strcmp(check,"\0") == 0 )
    {
		//printf("Testing\n");
	}
    if (prop == NULL)
    {
        return NULL;
    }
    int length = 0;
    char* str;
    length = (strlen((char*)prop) + 7);
    str = calloc(length,sizeof(char));
    if (str == NULL)
    {
        return NULL;
    }
    sprintf(str,"Value:%s\n",(char*)prop);
    return str;
}
void deleteDate(void* toBeDeleted)
{
    if (toBeDeleted == NULL)
    {
        return;
    }
    DateTime* date = (DateTime*)toBeDeleted;
    if (date->date != NULL)
    {
		free(date->date);
	}
	if (date->time != NULL)
	{
		free(date->time);
	}
	if (date->text != NULL)
	{
		
		free(date->text);
	}
	
    free(date);

}
int compareDates(const void* first,const void* second)
{
    //stub as comparing is not needed for a1
    return 0;
}
char* dateToString(void* date)
{
	char buffer[1000] = "";
    if (date == NULL)
    {
        return NULL;
    }
    char* str;
    DateTime* dateString = (DateTime*)date;
    if (dateString->isText == true)
    {

		strcpy(buffer,"Date\ndate:\ntime\nisUTC:false\nisText:true\ntext:");
		strcat(buffer,dateString->text);
		strcat(buffer,"\0");
		str = calloc(strlen(buffer)+1,sizeof(char));
		strcpy(str,buffer);
    }
    else if (dateString->isText == false)
    {
		strcpy(buffer,"Date\ndate:");
		strcat(buffer,dateString->date);
		strcat(buffer,"\ntime:");
		strcat(buffer,dateString->time);
		if (dateString->UTC == false)
		{
			strcat(buffer,"\nisUTC:false\n");
		}
		else if (dateString->UTC == true)
		{
			strcat(buffer,"\nisUTC:true\n");
		}
		strcat(buffer,"isText:false\n");
		strcat(buffer,"text:\n\0");
		str = calloc(strlen(buffer)+1,sizeof(char));
		strcpy(str,buffer);
    }

    return str;

}

void deleteProperty(void* toBeDeleted)
{
    Property* props = (Property*)toBeDeleted;
    if (props->name != NULL)
    {
		free(props->name);
	}
	if (props->group != NULL)
	{
		free(props->group);
	}


	freeList(props->parameters);
    freeList(props->values);

    free(props);
}
int compareProperties(const void* first,const void* second)
{
    return 0;
}

char* propertyToString(void* prop)
{
	
	//printf("Testing Prop to String");
	
	int length = 0;
    if (prop == NULL)
    {
        return NULL;
    }
    char* str;
    //char* parameters;
	//char *values;

	void* elem;
	void* elem2;
	void* elem3;
	void* elem4;

	
    Property *printProp = calloc(1,sizeof(Property));
    printProp = (Property*)prop;
    
	ListIterator iter1 = createIterator(printProp->parameters);
	
	ListIterator iter2 = createIterator(printProp->values);
	
	ListIterator iter3 = createIterator(printProp->parameters);
	
	ListIterator iter4 = createIterator(printProp->values);
	
	
    //parameter = toString(printProp->parameters);
    //values = toString(printProp->values);
	//printf("Name length = %d\n",strlen(printProp->name));
	//printf("group length = %d\n",strlen(printProp->group));
		//printf("Name len =%ld\n",strlen(printProp->name));
		//printf("group len =%ld\n",strlen(printProp->group));
    length = (strlen(printProp->name) + strlen(printProp->group) + 41);
    
	//printf("Testing 123\n");
	
	while ((elem = nextElement(&iter1)) != NULL)
	{
		//printf("Testing 456\n");
		Parameter* Param = (Parameter*)elem;
		//strcpy(param->name,xiter1->current);
		length += strlen(parameterToString((void*)Param));
		//printf("%s\n%s\n",param->name,param->value);
		
	}
	while ((elem2 = nextElement(&iter2)) != NULL)
	{
		//printf("Testing cheese\n");
		char* Param = (char*)elem2;
		length += strlen(valueToString((void*)Param));
	}


	// iteraotr for lists.

    str = calloc(length+1,sizeof(char));
    sprintf(str,"Property\nName:%s\nGroup:%s\n",printProp->name,printProp->group);

	while ((elem3 = nextElement(&iter3)) != NULL)
	{
		//printf("Testing adding parameter\n");
		Parameter* Param = (Parameter*)elem3;
		strcat(str,parameterToString((void*)Param));
	}
	while ((elem4 = nextElement(&iter4)) != NULL)
	{
		//printf("Testing adding values\n");
		char* Param = (char*)elem4;
		strcat(str,valueToString((void*)Param));
	}
	 
	return str;
	
	
	
	
    
}

void deleteParameter(void* toBeDeleted)
{
    if (toBeDeleted == NULL)
    {
        return;
    }
    Parameter* pram = (Parameter*)toBeDeleted;
    free(pram->name);
    free(pram->value);

    free(pram);
}
int compareParameters(const void* first,const void* second)
{
    return 0;
}
char* parameterToString(void* param)
{
    if (param == NULL)
    {
        return NULL;
    }
    Parameter* pram = calloc(1,sizeof(Parameter));
    pram = (Parameter*)param;
    char* string;
    int length;
    length = (strlen(pram->name) + strlen(pram->value) + 23);
    string = calloc(length+1,sizeof(char));
    sprintf(string,"Parameter\nName:%s\nValue:%s\n",pram->name,pram->value);
    strcat(string,"\0");


    return string;
}


int otherProperties(char* propertyValue,char *propertyName,Card* Vcard)
{
	//printf("Testing 1\n");
    Property *tempProp = calloc(1,sizeof(Property));
    
    

    tempProp->parameters = initializeList( &parameterToString, &deleteParameter, &compareParameters);
    tempProp->values = initializeList( &valueToString, &deleteValue, &compareValues);
    tempProp->name = NULL;
    tempProp->group = NULL;// temp values, will be reassigned assuming file is valid.
    int nameCheck = 0;
    int groupCheck = 0;
    //int checkProp = 0;

    char tempName[1000] ="";
    strcpy(tempName,propertyName);
    int semiCheck = 0;
    int equalCheck = 0;
    for(int i =0;i<strlen(tempName);i++)
    {
		if (tempName[i] == ';')
		{
			semiCheck++;
		}
		else if (tempName[i] == '=')
		{
			equalCheck++;
		}
		
	}
	if (equalCheck != semiCheck)
	{
		deleteProperty(tempProp);
		return 1;
	}
    char str[200] = "";
    const char s[2] = ";";                                              //The symbol you split from
    char *token = "\0";                                                 //Your token
    char temp2[200] = "";
    char temp1[200] = "";

    //printf("Original String: %s\n", tempName);
    strcpy(str,tempName);
    strcpy(temp2, str);
    //Creates a copy of the original string to a temp variable

    token = strtok(temp2, s); //Finds the first token before ";" **only run once**

    /* walk through other tokens */
    while (token != NULL)
    {
		//printf("Token = %s\n",token);
       // printf("left: %s\n", token); //This is very you assign token (left) to where you need it to go
		if ((strchr(token,'.') != NULL) &&(groupCheck == 0))
		{
			groupParser(token,tempProp);
			groupCheck = 1;
			nameCheck= 1;
		}
		else if ((strchr(token,'=') == NULL) &&(nameCheck == 0))
		{
			nameCheck = 1;
			//printf("Token name = %s",token);
			tempProp->name = calloc(strlen(token)+1,sizeof(char));
			strcpy(tempProp->name,token);
			strcat(tempProp->name,"\0");
			
		}

		else if (strchr(token,'=') != NULL)
		{
			//printf("Token sent to Param = %s",token);
			//printf("Token Testing = %s",token);
			//printf("Token Testing = %s",token);
			
			findParam(token,tempProp);
			
		}


        //To break loop when at the end of the string
        if (strcmp(token, tempName) == 0)
        {
			//printf("Token = %s\n",token);
			//printf("TempName = %s\n",tempName);
            break;
        }

		
		
        strcpy(temp1, tempName + strlen(token) + 1); //Copies everything from the original string minus the token to temp varible
        //printf("right: %s\n", temp1);           //This is the new string with the token part removed
        strcpy(tempName, temp1);                     //Copies everything from the new string to the second back to the original str varible
        //printf( "second strcpy: %s\n", str ); //Use to check if strcpy worked or not
        token = strtok(temp1, s); //Find the next token in using the temp variable
    }
    if (groupCheck == 0)
    {
		tempProp->group = calloc(1,sizeof(char));
		strcpy(tempProp->group,"\0");
	}
    
   

    // NOW CHECK FOR VALUES
    int checkVal = parseVal(tempProp,propertyValue);
    if (checkVal == 1)
    {
		deleteProperty(tempProp);
		return 1;
	}
	
    
    
    //printf("Testing VALUES\n");
    /*
    char tempVal[1000] ="\0";
    strcpy(tempVal,propertyValue);
    char *tokenVal;
    if (strchr(tempVal,';') != NULL )
    {
        tokenVal = strtok(tempVal,";");
        int i = 0;
        for(i=0;i<strlen(propertyValue)-1;i++)
        {
			if ((tempVal[i] == tempVal[i+1]) && (tempVal[i] == ';'))
			{
				// means an empty value is present 
				char *temp = calloc(1,sizeof(char));
				strcpy(temp,"\0");
				insertBack(tempProp->values,temp);
				
			}
		}

        while (tokenVal != NULL)
        {
			//printf("token = %s\n",tokenVal);
            char buffer[100] = "";
            strcpy(buffer,tokenVal);
            strcat(buffer,"\0");
            char* temp = calloc(strlen(buffer),sizeof(char));
            strcpy(temp,buffer);
            insertBack(tempProp->values,temp);
            tokenVal = strtok(NULL,";");
        }

    }
    else if (strchr(tempVal,';') == NULL )
    {
		//printf("ONLY 1 VALUE\n");
		//printf("Value is =%s\nStoredValue is=%s\n",*propertyValue,tempVal);
		char* temp = calloc(strlen(tempVal)+1,sizeof(char));
        strcpy(temp,tempVal);
        //strcat(temp,"\0");
        insertBack(tempProp->values,temp);
    }
    */
	//printf("Insert to VCARD\n");
    insertBack(Vcard->optionalProperties,tempProp);
   // printf("Inserting COMPLETE\n");
    return 0;
}

int parseVal(Property* tempProp,char* value)
{
	
	char buffer[1000] = "";
	char buffer1[1000] = "";
	char temp[1000] = "";
	strcpy(buffer,value);
	strcpy(buffer1,value);
	
	int posSemi = 0;
	int posVal = 0;
	//int length = 0;
	if (strchr(buffer,';') == NULL)
	{
		// only 1 value. 
		if (strlen(buffer) == 0)
		{
			// lone value is empty value
			char* str = calloc(1,sizeof(char));
			strcat(str,"\0");
			insertBack(tempProp->values,str); 
			return 0;
		}
		else if (strlen(buffer) > 0)
		{
			char* str = calloc(strlen(buffer)+1,sizeof(char));
			strcat(str,buffer);
			insertBack(tempProp->values,str);
			return 0;
		}
	
	}
	// multiple values exist. 
	for (int i = 0; i < strlen(buffer);i++)
	{
		if(buffer[i] == ';')
		{
			if(i == 0)
			{
				// empty case occured
				char* str = calloc(1,sizeof(char));
				strcat(str,"\0");
				insertBack(tempProp->values,str);
				posVal = i + 1; 
				posSemi = i;
				
			}
			else if (posSemi == (i-1))
			{
				// empty case occured.
				char* str = calloc(1,sizeof(char));
				strcat(str,"\0");
				insertBack(tempProp->values,str); 
				posVal = i + 1;
				posSemi = i;
			}
			else
			{
				
				// non empty string case.
				//strcpy(buffer,"");
				//strcpy(temp,"");
				//printf("temp = %s\n",temp);
				//printf("buffer =%s\n", buffer+posVal);
				strcpy(temp,buffer+posVal);
				char *token;
				token = strtok(temp,";");
				//printf("temp2 = %s\n",temp);
				//strcat(temp,"\0");
				posSemi = i;
				posVal = i + 1;
				char* str = calloc(strlen(token)+1,sizeof(char));
				//strcat(str,"\0");
				
				strcpy(str,token);
				strcat(str,"\0");
				insertBack(tempProp->values,str);
				
				
			}
			
			
		}
		
		//length = i;
	}
	strcpy(temp,buffer+posVal);
	strcat(temp,"\0");
	char * str = calloc(strlen(temp)+1,sizeof(char));
	strcpy(str,temp);
	insertBack(tempProp->values,str);
	
	
	
	/*
	char buffer[1000] = "\0";
	char buffer2[1000] = "\0";
	char* token;
	strcpy(buffer,value);
	int checkFirstSemi = 0;
	int checkDoubleSemi = 0;
	// this loop is ment to check what occurs first, an empty value, or a normal value, if multiple empty vals exist before first real val, stores them too.
	for (int i =0; i <strlen(buffer)-1;i++)
	{
		if ((buffer[i] == ';') && (buffer[i+1]==';') && (checkFirstSemi == 0))
		{
			checkDoubleSemi = 1;
			char *tempVal = calloc(1,sizeof(char));
			strcat(tempVal,"\0");
			insertBack(tempProp->values,tempVal);
		}
		else if ((buffer[i] == ';') && (buffer[i+1] != ';') )
		{
			checkFirstSemi = 1;
		}
		
	}
	strcpy(buffer2,buffer);// copies original string to temp buffer
	token = strtok(buffer,";");
	strcpy(buffer2,buffer2 + strlen(token)+1);
	while(token != NULL)
	{
		
		
		
	}
	
	
	// now to begin string splitting. 
	*/
	
	return 0;
}

void findParam(char input[200],Property* otherProps)
{
	//printf("Testing 2\n");
	
    Parameter* tempParam = calloc(1,sizeof(Parameter));
    /*
    char *token;
    char bufferName[100];
    char bufferVal[100];
    token = strtok(buffer,"=");


    strcpy(bufferName,token);
    strcat(bufferName,"\0");
    tempParam->name = calloc(strlen(bufferName)+1,sizeof(char));
    strcpy(tempParam->name,bufferName);
    token = strtok(NULL,"=");
    strcpy(bufferVal,token);
    tempParam->value = calloc(strlen(bufferVal)+1,sizeof(char));
    strcpy(tempParam->value,bufferVal);

    insertBack(otherProps->parameters,tempParam);

	*/
	char str[200] = "";
	const char s[2] = "=";                                              //The symbol you split from
    char *token = "\0";                                                 //Your token
    char temp2[200] = "";
    char temp1[200] = "";
    //int count = 0;
	int check = 0;
    //printf("Original String: %s\n", str);
    strcpy(str,input);
    strcpy(temp2, str); //Creates a copy of the original string to a temp variable

    token = strtok(temp2, s); //Finds the first token before ";" **only run once**

    /* walk through other tokens */
    while (token != NULL)
    {
		//printf(" ERROR Token Param = %s\n",token);
        //printf("left: %s\n", token); //This is very you assign token (left) to where you need it to go
        if (check == 0)
        {
			tempParam->name = calloc(strlen(token)+1,sizeof(char));
			strcpy(tempParam->name,token);
			strcat(tempParam->name,"\0");
			check = 1;
		}
		else if (check == 1)
		{
			check = 2;
			tempParam->value = calloc(strlen(token)+1,sizeof(char));
			strcpy(tempParam->value,token);
			strcat(tempParam->value,"\0");
			insertBack(otherProps->parameters,tempParam);
			//printf("HELLO\n");
			//return;
		}
		 
		
        //To break loop when at the end of the string
        if (strcmp(token, str) == 0)
        {
			//printf("HELLO\n");
            return;
        }

        strcpy(temp1, str + strlen(token) + 1); //Copies everything from the original string minus the token to temp varible
        //printf("right: %s\n", temp1);           //This is the new string with the token part removed
        strcpy(str, temp1);                     //Copies everything from the new string to the second back to the original str varible
        //printf( "second strcpy: %s\n", str ); //Use to check if strcpy worked or not
        token = strtok(temp1, s); //Find the next token in using the temp variable
    }

}
void groupParser(char str[1000],Property* otherProps)
{
	const char s[2] = ".";                                              //The symbol you split from
    char *token = "\0";                                                 //Your token
    char temp2[200] = "";
    char temp1[200] = "";
    int count = 0;
    int check = 0;
    int groupCheck = 0;

   // printf("Original String: %s\n", str);
    strcpy(temp2, str); //Creates a copy of the original string to a temp variable

    token = strtok(temp2, s); //Finds the first token before ";" **only run once**

    /* walk through other tokens */
   
    
    while (token != NULL)
    {

       // printf("left: %s\n", token); //This is very you assign token (left) to where you need it to go
		
		if (check == 0 )
		{
			check = 1;
			char buff[100] = "";
			strcpy(buff,token);
			strcat(buff,"\0");
			for (char* a = buff; *a != '\0';a++)
			{
				if (((isalnum(*a) == 0)&&(*a != '-')) || (*a == ' ') )
				{
					// invalid group name. 
					//printf("Group Invalid = %s\n",buff);
					groupCheck = 1;
					
				}
			} 
			if(groupCheck == 0)
			{
				//printf("Group Print = %s\n",token);
				otherProps->group = calloc(strlen(token)+1,sizeof(char));
				strcpy(otherProps->group,token);
				strcat(otherProps->group,"\0");
				count++;
			}
			else 
			{
				// invalid groupname. error. 
				return;
			}
		}
		else if (check == 1)
		{
			otherProps->name = calloc(strlen(token)+1,sizeof(char));
			strcpy(otherProps->name,token);
			strcat(otherProps->name,"\0");
			
		}
		
        //To break loop when at the end of the string
        if (strcmp(token, str) == 0)
        {
            break;
        }

        strcpy(temp1, str + strlen(token) + 1); //Copies everything from the original string minus the token to temp varible
        //printf("right: %s\n", temp1);           //This is the new string with the token part removed
        strcpy(str, temp1);                     //Copies everything from the new string to the second back to the original str varible
        //printf( "second strcpy: %s\n", str ); //Use to check if strcpy worked or not
        token = strtok(temp1, s); //Find the next token in using the temp variable
	}


}
int splitDateTime(char dayBuff[1000],DateTime* day)
{
	// T is not the beginning, date and Time are present. 
	char time[1000] = "";
	char date[1000] = "";
	char Tcheck = 0;
	int dateCount = 0;
	int timeCount = 0;
	int validDate = 0;
	int validTime = 0;
	for (int i = 0;i<strlen(dayBuff);i++)
	{
					
		if (dayBuff[i] == 'T')
		{
			Tcheck = 1;
		}
		
		if (Tcheck == 0)
		{
			// store date. 
			date[dateCount] = dayBuff[i];
			dateCount++;
						
		}
		else if ((Tcheck == 1) && (dayBuff[i] != 'T'))
		{
			// store Time.
			time[timeCount] = dayBuff[i];
			timeCount++;
		}
					
	}
				
	strcat(time,"\0");
	strcat(date,"\0");
	//VALIDATE DATE AND TIME.
	if (strchr(date,'-') == NULL)
	{
		if (strlen(date) == 8)
		{
			validDate = 1;
		}
	}
	else if (strchr(date,'-') != NULL)
	{
		if ((strlen(date) == 10) || (strlen(date) == 6) || (strlen(date) == 5) )
		{
			validDate = 1;
		}
	}
	if (strchr(time,'-') == NULL)
	{
					// no - in time 
		if(strlen(time) == 6)
		{
			validTime = 1;
		}
	}
	else if (strchr(time,'-') != NULL)
	{
		if ( (strlen(time) == 8) || (strlen(time) == 4) || (strlen(time) == 5) )
		{
			validTime = 1;
		}
	}
	if ((validTime != 1)||(validDate != 1))
	{
		return 1;
	}
				
	day->date = calloc(strlen(date)+1,sizeof(char));
	day->time = calloc(strlen(time)+1,sizeof(char));
	strcpy(day->date,date);
	strcpy(day->time,time);
	
	return 0;
}
int GetDateTime(char *propertyValue,char *propertyName,Card* newCardObject,int bdayAnniv)
{
	char buffer[1000] = "";
	DateTime *day = calloc(1,sizeof(DateTime));
	day->text = NULL;
	day->time = NULL;
	day->date = NULL;
	strcpy(buffer,propertyValue);
	int textCheck = 0;

	for (int i=0;i<strlen(buffer);i++)
	{
		if ((isalpha(buffer[i]) != 0)&&(buffer[i] != 'T') && (buffer[i] != 'Z'))
		{
			textCheck = 1;
		}
	}
	if (textCheck == 1)
	{
		//NO DATE/TIME.
		day->isText = true;
		day->UTC = false;
		day->date = calloc(1,sizeof(char));
		strcpy(day->date,"\0");
		day->time = calloc(1,sizeof(char));
		strcpy(day->time,"\0");
		day->text = calloc(strlen(buffer),sizeof(char));
		strcpy(day->text,buffer);
		if (bdayAnniv == 0)
        {
			newCardObject->birthday = day;
			// stores in bday var
		}
		if (bdayAnniv == 1)
		{
			newCardObject->anniversary = day;
			// stores in anniversary value.
		}
		
		return 0;
		
	}
	else if (textCheck == 0)
	{
		// NO TEXT
		day->isText = false;
		day->text = calloc(1,sizeof(char));
		strcpy(day->text,"\0");
		
	}
	if (strchr(buffer,'Z') != NULL)
	{
		//utc is true 
		day->UTC = true;
		char *token;
		char dayBuff[1000];
		token = strtok(buffer,"Z");
		strcpy(dayBuff,token);
		strcat(dayBuff,"\0");
		
		if (strchr(dayBuff,'T') == NULL)
		{
			int validDate = 0;
			// no time, only date. 
			if (strchr(dayBuff,'-') == NULL)
			{
				if (strlen(dayBuff) == 8)
				{
					validDate = 1;
				}
				
			}
			else if (strchr(dayBuff,'-') != NULL)
			{
				if ((strlen(dayBuff) == 10) || (strlen(dayBuff) == 6 ) || (strlen(dayBuff) == 5) )
				{
					validDate = 1;
				}
				
			}
			if (validDate == 0)
			{
				deleteDate(day);
				return 1;
			}
			day->time = calloc(1,sizeof(char));
			strcpy(day->time,"\0");
			day->date = calloc(strlen(dayBuff),sizeof(char));
			strcpy(day->date,dayBuff);
			
			if (bdayAnniv == 0)
			{
				newCardObject->birthday = day;
			// stores in bday var
			}
			if (bdayAnniv == 1)
			{
				newCardObject->anniversary = day;
			// stores in anniversary value.
			}
			return 0;
		}
		else if (strchr(dayBuff,'T') != NULL)
		{
			//time and or date is present 
			if (dayBuff[0] == 'T')
			{
				int validTime = 0;
				//no date only time 
				
				char timeBuffer[50];
				day->date = calloc(1,sizeof(char));
				strcpy(day->date,"\0");// stored empty date. 
				
				for (int i = 0; i<strlen(dayBuff)-1;i++)
				{
					timeBuffer[i] = dayBuff[i+1];
					// copies string post T to timeBuffer
				}
				if (strchr(timeBuffer,'-') == NULL)
				{
					// no - in time 
					if(strlen(timeBuffer) == 6)
					{
						validTime = 1;
					}
				}
				else if (strchr(timeBuffer,'-') != NULL)
				{
					if ( (strlen(timeBuffer) == 8) || (strlen(timeBuffer) == 4) || (strlen(timeBuffer) == 5) )
					{
						validTime = 1;
					}
				}
				if (validTime == 0)
				{
					deleteDate(day);
					return 1;
					//invalid time.
				}
				// VERIFY IF TIME IS VALID
				day->time = calloc(strlen(timeBuffer),sizeof(char));
				strcpy(day->time,timeBuffer);
				if (bdayAnniv == 0)
				{
					newCardObject->birthday = day;
					// stores in bday var
				}
				if (bdayAnniv == 1)
				{
					newCardObject->anniversary = day;
				// stores in anniversary value.
				}
				return 0;
			}
			else 
			{
				// T is not the beginning, date and Time are present. 
				int check = splitDateTime(dayBuff,day);
				if (check == 1)
				{
					// error return 1;
					deleteDate(day);
					return 1;
				}
				
				if (bdayAnniv == 0)
				{
					newCardObject->birthday = day;
					// stores in bday var
				}
				if (bdayAnniv == 1)
				{
					newCardObject->anniversary = day;
				// stores in anniversary value.
				}
				return 0;
				
			}

		} 
		
		
	}
	else if ( (strchr(buffer,'Z') == NULL))
	{
		day->UTC = false;
		
		char dayBuff[1000];
		strcpy(dayBuff,buffer);
		strcat(dayBuff,"\0");
		
		if (strchr(dayBuff,'T') == NULL)
		{
			int validDate = 0;
			//NO TIME ONLY DATE
			if (strchr(dayBuff,'-') == NULL)
			{
				if (strlen(dayBuff) == 8)
				{
					validDate = 1;
				}
				
			}
			else if (strchr(dayBuff,'-') != NULL)
			{
				if ((strlen(dayBuff) == 10) || (strlen(dayBuff) == 6 ) || (strlen(dayBuff) == 5) )
				{
					validDate = 1;
				}
				
			}
			if (validDate == 1)
			{
				day->time = calloc(1,sizeof(char));
				strcpy(day->time,"\0");
				
				day->date = calloc(strlen(dayBuff),sizeof(char));
				strcpy(day->date,dayBuff);
				//printf("PRinting date = %s\n",day->date);
				if (bdayAnniv == 0)
				{
					newCardObject->birthday = day;
					// stores in bday var
				}
				if (bdayAnniv == 1)
				{
					newCardObject->anniversary = day;
				// stores in anniversary value.
				}
				return 0;
			}
			else if (validDate == 0)
			{
				deleteDate(day);
				return 1;
			}
			
		}
		else if (strchr(dayBuff,'T') != NULL)
		{
		
			
			//time and or date is present 
			if (dayBuff[0] == 'T')
			{
				//no date only time 
				int validTime = 0;
				char timeBuffer[50] = "";
				day->date = calloc(1,sizeof(char));
				strcpy(day->date,"\0");// stored empty date. 
				
				for (int i = 0;i<strlen(dayBuff)-1;i++)
				{
					timeBuffer[i] = dayBuff[i+1];
					// copies string post T to timeBuffer
				}
				if (strchr(timeBuffer,'-') == NULL)
				{
					// no - in time 
					if(strlen(timeBuffer) == 6)
					{
						validTime = 1;
					}
				}
				else if (strchr(timeBuffer,'-') != NULL)
				{
					if ( (strlen(timeBuffer) == 8) || (strlen(timeBuffer) == 4) || (strlen(timeBuffer) == 5) )
					{
						validTime = 1;
					}
				}
				if (validTime == 1)
				{
				
					day->time = calloc(strlen(timeBuffer)+1,sizeof(char));
					strcpy(day->time,timeBuffer);
					//printf("DATE = %s\nTIME = %s\n",day->date,day->time);
					if (bdayAnniv == 0)
					{
						newCardObject->birthday = day;
					// stores in bday var
					}
					if (bdayAnniv == 1)
					{
						newCardObject->anniversary = day;
				// stores in anniversary value.
					}
					return 0;
				}
				else if (validTime == 0)
				{
					//invalid time
					deleteDate(day);
					return 1;
				}
			}
			else 
			{
				// T is not the beginning, date and Time are present. 
				int check = splitDateTime(dayBuff,day);
				if (check == 1)
				{
					// error return 1;
					deleteDate(day);
					return 1;
				}
				
				
				if (bdayAnniv == 0)
				{
					newCardObject->birthday = day;
					// stores in bday var
				}
				if (bdayAnniv == 1)
				{
					newCardObject->anniversary = day;
				// stores in anniversary value.
				}
				return 0;
				
			}
		}
	}
	
	

    return 0;
}

VCardErrorCode writeCard(const char* fileName, const Card* obj)
{
	char start[] = "BEGIN:VCARD\r\n";
	char ver[] = "VERSION:4.0\r\n";
	char end[] = "END:VCARD\r\n";
	// these strings represent the minimum required for a file(minus fn)
	if (fileName == NULL)
	{
		//file passed is null and thus invalid
		return WRITE_ERROR;
	}
	if (obj == NULL)
	{
		// card passed is null and thus invalid
		return WRITE_ERROR;
	}
	if (strstr(fileName,".vcf") == NULL)
	{
		//invalid file extension.
	}
	FILE* fp; 
	fp = fopen(fileName,"w");
	if (fp == NULL)
	{
		return WRITE_ERROR;
		// fopen did not work, might be invalid file error. 
	}
	
	
	//first write to file with begin line and version line.
	fprintf(fp,"%s%s",start,ver);
	
	// calls fn function and gets FN Value.
	char *fn = getFN(obj);
	
	fprintf(fp,"%s",fn);
	free(fn);
	//printf("FN = %s\n",fn);
	
	//checks and gets properties. 
	
	int numProps = getLength(obj->optionalProperties);
	//printf("Error check num properties\n");
	// checks if properties exist, if not, then doesnt call function to add to file, if they do, call function to add to file.
	
	if(numProps>0)
	{
		void* elem;

		ListIterator iter = createIterator(obj->optionalProperties);


		while ((elem = nextElement(&iter)) != NULL)
		{
			Property* props = (Property*)elem;
			addingProp(props,fp);
		}
	}
	if ((obj->birthday != NULL) || (obj->anniversary != NULL))
	{
		DateTime* day= NULL;
		 //= calloc(1,sizeof(DateTime));
		if (obj->birthday != NULL)
		{
			day = obj->birthday;
		//add date and time. for bday
			getDateTime(fp,day,0);
		}
		if (obj->anniversary != NULL)
		{
			day = obj->anniversary;
		// get date and time for anniversary
			getDateTime(fp,day,1);
		}
		//free(day);
		// freeing day 
	}
	//now end vcard end line.
	fprintf(fp,"%s",end);
	
	fclose(fp);
	//closes file as all writing to file is done. 
	
	
	return OK;
}
void getDateTime (FILE *fp, DateTime* obj, int Bday)
{
	char buffer[1000] = "";
	
	if (obj == NULL)
	{
		// bday or anniversary is not specified.
		return;
	}
	// if the above if is negative, means we can go ahead with geting date and or time.
	if (Bday == 0)
	{
		// is birthday.
		strcat(buffer,"BDAY:");
	}
	else if (Bday == 1)
	{
		strcat(buffer,"ANNIVERSARY:");
	}
	
	// now check if date is text. 
	
	if (obj->isText == true)
	{
		strcat(buffer,obj->text);
		strcat(buffer,"\r\n");
		fprintf(fp,"%s",buffer);
		return;
		// writes date as text to file and returns. 
	}
	else if (obj->isText == false)
	{
		// date or/and time are present. 
		// check if date is present. 
		if (strlen(obj->date) == 0)
		{
			// there is no date present, check and store time.
			strcat(buffer,"T");
			strcat(buffer,obj->time);
			// check if utc
			if (obj->UTC == true)
			{
				// add Z to end , add line ending, add to file and return.
				strcat(buffer,"Z");
				strcat(buffer,"\r\n");
				fprintf(fp,"%s",buffer);
				return;
			}
			else if (obj->UTC == false)
			{
				// not utc so no need to add Z, add line ending add to file and return.
				strcat(buffer,"\r\n");
				fprintf(fp,"%s",buffer);
				return;
			}
			
		}
		else if (strlen(obj->date) > 0)
		{
			// date is present, store and check for time. 
			strcat(buffer,obj->date);
			// now check for time.
			if (strlen(obj->time) == 0)
			{
				// no time add line ending and return.
				strcat(buffer,"\r\n");
				fprintf(fp,"%s",buffer);
				return;
			}
			else if (strlen(obj->time) >0)
			{
				// time is also present.
				strcat(buffer,"T");
				strcat(buffer,obj->time);
				// now check if UTC is true.
				if (obj->UTC == true)
				{
					// add Z to end , add line ending, add to file and return.
					strcat(buffer,"Z");
					strcat(buffer,"\r\n");
					fprintf(fp,"%s",buffer);
					return;
				}
				else if (obj->UTC == false)
				{
					// not utc so no need to add Z, add line ending add to file and return.
					strcat(buffer,"\r\n");
					fprintf(fp,"%s",buffer);
					return;
				}
			}
		}
		
	}
	
	
}

char* getFN(const Card* obj)
{
	char *str; 
	char buffer[1000] = "";
	strcat(buffer,"FN:");
	Property* FN = obj->fn;
	void* elem;

	ListIterator iter = createIterator(FN->values);


	while ((elem = nextElement(&iter)) != NULL)
	{
		char* val = (char*)elem;
		strcat(buffer,val);
	}
	strcat(buffer,"\r\n");
	str = calloc(strlen(buffer)+1,sizeof(char));
	strcpy(str,buffer);
	
	return str;
}
void addingProp(Property* prop,FILE *fp)
{
	int numParam = 0;
	char buffer[1000] = "";
	if (strlen(prop->group) == 0)
	{
		// group is empty.
		strcat(buffer,prop->name);
		//copies propertyname to string, if parameters exist, add to string, else add : to string. 
	}
	else if (strlen(prop->group) > 0)
	{
		// this property has a group, adds groupname then "." and then property name.
		strcat(buffer,prop->group);
		strcat(buffer,".");
		strcat(buffer,prop->name);
	}
	
	// now to check if parameters exist.
	numParam = getLength(prop->parameters);
	if (numParam == 0)
	{
		// no parameters, add : and move onto adding values.
		strcat(buffer,":");
	}
	if (numParam >0)
	{
		// parameters exist
		int count = 0;
		strcat(buffer,";");// means parameters exist.
		void* elem;
		ListIterator iter = createIterator(prop->parameters);
		
		while ((elem = nextElement(&iter)) != NULL)
		{
			count++;
			Parameter* param = (Parameter*)elem;
			strcat(buffer,param->name);
			strcat(buffer,"=");
			strcat(buffer,param->value);
			
			if (count <numParam)
			{
				strcat(buffer,";");// adds ; to end of each parameter, unless parameter is last parameter.
			}
			
		}
		strcat(buffer,":");
		//adds break from value at end of all params.
		
	}
	
	
	// adding values to print out whole. 
	
	int numVals = 0;
	numVals = getLength(prop->values);
	int valCount = 0;
	void* elem;
	ListIterator iter = createIterator(prop->values);
	
	while ((elem = nextElement(&iter)) != NULL)
	{
		valCount++;
		char *str = (char*)elem;
		strcat(buffer,str);
		if (valCount < numVals)
		{
			// not last value, so add ; to end of string.
			strcat(buffer,";");
		}
		
		
		
	}
	strcat(buffer,"\r\n");
	fprintf(fp,"%s",buffer);
	return;
	
}

VCardErrorCode validateCard(const Card* obj)
{
	int checkBday  = 0;
	int checkAnniv = 0;
	int checkFN = 0;
	if (obj == NULL)
	{
		return INV_CARD;
		// card is invalid.
	}
	if (obj->fn == NULL)
	{
		return INV_CARD;
	}
	else if (obj->fn != NULL)
	{
		checkFN = validateFn(obj->fn);
	}
	if (checkFN == 1)
	{
		// error return error case.
		return INV_CARD;
	}
	
	
	if (obj->birthday != NULL)
	{
		checkBday = validateDate(obj->birthday);
		if (checkBday == 1)
		{
			// error case, return.
			return INV_DT;
		}
	}
	if (obj->anniversary != NULL)
	{
		checkAnniv = validateDate(obj->anniversary);
		if (checkAnniv == 1)
		{
			// error, return.
			return INV_DT;
			
		}
	}
	if (obj->optionalProperties == NULL)
	{
		// return error case.
		return INV_CARD;
	}
	else if (obj->optionalProperties != NULL)
	{
		int x = validateStruct(obj);
		if (x == 1)
		{
			return INV_PROP;
		// error occured.
		
		}
		if (x == 2)
		{
			// version in other prop list. error
			return INV_CARD;
		}
		if (x == 3)
		{
			return INV_DT;
		}
	}
	
	return OK;
}

int validateDate(DateTime* day)
{
	if ((day->text == NULL)||(day->date == NULL)||(day->time == NULL))
	{
		// error, cannot be null should be empty.
		return 1;
	}
	if ((strlen(day->text) == 0) &&(strlen(day->date) == 0) && (strlen(day->time) == 0))
	{
		// all cannot be empty strings.
		return 1;
	}
	if ((day->UTC == true) && (strlen(day->text)>0))
	{
		//cannot have utc as true, with text date.
		return 1;
	}
	if ((strlen(day->text)>0) && (strlen(day->date)>0) && (strlen(day->time)>0))
	{
		// cant have text, date and time at the same time, something must be empty.
		return 1;
	}
	if ( (day->isText == true) && ( (strlen(day->date) > 0) || (strlen(day->time) > 0) ) )
	{
		// cannot have text as date, and have data in date and time, must be empty string 
		return 1;
	}
	if ( (day->isText == false) && (strlen(day->text)>0 ) )
	{
		// if date is not text version, text string must be empty, but here, string is not empty.
		return 1;
	}
	if ( (day->isText == true) && (strlen(day->text)==0) )
	{
		// date cannot be text, but also empty.
		return 1;
	}
	
	
	// validate date and or time.
	if (strlen(day->date) >0)
	{
		if (strchr(day->date,'-') != NULL)
		{
			// means - are present, could be truncation or yyyy-mm-dd format
			if ((strlen(day->date) != 4) && (strlen(day->date) != 5) && (strlen(day->date) != 10) )
			{
				return 1;
				//invalid date length.
			}
			
		}
		else if (strchr(day->date,'-') == NULL)
		{
			if (strlen(day->date) != 8)
			{
				return 1;
				// invalid date 
			}
		}
	}
	if (strlen(day->time) >0)
	{
		if (strchr(day->time,'-') != NULL)
		{
			if ((strlen(day->time) != 4) && (strlen(day->time) != 5) && (strlen(day->date) != 8) )
			{
				return 1;
				//invalid time.
			}
		}
		else if(strchr(day->time,'-') != NULL)
		{
			if (strlen(day->time) != 6)
			{
				return 1;
				//invalid length.
			}
		}
		
	}
	return 0;
}
int validateFn(Property* fn)
{

	int length = 0;
	if (fn->name == NULL)
	{
		// error. 
		return 1;
	}
	if (strcmp(fn->name,"FN") != 0)
	{
		// fn name is not FN. 
		return 1;
	}
	if (fn->group == NULL)
	{
		return 1;
		//invalid group can be empty string, cannot be null
	}
	if (fn->parameters == NULL)
	{
		// cannot be null.
		return 1;
	}
	if (fn->values == NULL)
	{
		// invalid, cannot be NULL
		return 1;
	}
	else if (fn->values != NULL)
	{
		length = getLength(fn->values);
		if (length == 0)
		{
			// error must have 1 value atleast.
			return 1;
		}
		else if (length >1)
		{
			// can only have 1 value not more than 1
			return 1;
		}
		else 
		{
			int x = validateValues(fn);
			if ( x == 1)
			{
				return 1;
			}
		}
	}
	
	// all validation clear, fn is ok.
	return 0;
	
}

int validateStruct(const Card* obj)
{
	// first must validate properties against names.
	int propList = getLength(obj->optionalProperties);
	if (propList>0)
	{
		// means list of properties exist, need to iterate.
		
		void *elem;
		ListIterator iter = createIterator(obj->optionalProperties);
		while ((elem = nextElement(&iter)) != NULL)
		{
			Property* tempProp = (Property*)elem;
			char buffer[100] = "";
			strcpy(buffer,tempProp->name);
			int valid = 0;
			if (tempProp->name == NULL)
			{
				// error
				return 1;
			}
			else if (tempProp->name != NULL)
			{
				if (strlen(tempProp->name) == 0)
				{
					// error name cannot be empty.
					return 1;
				}
			}
			if (tempProp->group == NULL)
			{
				return 1;
			}

			if (tempProp->values == NULL)
			{
				// error cannot be null
				return 1;
			}
			if (tempProp->parameters == NULL)
			{
				// invalid cannot be null.
				return 1;
			}
			// call function to validate prop name against valid properties.
			valid = validatePropName(buffer,tempProp,obj);
			if (valid == 1)
			{
					// invalid property name,error.
					 return 1;
			}
			else if (valid == 2)
			{
				return 2;
			}
			else if (valid == 3)
			{
				return 3;
			}
			valid = validatePropParam(tempProp);
			if (valid != 0)
			{
					// invalid property name,error.
					 return 1;
			}
			valid = validateValues(tempProp);
			if (valid != 0)
			{
					// invalid property name,error.
					 return 1;
			}
		}
	}
	
	
	return 0;
}
int validateValues(Property* prop)
{
	int length = 0;
	length = getLength(prop->values);
	if (length == 0)
	{
		// error no values.
		return 1;
	}
	
	void* elem;
	ListIterator iter = createIterator(prop->parameters);
		
	while ((elem = nextElement(&iter)) != NULL)
	{
		char* str = (char*)elem;
		if (str == NULL)
		{
			// value cannot be null.
			return 1;
		}
		
	}
	
	return 0;
}
int validatePropParam(Property* prop)
{
	int length = 0;
	length = getLength(prop->parameters);
	if (length > 0)
	{
		// iterate through list, and validate values for null and empty strings.
		void* elem;
		ListIterator iter = createIterator(prop->parameters);
		
		while ((elem = nextElement(&iter)) != NULL)
		{
			Parameter* param = (Parameter*)elem;
			
			if ((param->name == NULL) ||(param->value == NULL))
			{
				// invalid cannot be null.
				return 1;
			}
			if ((strlen(param->name) == 0) || (strlen(param->value) == 0))
			{
				// invalid param name or value.
				return 1;
			}
			
		}
		
	}
	
	return 0;
}
int validatePropName(char buffer[100],Property* currProp,const Card* obj)
{
	int checkVals = 0;
	if (strcmp(buffer,"BEGIN") == 0)
	{
		//error cannot have a duplicate begin
		return 2;
	}
	
	else if (strcmp(buffer,"END") == 0)
	{
		// invalid cannot have duplicate end.
		return 2;
	}
	
	else if (strcmp(buffer,"SOURCE") == 0)
	{
		// can appear any number of times, but each source is uri so it is 1 or more values.
		if (currProp->values != NULL)
		{
			checkVals = getLength(currProp->values);
			if (checkVals < 1)
			{
				return 1;
				// error.
			}
		}
		else if (currProp->values == NULL)
		{
			return 1;
			// values cannot be null
		}
		return 0;
	}
	
	else if (strcmp(buffer,"KIND") == 0)
	{
		// can only appear once, and can only have 1 value.
		//create iterator to loop through other properties and check if KIND appears more than once.
		void* elem;
		int count = 0;
		ListIterator iter = createIterator(obj->optionalProperties);
		
		while ((elem = nextElement(&iter)) != NULL)
		{
			Property* prop = (Property*)elem;
			if (strcmp(prop->name,"KIND") == 0)
			{
				count++;
			}
		}
		if (count > 1)
		{
			// error cannot have multiple KIND properties.
			return 1;
		}
		
		// now checking if property has only 1 value.
		if (currProp->values != NULL)
		{
			checkVals = getLength(currProp->values);
			if (checkVals != 1)
			{
				// too many or too few values, error.
				return 1;
			}
		}
		else if (currProp->values == NULL)
		{
			// error cannot be NULL.
			return 1;
		}
		
		return 0;
	}
		
	else if (strcmp(buffer,"XML") == 0)
	{
		// can appear any number of times, but can only have 1 value.
		checkVals = getLength (currProp->values);
		if (checkVals != 1)
		{
			// invalid number of values, either too many or too few.
			return 1;
		}
		return 0;
	}
	else if (strcmp(buffer,"FN") == 0)
	{
		// fn must appear once or more, and has 1 value.
		checkVals = getLength (currProp->values);
		if (checkVals != 1)
		{
			// invalid number of values, either too many or too few.
			return 1;
		}
		return 0;
	}	
		
	else if (strcmp(buffer,"N") == 0)
	{
		// can only appear once, with 5 values.
		void* elem;
		int count = 0;
		ListIterator iter = createIterator(obj->optionalProperties);
		
		while ((elem = nextElement(&iter)) != NULL)
		{
			Property* prop = (Property*)elem;
			if (strcmp(prop->name,"N") == 0)
			{
				count++;
			}
		}
		if (count > 1)
		{
			// error cannot have multiple KIND properties.
			return 1;
		}
		
		checkVals = getLength(currProp->values);
		if (checkVals != 5)
		{
			// must have 5 props error.
			return 1;
		}
		return 0;
	}	

	else if (strcmp(buffer,"NICKNAME") == 0)
	{
		checkVals = getLength(currProp->values);
		if (checkVals <1 )
		{
			// error. must have 1 or more values.
			return 1;
		}
		return 0;
	}	
	
	else if (strcmp(buffer,"PHOTO") == 0)
	{
		// any number of photos can appear, but each property must have 1 value.
		checkVals = getLength(currProp->values);
		if (checkVals < 1 )
		{
			// error. must have 1 value.
			return 1;
		}
		return 0;
	}	
		
	else if (strcmp(buffer,"BDAY") == 0)
	{
		// duplicate birthday. error.
		return 3;
	}	
	
	else if (strcmp(buffer,"ANNIVERSARY") == 0)
	{
		// duplicate anniversary. error.
		return 3;
	}	

	else if (strcmp(buffer,"GENDER") == 0)
	{
		// can appear exactly 1 time, with 1 value.
		void* elem;
		int count = 0;
		ListIterator iter = createIterator(obj->optionalProperties);
		// check if only 1 gender prop in list of props.
		while ((elem = nextElement(&iter)) != NULL)
		{
			Property* prop = (Property*)elem;
			if (strcmp(prop->name,"GENDER") == 0)
			{
				count++;
			}
		}
		if (count > 1)
		{
			// error cannot have multiple Gender properties.
			return 1;
		}
		// now check if value = 1
		checkVals = getLength(currProp->values);
		if ((checkVals != 1 ) && (checkVals !=2))
		{
			// error. must have 1 or 2 value.
			return 1;
		}
		
		return 0;
	}	
	
	else if (strcmp(buffer,"ADR") == 0)
	{
		// NEED TO VERIFY THIS ONE ...
		// can appear any number of times, but value must be 1.
		checkVals = getLength(currProp->values);
		if (checkVals != 7)
		{
			// error not correct number of values.
			return 1;
		}
		return 0;
	}	
	
	else if (strcmp(buffer,"TEL") == 0)
	{
		// can appear any number of times but values must be 1.
		checkVals = getLength(currProp->values);
		if (checkVals != 1)
		{
			// error not correct number of values.
			return 1;
		}
		return 0;
	}	
	
	else if (strcmp(buffer,"EMAIL") == 0)
	{
		// can appear any number of times but only 1 value.
		checkVals = getLength(currProp->values);
		if (checkVals != 1)
		{
			// error not correct number of values.
			return 1;
		}
		return 0;
	}	
	
	else if (strcmp(buffer,"IMPP") == 0)
	{
		// any number of times can appear, but since uri, can have 1 or more values.
		checkVals = getLength(currProp->values);
		if (checkVals < 1)
		{
			// error not correct number of values.
			return 1;
		}
		return 0;
	}
	
	else if (strcmp(buffer,"LANG") == 0)
	{
		// appear any number of times, but only 1 value.
		
		checkVals = getLength(currProp->values);
		if (checkVals != 1)
		{
			// error not correct number of values.
			return 1;
		}
		
		return 0;
	}	
	
	else if (strcmp(buffer,"TZ") == 0)
	{
		// can appear any number of times, but only 1 value.
		
		checkVals = getLength(currProp->values);
		if (checkVals != 1)
		{
			// error not correct number of values.
			return 1;
		}
		
		return 0;
	}	

	else if (strcmp(buffer,"GEO") == 0)
	{
		// can appear any number of times, but uri so can have multiple values.
		
		checkVals = getLength(currProp->values);
		if (checkVals < 1)
		{
			// error not correct number of values.
			return 1;
		}
		return 0;
	}
	
	else if (strcmp(buffer,"TITLE") == 0)
	{
		// can appear any number of times, but only 1 value.
		
		checkVals = getLength(currProp->values);
		if (checkVals != 1)
		{
			// error not correct number of values.
			return 1;
		}
		return 0;
	}	
	
	else if (strcmp(buffer,"ROLE") == 0)
	{
		// can appear any number of times, but only 1 value.
		
		checkVals = getLength(currProp->values);
		if (checkVals != 1)
		{
			// error not correct number of values.
			return 1;
		}
		return 0;
	}	
	
	else if (strcmp(buffer,"LOGO") == 0)
	{
		// can appear any number of times, but only 1 value, but since uri we treat as any number of values.
		
		checkVals = getLength(currProp->values);
		if (checkVals < 1)
		{
			// error not correct number of values.
			return 1;
		}
		return 0;
	}	

	else if (strcmp(buffer,"ORG") == 0)
	{
		// can appear any number of times, but only 1 value.
		
		checkVals = getLength(currProp->values);
		if (checkVals < 1)
		{
			// error not correct number of values.
			return 1;
		}
		return 0;
	}
	
	else if (strcmp(buffer,"MEMBER") == 0)
	{
		// can appear any number of times, but only 1 value.URI so can appear more than once.
		
		checkVals = getLength(currProp->values);
		if (checkVals < 1)
		{
			// error not correct number of values.
			return 1;
		}
		return 0;
	}	
	
	else if (strcmp(buffer,"RELATED") == 0)
	{
		// can appear any number of times, but only 1 value. As a uri we treat as any number of values.
		
		checkVals = getLength(currProp->values);
		if (checkVals < 1)
		{
			// error not correct number of values.
			return 1;
		}
		return 0;
	}
	
	else if (strcmp(buffer,"CATEGORIES") == 0)
	{
		// can appear any number of times, any number of values
		
		checkVals = getLength(currProp->values);
		if (checkVals < 1)
		{
			// error not correct number of values.
			return 1;
		}
		return 0;
	}	

	else if (strcmp(buffer,"NOTE") == 0)
	{
		// can appear any number of times, but only 1 value.
		
		checkVals = getLength(currProp->values);
		if (checkVals != 1)
		{
			// error not correct number of values.
			return 1;
		}
		return 0;
	}	
	
	else if (strcmp(buffer,"PRODID") == 0)
	{
		// appears 0-1 times, and only 1 value.
		void* elem;
		int count = 0;
		ListIterator iter = createIterator(obj->optionalProperties);
		// check if only 1 gender prop in list of props.
		while ((elem = nextElement(&iter)) != NULL)
		{
			Property* prop = (Property*)elem;
			if (strcmp(prop->name,"PRODID") == 0)
			{
				count++;
			}
		}
		if (count > 1)
		{
			// error cannot have multiple PRODID properties.
			return 1;
		}		
			
		checkVals = getLength(currProp->values);
		if (checkVals != 1)
		{
			// error not correct number of values.
			return 1;
		}
		return 0;
	}	
	
	else if (strcmp(buffer,"REV") == 0)
	{
		// appears 0-1 times, and only 1 value.
		void* elem;
		int count = 0;
		ListIterator iter = createIterator(obj->optionalProperties);
		// check if only 1 gender prop in list of props.
		while ((elem = nextElement(&iter)) != NULL)
		{
			Property* prop = (Property*)elem;
			if (strcmp(prop->name,"REV") == 0)
			{
				count++;
			}
		}
		if (count > 1)
		{
			// error cannot have multiple REV properties.
			return 1;
		}		
			
		checkVals = getLength(currProp->values);
		if (checkVals != 1)
		{
			// error not correct number of values.
			return 1;
		}
		return 0;
	}	
	
	else if (strcmp(buffer,"SOUND") == 0)
	{
		//can appear any number of times, but only 1 value.	As a URI can have multiple values
			
		checkVals = getLength(currProp->values);
		if (checkVals < 1)
		{
			// error not correct number of values.
			return 1;
		}
		return 0;
	}	
	
	else if (strcmp(buffer,"UID") == 0)
	{
		// appears 0-1 times, and only 1 value.
		void* elem;
		int count = 0;
		ListIterator iter = createIterator(obj->optionalProperties);
		// check if only 1 UID prop in list of props.
		while ((elem = nextElement(&iter)) != NULL)
		{
			Property* prop = (Property*)elem;
			if (strcmp(prop->name,"UID") == 0)
			{
				count++;
			}
		}
		if (count > 1)
		{
			// error cannot have multiple UID properties.
			return 1;
		}		
			
		checkVals = getLength(currProp->values);
		if (checkVals < 1)
		{
			// error not correct number of values. has 1 or more as it is a URI
			return 1;
		}
		return 0;
	}	
	
	else if (strcmp(buffer,"CLIENTPIDMAP") == 0)
	{
	// can appear any number of times, but only 2 values.
			
		checkVals = getLength(currProp->values);
		if (checkVals != 2)
		{
			// error not correct number of values.
			return 1;
		}
		return 0;
	}
	
	else if (strcmp(buffer,"URL") == 0)
	{
		// can appear any number of times, but 1 value per appearance.
		checkVals = getLength(currProp->values);
		if (checkVals < 1)
		{
			// error not correct number of values. as a uri it can have 1 or more
			return 1;
		}
		return 0;
	}	
	
	else if (strcmp(buffer,"VERSION") == 0)
	{
		// error duplicate version, should only appear once
		return 2;
	}	
	
	else if (strcmp(buffer,"KEY") == 0)
	{
		// can apear any number of times, with max 1 value per.
		
		checkVals = getLength(currProp->values);
		if (checkVals < 1)
		{
			// error not correct number of values.
			return 1;
		}
		return 0;
	}	
	
	else if (strcmp(buffer,"FBURL") == 0)
	{
		// can apear any number of times, with max 1 value per.
		
		checkVals = getLength(currProp->values);
		if (checkVals < 1)
		{
			// error not correct number of values.
			return 1;
		}
		return 0;
	}	
	
	else if (strcmp(buffer,"CALADURI") == 0)
	{
		// can apear any number of times, with max 1 value per.
		
		checkVals = getLength(currProp->values);
		if (checkVals < 1)
		{
			// error not correct number of values.
			return 1;
		}
	
		return 0;
	}	
	
	else if (strcmp(buffer,"CALURI") == 0)
	{
		// can apear any number of times, with max 1 value per.
		
		checkVals = getLength(currProp->values);
		if (checkVals < 1)
		{
			// error not correct number of values.
			return 1;
		}
		
		return 0;
	}	
	else 
	{
		return 1;
		//prop name is not any of above names, return error.
	}
	
	return 1;
}

void addProperty(Card* card, const Property* toBeAdded)
{
	//int check = 0;
	if (card == NULL)
	{
		return;
	}
	if (toBeAdded == NULL)
	{
		return;
	}
	if (toBeAdded->name == NULL)
	{
		return;
	}
	if (toBeAdded->group == NULL)
	{
		return;
	}
	if (toBeAdded->values == NULL)
	{
		return;
	}
	if (toBeAdded->parameters == NULL)
	{
		return;
	}
	if (strlen(toBeAdded->name) == 0)
	{
		return;
	}
	if (card->optionalProperties == NULL)
	{
		return;	
	}
	
	// if all that is clear, will add to optional properties list.
	insertBack(card->optionalProperties,(Property*)toBeAdded);
	
}
char* strListToJSON(const List* strList)
{
	if (strList == NULL)
	{
		return NULL;
	}
	char buffer[2000] = "";
	strcat(buffer,"[");
	int check = 0;
	int num = getLength((List*)strList);
	
	void* elem;
	ListIterator iter = createIterator((List*)strList);
	while ((elem = nextElement(&iter)) != NULL)
	{
		check++;
		char* val = (char*)elem;
		strcat(buffer,"\"");
		strcat(buffer,val);
		strcat(buffer,"\"");
		if (check == num)
		{
			// do not add , to end of string.
		}
		else if (check < num)
		{
			strcat(buffer,",");
		}
		
	}
	
	strcat(buffer,"]\0");
	
	char *str = calloc(strlen(buffer)+1, sizeof(char));
	strcpy(str,buffer);
	
	return str;
}
char* dtToJSON(const DateTime* prop)
{
	char buffer[2000] = "";
	char *str = NULL;
	if (prop == NULL)
	{
		str = calloc(1,sizeof(char));
		strcpy(str,"\0");
		return str;
	}
	strcat(buffer, "{");
	strcat(buffer,"\"isText\":");
	if (prop->isText == false)
	{
		strcat(buffer,"false,");
	}
	else if (prop->isText == true)
	{
		strcat(buffer,"true,");		
	}
	strcat(buffer,"\"date\":\"");
	strcat(buffer,prop->date);
	strcat(buffer,"\",\"time\":\"");
	strcat(buffer,prop->time);
	strcat(buffer,"\",\"text\":\"");
	strcat(buffer,prop->text);
	strcat(buffer,"\",\"isUTC\":");
	if (prop->UTC == false)
	{
		strcat(buffer,"false}\0");
	}
	else if (prop->UTC == true)
	{
		strcat(buffer,"true}\0");
	}
	str = calloc(strlen(buffer)+1,sizeof(char));
	strcpy(str,buffer);
	
	return str;
}

char* propToJSON(const Property* prop)
{
	char* str = NULL;
	char buffer[2000] = "";
	if (prop == NULL)
	{
		str = calloc(1,sizeof(char));
		strcpy(str,"\0");
	}
	strcat(buffer,"{\"group\":\"");
	strcat(buffer,prop->group);
	strcat(buffer,"\",\"name\":\"");
	strcat(buffer,prop->name);
	strcat(buffer,"\",\"values\":");
	strcat(buffer,strListToJSON(prop->values));
	strcat(buffer,"}\0");
	
	str = calloc(strlen(buffer)+1, sizeof(char));
	strcpy(str,buffer);
	
	return str;
}

Card* JSONtoCard(const char* str)
{
	if(str == NULL)
	{
		return NULL;
	}
	
	Card* obj = calloc(1,sizeof(Card));
	obj->optionalProperties = initializeList( &propertyToString, &deleteProperty, &compareProperties);
	obj->birthday = NULL;
	obj->anniversary = NULL;
   char buffer[1000] = "";
   strcpy(buffer,str);
   int checkColon =0;
   int checkQuote = 0;
   int count = 0;
   char fnName[1000];
   for (int i =0; i<strlen(buffer);i++)
   {
       if (buffer[i] == ':')
       {
           checkColon = 1;
       }
       if ((checkColon == 1) && (buffer[i] == '"'))
       {
           checkQuote++;
           
       }
       if ((checkColon == 1) && (checkQuote == 1) && (buffer[i] != '"'))
       {
           fnName[count] = buffer[i];
           count++;
           
       }
       if ((checkColon == 1) && (checkQuote > 1))
       {
		   // end case.
		   fnName[count] = '\0';
	   }
       
   }
   
	Property* fn = calloc(1,sizeof(Property));
	fn->group = calloc(1,sizeof(char));
	strcat(fn->group,"\0");
	fn->name = calloc(3,sizeof(char));
	strcat(fn->name, "FN\0");
	fn->parameters = initializeList(&parameterToString,&deleteParameter,&compareParameters);
	fn->values = initializeList(&valueToString,&deleteValue,&compareValues);
	
	char *val = calloc(strlen(fnName)+1,sizeof(char));
	strcpy(val,fnName);
	insertBack(fn->values,val);
	
	return obj;
}
List* JSONtoStrList(const char* str)
{
	if (str == NULL)
	{
		return NULL;
	}
	List* values = initializeList(&valueToString,&deleteValue,&compareValues);
	
  char buffer2[1000] = "";
  strcpy(buffer2,str);
  int semiCheck = 0; 
  //int commaCount = 0;
  char val[1000]= "";
  int count = 0;
  
  for (int i =0; i <strlen(buffer2);i++)
  {
      if (buffer2[i] == '"')
      {
          // this is where strin count begins
          semiCheck++;
          
      }
      if ((semiCheck == 1) && (buffer2[i] != '"'))
      {
          // starts storing string
          val[count] = buffer2[i];
         // printf("char = %c\n",val[count]);
          count++;
      }
      if ((semiCheck ==2) && (buffer2[i] == '"'))
      {
          val[count] = '\0';
          char* stringVal;
          stringVal= calloc(strlen(val)+1,sizeof(char));
          strcpy(stringVal,val);
          strcat(stringVal,"\0");
          // will be inserted to back of list here.
          //printf("Value = %s\n",str);
          insertBack(values,stringVal);
      }
      if ((buffer2[i] == ',') && (semiCheck > 1))
      {

          semiCheck = 0;
     
        strcpy(val,"\0");

          count = 0;
          
          
      }
      
      
  }	
	
	return values;
}
DateTime* JSONtoDT(const char* str)
{
	if (str == NULL)
	{
		return NULL;
	}
	DateTime* day = calloc(1,sizeof(DateTime));
	char buffer[1000] = "";
	strcpy(buffer,str);
    char tempVal[1000] = "";
    int count = 0;
    for(int i =0; i < strlen(buffer);i++)
    {
        
        if (( (buffer[i] == ',') && (buffer[i+1] == '"')) || (buffer[i] == '}'))
        {
            tempVal[count] = '\0';
            //printf("String = %s\n",tempVal);
            // call function here.
            splitValName(tempVal,day);
            // means this is the end of current parsing value.call spliting function, and then reset values.
            //reset values now.
            strcpy(tempVal,"");
            count = 0;
        }
        else
        {
            tempVal[count] = buffer[i];
            count++;
        }
        
    }
    return day;
	
	
	
}
void splitValName(char buffer[1000], DateTime* day)
{
    char val[100];
    char name[100];
    int colonCheck = 0;
    int valCount = 0;
    int nameCount = 0;
    for (int i = 0; i <strlen(buffer);i++)
    {
        if (buffer[i] == ':')
        {
            name[nameCount] = '\0';
            colonCheck ++;
        }
        if ((colonCheck == 0) && (buffer[i] != '"') && (buffer[i] != '{'))
        {
            name[nameCount] = buffer[i];
            nameCount++;
        }
        if ((colonCheck != 0) && (buffer[i] != '"') && (buffer[i] != ':'))
        {
            val[valCount] = buffer[i];
            valCount++;
        }
        if ((colonCheck != 0) && ((buffer[i] == '"') || (buffer[i] =='}')))
        {
            val[valCount] = '\0';
        }
        if (i == strlen(buffer)-1)
        {
            val[valCount] = '\0';
        }
    }
    
    if (strcmp(name,"isText") == 0)
    {
        // is text.
        if (strcmp(val,"true") == 0 )
        {
			day->isText = true;
            // set isText to true.
        }
        else if (strcmp(val,"false") == 0)
        {
			day->isText = false;
            // set isText to false.
        }
    }
    else if(strcmp(name,"date") == 0)
    {
        // check and store date prop to date param.
        day->date = calloc(strlen(val)+1,sizeof(char));
        strcpy(day->date, val);
        strcat(day->date,"\0");
    }
    else if (strcmp(name,"time") == 0)
    {
        // check and store time prop to date struct
        day->time = calloc(strlen(val)+1,sizeof(char));
        strcpy(day->time, val);
        strcat(day->time,"\0");
    }
    else if (strcmp(name,"text") == 0 )
    {
        day->text = calloc(strlen(val)+1,sizeof(char));
        strcpy(day->text, val);
        strcat(day->text,"\0");
        // check and store text prop to date struct.
    }
    else if (strcmp(name,"isUTC") == 0)
    {
        if (strcmp(val,"true") == 0 )
        {
			day->UTC = true;
            // set isText to true.
        }
        else if (strcmp(val,"false") == 0)
        {
			day->UTC = false;
            // set isText to false.
        }   
    }
    //printf("Name = %s\nVal = %s\n",name,val);
    strcpy(val,"");
    strcpy(name,"");
    
}
Property* JSONtoProp(const char* str)
{
	if (str == NULL)
	{
		
		return NULL;
	}
	Property* prop = calloc(1,sizeof(Property));
	char buffer[1000] = "";
	strcpy(buffer,str);
    char tempVal[1000] = "";
    int count = 0;
    int i = 0;
    prop->parameters = initializeList (&parameterToString, &deleteParameter,&compareParameters);
    // initializes list to empty.
    while((buffer[i] != '[') && (i < strlen(buffer)))
    {
        
        if (( (buffer[i] == ',') && (buffer[i+1] == '"')))
        {
           // printf("testing\n");
            tempVal[count] = '\0';
            //printf("String = %s\n",tempVal);
            // call function here.
            splitPropVal(tempVal,prop);
            // means this is the end of current parsing value.call spliting function, and then reset values.
            //reset values now.
            strcpy(tempVal,"");
            count = 0;
        }
        else
        {
            tempVal[count] = buffer[i];
            count++;
        }
        i++;
        
    }
    strcpy(tempVal,buffer+(i));
    tempVal[strlen(tempVal)-1] = '\0';	
    char* valuesParsing = calloc(strlen(tempVal)+1,sizeof(char));
    strcpy(valuesParsing,tempVal);
    prop->values = JSONtoStrList(valuesParsing);
    free(valuesParsing);
    
    return prop;
	
	
}
void splitPropVal(char buffer[1000],Property* prop)
{
    char val[100];
    char name[100];
    int colonCheck = 0;
    int valCount = 0;
    int nameCount = 0;
    for (int i = 0; i <strlen(buffer);i++)
    {
        if (buffer[i] == ':')
        {
            name[nameCount] = '\0';
            colonCheck ++;
        }
        if ((colonCheck == 0) && (buffer[i] != '"') && (buffer[i] != '{'))
        {
            name[nameCount] = buffer[i];
            nameCount++;
        }
        if ((colonCheck != 0) && (buffer[i] != '"') && (buffer[i] != ':'))
        {
            val[valCount] = buffer[i];
            valCount++;
        }
        if ((colonCheck != 0) && ((buffer[i] == '"') || (buffer[i] =='}')))
        {
            val[valCount] = '\0';
        }
        if (i == strlen(buffer)-1)
        {
            val[valCount] = '\0';
        }
    }
    
    if (strcmp(name,"group") == 0)
    {
        // copy group.
        prop->group = calloc(strlen(val)+1,sizeof(char));
        strcpy(prop->group,val);
        strcat(prop->group,"\0");
        
    }
    else if(strcmp(name,"name") == 0)
    {
        // check and store prop name.
        prop->name = calloc(strlen(val)+1,sizeof(char));
        strcpy(prop->name,val);
        strcat(prop->name,"\0");
    }
    else
    {
		return; 
	}
    
   // printf("Name = %s\nVal = %s\n",name,val);
    strcpy(val,"");
    strcpy(name,"");   
    
}



