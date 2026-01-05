//Assign Business object to project Based on object type
#include<stdlib.h>
#include<tcinit/tcinit.h>
#include<qry/qry.h>
#include<tccore/project.h>
#include<bom/bom.h>
#include<tccore/tctype.h>

typedef struct {
	char* cItemID;
	char* cItemType;
}input_t;

FILE* file;

int Report_Error(int iFail)
{
	char* cError = NULL;
	if (iFail != ITK_ok)
	{
		EMH_ask_error_text(iFail, &cError);
		printf("\n\n Error is : %s", cError);
		MEM_free(cError);
		exit(0);
	}
	return 0;
}


//////////////////////////////////////////////////////////////////
#include "Header.h"

int ITK_user_main(int argc, char* argv[])
{
	int iFail = 0;
	int row_count = 0;

	//tag_t tItemType = NULLTAG;
	//tag_t tItemCreateInput = NULLTAG;
	//tag_t tItem = NULLTAG;
	//tag_t tRev = NULLTAG;

	input_t record;

	char buf[1024];
	const char delim[2] = ",";
	char* Token = NULL;
	char* cError = NULL;
	char* cProID = NULL;
	char* cObjType = NULL;
	tag_t tProject = NULLTAG;
	tag_t tItem = NULLTAG;


	if (argc == 3)
	{
		cProID = ITK_ask_cli_argument("-proID=");
		cObjType = ITK_ask_cli_argument("-objectType=");

		FILE* fp = TC_fopen("C:\\Users\\AdminV\\source\\repos\\GaneshVD\\input_ID.csv", "r");

		if (!fp)
		{
			printf("Can not open file \n ");
			exit(0);
		}
		else
		{

			Report_Error(ITK_init_module("infodba", "infodba", "dba"));
			printf("\n Login success \n");

			Report_Error(iFail = PROJ_find(cProID, &tProject));

			if (tProject != NULLTAG)
			{
				while (fgets(buf, 1024, fp))
				{
					row_count++;
					if (row_count == 1)
					{
						continue;
					}
					record.cItemID = strtok_s(buf, delim, &Token);
					record.cItemType = strtok_s(NULL, delim, &Token);

					if (record.cItemType)
						record.cItemType[strcspn(record.cItemType, "\r\n")] = '\0';

					if (tc_strcmp(cObjType, record.cItemType) == 0)
					{
						Report_Error(iFail = ITEM_find_item(record.cItemID, &tItem));
						printf("\n Item find success");
						Report_Error(PROJ_assign_objects(1, &tProject, 1, &tItem));
						printf("\n project assign success");
					}

				}
			}
			else
			{
				printf("\n Project ID not found");
			}
			fclose(fp);

			Report_Error(iFail = ITK_exit_module(TRUE));
			printf("\n Logout Success");

		}

	}
	else
	{
		printf("\n Argument count is less or more");
	}


	
	return 0;
}
