#ifndef ALIENCPUASSEMBLER_H
#define ALIENCPUASSEMBLER_H

#include <string>
#include <map>

#include <../src/Motherboard/AlienCPU.h>
#include <../src/ConsoleColor.h>

class AlienCPUAssembler;


class AlienCPUAssembler {
    public:
        /**
         * Value labels expressions that have not be processed yet.
         */
        struct LabelExpressionPair {
            std::string label;
            std::string expression;

            LabelExpressionPair(std::string label, std::string expression) 
                    : label(label), expression(expression) {}
        };

        struct Token {
            std::string string;
            int location; // number of characters from the first character of the source code
            int lineNumber;

            Token(std::string string, int location, int lineNumber) : 
                    string(string), location(location), lineNumber(lineNumber) {}
        };

        enum ParsedTokenType {
            GLOBAL_LABEL,
            LOCAL_LABEL,

            INSTRUCTION,
            INSTRUCTION_OPERAND,

            DIRECTIVE,
            DIRECTIVE_OPERAND
        };

        struct ParsedToken {
            Token token;
            ParsedTokenType type;

            ParsedToken(Token token, ParsedTokenType type) : token(token), type(type) {}
        };

        struct ParsedMemoryToken : ParsedToken {
            int memoryAddress;

            ParsedMemoryToken(Token token, ParsedTokenType type, int memoryAddress) : 
                    ParsedToken(token, type), memoryAddress(memoryAddress) {}
        };

        struct ParsedInstructionToken : ParsedMemoryToken {
            AddressingMode addressingMode;

            ParsedInstructionToken(Token token, ParsedTokenType type, int memoryAddress, AddressingMode addressingMode) : 
                    ParsedMemoryToken(token, type, memoryAddress), addressingMode(addressingMode) {}
        };

        enum AssemblerError {
            ERROR,
            INVALID_TOKEN,
            MISSING_TOKEN,
        };

        enum AssemblerWarn {
            WARN,
        };

        enum AssemblerLog {
            LOG,
            TOKENIZING,
            PARSING,
            ASSEMBLING
        };


        AlienCPUAssembler(AlienCPU& cpu, bool debugOn = false);
        void assemble(std::string source);

        void betterAssemble(std::string source);
        void tokenize();
        void parseTokens();
        u64 parseValue(const Token token);
        AddressingMode getAddressingMode(Token tokenInstruction, Token token);
        void evaluateExpression(Token token);


        void error(AssemblerError error, Token currentToken, std::stringstream msg);
        void warn(AssemblerWarn warn, std::stringstream msg);
        void log(AssemblerLog log, std::stringstream msg);


        void OLDparseLine(std::string& line);
        bool OLDparseAssemblerDirective();

        bool OLDparseLabel(std::string& label);
        void OLDparseInstruction(std::string& instruction);

        void OLDassembleLine(std::string& line);
        void OLDassembleInstruction(std::string& instruction);

        u64 OLDparseValue(const std::string& value);


    private:
        /**
         * The AlienCPU to assemble the source code for and write the machine code to.
         */
        AlienCPU& cpu;
        bool debugOn;

        /**
         * The file name to output the assembled binary to. Default is 'A6502.bin'.
         */
        std::string outputFile;

        /**
         * The source code currently or most recently being assembled.
         */
        std::string sourceCode;


        /**
         * Tokenized source code
         */
        std::vector<Token> tokens;

        /**
         * Parsed tokens
         */
        std::vector<ParsedToken> parsedTokens;


        /**
         * Program counters for the data and text segments of the program
         */
        Word dataProgramCounter;
        Word textProgramCounter;







        /**
         * The memory address of the next program instruction to be assembled.
         */
        Word OLDlocationPointer;

        /**
         * The source code split into lines.
         */
        std::vector<std::string> OLDlines;


        /**
         * Index of the current line being processed in the source code.
         */
        int OLDcurrentLine;

