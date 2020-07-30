/**********************************************************************************************************************
 * Files inclusion
 *********************************************************************************************************************/
#include "Ntt_Queue.h"

/**********************************************************************************************************************
 * Internal Type Definition
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Internal Macros
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Static function declaration
 *********************************************************************************************************************/

static bool Ntt_IsQueueEmpty(queue_t *qInstance);
static bool Ntt_IsQueueFull(queue_t *qInstance);
static uint8_t Ntt_Peek(queue_t *qInstance);

/**********************************************************************************************************************
 * Variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Static function definition
 *********************************************************************************************************************/

static uint8_t Ntt_Peek(queue_t *qInstance)
{
    if (qInstance != 0x0)
        return qInstance->head;
}

static bool Ntt_IsQueueFull(queue_t *qInstance)
{
    if(qInstance != 0x0)
        return qInstance->count == MAX_QUEUE_ELEMENT;
}

static bool Ntt_IsQueueEmpty(queue_t *qInstance)
{
    if (qInstance != 0x0)
        return qInstance->count == 0;
}

/**********************************************************************************************************************
 * Public function definition
 *********************************************************************************************************************/

void Ntt_InitQueue(queue_t *qInstance)
{
    if (qInstance != 0x0)
    {
        for (uint8_t i = 0; i < NTT_MAX_QUEUE_ELEMENT; i++)
        {
            qInstance->data_element[i].data_pu8 = 0x0;
        }
        qInstance->count = 0;
        qInstance->head = 0;
        qInstance->tail = 0;
    }
}

bool Ntt_Enqueue(queue_t *qInstance, uint8_t* inBuffer_pu8, uint16_t inDataLen_u16)
{
    bool ret_b = false;

    if (qInstance != 0x0)
    {
        /* Check if queue is not full */
        if (!Ntt_IsQueueFull(qInstance))
        {
            qInstance->data_element[qInstance->tail].data_pu8 = (uint8_t*)malloc(sizeof(uint8_t) * inDataLen_u16);
            if (qInstance->data_element[qInstance->tail].data_pu8 != 0x0)
            {
                memcpy(qInstance->data_element[qInstance->tail].data_pu8, inBuffer_pu8, inDataLen_u16);
                qInstance->data_element[qInstance->tail].len_u16 = inDataLen_u16;
            }
            qInstance->tail = (qInstance->tail + 1) % NTT_MAX_QUEUE_ELEMENT; /* circular queue */
            qInstance->count++;
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

bool Ntt_Dequeue(queue_t *qInstance, uint8_t **outData_pu8, uint16_t *outDataLen_pu16)
{
    uint8_t i = 0;
    uint16_t len_u16 = 0;
    bool ret_b = false;

    if (qInstance != 0x0)
    {
        /* Check if queue is not empty */
        if (!Ntt_IsQueueEmpty(qInstance))
        {
            if (outDataLen_pu16 != 0x0)
            {
                len_u16 = qInstance->data_element[qInstance->head].len_u16;
                *outDataLen_pu16 = len_u16;
                if (*outData_pu8 == 0x0)
                {
                    *outData_pu8 = (uint8_t*)malloc(sizeof(uint8_t) * len_u16);
                    if (*outData_pu8 != 0x0)
                    {
                        memcpy(*outData_pu8, qInstance->data_element[qInstance->head].data_pu8, len_u16);
                        for (i = 0; i < len_u16; i++)
                        {
                            qInstance->data_element[qInstance->head].data_pu8[i] = 0;
                        }
                        free(qInstance->data_element[qInstance->head].data_pu8);
                        qInstance->head = (qInstance->head + 1) % NTT_MAX_QUEUE_ELEMENT;
                        qInstance->count = qInstance->count - 1;
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

