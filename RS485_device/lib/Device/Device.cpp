
#include "Device.h"


Reg::Reg(
        uint16_t addr_ ,
        uint32_t data_,
        bool readable_ ,
        bool writable_,
        bool executable_
        )
{
    addr = addr_;
    data = data_;
    readable = readable_;
    writable = writable_;
    executable = executable_;
}

SlaveMessage BaseDevice::generate_error_sm(uint8_t command, uint8_t error_code)
{
    // This function generates standart error slave message 
    // with self uid and given error code and requested command
    // All value fields filled with d_stub_value constant
    SlaveMessage error_message = SlaveMessage(
        (uint8_t)((_uid.data & 0x0000FF00) >> 8), // uint8_t NET_ADDR_1, - 
        (uint8_t)((_uid.data & 0x000000FF)),  //uint8_t DEV_ADDR_2,
        d_stub_value,   // uint8_t VALUE_1_,
        d_stub_value,   // uint8_t VALUE_2_,
        d_stub_value,   // uint8_t VALUE_3_,
        d_stub_value,   // uint8_t VALUE_4_,
        command , // uint8_t STATUS_1_,
        error_code // uint8_t STATUS_2_
    );
    return error_message;
}

SlaveMessage BaseDevice::generate_success_sm(uint8_t command, uint32_t data)
{
    // This function generates standart success slave message 
    // with self uid and given data and requested command
    SlaveMessage success_message = SlaveMessage(
        (uint8_t)((_uid.data & 0x0000FF00) >> 8), // uint8_t NET_ADDR_1, - 
        (uint8_t)((_uid.data & 0x000000FF)),  //uint8_t DEV_ADDR_2,
        (uint8_t)((data & 0xFF000000) >> 24),   // uint8_t VALUE_1_,
        (uint8_t)((data & 0x00FF0000) >> 16),   // uint8_t VALUE_2_,
        (uint8_t)((data & 0x0000FF00) >> 8),   // uint8_t VALUE_3_,
        (uint8_t)((data & 0x000000FF)),   // uint8_t VALUE_4_,
        command , // uint8_t STATUS_1_,
        d_success // uint8_t STATUS_2_
    );
    return success_message;
}


BaseDevice::BaseDevice(uint16_t uid, bool debug)
{
    // init all registers manually
    // its not so bad
    // we need them and there must be 3-4 registers for every device

    // init _uid register  read-only
    _uid = Reg(
        d_device_data_addr,  //default addr of _uid register
        uid,
        true,
        false,
        false
    );
    // now lets create regs array
    regs_len = 1;
    regs = new Reg*[regs_len];
    regs[0] = &_uid;  // put here pointer for _uid
    // thats all folks
    _debug_s = &Serial;
    D_RS485_DEBUG_OUTPUT_ALLOWED = debug;
}

SlaveMessage BaseDevice::handle_mm(MasterMeassage mm)
{
    // main procedure - handle mm and return correct sm with result
    // or with error flag

    if(D_RS485_DEBUG_OUTPUT_ALLOWED)
    {
    _debug_s->println(F("device: We start handle mm "));
    }

    // at first lets check if there is such register
    Reg *actual_reg = nullptr;
    uint16_t reg_addr = d_unite2(mm.REG_ADDR_1, mm.REG_ADDR_2);
    if(D_RS485_DEBUG_OUTPUT_ALLOWED)
    {
        _debug_s->print(F("we want find that: "));
        _debug_s->print(mm.REG_ADDR_1, HEX);
        _debug_s->print(" & ");
        _debug_s->print(mm.REG_ADDR_2, HEX);
        _debug_s->print("  =  ");
        _debug_s->println(reg_addr);
    }
    for(int i = 0; i<regs_len; i++)
    {
        if(regs[i]->addr == reg_addr)
        {
            // we have found
            if(D_RS485_DEBUG_OUTPUT_ALLOWED)
            {
            _debug_s->print(F("we have found register: "));
            _debug_s->println(reg_addr);
            }
            actual_reg = regs[i];
        }
    }
    // if we have not found
    if(actual_reg == nullptr)
    {
        if(D_RS485_DEBUG_OUTPUT_ALLOWED)
        {
        _debug_s->println(F("we have not found register"));
        }
        // return slave message with error  d_no_such_register
        return generate_error_sm(mm.COMMAND, d_no_such_register);
    }
    else
    {
        // next step of handling
        // then lets check if there is correct command
        // and use correct method
        // if not - return slave message with error d_incorrect_command
        switch (mm.COMMAND)
        {
            case d_user_read:
            {
                // read register and return generated sm
                return _read_register(actual_reg, false);
                break;
            }
            case d_user_write:
            {
                // write 
                uint32_t new_data = d_unite4(mm.VALUE_1, mm.VALUE_2, mm.VALUE_3, mm.VALUE_4);
                return _write_register(actual_reg, new_data, false);
                break;
            }
            case d_user_execute:
            {
                // execute
                return _execute_register(actual_reg, &mm);
                break;
            }
            case d_admin_read:
            {
                // read register as admin and return generated sm
                return _read_register(actual_reg, true);
                break;
            }
            case d_admin_write:
            {
                // write register as admin and return generated sm
                uint32_t ndata = d_unite4(mm.VALUE_1, mm.VALUE_2, mm.VALUE_3, mm.VALUE_4);
                return _write_register(actual_reg, ndata, true);
                break;
            }
            case d_admin_execute:
            {
                // execute register as admin and return generated sm
                return _execute_register(actual_reg, &mm, true);
                break;
            }
            default:
            {
                // return slave message with error  d_incorrect_command and goodbye
                return generate_error_sm(mm.COMMAND, d_incorrect_command);
                break;
            }
        }
    }
}

