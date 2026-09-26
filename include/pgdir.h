// Page directory functions

#ifndef _PGDIR_H
#define _PGDIR_H

// typedefs

typedef uint32_t PageDirectory_t[1024];

// externals
extern PageDirectory_t page_directory;

// page directory externals

// is valid page directory index
// Parameters:  idx = the index to check
// Returns:     true if index is in the range of 0-1023
extern bool pgdirIsValidIdx(uint32_t idx);

// Validate the page directory attributes with the actual page directory
// Parameters:  sync -- Resync attributes from actual page directory
// Returns:     true if in sync, false otherwise, sync should ensure a true return
extern bool pgdirValidatePgDir(bool sync);

// Is the page directory Page Attribute Table bit set
// Parameters:  idx - page directory index
// Returns:     true if set, false otherwise
extern bool pgdirIsPATMem(uint32_t idx);

// Is the property page directory attribute global
// Parameters:  idx - The property page index
// Returns:     true if is global, false otherwise
extern bool pgdirIsGlobal(uint32_t idx);

// Is the page directory attribute global
// Parameters:  idx - The page directory index
// Returns:     true if is global, false otherwise
extern bool pgdirIsGlobalMem(uint32_t idx);

// Set the page direcotry property bit to Global
// Parameteers: idx - The index of the property page
// Returns:     true on success, false otherwise
extern bool pgdirSetGlobal(uint32_t idx);

// Clear the page directory property bit for global
// Paraters:    idx - The indx of the proper page
// Returns:     true if successful, false otherwise
extern bool pgdirClearGlobal(uint32_t idx);

// Change the global bit on the page direcory
// Parameters:  idx - The index of the page directory
//              seton - Do we set the bit on (otherwise off)
//              invalidate - Do we want to invalidate the page
extern bool pgdirChgGlobalMem(uint32_t idx, bool seton, bool invalidate);

// Is the page size 4K
// Parameters:  idx - The index of the property array
// Returns:     true if 4K, false otherwise
extern bool pgdirIs4K(uint32_t idx);

// Is the page in the actual page directory 4K
// Parameters:  idx - The index of the page directory
// Returns:     true if 4K, false otherwise
extern bool pgdirIs4kMem(uint32_t idx);

// Set the 4K bit in the page directory property table
// Parameters:  idx - Index in the property table
// Returns:     true changed, false otherwise
extern bool pgdirSet4k(uint32_t idx);

// clear the 4k bit in the page directory property table
// Parameters:  idx - The index in the property table
// Returns:     true if changed, false otherwise
extern bool pgdirClear4k(uint32_t idx);

// clear or set the 4K page bit in the actual page directory
// Parameters:  idx - The page directory index
//              seton - Set bit on
//              invalidate - invalidate page
// Return:      true if successful, false otherwise
extern bool pgdirChg4kMem(uint32_t idx, bool seton, bool invalidate);

// Is the cache disabled in the property array
// Parameters:  idx - The index of the property array
// Returns:     true if 4K, false otherwise
extern bool pgdirIsCacheDisabled(uint32_t idx);

// Is the disabled in the actual page directory 4K
// Parameters:  idx - The index of the page directory
// Returns:     true if disableed, false otherwise
extern bool pgdirIsCacheDisabledMem(uint32_t idx);

// Is the write through in the property array
// Parameters:  idx - The index of the property array
// Returns:     true if 4K, false otherwise
extern bool pgdirIsWriteThru(uint32_t idx);

// Is the write through in the actual page directory 4K
// Parameters:  idx - The index of the page directory
// Returns:     true if disableed, false otherwise
extern bool pgdirIsWriteThruMem(uint32_t idx);

// Is the User in the property array
// Parameters:  idx - The index of the property array
// Returns:     true if 4K, false otherwise
extern bool pgdirIsUserPage(uint32_t idx);

// Is the User in the actual page directory 4K
// Parameters:  idx - The index of the page directory
// Returns:     true if disableed, false otherwise
extern bool pgdirIsUserPageMem(uint32_t idx);

// Is the Kernel in the property array
// Parameters:  idx - The index of the property array
// Returns:     true if 4K, false otherwise
extern bool pgdirIsKernelPage(uint32_t idx);

// Is the Kernel in the actual page directory 4K
// Parameters:  idx - The index of the page directory
// Returns:     true if disabled, false otherwise
extern bool pgdirIsKernelPageMem(uint32_t idx);

// set the property array to user for page
// Parameters:  idx - the index of the property array
// Return:      true if set completes, false otherwise
extern bool pgdirSetUserPage(uint32_t idx);

// set the property array to Kernel for page
// Parameters:  idx - the index of the property array
// Return:      true if set completes, false otherwise
extern bool pgdirSetKernelPage(uint32_t idx);

// change the user memory bit in the page directory itself
// Parameters:  idx - The index of the page directory
//              isuser - memory is user
//              invalidate - Invalidate page
extern bool pgdirChgUserPage(uint32_t idx, bool isuser, bool invalidate);

// Is the read only in the property array
// Parameters:  idx - The index of the property array
// Returns:     true if 4K, false otherwise
extern bool pgdirIsReadOnly(uint32_t idx);

// Is the read write in the property array
// Parameters:  idx - The index of the property array
// Returns:     true if 4K, false otherwise
extern bool pgdirIsReadWrite(uint32_t idx);

// Is the Read only in the actual page directory 4K
// Parameters:  idx - The index of the page directory
// Returns:     true if disableed, false otherwise
extern bool pgdirIsReadOnlyMem(uint32_t idx);

// Is the actual page directory read/write
// Parameters:  idx - The index of the page directory
// Returns:     true if disableed, false otherwise
extern bool pgdirIsReadWriteMem(uint32_t idx);

// Set page properties to read only
// Parameters:  idx - the index of hte property page
// Returns:     tru if changed, false otherwise
extern bool pgdirSetReadOnly(uint32_t idx);

// Set page properties to read/write
// Parameters:  idx - The index of the property page
// Returns:     true if changed, false otherwise
extern bool pgdirSetReadWrite(uint32_t idx);

// change the read/write bit in the actual page directory
// Parameters:  idx - The index of the page directory entyr
// Returns:     true if successful, false otherwise
extern bool pgdirChgReadOnly(uint32_t idx, bool readonly, bool invalidate);

// is the page directory page currently present in memory
// Parameters:  idx - Index of page directory entry
// returns:     True if present, false otherwise
extern bool pgdirIsPresentMem(uint32_t idx);

// set the page directory entry to present 
// Parameters:  idx - the page directory index
//              invalidate - do you want to invalidate the page
// Returns:     True if successful, false otherwise
extern bool pgdirSetPresentMem(uint32_t idx, bool invalidate);

// clear the present bit on the actual page dir
// Parameters:  idx - The index of the page directory entry
//              invalidte - Invalidate the page?
// Returns:     true if successful, false otherwise
extern bool pgdirClearPresentMem(uint32_t idx, bool invalidate);

// Set the current page directory
// Parameters:  pgdir - A pointer to the page directory
// Returns:     The old page directory
PageDirectory_t *pgdirSetPgDir(PageDirectory_t *dir);

// Initialize the Page Directory structures
// Parameters:  None
// Returns:     true if successful, false otherwise
extern bool pgdirInitPgDir();

#endif
