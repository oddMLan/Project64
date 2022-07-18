#include "stdafx.h"
#include <Project64-core/N64System/Mips/Register.h>
#include <Project64-core/N64System/N64System.h>
#include <Project64-core/N64System/SystemGlobals.h>
#include <Project64-core/Logging.h>

const char * CRegName::GPR[32] = { "R0", "AT", "V0", "V1", "A0", "A1", "A2", "A3",
"T0", "T1", "T2", "T3", "T4", "T5", "T6", "T7",
"S0", "S1", "S2", "S3", "S4", "S5", "S6", "S7",
"T8", "T9", "K0", "K1", "GP", "SP", "FP", "RA" };

const char *CRegName::GPR_Hi[32] = { "r0.HI", "at.HI", "v0.HI", "v1.HI", "a0.HI", "a1.HI",
"a2.HI", "a3.HI", "t0.HI", "t1.HI", "t2.HI", "t3.HI",
"t4.HI", "t5.HI", "t6.HI", "t7.HI", "s0.HI", "s1.HI",
"s2.HI", "s3.HI", "s4.HI", "s5.HI", "s6.HI", "s7.HI",
"t8.HI", "t9.HI", "k0.HI", "k1.HI", "gp.HI", "sp.HI",
"fp.HI", "ra.HI" };

const char *CRegName::GPR_Lo[32] = { "r0.LO", "at.LO", "v0.LO", "v1.LO", "a0.LO", "a1.LO",
"a2.LO", "a3.LO", "t0.LO", "t1.LO", "t2.LO", "t3.LO",
"t4.LO", "t5.LO", "t6.LO", "t7.LO", "s0.LO", "s1.LO",
"s2.LO", "s3.LO", "s4.LO", "s5.LO", "s6.LO", "s7.LO",
"t8.LO", "t9.LO", "k0.LO", "k1.LO", "gp.LO", "sp.LO",
"fp.LO", "ra.LO" };

const char * CRegName::Cop0[32] = { "Index", "Random", "EntryLo0", "EntryLo1", "Context", "PageMask", "Wired", "",
"BadVAddr", "Count", "EntryHi", "Compare", "Status", "Cause", "EPC", "PRId",
"Config", "LLAddr", "WatchLo", "WatchHi", "XContext", "", "", "",
"", "", "ECC", "CacheErr", "TagLo", "TagHi", "ErrEPC", "" };

const char * CRegName::FPR[32] = { "F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7",
"F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15",
"F16", "F17", "F18", "F19", "F20", "F21", "F22", "F23",
"F24", "F25", "F26", "F27", "F28", "F29", "F30", "F31" };

const char * CRegName::FPR_Ctrl[32] = { "Revision", "Unknown", "Unknown", "Unknown", "Unknown",
"Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown",
"Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown",
"Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown",
"Unknown", "Unknown", "Unknown", "Unknown", "Unknown", "Unknown",
"Unknown", "Unknown", "FCSR" };

uint32_t      * CSystemRegisters::_PROGRAM_COUNTER = nullptr;
MIPS_DWORD    * CSystemRegisters::_GPR = nullptr;
MIPS_DWORD    * CSystemRegisters::_FPR = nullptr;
uint32_t      * CSystemRegisters::_CP0 = nullptr;
MIPS_DWORD    * CSystemRegisters::_RegHI = nullptr;
MIPS_DWORD    * CSystemRegisters::_RegLO = nullptr;
float        ** CSystemRegisters::_FPR_S;
double       ** CSystemRegisters::_FPR_D;
uint32_t      * CSystemRegisters::_FPCR = nullptr;
uint32_t      * CSystemRegisters::_LLBit = nullptr;
int32_t       * CSystemRegisters::_RoundingModel = nullptr;

CP0registers::CP0registers(uint32_t * _CP0) :
    INDEX_REGISTER(_CP0[0]),
    RANDOM_REGISTER(_CP0[1]),
    ENTRYLO0_REGISTER(_CP0[2]),
    ENTRYLO1_REGISTER(_CP0[3]),
    CONTEXT_REGISTER(_CP0[4]),
    PAGE_MASK_REGISTER(_CP0[5]),
    WIRED_REGISTER(_CP0[6]),
    BAD_VADDR_REGISTER(_CP0[8]),
    COUNT_REGISTER(_CP0[9]),
    ENTRYHI_REGISTER(_CP0[10]),
    COMPARE_REGISTER(_CP0[11]),
    STATUS_REGISTER(_CP0[12]),
    CAUSE_REGISTER(_CP0[13]),
    EPC_REGISTER(_CP0[14]),
    PREVID_REGISTER(_CP0[15]),
    CONFIG_REGISTER(_CP0[16]),
    TAGLO_REGISTER(_CP0[28]),
    TAGHI_REGISTER(_CP0[29]),
    ERROREPC_REGISTER(_CP0[30]),
    FAKE_CAUSE_REGISTER(_CP0[32])
{
}

