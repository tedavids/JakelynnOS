// page directory functions

#include <stdio.h>
#include <bitmap.h>
#include <pmm.h>

#include <pgdir.h>

// defines 

// Page directory defines
#define PGDIR_PRESENT           0b1
#define PGDIR_READ_WRITE        0b10
#define PGDIR_USER_MEM          0b100
#define PGDIR_PAGE_WRITE_THRU   0b1000
#define PGDIR_CACHE_DISABLE     0b10000
#define PGDIR_4M                0b100000
#define PGDIR_GLOBAL            0b1000000
#define PGDIR_PAGE_ATTR_TBL     0b10000000

// actual page directory bits
#define DIR_PRESENT             0b1
#define DIR_READ_WRITE          0b10
#define DIR_USER_MEM            0b100
#define DIR_PAGE_WRITE_THRU     0b1000
#define DIR_CACHE_DISABLE       0b10000
#define DIR_4M                  0b10000000
#define DIR_GLOBAL              0b100000000
#define DIR_PAGE_ATTR_TBL       0b100000000000

// macros 
#define PGDIR_ATTR_MASK             (DIR_PRESENT | DIR_READ_WRITE | DIR_USER_MEM | DIR_PAGE_WRITE_THRU | DIR_CACHE_DISABLE | DIR_4M | DIR_GLOBAL | DIR_PAGE_ATTR_TBL)

// page directory Properties


PgDirProp_t     pgdirPgDirProp;
PgDirProp_t     *pgdirPgDirPropPID = &pgdirPgDirProp;
PageDirectory_t *pgdirPageDirectoryPID = &page_directory;


// PPPPPP      A      GGGGGGGG EEEEEEE     DDDDDD  IIII RRRRRR
// PP   PP    AAA     GG       EE          DD   DD  II  RR   RR
// PPPPPP    AA AA    GG       EEEEE       DD   DD  II  RR   RR
// PP       AAAAAAA   GG  GGGG EE          DD   DD  II  RRRRRR
// PP      AA     AA  GG    GG EE          DD   DD  II  RR  RR 
// PP     AA       AA GGGGGGGG EEEEEEE     DDDDDD  IIII RR   RR

// Internal functions

// is valid page directory index
// Parameters:  idx = the index to check
// Returns:     true if index is in the range of 0-1023
bool pgdirIsValidIdx(uint32_t idx) {
    return (idx < 1024);
}

// Is the page directory properties Page Attribute Table bit se
// Parameters:  idx - page directory index
// Returns:     true if set, false otherwise
bool pgdirIsPAT(uint32_t idx) {
    // check parameter
    if (!pgdirIsValidIdx(idx)) return false;

    return isBitSet8((*pgdirPgDirPropPID)[idx],PGDIR_PAGE_ATTR_TBL);
}

// Is the page directory Page Attribute Table bit set
// Parameters:  idx - page directory index
// Returns:     true if set, false otherwise
bool pgdirIsPATMem(uint32_t idx) {
    // check parameter
    if (!pgdirIsValidIdx(idx)) return false;

    return isBitSet32((*pgdirPageDirectoryPID)[idx], DIR_PAGE_ATTR_TBL);
}

// Is the property page directory attribute global
// Parameters:  idx - The property page index
// Returns:     true if is global, false otherwise
bool pgdirIsGlobal(uint32_t idx) {
    return isBitSet8((*pgdirPgDirPropPID)[idx],PGDIR_GLOBAL);
}

// Is the page directory attribute global
// Parameters:  idx - The page directory index
// Returns:     true if is global, false otherwise
bool pgdirIsGlobalMem(uint32_t idx) {
    return isBitSet32((*pgdirPageDirectoryPID)[idx],DIR_GLOBAL);
}

// Set the page directory property bit to Global
// Parameteers: idx - The index of the property page
// Returns:     true on success, false otherwise
bool pgdirSetGlobal(uint32_t idx) {
    // if it is already set we can't set it agin
    if (pgdirIsGlobal(idx)) return false;
    // set it
    (*pgdirPgDirPropPID)[idx] = setBits8((*pgdirPgDirPropPID)[idx],PGDIR_GLOBAL);
    return true;
}

// Clear the page directory property bit for global
// Paraters:    idx - The indx of the proper page
// Returns:     true if successful, false otherwise
bool pgdirClearGlobal(uint32_t idx) {
    // if the bit is clear, we can't clear it again
    if (!pgdirIsGlobal(idx)) return false;

    // clear the bit
    (*pgdirPgDirPropPID)[idx] = clearBits8((*pgdirPgDirPropPID)[idx],(uint8_t)PGDIR_GLOBAL);
    return true;
}

