#include "memory.h"

//Credits: learn_more, stevemk14ebr
size_t findPattern(const PBYTE rangeStart, size_t len, const char* pattern)
{
	size_t l = strlen(pattern);
	PBYTE patt_base = static_cast<PBYTE>(malloc(l >> 1));
	PBYTE msk_base = static_cast<PBYTE>(malloc(l >> 1));
	PBYTE pat = patt_base;
	PBYTE msk = msk_base;
	if (pat && msk)
	{
		l = 0;
		while (*pattern)
		{
			if (*pattern == ' ')
				pattern++;
			if (!*pattern)
				break;
			if (*(PBYTE)pattern == (BYTE)'\?')
			{
				*pat++ = 0;
				*msk++ = '?';
				pattern += ((*(PWORD)pattern == (WORD)'\?\?') ? 2 : 1);
			}
			else
			{
				*pat++ = getByte(pattern);
				*msk++ = 'x';
				pattern += 2;
			}
			l++;
		}
		*msk = 0;
		pat = patt_base;
		msk = msk_base;
		for (size_t n = 0; n < (len - l); ++n)
		{
			if (isMatch(rangeStart + n, patt_base, msk_base))
			{
				free(patt_base);
				free(msk_base);
				return n;
			}
		}
		free(patt_base);
		free(msk_base);
	}
	return -1;
}

uint64_t Memory::get_proc_baseaddr()
{
	return proc.baseaddr;
}

process_status Memory::get_proc_status()
{
	return status;
}

void Memory::check_proc()
{
	if (status == process_status::FOUND_READY)
	{
		short c;
        Read<short>(proc.baseaddr, c);

		if (c != 0x5A4D)
		{
			status = process_status::FOUND_NO_ACCESS;
			close_proc();
		}
	}
}

void Memory::open_proc(const char* name)
{
    if(!conn)
    {
        ConnectorInventory *inv = inventory_scan();
        conn = inventory_create_connector(inv, "qemu_procfs", "");
        inventory_free(inv);
    }

    if (conn)
    {
        if(!kernel)
        {
            kernel = kernel_build(conn);
        }

        if(kernel)
        {
            Kernel *tmp_ker = kernel_clone(kernel);
		    proc.hProcess = kernel_into_process(tmp_ker, name);
        }
		
        if (proc.hProcess)
        {
			Win32ModuleInfo *module = process_module_info(proc.hProcess, name);

			if (module)
            {
				OsProcessModuleInfoObj *obj = module_info_trait(module);
				proc.baseaddr = os_process_module_base(obj);
				os_process_module_free(obj);
				mem = process_virt_mem(proc.hProcess);

		    /////////////////////////////////CR3 FIX//////////////////////////////
	//修复cr3
    const short MZ_HEADER = 0x5a4d;
    char *base_section = new char[8];
    long *base_section_value= (long *)base_section;
    memset(base_section, 0, 8);
    CSliceMut<uint8_t> slice(base_section, 8);
    os.read_raw_into(proc.hProcess.info()->address + 0x520, slice); //win10
    proc.baseaddr=*base_section_value;
    	//遍历dtb
    for (size_t dtb = 0; dtb < SIZE_MAX; dtb += 0x1000){
        proc.hProcess.set_dtb(dtb, Address_INVALID);
        short c5;
        Read<short>(*base_section_value,c5);
            if(c5==MZ_HEADER){
	break;
            }
        }
	////////////////////////////////////END/////////////////////////////////// 
                status = process_status::FOUND_READY;
            }
            else
            {
                status = process_status::FOUND_NO_ACCESS;
				close_proc();
            }
        }
        else
        {
            status = process_status::NOT_FOUND;
        }
    }
    else
    {
        printf("Can't create connector\n");
		exit(0);
    }
}

void Memory::close_proc()
{
	if (proc.hProcess)
	{
		process_free(proc.hProcess);
		virt_free(mem);	
	}

	proc.hProcess = 0;
	proc.baseaddr = 0;
	mem = 0;
}

uint64_t Memory::ScanPointer(uint64_t ptr_address, const uint32_t offsets[], int level)
{
	if (!ptr_address)
		return 0;

	uint64_t lvl = ptr_address;

	for (int i = 0; i < level; i++)
	{
		if (!Read<uint64_t>(lvl, lvl) || !lvl)
			return 0;
		lvl += offsets[i];
	}

	return lvl;
}
