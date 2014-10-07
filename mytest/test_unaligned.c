#include<stdio.h>
#include<stdint.h>

void showhex(void* data, uint8_t size){
   uint8_t* ptr = data;
   uint8_t i = size - 1;
   while(1){
        printf("%x ", *(ptr+i));
        if (i == 0){
            break;
        }
        --i;
   }
   printf("\n");
}

uint32_t get_unaligned(const uint8_t * d) {
    return ((uint32_t) d[0]) | ((uint32_t) (d[1] << 8)) | ((uint32_t) (d[2] << 16)) | ((uint32_t) (d[3] << 24));
}

int main(int argc, const char *argv[])
{
    uint32_t a = 65536*65535+65535;
    showhex(&a, sizeof(a));  
    uint32_t b = get_unaligned((uint8_t*)&a);
    showhex(&b, sizeof(b));  

    printf("---------\n");

    uint32_t c = 20938475;
    showhex(&c, sizeof(c));
    const uint8_t * d = (const uint8_t *) &c;
    uint32_t e = 255;
    e = (uint32_t)d[0];
    showhex(&e, sizeof(e));
    e = (uint32_t)(d[1] << 8);
    showhex(&e, sizeof(e));
    e = (uint32_t)(d[2] << 16);
    showhex(&e, sizeof(e));
    e = (uint32_t)(d[3] << 24);
    showhex(&e, sizeof(e));

    return 0;
}