// Change the global bit on the page direcory
// Parameters:  idx - The index of the page directory
//              seton - Do we set the bit on (otherwise off)
//              invalidate - Do we want to invalidate the page
bool pgdirChgGlobalMem(uint32_t idx, bool seton, bool invalidate) {
    // check parameters
    if (!pgdirIsValidIdx(idx)) return false;
    // set bit on
    if (seton) {
        // if it is already on we can't set it again
        if (pgdirIsGlobalMem(idx)) return false;
        // set it on
        (*pgdirPageDirectoryPID)[idx] = setBits32((*pgdirPageDirectoryPID)[idx],DIR_GLOBAL);
    } else {
        // clear bit
        // if already clear we can't clear it again
        if (!pgdirIsGlobalMem(idx)) return false;
        // clear it
        (*pgdirPageDirectoryPID)[idx] = clearBits32((*pgdirPageDirectoryPID)[idx],DIR_GLOBAL);
    }
    // invalidate if necessary
    if (invalidate) {
        invalidatePage(&(*pgdirPageDirectoryPID)[idx]);
    }

    return true;
}

// Is the page size 4K
// Parameters:  idx - The index of the property array
// Returns:     true if 4K, false otherwise
bool pgdirIs4M(uint32_t idx) {
    // check index
    if (!pgdirIsValidIdx(idx)) return false;

    // send back if it is
    return (isBitSet8((*pgdirPgDirPropPID)[idx],PGDIR_4M));
}

// Is the page in the actual page directory 4K
// Parameters:  idx - The index of the page directory
// Returns:     true if 4K, false otherwise
bool pgdirIs4MMem(uint32_t idx) {
    // check index
    if (!pgdirIsValidIdx(idx)) return false;

    // send back if it is
    return (isBitSet32((*pgdirPageDirectoryPID)[idx],DIR_4M));
}

// Set the 4K bit in the page directory property table
// Parameters:  idx - Index in the property table
// Returns:     true changed, false otherwise
bool pgdirSet4M(uint32_t idx) {
    //check index
    if (!pgdirIsValidIdx(idx)) return false;
    // if set already we can't set it again
    if (pgdirIs4M(idx)) return false;

    // set it 
    (*pgdirPgDirPropPID)[idx] = setBits8((*pgdirPgDirPropPID)[idx],PGDIR_4M);
    return true;
}

// clear the 4k bit in the page directory property table
// Parameters:  idx - The index in the property table
// Returns:     true if changed, false otherwise
bool pgdirClear4M(uint32_t idx) {
    // check parameters
    if (!pgdirIsValidIdx(idx)) return false;

    // clear
    (*pgdirPgDirPropPID)[idx] = clearBits8((*pgdirPgDirPropPID)[idx],PGDIR_4M);
    return true;
}

// clear or set the 4K page bit in the actual page directory
// Parameters:  idx - The page directory index
//              seton - Set bit on
//              invalidate - invalidate page
// Return:      true if successful, false otherwise
bool pgdirChg4MMem(uint32_t idx, bool seton, bool invalidate) {
    // check index
    if (!pgdirIsValidIdx(idx)) return false;
    // set bit on
    if (seton) {
        // if it is already on we can't set it again
        if (pgdirIs4MMem(idx)) return false;
        // set it on
        (*pgdirPageDirectoryPID)[idx] = setBits32((*pgdirPageDirectoryPID)[idx],DIR_4M);
    } else {
        // clear bit
        // if already clear we can't clear it again
        if (!pgdirIs4MMem(idx)) return false;
        // clear it
        (*pgdirPageDirectoryPID)[idx] = clearBits32((*pgdirPageDirectoryPID)[idx],DIR_4M);
    }
    // invalidate if necessary
    if (invalidate) {
        invalidatePage(&(*pgdirPageDirectoryPID)[idx]);
    }
    return true;
}

// Is the cache disabled in the property array
// Parameters:  idx - The index of the property array
// Returns:     true if 4K, false otherwise
bool pgdirIsCacheDisabled(uint32_t idx) {
    // check parmeter
    if (!pgdirIsValidIdx(idx)) return false;

    return isBitSet8((*pgdirPgDirPropPID)[idx],PGDIR_CACHE_DISABLE);
}