SlaveMessage BaseDevice::_read_register(Reg* r, bool isadmin)
{
    // handle all things about reading data from register
    if(D_RS485_DEBUG_OUTPUT_ALLOWED)
    {
    _debug_s->print(F("device: We start read register: 0x"));
    _debug_s->println(r->addr, HEX);
    }
    if(isadmin)
    {
        // we should use admin method
        // so ignore permission flags
        // lets get data from register and put to VALUE fields of message
        return  generate_success_sm(d_admin_read, r->data);
    }
    else
    {
        // we should check if there is readable flag
        if(r->readable != true)
        {
            // return error sm with d_access_denied error flag
            return generate_error_sm(d_user_read, d_access_denied);
        }
        else
        {
            // lets get data from register and put to VALUE fields of message
            return  generate_success_sm(d_user_read, r->data);
        }
        
    } 
}

SlaveMessage BaseDevice::_write_register(Reg* r, uint32_t new_data, bool isadmin)
{
    // handle all things about writing data from register
    if(D_RS485_DEBUG_OUTPUT_ALLOWED)
    {
    _debug_s->print(F("device: We start write to register: 0x"));
    _debug_s->println(r->addr, HEX);
    }
    if(isadmin)
    {
        // we should use admin method
        // so ignore permission flags
        // lets write data to register and put that data to VALUE fields of message again
        r->data = new_data;
        return  generate_success_sm(d_admin_write, r->data);
    }
    else
    {
        // we should check if there is readable flag
        if(r->writable != true)
        {
            // return error sm with d_access_denied error flag
            return generate_error_sm(d_user_write, d_access_denied);
        }
        else
        {
            // lets write data to register and put that data to VALUE fields of message again
            r->data = new_data;
            return  generate_success_sm(d_user_write, r->data);
        }
        
    }
}

SlaveMessage BaseDevice::_execute_register(Reg* r, MasterMeassage * mm, bool isadmin)
{
    // handle all things about executing data from register
    {
    if(D_RS485_DEBUG_OUTPUT_ALLOWED)
    {
    _debug_s->print(F("device: We start execute register: 0x"));
    _debug_s->println(r->addr, HEX);
    }
        if(isadmin)
        {
            // we should use admin method
            // so ignore permission flags
            // just execute __private_execute
            return __private_execute(r, mm, true);
        }
        else
        {
            // we should check if there is executable flag
            if(r->executable != true)
            {
                // return error sm with d_access_denied error flag
                return generate_error_sm(d_user_execute, d_access_denied);
            }
            else
            {
                // lets get data from register and put to VALUE fields of message
                return __private_execute(r, mm);
            }
        }
    }
}

SlaveMessage BaseDevice::__private_execute(Reg* r, MasterMeassage* mm, bool isadmin)
{
    // must be overriden in child devices
    // because it uniq for all devices and depends on device role
    if(D_RS485_DEBUG_OUTPUT_ALLOWED)
    {
    _debug_s->print(F("device: We start stub-__private_execute register: 0x"));
    _debug_s->println(r->addr, HEX);
    }
    // in base device just send back r->data
    return generate_success_sm(mm->COMMAND, r->data);
}

void BaseDevice::do_your_duty()
{
    // nothing for base class
    // must be overriden in child class
}

uint16_t BaseDevice::get_uid()
{
    // yeah its stupid but we need some dirty service methods
    uint16_t uid_ = (uint16_t)_uid.data;
    return uid_;
}

// old version
/*SlaveMessage BaseDevice::_execute_register(Reg* r, bool isadmin = false)
{
    // handle all things about executing data from register

    // this function must be overloaded in every new child class
    // because its behavior depends on specific parameters of device


    // here we have to manually (by use hardcode) check if there any function, bounded to this reg
    // if we cannot find, it means that we have to send sm with error  d_access_denied
    // and we dont care are you admin or not 
    
    // lets create pointer to function that gets nothing and returns uint32_t
    uint32_t (*exec_method_pointer)() = nullptr;

    //then in switch-case lets found needed function and put its pointer to exec_method_pointer

    // we use r-> data to keep last execution result
    // and r-> addr to find correspond method between other device methods
    switch (r->addr)  
    {
        // no cases here in base class
        default:
        // by default put here nullptr
        exec_method_pointer = nullptr;
        break;
    }


    // lets use function _execute_method (exec_method_pointer)


    // then lets check if we have found any method


    if(exec_method_pointer == nullptr)
    {
        // return error sm with d_access_denied error flag
        return generate_error_sm(d_user_read, d_access_denied);
    }
    else
    {

        


        if(isadmin)
        {
            // we should use admin method
            // so ignore permission flags
            // just execute exec_method_pointer and get put result to r-> data
            r->data = exec_method_pointer();
            return  generate_success_sm(d_admin_execute, r->data);
        }
        else
        {
            // we should check if there is readable flag
            if(r->executable != true)
            {
                // return error sm with d_access_denied error flag
                return generate_error_sm(d_user_read, d_access_denied);
            }
            else
            {
                // lets get data from register and put to VALUE fields of message
                return  generate_success_sm(d_user_execute, r->data);
            }
            
        }
    }
}*/