DisplayControlReg::DisplayControlReg(uint32_t * _DisplayProcessor) :
    DPC_START_REG(_DisplayProcessor[0]),
    DPC_END_REG(_DisplayProcessor[1]),
    DPC_CURRENT_REG(_DisplayProcessor[2]),
    DPC_STATUS_REG(_DisplayProcessor[3]),
    DPC_CLOCK_REG(_DisplayProcessor[4]),
    DPC_BUFBUSY_REG(_DisplayProcessor[5]),
    DPC_PIPEBUSY_REG(_DisplayProcessor[6]),
    DPC_TMEM_REG(_DisplayProcessor[7])
{
}

CRegisters::CRegisters(CN64System * System, CSystemEvents * SystemEvents) :
    CP0registers(m_CP0),
    RDRAMRegistersReg(m_RDRAM_Registers),
    MIPSInterfaceReg(m_Mips_Interface),
    VideoInterfaceReg(m_Video_Interface),
    AudioInterfaceReg(m_Audio_Interface),
    PeripheralInterfaceReg(m_Peripheral_Interface),
    RDRAMInterfaceReg(m_RDRAM_Interface),
    SPRegistersReg(m_SigProcessor_Interface),
    DisplayControlReg(m_Display_ControlReg),
    SerialInterfaceReg(m_SerialInterface),
    DiskInterfaceReg(m_DiskInterface),
    m_System(System),
    m_SystemEvents(SystemEvents)
{
    Reset();
}

void CRegisters::Reset()
{
    m_FirstInterupt = true;

    memset(m_GPR, 0, sizeof(m_GPR));
    memset(m_CP0, 0, sizeof(m_CP0));
    memset(m_FPR, 0, sizeof(m_FPR));
    memset(m_FPCR, 0, sizeof(m_FPCR));
    m_HI.DW = 0;
    m_LO.DW = 0;
    m_RoundingModel = FE_TONEAREST;

    m_LLBit = 0;

    // Reset system registers
    memset(m_RDRAM_Interface, 0, sizeof(m_RDRAM_Interface));
    memset(m_RDRAM_Registers, 0, sizeof(m_RDRAM_Registers));
    memset(m_Mips_Interface, 0, sizeof(m_Mips_Interface));
    memset(m_Video_Interface, 0, sizeof(m_Video_Interface));
    memset(m_Display_ControlReg, 0, sizeof(m_Display_ControlReg));
    memset(m_Audio_Interface, 0, sizeof(m_Audio_Interface));
    memset(m_SigProcessor_Interface, 0, sizeof(m_SigProcessor_Interface));
    memset(m_Peripheral_Interface, 0, sizeof(m_Peripheral_Interface));
    memset(m_SerialInterface, 0, sizeof(m_SerialInterface));
    memset(m_DiskInterface, 0, sizeof(m_DiskInterface));

    m_AudioIntrReg = 0;
    m_GfxIntrReg = 0;
    m_RspIntrReg = 0;

    FixFpuLocations();
}

void CRegisters::SetAsCurrentSystem()
{
    _PROGRAM_COUNTER = &m_PROGRAM_COUNTER;
    _GPR = m_GPR;
    _FPR = m_FPR;
    _CP0 = m_CP0;
    _RegHI = &m_HI;
    _RegLO = &m_LO;
    _FPR_S = m_FPR_S;
    _FPR_D = m_FPR_D;
    _FPCR = m_FPCR;
    _LLBit = &m_LLBit;
    _RoundingModel = &m_RoundingModel;
}

void CRegisters::CheckInterrupts()
{
    uint32_t mi_intr_reg = MI_INTR_REG, status_register;
    if (!m_System->bFixedAudio() && CpuType() != CPU_SyncCores)
    {
        mi_intr_reg &= ~MI_INTR_AI;
        mi_intr_reg |= (m_AudioIntrReg & MI_INTR_AI);
    }
    mi_intr_reg |= (m_RspIntrReg & MI_INTR_SP);
    mi_intr_reg |= (m_GfxIntrReg & MI_INTR_DP);
    if ((MI_INTR_MASK_REG & mi_intr_reg) != 0)
    {
        FAKE_CAUSE_REGISTER |= CAUSE_IP2;
    }
    else
    {
        FAKE_CAUSE_REGISTER &= ~CAUSE_IP2;
    }
    MI_INTR_REG = mi_intr_reg;
    status_register = STATUS_REGISTER;

    if ((status_register & STATUS_IE) == 0)
    {
        return;
    }
    if ((status_register & STATUS_EXL) != 0)
    {
        return;
    }
    if ((status_register & STATUS_ERL) != 0)
    {
        return;
    }

    if ((status_register & FAKE_CAUSE_REGISTER & 0xFF00) != 0)
    {
        if (m_FirstInterupt)
        {
            m_FirstInterupt = false;
            if (g_Recompiler)
            {
                g_Recompiler->ClearRecompCode_Virt(0x80000000, 0x200, CRecompiler::Remove_InitialCode);
            }
        }
        m_SystemEvents->QueueEvent(SysEvent_ExecuteInterrupt);
    }
}