// Is the disabled in the actual page directory 4K
// Parameters:  idx - The index of the page directory
// Returns:     true if disableed, false otherwise
bool pgdirIsCacheDisabledMem(uint32_t idx) {
    // check index
    if (!pgdirIsValidIdx(idx)) return false;

    // send back if it is
    return (isBitSet32((*pgdirPageDirectoryPID)[idx],DIR_CACHE_DISABLE));
}

// Is the write through in the property array
// Parameters:  idx - The index of the property array
// Returns:     true if 4K, false otherwise
bool pgdirIsWriteThru(uint32_t idx) {
    // check parmeter
    if (!pgdirIsValidIdx(idx)) return false;

    return isBitSet8((*pgdirPgDirPropPID)[idx],PGDIR_PAGE_WRITE_THRU);
}

// Is the write through in the actual page directory 4K
// Parameters:  idx - The index of the page directory
// Returns:     true if disableed, false otherwise
bool pgdirIsWriteThruMem(uint32_t idx) {
    // check index
    if (!pgdirIsValidIdx(idx)) return false;

    // send back if it is
    return (isBitSet32((*pgdirPageDirectoryPID)[idx],DIR_PAGE_WRITE_THRU));
}

// Is the User in the property array
// Parameters:  idx - The index of the property array
// Returns:     true if 4K, false otherwise
bool pgdirIsUserPage(uint32_t idx) {
    // check parmeter
    if (!pgdirIsValidIdx(idx)) return false;

    return isBitSet8((*pgdirPgDirPropPID)[idx],PGDIR_USER_MEM);
}

// Is the User in the actual page directory 4K
// Parameters:  idx - The index of the page directory
// Returns:     true if disableed, false otherwise
bool pgdirIsUserPageMem(uint32_t idx) {
    // check index
    if (!pgdirIsValidIdx(idx)) return false;

    // send back if it is
    return (isBitSet32((*pgdirPageDirectoryPID)[idx],DIR_USER_MEM));
}

// Is the Kernel in the property array
// Parameters:  idx - The index of the property array
// Returns:     true if 4K, false otherwise
bool pgdirIsKernelPage(uint32_t idx) {
    // check parmeter
    if (!pgdirIsValidIdx(idx)) return false;

    return !isBitSet32((*pgdirPgDirPropPID)[idx],PGDIR_USER_MEM);
}

// Is the Kernel in the actual page directory 4K
// Parameters:  idx - The index of the page directory
// Returns:     true if disabled, false otherwise
bool pgdirIsKernelPageMem(uint32_t idx) {
    // check index
    if (!pgdirIsValidIdx(idx)) return false;

    // send back if it is
    return (!isBitSet32((*pgdirPageDirectoryPID)[idx],PGDIR_USER_MEM));
}

// set the property array to user for page
// Parameters:  idx - the index of the property array
// Return:      true if set completes, false otherwise
bool pgdirSetUserPage(uint32_t idx) {
    // check index
    if (!pgdirIsValidIdx(idx)) return false;

    // set page
    (*pgdirPgDirPropPID)[idx] = setBits8((*pgdirPgDirPropPID)[idx],PGDIR_USER_MEM);
    return true;
}

// set the property array to Kernel for page
// Parameters:  idx - the index of the property array
// Return:      true if set completes, false otherwise
bool pgdirSetKernelPage(uint32_t idx) {
    // check index
    if (!pgdirIsValidIdx(idx)) return false;

    // set page
    (*pgdirPgDirPropPID)[idx] = clearBits8((*pgdirPgDirPropPID)[idx],PGDIR_USER_MEM);
    return true;
}

// change the user memory bit in the page directory itself
// Parameters:  idx - The index of the page directory
//              isuser - memory is user
//              invalidate - Invalidate page
bool pgdirChgUserPage(uint32_t idx, bool isuser, bool invalidate) {
    // check index
    if (!pgdirIsValidIdx(idx)) return false;

    if (isuser) {
        // set it to user
        // if it's already user we can't set it again
        if (pgdirIsUserPage(idx)) return false;

        // set it to user
        if (!pgdirSetUserPage(idx)) return false;
    } else {
        // set it to kernel
        // if it's already kernel, we can't make kernel again
        if (pgdirIsKernelPage(idx)) return false;

        // set it to kernel
        if (!pgdirSetKernelPage(idx)) return false;
    }

    // do we invalidate
    if (invalidate) {
        invalidatePage(&(*pgdirPageDirectoryPID)[idx]);
    }

    return true;
}


