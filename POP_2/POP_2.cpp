// POP_2.cpp
//  [12/31/2016 Janek]
//
//	Simple VM
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h" //Header files

#define ADD 0 //Add
#define SUB 1 //Substract
#define MUL 2 //Multiply
#define DIV 3 //Divide
#define COM 4 //Compare
#define CPY 5 //Copy
#define JMP 6 //Jump
#define REA 7 //Read
#define KIN 8 //Keyboard INput
#define SOU 9 //Screen Output
#define END 10 //End

struct Instruction //This struct help us when for example we need to convert line to instruction
{
	uint16_t r2;
	uint16_t r1;
	uint16_t Op;
};

struct Memory
{
	int reg[64];
	int ins_counter;
	int flag_register;
	int const_counter;
	int jump_counter;
	std::vector<int> constant_mem; //Namespace is after
	std::vector<int> jump_mem; //Namespace is after
};



using namespace std;// No more std:: witch cout etc.
//Declaration
void mainMenuDisplay(void);
Instruction lineToInstruction(uint16_t line);
bool openFileIn(string filename, ifstream & file);
bool openFileOut(string filename, ofstream & file);
void prepareFilePath(string & path);
void fileToVec(vector <Instruction> &instructions, ifstream &file, Memory &memory);
void compile(Memory & memory, vector<Instruction>& instructions);
void clrMem(Memory &memory);
int main()
{
	
	
	//////////////////////////////////////////////////////////////////////////
	//Init main var
	//////////////////////////////////////////////////////////////////////////
	Memory memory;
	clrMem(memory); //Clear on init
	int menu = 0;
	
	while (1) //Infinite loop for main program
	{
		menu = 0;
		mainMenuDisplay();
		cin >> menu;
		if (cin.fail())
		{
			cout << "podales zly znak sproboj ponownie" << endl;
			system("pause");
			cin.clear(); //It's needed to cleanup cin stream before continue otherwise its starts to infinite loop
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			continue;
		}

		//////////////////////////////////////////////////////////////////////////
		//Menu
		//////////////////////////////////////////////////////////////////////////

		switch (menu)
		{

		case 1: //VM
		{
			clrMem(memory); //Clear on init

			//Get file path
			cout << "Przeciagnij plik .bin na okienko programu, nastêpnie nacisnij enter\n";
			string path;
			cin.clear(); //It's needed to cleanup cin stream before continue otherwise its starts to infinite loop
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			getline(cin,path);
			prepareFilePath(path);
			//Read input file
			vector <Instruction> instructions; //Storage for converted instructions read from input file
			ifstream input_file;
			if (!openFileIn(path, input_file)) //Open file and check error. If error go to hell
			{
				cout << "Nie udalo sie otworzyc pliku z lokalizacji\n" << path;
				system("pause");
				break;
			}
			fileToVec(instructions, input_file, memory);
			
			//Make program
			compile(memory, instructions);

			//End
			clrMem(memory);
			break; 
		}

		case 2: //Editor
		{
			break;
		}

		case 3://Debugger, work step by step
		{
			break;
		}

		case 0:
		{
			return 0;
		}

		default:
		{
			cout << "Podales zly numer instrukcji";
			system("pause");
			break;
		}


		}
	}
    return 0; //Return on end of application
}

void mainMenuDisplay(void)
{
	system("cls");
	cout << "Wybierz odpowiednia opcje wpisuj¹c jej numer oraz wcisnij enter<<"<<endl;
	cout << "1. Maszyna wirtualna" << endl;
	cout << "2. Edytor kodu" << endl;
	cout << "3. Praca krokowa" << endl;
	cout << "0. Wyjscie z programu" << endl;
}

Instruction lineToInstruction(uint16_t line) //Littlendian?!?!? THINK NEEDED ALSO
{
	Instruction temp;
	temp.Op = line & 0b1111;
	temp.r1 = (line & 0b1111110000) >> 4;
	temp.r2 = (line & 0b1111110000000000) >> 10;
	return temp;
}

bool openFileIn(string filename, ifstream &file)
{
	file.open(filename.c_str(), ios::binary);
	return file.good();
}

bool openFileOut(string filename, ofstream &file)//UT
{
	file.open(filename.c_str(), ios::binary);
	return file.good();
}