void CRegisters::DoAddressError(bool DelaySlot, uint32_t BadVaddr, bool FromRead)
{
    if (HaveDebugger())
    {
        g_Notify->DisplayError(stdstr_f("AddressError %s Vaddr: %X", FromRead ? "reading" : "writing", BadVaddr).c_str());
        if ((STATUS_REGISTER & STATUS_EXL) != 0)
        {
            g_Notify->DisplayError("EXL set in AddressError exception");
        }
        if ((STATUS_REGISTER & STATUS_ERL) != 0)
        {
            g_Notify->DisplayError("ERL set in AddressError exception");
        }
    }

    if (FromRead)
    {
        CAUSE_REGISTER = EXC_RADE;
    }
    else
    {
        CAUSE_REGISTER = EXC_WADE;
    }
    BAD_VADDR_REGISTER = BadVaddr;
    if (DelaySlot)
    {
        CAUSE_REGISTER |= CAUSE_BD;
        EPC_REGISTER = m_PROGRAM_COUNTER - 4;
    }
    else
    {
        EPC_REGISTER = m_PROGRAM_COUNTER;
    }
    STATUS_REGISTER |= STATUS_EXL;
    m_PROGRAM_COUNTER = 0x80000180;
}

void CRegisters::FixFpuLocations()
{
    if ((STATUS_REGISTER & STATUS_FR) == 0)
    {
        for (int count = 0; count < 32; count++)
        {
            m_FPR_S[count] = &m_FPR[count & ~1].F[count & 1];
            m_FPR_D[count] = &m_FPR[count & ~1].D;
        }
    }
    else
    {
        for (int count = 0; count < 32; count++)
        {
            m_FPR_S[count] = &m_FPR[count].F[0];
            m_FPR_D[count] = &m_FPR[count].D;
        }
    }
}

void CRegisters::DoBreakException(bool DelaySlot)
{
    if (HaveDebugger())
    {
        if ((STATUS_REGISTER & STATUS_EXL) != 0)
        {
            g_Notify->DisplayError("EXL set in break exception");
        }
        if ((STATUS_REGISTER & STATUS_ERL) != 0)
        {
            g_Notify->DisplayError("ERL set in break exception");
        }
    }

    CAUSE_REGISTER = EXC_BREAK;
    if (DelaySlot)
    {
        CAUSE_REGISTER |= CAUSE_BD;
        EPC_REGISTER = m_PROGRAM_COUNTER - 4;
    }
    else
    {
        EPC_REGISTER = m_PROGRAM_COUNTER;
    }
    STATUS_REGISTER |= STATUS_EXL;
    m_PROGRAM_COUNTER = 0x80000180;
}

void CRegisters::DoTrapException(bool DelaySlot)
{
    CAUSE_REGISTER = EXC_TRAP;
    if (DelaySlot)
    {
        EPC_REGISTER = m_PROGRAM_COUNTER - 4;
        CAUSE_REGISTER |= CAUSE_BD;
    }
    else
    {
        EPC_REGISTER = m_PROGRAM_COUNTER;
    }
    m_PROGRAM_COUNTER = 0x80000180;

}

void CRegisters::DoCopUnusableException(bool DelaySlot, int32_t Coprocessor)
{
    if (HaveDebugger())
    {
        if ((STATUS_REGISTER & STATUS_EXL) != 0)
        {
            g_Notify->DisplayError("EXL set in break exception");
        }
        if ((STATUS_REGISTER & STATUS_ERL) != 0)
        {
            g_Notify->DisplayError("ERL set in break exception");
        }
    }

    CAUSE_REGISTER = EXC_CPU;
    if (Coprocessor == 1)
    {
        CAUSE_REGISTER |= 0x10000000;
    }
    if (DelaySlot)
    {
        CAUSE_REGISTER |= CAUSE_BD;
        EPC_REGISTER = m_PROGRAM_COUNTER - 4;
    }
    else
    {
        EPC_REGISTER = m_PROGRAM_COUNTER;
    }
    STATUS_REGISTER |= STATUS_EXL;
    m_PROGRAM_COUNTER = 0x80000180;
}