// Is the read only in the property array
// Parameters:  idx - The index of the property array
// Returns:     true if 4K, false otherwise
bool pgdirIsReadOnly(uint32_t idx) {
    // check parmeter
    if (!pgdirIsValidIdx(idx)) return false;

    return !isBitSet8((*pgdirPgDirPropPID)[idx],PGDIR_READ_WRITE);
}

// Is the read write in the property array
// Parameters:  idx - The index of the property array
// Returns:     true if 4K, false otherwise
bool pgdirIsReadWrite(uint32_t idx) {
    
    return !pgdirIsReadOnly(idx);
}

// Is the Read only in the actual page directory 4K
// Parameters:  idx - The index of the page directory
// Returns:     true if disableed, false otherwise
bool pgdirIsReadOnlyMem(uint32_t idx) {
    // check index
    if (!pgdirIsValidIdx(idx)) return false;

    // send back if it is
    return (!isBitSet32((*pgdirPageDirectoryPID)[idx],DIR_READ_WRITE));
}

// Is the actual page directory read/write
// Parameters:  idx - The index of the page directory
// Returns:     true if disableed, false otherwise
bool pgdirIsReadWriteMem(uint32_t idx) {
    return !pgdirIsReadOnlyMem(idx);
}

// Set page properties to read only
// Parameters:  idx - the index of hte property page
// Returns:     tru if changed, false otherwise
bool pgdirSetReadOnly(uint32_t idx) {
    // check index
    if (!pgdirIsValidIdx(idx)) return false;

    // if it's already read only we can't set again
    if (pgdirIsReadOnly(idx)) return false;
    // change it
    (*pgdirPgDirPropPID[idx]) = clearBits8((*pgdirPgDirPropPID[idx]),PGDIR_READ_WRITE);
    return true;
}

// Set page properties to read/write
// Parameters:  idx - The index of the property page
// Returns:     true if changed, false otherwise
bool pgdirSetReadWrite(uint32_t idx) {
    // check index
    if (!pgdirIsValidIdx(idx)) return false;

    // if it is already read/write we can't change it
    if (pgdirIsReadWrite(idx)) return false;

    // change it
    (*pgdirPgDirPropPID[idx]) = setBits8((*pgdirPgDirPropPID[idx]),PGDIR_READ_WRITE);
    return true;
}

// change the read/write bit in the actual page directory
// Parameters:  idx - The index of the page directory entyr
// Returns:     true if successful, false otherwise
bool pgdirChgReadOnly(uint32_t idx, bool readonly, bool invalidate) {
    // check the index
    if (!pgdirIsValidIdx(idx)) return false;

    // change it
    if (readonly) {
        // set read only
        // if it is already read only we can't change it
        if (pgdirIsReadOnlyMem(idx)) return false;

        // Change it 
        (*pgdirPageDirectoryPID[idx]) = clearBits32((*pgdirPageDirectoryPID[idx]),DIR_READ_WRITE);
    } else {
        // Set read/write
        // if it's already read write don't set it
        if (pgdirIsReadWriteMem(idx)) return false;

        // Change it
        (*pgdirPageDirectoryPID[idx]) = setBits32((*pgdirPageDirectoryPID[idx]),DIR_READ_WRITE);
    }

    if (invalidate) {
        invalidatePage(&(*pgdirPageDirectoryPID)[idx]);
    }

    return true;
}

// is the page directory page currently present in memory
// Parameters:  idx - Index of page directory entry
// returns:     True if present, false otherwise
bool pgdirIsPresentMem(uint32_t idx) {
    // is the index valid
    if (!pgdirIsValidIdx(idx)) return false;

    // send it back
    return (isBitSet32((*pgdirPageDirectoryPID[idx]),DIR_PRESENT));
}

// set the page directory entry to present 
// Parameters:  idx - the page directory index
//              invalidate - do you want to invalidate the page
// Returns:     True if successful, false otherwise
bool pgdirSetPresentMem(uint32_t idx, bool invalidate) {
    // check index
    if (!pgdirIsValidIdx(idx)) return false;

    // is it already present
    if (pgdirIsPresentMem(idx)) return false;

    // set present
    (*pgdirPageDirectoryPID[idx]) = setBits32((*pgdirPageDirectoryPID[idx]), DIR_PRESENT);
    
    // invalidate?
    if (invalidate) {
        invalidatePage(&(*pgdirPageDirectoryPID[idx]));
    }

    return true;
}

