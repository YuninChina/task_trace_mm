#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "task.h"
#include "multicast.h"
#include "parson.h"

#define MUTICAST_ADDR "239.255.255.250"

int main(int argc ,char *argv[])  
{
	const char *process_name = NULL;
	task_arr_t taskArr;
	int ret = -1;
	int i;
	multicast_t *m = NULL;
	unsigned short port = 6666;
	if (argc != 3) {
		fprintf(stderr, "Usage: %s <process_name> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	process_name = argv[1];
	assert(1 == sscanf(argv[2],"%hu",&port));
	m = multicast_create(MUTICAST_ADDR,port);
	assert(m);
	
	while(1)
	{
		//
		ret = system("clear");
		assert(0 == ret);
		printf("%-20s %-8s %-8s %-12s %-12s %-12s %-12s %-12s %-12s %-8s %-4s\n",
		"task_name",
		"task_id",
		"state",
		"threads",
		"cpu_utime",
		"cpu_stime",
		"cpu_cutime",
		"cpu_cstime",
		"priority",
		"nice",
		"cpu"
		);
		printf("==============================================================="
		"==================================================================\n");
		bzero(&taskArr,sizeof(taskArr));
		task_info_new(process_name,&taskArr);
		task_info_show(&taskArr);
		/////////////////////////////
		JSON_Value *jValRoot = json_value_init_array();
		assert(jValRoot);
		JSON_Array *jArrRoot = json_array(jValRoot);
		assert(jArrRoot);
		for(i = 0;i < taskArr.num;i++)
		{
			JSON_Value *jVal = NULL;
			JSON_Object *jObj = NULL;
			jVal = json_value_init_object();
			assert(jVal);
			jObj = json_value_get_object(jVal);
			assert(jObj);
			json_object_dotset_number(jObj, "id", taskArr.array[i].id);
			json_object_dotset_string(jObj, "name", taskArr.array[i].name);
			json_object_dotset_number(jObj, "state", taskArr.array[i].state);
			json_object_dotset_number(jObj, "utime", taskArr.array[i].utime);
			json_object_dotset_number(jObj, "stime", taskArr.array[i].stime);
			json_object_dotset_number(jObj, "cutime", taskArr.array[i].cutime);
			json_object_dotset_number(jObj, "priority", taskArr.array[i].cstime);
			json_object_dotset_number(jObj, "priority", taskArr.array[i].priority);
			json_object_dotset_number(jObj, "nice", taskArr.array[i].nice);
			json_object_dotset_number(jObj, "cpu", taskArr.array[i].cpu);
			json_object_dotset_number(jObj, "num_threads", taskArr.array[i].num_threads);
			json_array_append_value(jArrRoot,jVal);
		}
		//
		char *jStr = json_serialize_to_string(jValRoot);
		multicast_send(m, (unsigned char *)jStr, strlen(jStr));
		if(jStr) 
		{
			free(jStr);
			jStr = NULL;
		}
		//
		if(jArrRoot)
		{
			json_array_clear(jArrRoot);
			jArrRoot = NULL;
		}
		if(jValRoot)
		{
			json_value_free(jValRoot);
			jValRoot = NULL;
		}
		/////////////////////////////
		task_info_free(&taskArr);
		/////////////////////////////
		sleep(1);
	}
	
	multicast_destroy(m);
	
	return 0;
}