uint16_t readNextValue(ifstream &file)//UT
{
	uint16_t temp;
	file.read((char *)& temp, sizeof temp);
	return temp;
}

int readIntConst(ifstream &file)
{
	int temp;
	file.read((char *)& temp, sizeof temp);
	return temp;
}

void removeCharFromString(string &str, char charToRemove)
{
	str.erase(remove(str.begin(), str.end(), charToRemove), str.end());
}

void prepareFilePath(string &path)
{
	removeCharFromString(path, '"');
}

void lineToVec(vector <Instruction> &instructions, ifstream &file)
{
	instructions.push_back(lineToInstruction(readNextValue(file)));
}

void fileToVec(vector <Instruction> &instructions, ifstream &file, Memory &memory)
{
	while (!file.eof())
	{
		lineToVec(instructions, file);
		if (instructions[instructions.size() - 1].Op == REA)//If last read operation is REA then read 4 bytes to cons buff
		{
			//readIntConst(file);//Reading const from file
			memory.constant_mem.push_back(readIntConst(file));
		}
		if (instructions[instructions.size() - 1].Op == JMP)//Like upper
		{
			//readIntConst(file);//Reading const from file
			memory.jump_mem.push_back(readIntConst(file));
		}
	}
}

void compile(Memory &memory, vector <Instruction> &instructions)//adding a flag for seq work?line after enter
{
	system("cls");//Clean screen for better effect
	while (!(instructions[memory.ins_counter].Op == END))//While instruction isn't end do the program
	{
		switch (instructions[memory.ins_counter].Op)
		{
		case ADD:
		{
			memory.reg[instructions[memory.ins_counter].r1] = memory.reg[instructions[memory.ins_counter].r1] + memory.reg[instructions[memory.ins_counter].r2];
			break;
		}
		case SUB:
		{
			memory.reg[instructions[memory.ins_counter].r1] = memory.reg[instructions[memory.ins_counter].r1] - memory.reg[instructions[memory.ins_counter].r2];
			break;
		}
		case MUL:
		{
			memory.reg[instructions[memory.ins_counter].r1] = memory.reg[instructions[memory.ins_counter].r1] * memory.reg[instructions[memory.ins_counter].r2];
			break;
		}
		case DIV:
		{
			memory.reg[instructions[memory.ins_counter].r1] = memory.reg[instructions[memory.ins_counter].r1] / memory.reg[instructions[memory.ins_counter].r2];
			memory.reg[instructions[memory.ins_counter].r2] = memory.reg[instructions[memory.ins_counter].r1] * memory.reg[instructions[memory.ins_counter].r2];
			break;
		}
		case COM:
		{
			int com = 0;
			com = memory.reg[instructions[memory.ins_counter].r1] - memory.reg[instructions[memory.ins_counter].r2];
			if (com == 0)
			{
				memory.flag_register = 1; //Z
			}
			else
			{
				 if (com > 0)
				{
					memory.flag_register = 2; //D
				}
				 else
				{
					memory.flag_register = 4; //U
				}
				
				
			}
			break;
			
		}
		case CPY:
		{
			memory.reg[instructions[memory.ins_counter].r1] = memory.reg[instructions[memory.ins_counter].r2];
			break;
		}
		case JMP:
		{

		}
		case REA:
		{

		}
		case KIN:
		{
			int temp;
			cout << "podaj liczbê ";
			cin >> temp;
			//TODO: Error service
			memory.reg[instructions[memory.ins_counter].r1] = temp;
			break;
		}
		case SOU:
		{
			cout << endl << memory.reg[instructions[memory.ins_counter].r1];
			break;
		}
		case END:
		{
			cout >> "Program siê spierdoli³ nigdy nie powinien tu wejœæ";
		}
		default:
		{
			cout << "Uzyto nieobslugiwanej instrukcji";
		}

		}
		memory.ins_counter++; //Next line
	} 

}

void clrMem(Memory &memory)
{
	memory.flag_register = 0;
	memory.ins_counter = 0;
	memory.jump_counter = 0;
	memory.const_counter = 0;
	for (int i = 0; i < 64; i++) memory.reg[i] = 0; //Set all registers to 0
	//TODO: Clear 2 vectors 
}

