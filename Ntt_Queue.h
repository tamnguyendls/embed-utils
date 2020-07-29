#ifndef NTT_QUEUE_H_
#define NTT_QUEUE_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NTT_MAX_QUEUE_ELEMENT   5
/**********************************************************************************************************************
 * Internal Type Definition
 *********************************************************************************************************************/
typedef struct
{
    uint8_t *data_pu8;
    uint16_t len_u16;
}data_element_t;

typedef struct
{
    data_element_t data_element[NTT_MAX_QUEUE_ELEMENT];     // ptr to array of data
    uint8_t size;            // the physical array size
    uint8_t count;           // number of items in queue
    uint8_t head;            // location to pop from
    uint8_t tail;            // location to push to
}queue_t;

/**********************************************************************************************************************
 * Internal Macros
 *********************************************************************************************************************/


/**********************************************************************************************************************
 * Public function definition
 *********************************************************************************************************************/

void Ntt_InitQueue(queue_t *qInstance);
bool Ntt_Enqueue(queue_t *qInstance, uint8_t* inBuffer_pu8, uint16_t inDataLen_u16);
bool Ntt_Dequeue(queue_t *qInstance, uint8_t **outData_pu8, uint16_t *outDataLen_pu16);

#ifdef __cplusplus
}
#endif

#endif /* NTT_QUEUE_H_ */