bool CRegisters::DoIntrException(bool DelaySlot)
{
    if ((STATUS_REGISTER & STATUS_IE) == 0)
    {
        return false;
    }

    if ((STATUS_REGISTER & STATUS_EXL) != 0)
    {
        return false;
    }

    if ((STATUS_REGISTER & STATUS_ERL) != 0)
    {
        return false;
    }

    if (GenerateLog() && LogExceptions() && !LogNoInterrupts())
    {
        LogMessage("%08X: Interrupt generated", m_PROGRAM_COUNTER);
    }

    CAUSE_REGISTER = FAKE_CAUSE_REGISTER;
    CAUSE_REGISTER |= EXC_INT;

    if (DelaySlot)
    {
        CAUSE_REGISTER |= CAUSE_BD;
        EPC_REGISTER = m_PROGRAM_COUNTER - 4;
    }
    else
    {
        EPC_REGISTER = m_PROGRAM_COUNTER;
    }

    STATUS_REGISTER |= STATUS_EXL;
    m_PROGRAM_COUNTER = 0x80000180;
    return true;
}

void CRegisters::DoTLBReadMiss(bool DelaySlot, uint32_t BadVaddr)
{
    CAUSE_REGISTER = EXC_RMISS;
    BAD_VADDR_REGISTER = BadVaddr;
    CONTEXT_REGISTER &= 0xFF80000F;
    CONTEXT_REGISTER |= (BadVaddr >> 9) & 0x007FFFF0;
    ENTRYHI_REGISTER = (BadVaddr & 0xFFFFE000);
    if ((STATUS_REGISTER & STATUS_EXL) == 0)
    {
        if (DelaySlot)
        {
            CAUSE_REGISTER |= CAUSE_BD;
            EPC_REGISTER = m_PROGRAM_COUNTER - 4;
        }
        else
        {
            EPC_REGISTER = m_PROGRAM_COUNTER;
        }
        if (g_TLB->AddressDefined(BadVaddr))
        {
            m_PROGRAM_COUNTER = 0x80000180;
        }
        else
        {
            m_PROGRAM_COUNTER = 0x80000000;
        }
        STATUS_REGISTER |= STATUS_EXL;
    }
    else
    {
        if (HaveDebugger())
        {
            g_Notify->DisplayError(stdstr_f("TLBMiss - EXL set\nBadVaddr = %X\nAddress defined: %s", BadVaddr, g_TLB->AddressDefined(BadVaddr) ? "true" : "false").c_str());
        }
        m_PROGRAM_COUNTER = 0x80000180;
    }
}

void CRegisters::DoTLBWriteMiss(bool DelaySlot, uint32_t BadVaddr)
{
    CAUSE_REGISTER = EXC_WMISS;
    BAD_VADDR_REGISTER = BadVaddr;
    CONTEXT_REGISTER &= 0xFF80000F;
    CONTEXT_REGISTER |= (BadVaddr >> 9) & 0x007FFFF0;
    ENTRYHI_REGISTER = (BadVaddr & 0xFFFFE000);
    if ((STATUS_REGISTER & STATUS_EXL) == 0)
    {
        if (DelaySlot)
        {
            CAUSE_REGISTER |= CAUSE_BD;
            EPC_REGISTER = m_PROGRAM_COUNTER - 4;
        }
        else
        {
            EPC_REGISTER = m_PROGRAM_COUNTER;
        }
        if (g_TLB->AddressDefined(BadVaddr))
        {
            m_PROGRAM_COUNTER = 0x80000180;
        }
        else
        {
            m_PROGRAM_COUNTER = 0x80000000;
        }
        STATUS_REGISTER |= STATUS_EXL;
    }
    else
    {
        if (HaveDebugger())
        {
            g_Notify->DisplayError(stdstr_f("TLBMiss - EXL set\nBadVaddr = %X\nAddress defined: %s", BadVaddr, g_TLB->AddressDefined(BadVaddr) ? "true" : "false").c_str());
        }
        m_PROGRAM_COUNTER = 0x80000180;
    }
}

void CRegisters::DoSysCallException(bool DelaySlot)
{
    if (HaveDebugger())
    {
        if ((STATUS_REGISTER & STATUS_EXL) != 0)
        {
            g_Notify->DisplayError("EXL set in syscall exception");
        }
        if ((STATUS_REGISTER & STATUS_ERL) != 0)
        {
            g_Notify->DisplayError("ERL set in syscall exception");
        }
    }

    CAUSE_REGISTER = EXC_SYSCALL;
    if (DelaySlot)
    {
        CAUSE_REGISTER |= CAUSE_BD;
        EPC_REGISTER = m_PROGRAM_COUNTER - 4;
    }
    else
    {
        EPC_REGISTER = m_PROGRAM_COUNTER;
    }
    STATUS_REGISTER |= STATUS_EXL;
    m_PROGRAM_COUNTER = 0x80000180;
}
