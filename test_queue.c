
#include <stdio.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define VIPC_PDU_MAX_QUEUE_ELEMENT   5

typedef struct
{
    uint8_t *data_pu8;
    uint16_t len_u16;
}Vfx_Queue_DataElement_tst;

typedef struct
{
    Vfx_Queue_DataElement_tst data_element[VIPC_PDU_MAX_QUEUE_ELEMENT];     // ptr to array of data
    uint8_t size_u8;            // the physical array size
    uint8_t count_u8;           // number of items in queue
    uint8_t front_u8;            // location to pop from
    uint8_t rear_u8;            // location to push to
}Vfx_CommonQueue_tst;


static bool vIPC_Pdu_IsQueueEmpty(Vfx_CommonQueue_tst *qInstance);
static bool vIPC_Pdu_IsQueueFull(Vfx_CommonQueue_tst *qInstance);
static uint8_t vIPC_Pdu_GetPeek(Vfx_CommonQueue_tst *qInstance);

static uint8_t vIPC_Pdu_GetPeek(Vfx_CommonQueue_tst *qInstance)
{
    if (qInstance != 0x0)
        return qInstance->front_u8;
}

static bool vIPC_Pdu_IsQueueFull(Vfx_CommonQueue_tst *qInstance)
{
    if(qInstance != 0x0)
        return qInstance->count_u8 == VIPC_PDU_MAX_QUEUE_ELEMENT;
}

static bool vIPC_Pdu_IsQueueEmpty(Vfx_CommonQueue_tst *qInstance)
{
    if (qInstance != 0x0)
        return qInstance->count_u8 == 0;
}

/**********************************************************************************************************************
 * Public function definition
 *********************************************************************************************************************/

void vIPC_Pdu_InitQueue(Vfx_CommonQueue_tst *qInstance)
{
    if (qInstance != 0x0)
    {
        for (uint8_t i = 0; i < VIPC_PDU_MAX_QUEUE_ELEMENT; i++)
        {
            qInstance->data_element[i].data_pu8 = 0x0;
        }
        qInstance->count_u8 = 0;
        qInstance->front_u8 = 0;
        qInstance->rear_u8 = 0;
    }
}

bool vIPC_Pdu_Enqueue(Vfx_CommonQueue_tst *qInstance, uint8_t* inBuffer_pu8, uint16_t inDataLen_u16)
{
    bool ret_b = false;

    if (qInstance != 0x0)
    {
        /* Check if queue is not full */
        if (!vIPC_Pdu_IsQueueFull(qInstance))
        {
            qInstance->data_element[qInstance->rear_u8].data_pu8 = (uint8_t*)malloc(sizeof(uint8_t) * inDataLen_u16);
            if (qInstance->data_element[qInstance->rear_u8].data_pu8 != 0x0)
            {
                memcpy(qInstance->data_element[qInstance->rear_u8].data_pu8, inBuffer_pu8, inDataLen_u16);
                qInstance->data_element[qInstance->rear_u8].len_u16 = inDataLen_u16;
            }
            qInstance->rear_u8 = (qInstance->rear_u8 + 1) % VIPC_PDU_MAX_QUEUE_ELEMENT; /* circular queue */
            qInstance->count_u8++;
            ret_b = true;
        }
        else
        {
            printf("Queue is full\n");
            ret_b = false;
        }
    }
    return ret_b;
}

