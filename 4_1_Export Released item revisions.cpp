//1
#include<stdio.h>
#include<tcinit/tcinit.h>
#include<tc/tc_startup.h>
#include<tc/emh.h>
#include<stdlib.h>
#include<qry/qry.h>
#include<fclasses/tc_string.h>
#include<tccore/workspaceobject.h>
#include<tc/tc_util.h>
#include<tccore/aom_prop.h>


void Display_Usage() {
	printf("\n\n Teamcenter Utility to Import Named Referance to Dataset");
	printf("\n\n -u=\t Login User ID");
	printf("\n\n -p=\t Login User Password");
	printf("\n\n -g=\t Login User Group");
	printf("\n\n -h\t to display the detailed information of utility");
	printf("\n\n -help\t to display the detailed information of utility");
	exit(0);
}

int Report_Error(int iFail)
{
	char* cError = NULL;
	if (iFail != ITK_ok)
	{
		EMH_ask_error_text(iFail, &cError);
		printf("\n Error is : %s", cError);
		if (cError)MEM_free(cError);
		exit(0);
	}
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Header.h"

int ITK_user_main(int argc, char* argv[])
{
	int iFail = 0;
	char* cError = NULL;
	char* cUserID = NULL;
	char* cPassword = NULL;
	char* cGroup = NULL;
	int iCount = 2;
	int iFound = 0;
	char* cEntries[2] = {"Release Status","Type"};
	char** cValues = NULL;
	tag_t* tResults = NULLTAG;
	char* cStatus = NULL;
	char* cType = NULL;
	//char* cReleasedAfter = NULL;
	
	FILE* fp = NULL;
	
	char* cItemName = NULL;
	char* cItemID = NULL;
	char* cRevID = NULL;
	char* cCurrStatus = NULL;
	char* cDate = NULL;
	char* cOwner = NULL;

	tag_t tQuery = NULLTAG;

	if (ITK_ask_cli_argument("-h") || ITK_ask_cli_argument("-help"))
	{
		Display_Usage();
	}

	if (argc == 6)
	{
		cUserID = ITK_ask_cli_argument("-u=");
		cPassword = ITK_ask_cli_argument("-p=");
		cGroup = ITK_ask_cli_argument("-g=");
		cStatus = ITK_ask_cli_argument("-status=");
		cType = ITK_ask_cli_argument("-type=");
		//cReleasedAfter = ITK_ask_cli_argument("-releasedAfter=");

		fp = TC_fopen("Relesed Revisions.csv", "w+");
		if (fp == NULL)
		{
			printf("\n file not found");
			return 1;
		}
		TC_fprintf(fp, "Object Name,Item ID,Revision ID,Release status,Owner,Date");

		Report_Error(iFail = ITK_init_module(cUserID, cPassword, cGroup));
		printf("\n Login Success");

		cValues = (char**)MEM_alloc(iCount * sizeof(char*));          // memory allocation for array entries for manual input...it is runtime memory allocation

		cValues[0] = (char*)MEM_alloc(tc_strlen(cStatus) + 1);
		tc_strcpy(cValues[0], cStatus);
		cValues[1] = (char*)MEM_alloc(tc_strlen(cType) + 1);
		tc_strcpy(cValues[1], cType);
		//cValues[1] = (char*)MEM_alloc(tc_strlen(cReleasedAfter) + 1);
		//tc_strcpy(cValues[2], cReleasedAfter);

		Report_Error(iFail = QRY_find2("Item Revision...", &tQuery));
		if (tQuery == NULLTAG)
		{
			printf("\n Query Not Found");
			return 1;

		}
        Report_Error(QRY_execute(tQuery, 2, cEntries, cValues, &iFound, &tResults));
		printf("\n query starts");
		for (int j = 0; j < iFound; j++)
			{
				
				Report_Error(iFail = AOM_ask_value_string(tResults[j], "object_name", &cItemName));
				Report_Error(iFail = AOM_ask_value_string(tResults[j], "item_id", &cItemID));
				Report_Error(iFail = AOM_ask_value_string(tResults[j], "item_revision_id", &cRevID));
				Report_Error(iFail = AOM_ask_value_string(tResults[j], "release_status_list", &cCurrStatus));
				Report_Error(iFail = AOM_ask_value_string(tResults[j], "date_released", &cDate));
				Report_Error(iFail = AOM_ask_value_string(tResults[j], "owning_user", &cOwner));
				TC_fprintf(fp,"\n%s,%s,%s,%s,%s,%s",cItemName,cItemID,cRevID,cCurrStatus,cDate,cOwner);
			}

		Report_Error(iFail = ITK_exit_module(TRUE));
		printf("\n Logout Success");


		fflush(fp);
		fclose(fp);
	}
	else
	{
		printf("Argument Count is less or more");
	}

	//if (cValues)MEM_free(cValues);
	if (tResults)MEM_free(tResults);


	return 0;

}



