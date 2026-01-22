# include <iostream>
# include <cstring>
# include <fstream>

// Mem region
const int mem_size = 1024 * 1024;
long long memory[mem_size];

// Thread Meta Data
auto state                      = []() -> long long& {return memory[mem_size-1];};
auto thread_end_index           = []() -> long long& {return memory[mem_size-2];};
auto frame_count                = []() -> long long& {return memory[mem_size-3];};
auto current_frame_index        = []() -> long long& {return memory[mem_size-4];};

// Frame Meta Data
auto current_command_index      = []() -> long long& {return memory[current_frame_index()];};
auto prev_frame_index           = []() -> long long& {return memory[current_frame_index()-1];};
auto local_value_max_size       = []() -> long long& {return memory[current_frame_index()-2];};
auto operand_stack_size         = []() -> long long& {return memory[current_frame_index()-3];};

// Using ptr
auto current_word               = []() -> long long& {return memory[current_command_index()];};
auto operand_value              = [](int index) -> long long& {
    int req_index = current_frame_index()-4-local_value_max_size()-operand_stack_size()+index;
    // valid check
        if(req_index < thread_end_index() || mem_size <= req_index){
            std::cout << "Stack Memory Overflow" << std::endl;
            state() = -1;
            return memory[0];
        }
    return memory[req_index];
};
auto local_value                = [](int index) -> long long& {
    // valid check
     if(index < 0 || local_value_max_size() <= index){
            std::cout << "Local Address out of Range" << std::endl;
            state() = -2;
            return memory[0];
     }
    return memory[current_frame_index()-4-index];
};
auto heap_value                 = [](int index) -> long long& {
    if(index < 0 || thread_end_index() - memory[0] - 2 < index){
            std::cout << "Heap Address out of Range" << std::endl;
            state() = -3;
            return memory[0];
    }
    return memory[memory[0]+index];
};

void load_binary(const char* filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) return;

    file.read(reinterpret_cast<char*>(memory), mem_size * sizeof(long long));
}

int operate_command(long long byte);

int main(){
    load_binary("program.bin");

    operate_command(memory[1]);
    while(state() >= 1 && current_command_index() < 1024){
        current_command_index() ++;
        operate_command(current_word());
        if(state() == 2){
            // debug mode
            std::cout << "\n\n\ncurrent command: " << current_word() 
            << " (" << current_command_index() << ")" << std::endl;
            operate_command(0xF0);
            operate_command(0xF1);
            operate_command(0xF3);
            operate_command(0xF4);
        }
    }
    if(state() == 0){
        std::cout << "(^^)b iikanzi: 0" << std::endl;
        return 0;
    }
    std::cout << "orz era-: " << state() << std::endl;
    return 0;
}

double read_as_f64(const long long& data) {
    double res;
    std::memcpy(&res, &data, sizeof(double));
    return res;
}

int write_from_f64(long long& to, double from) {
    std::memcpy(&to, &from, sizeof(long long));
    return 0;
}



