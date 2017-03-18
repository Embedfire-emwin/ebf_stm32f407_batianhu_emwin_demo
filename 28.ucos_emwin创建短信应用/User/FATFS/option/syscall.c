/*------------------------------------------------------------------------*/
/* Sample code of OS dependent controls for FatFs                         */
/* (C)ChaN, 2014                                                          */
/*------------------------------------------------------------------------*/
#include "os.h"
#include "ff.h"

#if _FS_REENTRANT


//定义文件系统使用的互斥信号量
static OS_MUTEX fatfs_mutex;

/*------------------------------------------------------------------------*/
/* Create a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to create a new
/  synchronization object, such as semaphore and mutex. When a 0 is returned,
/  the f_mount() function fails with FR_INT_ERR.
*/
int ff_cre_syncobj (	/* !=0:Function succeeded, ==0:Could not create due to any error */
	BYTE vol,			/* Corresponding logical drive being processed */
	_SYNC_t *sobj		/* Pointer to return the created sync object */
)
{
	int ret;

		OS_ERR     err;

//	*sobj = CreateMutex(NULL, FALSE, NULL);		/* Win32 */
//	ret = (int)(*sobj != INVALID_HANDLE_VALUE);

//	*sobj = SyncObjects[vol];			/* uITRON (give a static created sync object) */
//	ret = 1;							/* The initial value of the semaphore must be 1. */

//	*sobj = OSMutexCreate(0, &err);		/* uC/OS-II */
	
	/* 创建互斥信号量 mutex */
    OSMutexCreate ((OS_MUTEX*)&fatfs_mutex,           //指向信号量变量的指针
										 (CPU_CHAR  *)"Mutex For Fatfs", //信号量的名字
										 (OS_ERR    *)&err);            //错误类型
		*sobj = &fatfs_mutex;  

		ret = (int)(err == OS_ERR_NONE);

//	*sobj = xSemaphoreCreateMutex();	/* FreeRTOS */
//	ret = (int)(*sobj != NULL);

	return ret;
}



/*------------------------------------------------------------------------*/
/* Delete a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to delete a synchronization
/  object that created with ff_cre_syncobj function. When a 0 is returned,
/  the f_mount() function fails with FR_INT_ERR.
*/

int ff_del_syncobj (	/* !=0:Function succeeded, ==0:Could not delete due to any error */
	_SYNC_t sobj		/* Sync object tied to the logical drive to be deleted */
)
{
	int ret;
	OS_ERR      err;


//	ret = CloseHandle(sobj);	/* Win32 */
//	ret = 1;					/* uITRON (nothing to do) */

//	OSMutexDel(sobj, OS_DEL_ALWAYS, &err);	/* uC/OS-II */
		
		OSMutexDel	((OS_MUTEX*)sobj,                  //释放互斥信号量 mutex
								 (OS_OPT     )OS_OPT_DEL_ALWAYS,        //进行任务调度
								 (OS_ERR    *)&err);    
	
		ret = (int)(err == OS_ERR_NONE);

//  vSemaphoreDelete(sobj);		/* FreeRTOS */
//	ret = 1;

	return ret;
}



/*------------------------------------------------------------------------*/
/* Request Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on entering file functions to lock the volume.
/  When a 0 is returned, the file function fails with FR_TIMEOUT.
*/

int ff_req_grant (	/* 1:Got a grant to access the volume, 0:Could not get a grant */
	_SYNC_t sobj	/* Sync object to wait */
)
{
	int ret;
	OS_ERR      err;

//	ret = (int)(WaitForSingleObject(sobj, _FS_TIMEOUT) == WAIT_OBJECT_0);	/* Win32 */

//	ret = (int)(wai_sem(sobj) == E_OK);			/* uITRON */

//	OSMutexPend(sobj, _FS_TIMEOUT, &err));		/* uC/OS-II */

	OSMutexPend ((OS_MUTEX*)sobj,                  //申请互斥信号量 mutex
								 (OS_TICK    )_FS_TIMEOUT,                       //无期限等待
								 (OS_OPT     )OS_OPT_PEND_BLOCKING,    //如果不能申请到信号量就堵塞任务
								 (CPU_TS    *)0,                       //不想获得时间戳
								 (OS_ERR    *)&err);                   //返回错误类型		

	ret = (int)(err == OS_ERR_NONE);


//	ret = (int)(xSemaphoreTake(sobj, _FS_TIMEOUT) == pdTRUE);	/* FreeRTOS */

	return ret;
}



/*------------------------------------------------------------------------*/
/* Release Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on leaving file functions to unlock the volume.
*/

void ff_rel_grant (
	_SYNC_t sobj	/* Sync object to be signaled */
)
{
		OS_ERR      err;

//	ReleaseMutex(sobj);		/* Win32 */

//	sig_sem(sobj);			/* uITRON */

//	OSMutexPost(sobj);		/* uC/OS-II */
		OSMutexPost ((OS_MUTEX*)sobj,                  //释放互斥信号量 mutex
									 (OS_OPT     )OS_OPT_POST_1,        //进行任务调度
									 (OS_ERR    *)&err);                   //返回错误类型		

//	xSemaphoreGive(sobj);	/* FreeRTOS */
}

#endif




#if _USE_LFN == 3	/* LFN with a working buffer on the heap */
/*------------------------------------------------------------------------*/
/* Allocate a memory block                                                */
/*------------------------------------------------------------------------*/
/* If a NULL is returned, the file function fails with FR_NOT_ENOUGH_CORE.
*/

void* ff_memalloc (	/* Returns pointer to the allocated memory block */
	UINT msize		/* Number of bytes to allocate */
)
{
	return malloc(msize);	/* Allocate a new memory block with POSIX API */
}


/*------------------------------------------------------------------------*/
/* Free a memory block                                                    */
/*------------------------------------------------------------------------*/

void ff_memfree (
	void* mblock	/* Pointer to the memory block to free */
)
{
	free(mblock);	/* Discard the memory block with POSIX API */
}

#endif