// clear the present bit on the actual page dir
// Parameters:  idx - The index of the page directory entry
//              invalidte - Invalidate the page?
// Returns:     true if successful, false otherwise
bool pgdirClearPresentMem(uint32_t idx, bool invalidate) {
    // check index
    if (!pgdirIsValidIdx(idx)) return false;

    // if it is already clear we can't clear it
    if (!pgdirIsPresentMem(idx)) return false;

    // clear it
    (*pgdirPageDirectoryPID[idx]) = clearBits32((*pgdirPageDirectoryPID[idx]), DIR_PRESENT);
    
    // invalidate?
    if (invalidate) {
        invalidatePage(&(*pgdirPageDirectoryPID[idx]));
    }

    return true;
}
// Change the properties to an Attribute mask
// Parameters:  idx - index of property array
// Returns:     a mask of the properties or 0 if errnor
uint32_t pgdirPropToAttr(uint32_t idx) {
    // check index 
    if (!pgdirIsValidIdx(idx)) return 0;
    // if it isn't present, you shouldn't be checking
    if (!((*pgdirPgDirPropPID)[idx] & PGDIR_PRESENT)) return 0;

    uint32_t mask = DIR_PRESENT;
    if ((*pgdirPgDirPropPID)[idx] & PGDIR_READ_WRITE) mask |= DIR_READ_WRITE;
    if ((*pgdirPgDirPropPID)[idx] & PGDIR_USER_MEM) mask |= DIR_USER_MEM;
    if ((*pgdirPgDirPropPID)[idx] & PGDIR_PAGE_WRITE_THRU) mask |= DIR_PAGE_WRITE_THRU;
    if ((*pgdirPgDirPropPID)[idx] & PGDIR_CACHE_DISABLE) mask |= DIR_CACHE_DISABLE;
    if ((*pgdirPgDirPropPID)[idx] & PGDIR_4M) mask |= DIR_4M;
    if ((*pgdirPgDirPropPID)[idx] & PGDIR_GLOBAL) mask |= DIR_GLOBAL;
    if ((*pgdirPgDirPropPID)[idx] & PGDIR_PAGE_ATTR_TBL) mask |= DIR_PAGE_ATTR_TBL;

    return mask;  
}

// Evaluate page directory entry and see if it matches the attributes
// ignores dirty and accesses bits
// Parameters:  idx - the page directories index
// Returns:     True if OK, false otherwise
bool pgdirEvalPde(uint32_t idx) {
    // check parameters
    if (!pgdirIsValidIdx(idx)) return false;
    auto dir = ((*pgdirPageDirectoryPID)[idx] & PGDIR_ATTR_MASK);
    auto mask = pgdirPropToAttr(idx);
    if (dir != mask) printf("pgdirEvalPde failed on index idx(0x%xl), dir = 0x%xl, mask = 0x%xl\n\r", idx,dir,mask);
    return (dir == mask);
}


// Set page directory entry's attributes to as they should be
// leaves dirty and accessed bits as they were
// Parameters:  idx - The page directory index
//              invalidate - Set page directory entries to as they should be
// Returns:     true if successful, false otherwise
bool pgdirFixPde(uint32_t idx, bool invalidate) {
    // check parameters
    if (!pgdirIsValidIdx(idx)) return false;
    // does it need changing
    if (pgdirEvalPde(idx)) return true;

    // it needs changing
    uint32_t mask = (*pgdirPropToAttr)(idx);  // initial mask
    mask |= 0b100000; // set accessed
    mask |= 0b1000000; // set dirty
    mask |= 0xFFFFE000; // high bits

    (*pgdirPageDirectoryPID)[idx] &=  mask;
    if (invalidate) {
        invalidatePage(&(*pgdirPageDirectoryPID)[idx]);
    }

    return true;
}

// sync page directory to its attributes and flush
// Parameters:  None
// Returns:     True if there were no errors, false if there were errors
bool pgdirSyncAttr(bool fix) {
    
    bool rtncde = true;

    for (uint32_t i = 0; i < 1024; i++) {
        // does it exist, if not the rest doesn't matter
        if ((*pgdirPageDirectoryPID)[i] & DIR_PRESENT) {
            // is it out of sync
            if (!pgdirEvalPde(i)) {
                if (fix) {
                    pgdirFixPde(i, true);
                }
                rtncde = false;
            }
        }
        
    }

    return rtncde;
}