int operate_command(long long byte){
    switch(byte){
        case 0x00: // START
            std::cout<<"START"<<std::endl;
            state() = 1;
            thread_end_index() = mem_size - 512*1024;
            frame_count() = 1;
            current_frame_index() = mem_size-5;

            prev_frame_index() = 0;
            local_value_max_size() = 0;
            operand_stack_size() = 0;
            current_command_index() = 1;
            break;
        case 0x01: // STOP
            state() = 0;
            std::cout<<"\nSTOP"<<std::endl;
            break;
        case 0x02: // CONST
            current_command_index() ++;
            operand_value(0) = current_word();
            operand_stack_size() ++;
            break;
        case 0x10: // LPOP
            local_value(operand_value(1)) = operand_value(2);
            operand_stack_size() -= 2;
            break;
        case 0x11: // HPOP
            heap_value(operand_value(1)) = operand_value(2);
            operand_stack_size() -= 2;
            break;
        case 0x12: // LPUSH
            operand_value(1) = local_value(operand_value(1));
            break;
        case 0x13: // HPUSH
            operand_value(1) = heap_value(operand_value(1));
            break;
        case 0x20: // IADD
            operand_value(2) = operand_value(2) + operand_value(1);
            operand_stack_size() --;
            break;
        case 0x21: // ISUB
            operand_value(2) = operand_value(2) - operand_value(1);
            operand_stack_size() --;
            break;
        case 0x22: // IMUL
            operand_value(2) = operand_value(2) * operand_value(1);
            operand_stack_size() --;
            break;
        case 0x23: // IDIV
            operand_value(2) = operand_value(2) / operand_value(1);
            operand_stack_size() --;
            break;
        case 0x24: // IMOD
            operand_value(2) = operand_value(2) % operand_value(1);
            operand_stack_size() --;
            break;
        case 0x25: // IMORE
            operand_value(2) = operand_value(2) < operand_value(1);
            operand_stack_size() --;
            break;
        case 0x26: // ILESS
            operand_value(2) = operand_value(2) > operand_value(1);
            operand_stack_size() --;
            break;
        case 0x27: // IEQUAL
            operand_value(2) = operand_value(2) == operand_value(1);
            operand_stack_size() --;
            break;
        case 0x28: // ILSHIFT
            if(operand_value(1) < 0 || 63 < operand_value(1)){
                std::cout << "Invalid Value of Bit Shift" << std::endl;
                state() = -4;
                return 0;
            }
            operand_value(2) = static_cast<long long>(static_cast<unsigned long long>(operand_value(2)) << operand_value(1));
            operand_stack_size() --;
            break;
        case 0x29: // IRSHIFT
            if(operand_value(1) < 0 || 63 < operand_value(1)){
                std::cout << "Invalid Value of Bit Shift" << std::endl;
                state() = -4;
                return 0;
            }
            operand_value(2) = static_cast<long long>(static_cast<unsigned long long>(operand_value(2)) >> operand_value(1));
            operand_stack_size() --;
            break;
        case 0x30: // FADD
            write_from_f64(operand_value(2),
            read_as_f64(operand_value(2)) + read_as_f64(operand_value(1)));
            operand_stack_size() --;
            break;
        case 0x31: // FSUB
            write_from_f64(operand_value(2),
            read_as_f64(operand_value(2)) - read_as_f64(operand_value(1)));
            operand_stack_size() --;
            break;
        case 0x32: // FMUL
            write_from_f64(operand_value(2),
            read_as_f64(operand_value(2)) * read_as_f64(operand_value(1)));
            operand_stack_size() --;
            break;
        case 0x33: // FDIV
            write_from_f64(operand_value(2),
            read_as_f64(operand_value(2)) / read_as_f64(operand_value(1)));
            operand_stack_size() --;
            break;
        case 0x34: // FMORE
            operand_value(1) = 
            read_as_f64(operand_value(2)) > read_as_f64(operand_value(1));
            operand_stack_size() --;
            break;
        case 0x35: // FLESS
            operand_value(1) = 
            read_as_f64(operand_value(2)) < read_as_f64(operand_value(1));
            operand_stack_size() --;
            break;
        case 0x36: // FEQUAL
            operand_value(1) = 
            read_as_f64(operand_value(2)) == read_as_f64(operand_value(1));
            operand_stack_size() --;
            break;
        case 0x40: // AND
            operand_value(1) = 
            operand_value(2) == 1 && operand_value(1) == 1;
            operand_stack_size() --;
            break;
        case 0x41: // OR
            operand_value(1) = 
            operand_value(2) == 1 || operand_value(1) == 1;
            operand_stack_size() --;
            break;
        case 0x42: // NOT
            operand_value(1) = operand_value(1) != 1;
            break;
        case 0x50: // ITF
            write_from_f64(operand_value(1), static_cast<double>(operand_value(1)));
            break;
        case 0x51: // FTI
            operand_value(1) = static_cast<long long>(read_as_f64(operand_value(1)));
            break;
        case 0x60: // RET
            /*
                2026/01/22
                これを「準備された秩序の崩壊モデル」と呼ぶ。
            */
            current_command_index() = operand_value(1); // *current_command_index() == *(prev operand_value(0))
            current_frame_index() = prev_frame_index();
            break;
        case 0x61: // CALL
            operand_value(0)=current_frame_index();
            operand_stack_size() ++;
            break;
        case 0x62: // ENTER
            // exist check
            if(operand_value(1) + 5 > local_value_max_size() + operand_stack_size()){
                std::cout << "Invalid Frame Push" << std::endl;
                state() = -5;
                break;
            }
            // new frame
            operand_stack_size() -= operand_value(1)+4;
            current_frame_index() -= local_value_max_size() + operand_stack_size() + 3;
            /*
                // req
                command_index       at current_frame_index
                prev_frame_index
                local_value_size    =: ls
                operand_stack_size  =: os
                local_1             at current_frame_index - 4 ((check!)>= current_frame_index + 1 - ls - os + ac)
                ...
                local_ls            at current_frame_index - 3 - ls
                operand_1           at current_frame_index - 4 - ls
                ...
                operand_{os-ac-4}   at current_frame_index + 1 - ls - os + ac // new frame command_index
                operand_{os-ac-3}   at current_frame_index - 0 - ls - os + ac // new frame prev_frame_index
                operand_{os-ac-2}   at current_frame_index - 1 - ls - os + ac // new frame local_value_size
                operand_{os-ac-1}   at current_frame_index - 2 - ls - os + ac // new frame operand_stack_size
                operand_{os-ac}     at current_frame_index - 3 - ls - os + ac // arg_1
                ...
                operand_{os-ac-n+1} at current_frame_index - 2 - n - ls - os + ac // arg_n
                ...
                operand_{os-1}      at current_frame_index - 2 - ls - os // arg_ac
                operand_os   =: ac  at current_frame_index - 3 - ls - os
            */
            break;
        case 0x70: // JMP
            current_command_index() += operand_value(1);
            operand_stack_size() --;
            break;
        case 0x71: // JMPIF
            if(operand_value(1) == 1){
                current_command_index() += operand_value(2);
            }
            operand_stack_size() -= 2;
            break;
        case 0x80: // CCOUT
            std::cout << static_cast<char>(operand_value(1));
            std::cout.flush();
            operand_stack_size() --;
            break;
        case 0x81: // ICOUT
            std::cout << operand_value(1);
            std::cout.flush();
            operand_stack_size() --;
            break;
        case 0x82: // FCOUT
            std::cout << read_as_f64(operand_value(1));
            std::cout.flush();
            operand_stack_size() --;
            break;
        case 0x90: // DUP
            operand_value(0) = operand_value(1);
            operand_stack_size() ++;
            break;
        case 0x91: // POP
            operand_stack_size() --;
            break;
        case 0x92: // SWAP
            operand_value(0) = operand_value(1);
            operand_value(1) = operand_value(2);
            operand_value(2) = operand_value(0);
            break;
        case 0x93: // OVER
            operand_value(0) = operand_value(2);
            operand_stack_size() ++;
            break;
        case 0x94: // DDUP
            operand_value(0) = operand_value(2);
            operand_stack_size() ++;
            operand_value(0) = operand_value(2);
            operand_stack_size() ++;
            break;
        case 0x95: // ROT
            operand_value(0) = operand_value(3); // 3 2 1 3
            operand_value(3) = operand_value(2); // 2 2 1 3
            operand_value(2) = operand_value(1); // 2 1 1 3
            operand_value(1) = operand_value(0); // 2 1 3 3
            break;
        case 0xF0: // dTMETA debug: check thread meta
            std::cout << "[debug: thread]" << std::endl;
            std::cout << "\nstate                 :" << state() << std::endl;
            std::cout << "thread end index      :" << thread_end_index() << std::endl;
            std::cout << "frame count           :" << frame_count() << std::endl;
            std::cout << "current frame index   :" << current_frame_index() << std::endl;
            break;
        case 0xF1: // dFMETA debug: check frame meta
            std::cout << "[debug: frame]" << std::endl;
            std::cout << "\nprev frame index      :" << prev_frame_index() << std::endl;
            std::cout << "command index         :" << current_command_index() << std::endl;
            std::cout << "local value max size  :" << local_value_max_size() << std::endl;
            std::cout << "operand size          :" << operand_stack_size() << std::endl;
            break;
        case 0xF2: // dOPTOP debug: check opperand stack top
            std::cout << "[debug: operand top]" << std::endl;
            std::cout << "\ncurrent top of operand stack: " << operand_value(1) << std::endl;
            break;
        case 0xF3: // dLOCALS debug: check locals
            std::cout << "[debug: locals]" << std::endl;
            for(int i = 0; i < local_value_max_size(); i++){
                std::cout << i << "   " << local_value(i) << std::endl;
            }
            break;
        case 0xF4: // dOPERANDS debug: check operands
            std::cout << "[debug: operands]" << std::endl;
            for(int i = 0; i < operand_stack_size(); i++){
                std::cout << i << "   " << operand_value(i+1) << std::endl;
            }
            break;
        case 0xF5: // dSTART debug: debug mode start
            std::cout<<"debug mode: START"<<std::endl;
            state() = 2;
            thread_end_index() = mem_size - 512*1024;
            frame_count() = 1;
            current_frame_index() = mem_size-5;

            prev_frame_index() = 0;
            local_value_max_size() = 0;
            operand_stack_size() = 0;
            current_command_index() = 1;
            break;
        default:
            std::cout << "Undefined Command" << std::endl;
            break;
    }
    return 0;
}