        /**
         * The tokens of the current line being processed.
         * 
         * These tokens only include labels and code or pseduo instructions, no comments.
         */
        std::vector<std::string> OLDcurrentLineTokens;

        /**
         * The most recently processed global label.
         * 
         * This is used to determine the scope of local labels.
         */
        std::string OLDpreviousGlobalLabel;

        /**
         * Labels that reference code locations.
         * 
         * Global labels can be referenced from anywhere in the program's source code
         * Local labels can only be referenced locally in a subroutine, must be defined after 
         * a global label. The scope of the local label lasts until the next defined global label.
         */
        std::map<std::string, Word> OLDglobalCodeLabels;

        // Maps a local label name to a map of the global label parent to the memory address the label points to
        std::map<std::string, std::map<std::string, Word>> OLDlocalCodeLabels;


        /**
         * Labels that reference values expressions.
         * 
         * These are pseudo instructions that are defined to reference a simple value expression.
         * The value expression can be a simple number or a previously defined label that 
         * references a value. It can also be a simple arithmetic expression of numbers and
         * previously defined value labels. 
         * 
         * Note, the value labels can only reference other labels that have already been defined.
         */
        std::map<std::string, Word> OLDglobalValueLabels;
        std::map<std::string, Word> OLDlocalValueLabels;

        std::vector<LabelExpressionPair> OLDglobalUnprocessedValueLabels;
        std::vector<LabelExpressionPair> OLDlocalUnprocessedValueLabels;
};


/**
 * Splits a string into a vector of strings based on the given delimiter.
 * 
 * @param source The string to split.
 * @param delimiter The delimiter to split the string on.
 * @return A vector of strings split from the given string based on the given delimiter.
 */
static std::vector<std::string> split(std::string source, char delimiter) {
    std::vector<std::string> lines;
    std::string line;
    std::stringstream ss(source);

    while (std::getline(ss, line, delimiter)) {
        lines.push_back(line);
    }

    return lines;
}


/**
 * Trim any leading and trailing whitespace from the given string.
 * 
 * @param source The string to trim
 * @return The given string with any leading and trailing whitespace removed.
 */
static std::string trim(std::string& string) {
    std::string result = string;

    // remove leading whitespace
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](int ch) {
        return !std::isspace(ch);
    }));

    // remove trailing whitespace
    result.erase(std::find_if(result.rbegin(), result.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), result.end());

    return result;
}


/**
 * Converts a vector of strings into a single string.
 * 
 * Formats the string in the following way:
 * [string1, string2, string3, ...]
 * 
 * @param strings The vector of strings to convert.
 * @return A single string containing all of the strings in the given vector.
 */
static std::string tostring(std::vector<std::string>& strings) {
    std::string result = "[";

    for (std::string string : strings) {
        result += string + ", ";
    }

    // remove the last comma and space from string
    result.pop_back();
    result.pop_back();

    result += "]";

    return result;
}


static std::string tostring(std::vector<AlienCPUAssembler::Token>& tokens) {
    std::string result = "[";

    for (AlienCPUAssembler::Token token : tokens) {
        result += token.string + ", ";
    }

    // remove the last comma and space from string
    result.pop_back();
    result.pop_back();

    result += "]";

    return result;
}


/**
 * Check if a string contains only digits
 * 
 * @param str string to check
 * @return true if the string contains only digits
 */
static bool isNumber(const std::string& string) {
    std::string::const_iterator it = string.begin();
    while (it != string.end() && std::isdigit(*it)) {
        ++it;
    }
    return !string.empty() && it == string.end();
}


/**
 * Check if a string contains only hexadecimal digits
 * 
 * @param str string to check
 * @return true if the string contains only hexadecimal digits
 */
static bool isHexadecimalNumber(const std::string& string) {
    std::string::const_iterator it = string.begin();
    while (it != string.end() && std::isxdigit(*it)) {
        ++it;
    }
    return !string.empty() && it == string.end();
}


