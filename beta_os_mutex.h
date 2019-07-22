#ifndef BETA_OS_MUTEX_H
#define BETA_OS_MUTEX_H

#include "beta_os_defines.h"

//INIT
void initMutex(osBetaMutex_t *m) {
    
		m->id = getNextId(&next_mutex_id);
    m->inUse = false;
		m->owner = -1;
		m->waitList = NULL;
}

//BLOCK
void blockTaskMutex(osBetaMutex_t *m) {
	
		printf("BLOCKING TASK %d!\n", runningTask->id);
    
    osBetaThread_t *cursor = m->waitList;
    
    if(cursor == NULL)
        m->waitList = runningTask;
		
    else {
        while(cursor->next != NULL)
            cursor = cursor->next;
			
        cursor->next = runningTask;
		}

		runningTask->state = osThreadBlocked;
		
		triggerPendSV();
    
    
}

//UNBLOCK
void unblockTaskMutex(osBetaMutex_t *m) {  
	
    if(m->waitList == NULL)
        return;
    else {
				printf("UNBLOCKING TASK %d!\n", m->waitList->id);
        osBetaThread_t *thread = m->waitList;
			
				thread->state = osThreadReady;
        m->waitList = thread->next;
			
        addThreadToScheduler(thread);
				triggerPendSV();
    }
    
}

//ACQUIRE
void acquire(osBetaMutex_t *m, osBetaThread_id currentThread) {
    __disable_irq();
	
    if( m->inUse == true ) {
				printf("thread %d failed to acquire mutex %d\n", currentThread, m->id);
        blockTaskMutex(m);
    }
		
		else
		{
			printf("mutex %d acquired by thread %d\n", m->id, currentThread);
			m->inUse = true;
			m->owner = currentThread;
		}
    
    __enable_irq();
}

//RELEASE
void release(osBetaMutex_t *	m, osBetaThread_id currentThread) {
    __disable_irq();
    
		printf("thread %d tried to release mutex %d\n", currentThread, m->id);
	
    if(m->owner == currentThread)
		{
			printf("mutex %d released by thread %d\n", m->id, currentThread);
			m->inUse = false;
			m->owner = -1;
			unblockTaskMutex(m);
		}
		
    __enable_irq();
}

#endif