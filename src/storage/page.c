#include "storage/page.h"
#include <string.h>

void PageInit(Page page, size_t pageSize) {
    PageHeaderData* hdr = (PageHeaderData*)page;
    hdr->pd_lsn = 0;
    hdr->pd_checksum = 0;
    hdr->pd_flags = 0;
    hdr->pd_lower = (LocationIndex)sizeof(PageHeaderData);
    hdr->pd_upper = (LocationIndex)pageSize;
    hdr->pd_special = (LocationIndex)pageSize;
    hdr->pd_pagesize_version = 4;
}

bool PageAddItem(Page page, const char* item, size_t size, LocationIndex* outIndex) {
    PageHeaderData* hdr = (PageHeaderData*)page;
    size_t needed = sizeof(ItemIdData) + size;

    if ((size_t)(hdr->pd_upper - hdr->pd_lower) < needed) {
        return false; 
    }

    hdr->pd_upper = (LocationIndex)(hdr->pd_upper - size);
    memcpy(page + hdr->pd_upper, item, size);


    size_t index = (hdr->pd_lower - sizeof(PageHeaderData)) / sizeof(ItemIdData);
    ItemId itemId = (ItemId)(page + sizeof(PageHeaderData) + (index * sizeof(ItemIdData)));
    itemId->lp_off = hdr->pd_upper;
    itemId->lp_len = (unsigned)size;
    itemId->lp_flags = LP_USED;

    hdr->pd_lower = (LocationIndex)(hdr->pd_lower + sizeof(ItemIdData));
    *outIndex = (LocationIndex)index;
    return true;
}

char* PageGetItem(Page page, LocationIndex index, size_t* outSize) {
    ItemId itemId = (ItemId)(page + sizeof(PageHeaderData) + (index * sizeof(ItemIdData)));

    if (itemId->lp_flags != LP_USED) {
        return NULL;
    }

    *outSize = itemId->lp_len;
    return page + itemId->lp_off;
}
bool PageDeleteItem(Page page,LocationIndex index){
    PageHeaderData* hdr = (PageHeaderData*)page;
    size_t index1 = (hdr->pd_lower - sizeof(PageHeaderData)) / sizeof(ItemIdData);
    if(index>=index1){
        return false;
    }
    ItemId itemID=(ItemId)(page+sizeof(PageHeaderData)+(index*sizeof(ItemIdData)));
    if (itemID->lp_flags!=LP_USED){
        return false;
    }
    itemID->lp_flags=LP_DEAD;
    return true;
}
void PageDefragment(Page page){
    PageHeaderData* hdr = (PageHeaderData*)page;
    size_t itemCount= ((hdr->pd_lower-sizeof(PageHeaderData))/sizeof(ItemIdData));
    char temp[BLCKSZ];
    LocationIndex new_upper = (LocationIndex)BLCKSZ;
    for(int i=0;i<itemCount;i++){
        ItemId itemid = (ItemId)(page + sizeof(PageHeaderData)+(i*sizeof(ItemIdData)));
        if(itemid->lp_flags==LP_USED){
        new_upper=new_upper-itemid->lp_len;
        memcpy(temp+new_upper,page+itemid->lp_off,itemid->lp_len);
        itemid->lp_off=new_upper;
    }else{
    itemid->lp_off=0;
    itemid->lp_len=0;}
    }
    memcpy(page+new_upper,temp+new_upper,BLCKSZ-new_upper);
    hdr->pd_upper=new_upper;
}
size_t PageFreeSpace(Page page){
    PageHeaderData* hdr=(PageHeaderData*)page;
    if((hdr->pd_upper<=hdr->pd_lower+sizeof(ItemIdData))){
        return 0;
    }
    return hdr->pd_upper-hdr->pd_lower-sizeof(ItemIdData);
}