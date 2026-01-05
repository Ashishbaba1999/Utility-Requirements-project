// Find all change Request Revisions with problem Items


#include<stdio.h>
#include<tc/tc_startup.h>
#include<tcinit/tcinit.h>
#include<tc/emh.h>
#include<stdlib.h>
#include<tccore/workspaceobject.h>
#include<sa/user.h>
#include<fclasses/tc_string.h>
#include<tc/folder.h>
#include<tccore/aom.h>
#include<tc/tc_util.h>
#include<tccore/aom_prop.h>
#include<tccore/grm.h>

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
	if (iFail != ITK_ok) {
		EMH_ask_error_text(iFail, &cError);
		printf("\n\n Error is : %s", cError);
		MEM_free(cError);
		exit(0);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
#include "Header.h"

int ITK_user_main(int argc, char* argv[]) {

	int iFail = 0;
	int iCount = 0;
	int iObjectCount = 0;
	char* cError = NULL;
	char* cUserID = NULL;
	char* cPassword = NULL;
	char* cGroup = NULL;
	char* cCRrev = NULL;

	FILE* fp = NULL;

	tag_t tUser = NULLTAG;
	tag_t tNewUser = NULLTAG;
	tag_t tGroup = NULLTAG;
	WSO_search_criteria_t tCriteria;
	tag_t* tSearchList = NULLTAG;
	tag_t tRelationType = NULLTAG;
	int iSecObjCount = 0;
	tag_t* tSecObjects = NULLTAG;
	char* cProblemItem = NULL;


	if (ITK_ask_cli_argument("-h") || ITK_ask_cli_argument("-help")) {
		Display_Usage();
	}

	if (argc == 4) {
		cUserID = ITK_ask_cli_argument("-u=");
		cPassword = ITK_ask_cli_argument("-p=");
		cGroup = ITK_ask_cli_argument("-g=");

		fp = TC_fopen("ProblemItems.csv", "w+");
		if (fp == NULL)
		{
			printf("\n file not found");
			return 1;
		}
		Report_Error(iFail = ITK_init_module(cUserID, cPassword, cGroup));
		printf("\n\n Login Success");

		Report_Error(iFail = WSOM_clear_search_criteria(&tCriteria));


		tc_strcpy(tCriteria.class_name, "ChangeRequestRevision");
		tc_strcpy(tCriteria.name, "*");


		Report_Error(iFail = WSOM_search(tCriteria, &iCount, &tSearchList));
		if (iCount > 0) 
		{
			printf("\n Search count = %d", iCount);
			for (int j = 0; j < iCount; j++)
			{
				Report_Error(iFail = AOM_UIF_ask_value(tSearchList[j],"object_name", &cCRrev));
				printf("\n change request revision --> %s", cCRrev);
				Report_Error(iFail = GRM_find_relation_type("CMHasProblemItem", &tRelationType));                              //in place use P2CCS_Document_Relation
				Report_Error(iFail = GRM_list_secondary_objects_only(tSearchList[j], tRelationType, &iSecObjCount, &tSecObjects));
				for (int i = 0; i < iSecObjCount; i++)
				{
					Report_Error(iFail = AOM_UIF_ask_value(tSecObjects[i],"object_name", &cProblemItem));
					printf("\n Problem Item under is %s --> %s\n", cCRrev, cProblemItem);
	
				}
			}
		}

		Report_Error(iFail = ITK_exit_module(TRUE));
		printf("\n\n Logout Success");
	}
	else {
		printf("\n\n Argument count is less or more");
	}
	if (tSearchList) {
		MEM_free(tSearchList);
	}
	if (tSecObjects) {
		MEM_free(tSecObjects);
	}
	return 0;
}