/**
 * Map of addressing modes to how many bytes are needed
 */
std::map<AddressingMode,u8> addressingModeOperandBytes = {
    {ACCUMULATOR, 0},
    {IMPLIED, 0},
    {IMMEDIATE, 2},
    {RELATIVE, 2},
    {INDIRECT, 4},
    {ABSOLUTE, 4},
    {ABSOLUTE_XINDEXED, 4},
    {ABSOLUTE_YINDEXED, 4},
    {ZEROPAGE, 2},
    {ZEROPAGE_XINDEXED, 2},
    {ZEROPAGE_YINDEXED, 2},
    {XINDEXED_INDIRECT, 2},
    {INDIRECT_YINDEXED, 2}
};


/**
 * Map of processor instructions syntactical names and addressing mode to their opcode value
 */
std::map<std::string, std::map<AddressingMode, u8>> instructionMap = {
    {"LDA", 
        {
            {IMMEDIATE, 0xA9}, 
            {ABSOLUTE, 0xAD}, 
            {ABSOLUTE_XINDEXED, 0xBD}, 
            {ABSOLUTE_YINDEXED, 0xB9},
            {ZEROPAGE, 0xA5},
            {ZEROPAGE_XINDEXED, 0xB5},
            {XINDEXED_INDIRECT, 0xA1},
            {INDIRECT_YINDEXED, 0xB1}
        }
    }, {"LDX",
        {
            {IMMEDIATE, 0xA2},
            {ABSOLUTE, 0xAE},
            {ABSOLUTE_YINDEXED, 0xBE},
            {ZEROPAGE, 0xA6},
            {ZEROPAGE_YINDEXED, 0xB6}
        }
    }, {"LDY", 
        {
            {IMMEDIATE, 0xA0},
            {ABSOLUTE, 0xAC},
            {ABSOLUTE_XINDEXED, 0xBC},
            {ZEROPAGE, 0xA4},
            {ZEROPAGE_XINDEXED, 0xB4}
        }
    }, {"STA",
        {
            {ABSOLUTE, 0x8D},
            {ABSOLUTE_XINDEXED, 0x9D},
            {ABSOLUTE_YINDEXED, 0x99},
            {ZEROPAGE, 0x85},
            {ZEROPAGE_XINDEXED, 0x95},
            {XINDEXED_INDIRECT, 0x81},
            {INDIRECT_YINDEXED, 0x91}
        }
    }, {"STX",
        {
            {ABSOLUTE, 0x8E},
            {ZEROPAGE, 0x86},
            {ZEROPAGE_YINDEXED, 0x96}
        }
    }, {"STY",
        {
            {ABSOLUTE, 0x8C},
            {ZEROPAGE, 0x84},
            {ZEROPAGE_XINDEXED, 0x94}
        }
    }, {"TAX",
        {
            {IMPLIED, 0xAA}
        }
    }, {"TAY",
        {
            {IMPLIED, 0xA8}
        }
    }, {"TSX",
        {
            {IMPLIED, 0xBA}
        }
    }, {"TXA",
        {
            {IMPLIED, 0x8A}
        }
    }, {"TXS",
        {
            {IMPLIED, 0x9A}
        }
    }, {"TYA",
        {
            {IMPLIED, 0x98}
        }
    }, {"PHA",
        {
            {IMPLIED, 0x48}
        }
    }, {"PHP",
        {
            {IMPLIED, 0x08}
        }
    }, {"PLA",
        {
            {IMPLIED, 0x68}
        }
    }, {"PLP",
        {
            {IMPLIED, 0x28}
        }
    }, {"DEC",
        {
            {ABSOLUTE, 0xCE},
            {ABSOLUTE_XINDEXED, 0xDE},
            {ZEROPAGE, 0xC6},
            {ZEROPAGE_XINDEXED, 0xD6}
        }
    }, {"DEX",
        {
            {IMPLIED, 0xCA}
        }
    }, {"DEY",
        {
            {IMPLIED, 0x88}
        }
    }, {"INC",
        {
            {ABSOLUTE, 0xEE},
            {ABSOLUTE_XINDEXED, 0xFE},
            {ZEROPAGE, 0xE6},
            {ZEROPAGE_XINDEXED, 0xF6}
        }
    }, {"INX",
        {
            {IMPLIED, 0xE8}
        }
    }, {"INY",
        {
            {IMPLIED, 0xC8}
        }
    }, {"ADC",
        {
            {IMMEDIATE, 0x69},
            {ABSOLUTE, 0x6D},
            {ABSOLUTE_XINDEXED, 0x7D},
            {ABSOLUTE_YINDEXED, 0x79},
            {ZEROPAGE, 0x65},
            {ZEROPAGE_XINDEXED, 0x75},
            {XINDEXED_INDIRECT, 0x61},
            {INDIRECT_YINDEXED, 0x71}
        }
    }, {"SBC",
        {
            {IMMEDIATE, 0xE9},
            {ABSOLUTE, 0xED},
            {ABSOLUTE_XINDEXED, 0xFD},
            {ABSOLUTE_YINDEXED, 0xF9},
            {ZEROPAGE, 0xE5},
            {ZEROPAGE_XINDEXED, 0xF5},
            {XINDEXED_INDIRECT, 0xE1},
            {INDIRECT_YINDEXED, 0xF1}
        }
    }, {"AND",
        {
            {IMMEDIATE, 0x29},
            {ABSOLUTE, 0x2D},
            {ABSOLUTE_XINDEXED, 0x3D},
            {ABSOLUTE_YINDEXED, 0x39},
            {ZEROPAGE, 0x25},
            {ZEROPAGE_XINDEXED, 0x35},
            {XINDEXED_INDIRECT, 0x21},
            {INDIRECT_YINDEXED, 0x31}
        }
    }, {"EOR",
        {
            {IMMEDIATE, 0x49},
            {ABSOLUTE, 0x4D},
            {ABSOLUTE_XINDEXED, 0x5D},
            {ABSOLUTE_YINDEXED, 0x59},
            {ZEROPAGE, 0x45},
            {ZEROPAGE_XINDEXED, 0x55},
            {XINDEXED_INDIRECT, 0x41},
            {INDIRECT_YINDEXED, 0x51}
        }
    }, {"ORA",
        {
            {IMMEDIATE, 0x09},
            {ABSOLUTE, 0x0D},
            {ABSOLUTE_XINDEXED, 0x1D},
            {ABSOLUTE_YINDEXED, 0x19},
            {ZEROPAGE, 0x05},
            {ZEROPAGE_XINDEXED, 0x15},
            {XINDEXED_INDIRECT, 0x01},
            {INDIRECT_YINDEXED, 0x11}
        }
    }, {"ASL",
        {
            {ACCUMULATOR, 0x0A},
            {ABSOLUTE, 0x0E},
            {ABSOLUTE_XINDEXED, 0x1E},
            {ZEROPAGE, 0x06},
            {ZEROPAGE_XINDEXED, 0x16}
        }
    }, {"LSR",
        {
            {ACCUMULATOR, 0x4A},
            {ABSOLUTE, 0x4E},
            {ABSOLUTE_XINDEXED, 0x5E},
            {ZEROPAGE, 0x46},
            {ZEROPAGE_XINDEXED, 0x56}
        }
    }, {"ROL",
        {
            {ACCUMULATOR, 0x2A},
            {ABSOLUTE, 0x2E},
            {ABSOLUTE_XINDEXED, 0x3E},
            {ZEROPAGE, 0x26},
            {ZEROPAGE_XINDEXED, 0x36}
        }
    }, {"ROR",
        {
            {ACCUMULATOR, 0x6A},
            {ABSOLUTE, 0x6E},
            {ABSOLUTE_XINDEXED, 0x7E},
            {ZEROPAGE, 0x66},
            {ZEROPAGE_XINDEXED, 0x76}
        }
    }, {"CLC",
        {
            {IMPLIED, 0x18}
        }
    }, {"CLD", 
        {
            {IMPLIED, 0xD8}
        }
    }, {"CLI",
        {
            {IMPLIED, 0x58}
        }
    }, {"CLV",
        {
            {IMPLIED, 0xB8}
        }
    }, {"SEC",
        {
            {IMPLIED, 0x38}
        }
    }, {"SED",
        {
            {IMPLIED, 0xF8}
        }
    }, {"SEI",
        {
            {IMPLIED, 0x78}
        }
    }, {"CMP",
        {
            {IMMEDIATE, 0xC9},
            {ABSOLUTE, 0xCD},
            {ABSOLUTE_XINDEXED, 0xDD},
            {ABSOLUTE_YINDEXED, 0xD9},
            {ZEROPAGE, 0xC5},
            {ZEROPAGE_XINDEXED, 0xD5},
            {XINDEXED_INDIRECT, 0xC1},
            {INDIRECT_YINDEXED, 0xD1}
        }
    }, {"CPX",
        {
            {IMMEDIATE, 0xE0},
            {ABSOLUTE, 0xEC},
            {ZEROPAGE, 0xE4}
        }
    }, {"CPY",
        {
            {IMMEDIATE, 0xC0},
            {ABSOLUTE, 0xCC},
            {ZEROPAGE, 0xC4}
        }
    }, {"BCC",
        {
            {RELATIVE, 0x90}
        }
    }, {"BCS",
        {
            {RELATIVE, 0xB0}
        }
    }, {"BEQ",
        {
            {RELATIVE, 0xF0}
        }
    }, {"BPL",
        {
            {RELATIVE, 0x10}
        }
    }, {"BMI",
        {
            {RELATIVE, 0x30}
        }
    }, {"BNE",
        {
            {RELATIVE, 0xD0}
        }
    }, {"BVC", 
        {
            {RELATIVE, 0x50}
        }
    }, {"BVS",
        {
            {RELATIVE, 0x70}
        }
    }, {"JMP",
        {
            {ABSOLUTE, 0x4C},
            {INDIRECT, 0x6C}
        }
    }, {"JSR",
        {
            {ABSOLUTE, 0x20}
        }
    }, {"RTS",
        {
            {IMPLIED, 0x60}
        }
    }, {"BRK",
        {
            {IMPLIED, 0x00}
        }
    }, {"RTI",
        {
            {IMPLIED, 0x40}
        }
    }, {"BIT",
        {
            {ABSOLUTE, 0x2C},
            {ZEROPAGE, 0x24}
        }
    }
};

/**
 * Determines if the given processor instruction is valid for the given addressing mode.
 * 
 * @param instruction The processor instruction to check.
 * @param addressingMode The addressing mode to check.
 * @return True if the processor instruction is valid for the given addressing mode, false otherwise.
 */
static bool validInstruction(std::string& instruction, AddressingMode addressingMode) {
    if (instructionMap.count(instruction) == 0) {
        return false;
    }
    
    return instructionMap.at(instruction).count(addressingMode) > 0;
}


/**
 * Gets the opcode for the given processor instruction and addressing mode.
 * 
 * @param instruction The processor instruction to get the opcode for.
 * @param addressingMode The addressing mode to get the opcode for.
 * 
 * @return The opcode for the given processor instruction and addressing mode.
 */
static u8 getProcessorInstructionOpcode(std::string& instruction, AddressingMode addressingMode) {
    return instructionMap.at(instruction).at(addressingMode);
}

#endif // ALIENCPUASSEMBLER_H