// Validate the page directory attributes with the actual page directory
// Parameters:  sync -- Resync attributes from actual page directory
// Returns:     true if in sync, false otherwise, sync should ensure a true return
bool pgdirValidatePgDir(bool sync) {
    return pgdirSyncAttr(sync);
}

// Initialize the Page Directory structures
// Parameters:  None
// Returns:     true if successful, false otherwise
bool pgdirInitPgDirProps() {
    bool rtncde = true;

    for (uint32_t i = 0; i < 1024; i++) {
        //make sure entry is i n use
        if ((*pgdirPageDirectoryPID)[i]) {
            (*pgdirPgDirPropPID)[i] = 0; // make sure it's clean
            if ((*pgdirPageDirectoryPID)[i] & DIR_PRESENT) (*pgdirPgDirPropPID)[i] |= PGDIR_PRESENT;
            if ((*pgdirPageDirectoryPID)[i] & DIR_READ_WRITE) (*pgdirPgDirPropPID)[i] |= PGDIR_READ_WRITE;
            if ((*pgdirPageDirectoryPID)[i] & DIR_USER_MEM) (*pgdirPgDirPropPID)[i] |= PGDIR_USER_MEM;   
            if ((*pgdirPageDirectoryPID)[i] & DIR_PAGE_WRITE_THRU) (*pgdirPgDirPropPID)[i] |= PGDIR_PAGE_WRITE_THRU;
            if ((*pgdirPageDirectoryPID)[i] & DIR_CACHE_DISABLE) (*pgdirPgDirPropPID)[i] |= PGDIR_CACHE_DISABLE;
            if ((*pgdirPageDirectoryPID)[i] & DIR_4M) (*pgdirPgDirPropPID)[i] |= PGDIR_4M;
            if ((*pgdirPageDirectoryPID)[i] & DIR_GLOBAL) (*pgdirPgDirPropPID)[i] |= PGDIR_GLOBAL;
            if ((*pgdirPageDirectoryPID)[i] & DIR_PAGE_ATTR_TBL) (*pgdirPgDirPropPID)[i] |= PGDIR_PAGE_ATTR_TBL;
        } else {
            (*pgdirPgDirPropPID)[i] = 0;
            // properties common to all page directory entries
            pgdirSet4M(i);
        }
    }

    // set kernel flags
    // any address above C0000000 is kernel
    for (uint32_t idx = 768; idx < 1024; idx++) {
        pgdirSetKernelPage(idx);
    }

    return rtncde;
}

// Externals

// Set the current page directory
// Parameters:  pgdir - A pointer to the page directory
// Returns:     The old page directory
PageDirectory_t *pgdirSetPgDir(PageDirectory_t *dir) {
    auto olddir = pgdirPageDirectoryPID;
    if (!dir) {
        pgdirPageDirectoryPID = &page_directory;
    } else {
        pgdirPageDirectoryPID = dir;
    };

    return olddir;
}

// Set the current page directory property page
// Parameters:  props -- The propterty array to use
// Returns:     A pointer to the old property array
PgDirProp_t * pgdirSetPgDirProps(PgDirProp_t * props) {
    auto oldprops = pgdirPgDirPropPID;
    if (!props) {
        pgdirPgDirPropPID = &pgdirPgDirProp;
    } else {
        pgdirPgDirPropPID = props;
    }

    return oldprops;
}

// swap page directory
// Parameters:  dir -- the new page directory
//              props -- The new property array
// Returns:     true if successful, false otherwise
bool pgdirSwapPgDir(PageDirectory_t * dir, PgDirProp_t *props) {
    // check you get both or neither
    if (!dir && props) return false;
    if (dir && !props) return false;   

    // set the page directory
    pgdirSetPgDir(dir);
    // set properties array
    pgdirSetPgDirProps(props);

    return true;
}

// Initialize the Page Directory structures
// Parameters:  dir - the page directory to initialize, if null the kernels page directory
//              props - The property array of the page directory, if null defaults to pgdirPgDirProp
// Returns:     true if successful, false otherwise
bool pgdirInitPgDir(PageDirectory_t * dir, PgDirProp_t *props) {
    if (!pgdirSwapPgDir(dir, props)) return false;

    // the *PID pointers are set, so we can init
    // init flags
    // set property flags
    bool rtncde = pgdirInitPgDirProps();

 
    return rtncde;
}