bool vIPC_Pdu_Dequeue(Vfx_CommonQueue_tst *qInstance, uint8_t **outData_pu8, uint16_t *outDataLen_pu16)
{
    uint8_t i = 0;
    uint16_t len_u16 = 0;
    bool ret_b = false;

    if (qInstance != 0x0)
    {
        /* Check if queue is not empty */
        if (!vIPC_Pdu_IsQueueEmpty(qInstance))
        {
            if (outDataLen_pu16 != 0x0)
            {
                len_u16 = qInstance->data_element[qInstance->front_u8].len_u16;
                *outDataLen_pu16 = len_u16;
                if (*outData_pu8 == 0x0)
                {
                    *outData_pu8 = (uint8_t*)malloc(sizeof(uint8_t) * len_u16);
                    if (*outData_pu8 != 0x0)
                    {
                        memcpy(*outData_pu8, qInstance->data_element[qInstance->front_u8].data_pu8, len_u16);
                        for (i = 0; i < len_u16; i++)
                        {
                            qInstance->data_element[qInstance->front_u8].data_pu8[i] = 0;
                        }
                        free(qInstance->data_element[qInstance->front_u8].data_pu8);
                        qInstance->front_u8 = (qInstance->front_u8 + 1) % VIPC_PDU_MAX_QUEUE_ELEMENT;
                        qInstance->count_u8 = qInstance->count_u8 - 1;
                        ret_b = true;
                    }
                }
            }
        }
        else
        {
            printf("Queue is empty\n");
            ret_b = false;
        }
    }
    return ret_b;
}

uint8_t vIPC_Pdu_CountQueueElement(Vfx_CommonQueue_tst *qInstance)
{
    uint8_t retVal_u8 = 0;
    if (qInstance != 0x0)
    { 
        retVal_u8 = qInstance->count_u8;
    }
    return retVal_u8;
}

Vfx_CommonQueue_tst queueInstanceTx_st;

int main()
{
    uint8_t data1[8] = {1,2,3,4,5,6,7,8};
    uint8_t data2[10] = {10,20,30,40,50,60,70,80, 90, 120};
    uint8_t count = 0;
    int i = 0;
    
    printf("Hello World");
    
    vIPC_Pdu_InitQueue(&queueInstanceTx_st);
    
    /* TEST EN-QUEUE #########################################################*/
    vIPC_Pdu_Enqueue(&queueInstanceTx_st, data1, 8);
    vIPC_Pdu_Enqueue(&queueInstanceTx_st, data2, 10);
    
    count = vIPC_Pdu_CountQueueElement(&queueInstanceTx_st);
    printf("\nCount: %d\n", count);
    
    for (i = 0; i < queueInstanceTx_st.data_element[0].len_u16; i++)
    {
        printf("%d-", queueInstanceTx_st.data_element[0].data_pu8[i]);
    }
    
    printf("\n");
    
    for (i = 0; i < queueInstanceTx_st.data_element[1].len_u16; i++)
    {
        printf("%d-", queueInstanceTx_st.data_element[1].data_pu8[i]);
    }
    
    /* TEST DE-QUEUE #########################################################*/
    printf("\n\nTEST DE-QUEUE 1\n");
    uint8_t* pdata = 0x0;
    uint16_t dataLen = 0;
    uint8_t peek = 0;
    
    vIPC_Pdu_Dequeue(&queueInstanceTx_st, &pdata, &dataLen);
    
    for (i = 0; i < dataLen; i++)
    {
        printf("%d-", pdata[i]);
    }
    
    if (pdata != 0x0)
    {
        printf("\nPtr addr:%x\n", *pdata);
        free(pdata);
        pdata = 0x0;
    }

    count = vIPC_Pdu_CountQueueElement(&queueInstanceTx_st);
    printf("\nCount: %d\n", count);
    
    peek = vIPC_Pdu_GetPeek(&queueInstanceTx_st);
    
    printf("\nFront Pos: %d\n", peek);
    
    /*########################################################################*/
    printf("\n\nTEST DE-QUEUE 2\n");
    vIPC_Pdu_Dequeue(&queueInstanceTx_st, &pdata, &dataLen);
        for (i = 0; i < dataLen; i++)
    {
        printf("%d-", pdata[i]);
    }
    
    if (pdata != 0x0)
    {
        printf("\nPtr addr:%x\n", *pdata);
        free(pdata);
        pdata = 0x0;
    }

    count = vIPC_Pdu_CountQueueElement(&queueInstanceTx_st);
    printf("\nCount: %d\n", count);
    
    peek = vIPC_Pdu_GetPeek(&queueInstanceTx_st);
    
    printf("\nFront Pos: %d\n", peek);
    
    return 0;
}
