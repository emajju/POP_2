// POP_2.cpp
//  [12/31/2016 Janek]
//  WARNING Number of jumps and constants is limited to 64 (6bits)!!!!!!!!!!!!!!!!!
//	Simple VM
// TODO: Close files!
// VERY IMPORTANT TODO CHECKING DIGITS IN EDITOR(SENSITIVE FOR LETTER IN PLACE OF DIGIT)
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h" //Header files

#define ADD 0 //Add
#define SUB 1 //Subtraction
#define MUL 2 //Multiply
#define DIV 3 //Divide
#define COM 4 //Compare
#define CPY 5 //Copy
#define JMP 6 //Jump
#define REA 7 //Read
#define KIN 8 //Keyboard INput
#define SOU 9 //Screen Output
#define END 10 //End

#define PAUSE system("pause")
#define CLS system("cls")

struct Instruction //This struct help us when for example we need to convert line to instruction
{
	uint16_t r2;
	uint16_t r1;
	uint16_t Op;
};

struct Memory
{
	int reg[64];
	int insCounter;
	int flagRegister;
	int constCounter;
	int jumpCounter;
	std::vector<int> constMem; //Namespace is after
	std::vector<int> jumpMem; //Namespace is after
};



using namespace std;// No more std:: with cout etc.

//Declaration
void mainMenuDisplay(void);
Instruction lineToInstruction(uint16_t line);
bool openFileIn(string filename, ifstream & file);
bool openFileOut(string filename, ofstream & file);
void prepareFilePath(string & path);
void fileToVec(vector <Instruction> &instructions, ifstream &file, Memory &memory);
int compile(Memory & memory, vector<Instruction>& instructions, bool debug);
void clrMem(Memory &memory);

void setFlags(Memory & memory, uint16_t value);

void printMemDebug(Memory & memory);

void printFileEditor(vector<Instruction>& instruction, Memory memory);

int interpretCommand(string command, vector<Instruction>& instructions, Memory memory);

void saveVectorToFile(string fileName, vector<Instruction> instructions, Memory memory);
//////////////////////////////////////////////////////////////////////////
int main()
{
	
	//////////////////////////////////////////////////////////////////////////
	//Init main vars
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
			PAUSE;
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
				PAUSE;
				break;
			}
			fileToVec(instructions, input_file, memory);
			if (instructions.size() == 0) break;
			//Make program
			compile(memory, instructions, false);

			//End
			clrMem(memory);
			CLS;
			break; 
		}

		case 2: //Editor, 2 conditions new file or edit existing file 
		{
			CLS;
			int menu2;
			cout << "1. Edycja" << endl;
			cout << "2. Utwórz nowy plik" << endl;
			cout << "0. Wróæ do poprzedniego menu" << endl;
			cin >> menu2;
			switch (menu2)
			{
			case 1:
			{

				//Get file path
				cout << "Przeciagnij plik .bin na okienko programu, nastêpnie nacisnij enter\n";
				string path;
				cin.clear(); //It's needed to cleanup cin stream before continue otherwise its starts to infinite loop
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				getline(cin, path);
				prepareFilePath(path);
				vector <Instruction> editedFile;
				ifstream input_file;
				if (!openFileIn(path, input_file)) //Open file and check error. If error go to hell
				{
					cout << "Nie udalo sie otworzyc pliku z lokalizacji\n" << path;
					PAUSE;
					break;
				}
				clrMem(memory);
				string command;
				fileToVec(editedFile, input_file, memory);
				input_file.close();
				int status = 0;
				while (1)
				{
					CLS;
					printFileEditor(editedFile, memory);
					cout << "Podaj komende: \n";
					cin.clear();
					cin.sync();
					getline(cin, command);
					//cin >> command;
					status = interpretCommand(command, editedFile, memory);
					if (status == 2) break;
					if (status == 3)
					{
						//TODO save
						saveVectorToFile(path, editedFile, memory);
						break;
					}
					if (status == 0)
					{
						cout << "Blad w podanej komendzie, sprawdz dostepne komendy wpisujac 'help'" << endl;
						PAUSE;
					}
				}
				PAUSE;
				break;
			}
			case 2:
			{
				cout << "Plik zostanie utworzony w lokalizacji programu z rozszerzeniem .bin\n";
				cout << "Podaj nazwê pliku do utworzenia\n";
				string name;
				cin >> name;
				vector <Instruction> createdFile;
				

				break;
			}
			case 0:
				break;
			}
			break;
		}

		case 3://Debugger, work step by step
		{
			clrMem(memory); //Clear on init
			cout << "Tryb pracy krokowej\n";
			//Get file path
			cout << "Przeciagnij plik .bin na okienko programu, nastêpnie nacisnij enter\n";
			string path;
			cin.clear(); //It's needed to cleanup cin stream before continue otherwise its starts to infinite loop
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			getline(cin, path);
			prepareFilePath(path);
			//Read input file
			vector <Instruction> instructions; //Storage for converted instructions read from input file
			ifstream input_file;
			if (!openFileIn(path, input_file)) //Open file and check error. If error go to hell
			{
				cout << "Nie udalo sie otworzyc pliku z lokalizacji\n" << path;
				PAUSE;
				break;
			}
			fileToVec(instructions, input_file, memory);
			if (instructions.size() == 0) break;
			//Make program
			compile(memory, instructions,true);

			//End
			clrMem(memory);
			CLS;
			break;
		}

		case 0:
		{
			return 0;
		}

		default:
		{
			cout << "Podales zly numer instrukcji";
			PAUSE;
			break;
		}


		}
	}
    return 0; //Return on end of application
}
//////////////////////////////////////////////////////////////////////////
//Definition
void mainMenuDisplay(void)
{
	CLS;
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

uint16_t readNextValue(ifstream &file)
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
	file.seekg(0, file.end);
	int length = (int)file.tellg();
	file.seekg(0, file.beg);
	//This construction was needed because EOF works wrong
	while (length)
	{
		lineToVec(instructions, file);
		if (instructions[instructions.size() - 1].Op == REA)//If last read operation is REA then read 4 bytes to cons buff
		{
			//readIntConst(file);//Reading const from file
			memory.constMem.push_back(readIntConst(file));
			instructions[instructions.size() - 1].r2 = memory.constCounter;
			memory.constCounter++;
			length -= 4;//int
		}
		if (instructions[instructions.size() - 1].Op == JMP)//Like upper
		{
			//readIntConst(file);//Reading const from file
			memory.jumpMem.push_back(readIntConst(file));
			instructions[instructions.size() - 1].r2 = memory.jumpCounter;
			memory.jumpCounter++;
			length -= 4;//int
		}
		length -= 2;//uint16
		if(length<0)
		{
			cout << "Wczytywany plik jest uszkodzony";
			PAUSE;
			length = 0;
			clrMem(memory);
			vector<Instruction>().swap(instructions);
		}
	}
	file.close();//After use 
}

int compile(Memory &memory, vector <Instruction> &instructions, bool debug)//adding a flag for seq work?line after enter
{
	CLS;//Clean screen for better effect
	while (!(instructions[memory.insCounter].Op == END))//While instruction isn't end do the program
	{
		switch (instructions[memory.insCounter].Op)
		{
		case ADD:
		{
			memory.reg[instructions[memory.insCounter].r1] = memory.reg[instructions[memory.insCounter].r1] + memory.reg[instructions[memory.insCounter].r2];
			setFlags(memory, memory.reg[instructions[memory.insCounter].r1]);
			break;
		}
		case SUB:
		{
			memory.reg[instructions[memory.insCounter].r1] = memory.reg[instructions[memory.insCounter].r1] - memory.reg[instructions[memory.insCounter].r2];
			setFlags(memory, memory.reg[instructions[memory.insCounter].r1]);
			break;
		}
		case MUL:
		{
			memory.reg[instructions[memory.insCounter].r1] = memory.reg[instructions[memory.insCounter].r1] * memory.reg[instructions[memory.insCounter].r2];
			setFlags(memory, memory.reg[instructions[memory.insCounter].r1]);
			break;
		}
		case DIV:
		{
			if (memory.reg[instructions[memory.insCounter].r2] == 0)
			{
				cout << "Program probowal podzielic przez 0\n";
				return 1;
			}
			uint16_t r1 = memory.reg[instructions[memory.insCounter].r1];
			memory.reg[instructions[memory.insCounter].r1] = r1 / memory.reg[instructions[memory.insCounter].r2];
			memory.reg[instructions[memory.insCounter].r2] = r1 % memory.reg[instructions[memory.insCounter].r2];
			setFlags(memory, memory.reg[instructions[memory.insCounter].r1]);
			break;
		}
		case COM:
		{
			int com = 0;
			com = memory.reg[instructions[memory.insCounter].r1] - memory.reg[instructions[memory.insCounter].r2];
			setFlags(memory, memory.reg[instructions[memory.insCounter].r1]);
			
			break;
			
		}
		case CPY:
		{
			memory.reg[instructions[memory.insCounter].r1] = memory.reg[instructions[memory.insCounter].r2];
			break;
		}
		case JMP:
		{
			int conditon = instructions[memory.insCounter].r1;
			int steps = memory.jumpMem[instructions[memory.insCounter].r2];
			if (memory.insCounter + steps < 0)
			{
				cout << "Blad operacja skoku probowala odwolac sie do lini o numerze mniejszym od zera";
				return 1;
			}
			switch (conditon)
			{
			case 0: //always
			{
				memory.insCounter += (steps - 1); // -1 is needed because on end of loop insCounter is getting one bigger
				break;
			}
			case 1://Z is set
			{
				if (memory.flagRegister==1)
				{
					memory.insCounter += (steps - 1); // -1 is needed because on end of loop insCounter is getting one bigger
				}
				break;
			}
			case 2:
			{
				if (memory.flagRegister!=1)
				{
					memory.insCounter += (steps - 1); // -1 is needed because on end of loop insCounter is getting one bigger
				}
				break;
			}
			case 3:
			{
				if (memory.flagRegister == 2)
				{
					memory.insCounter += (steps - 1); // -1 is needed because on end of loop insCounter is getting one bigger
				}
				break;
			}
			case 4:
			{
				if (memory.flagRegister == 4)
				{
					memory.insCounter += (steps - 1); // -1 is needed because on end of loop insCounter is getting one bigger
				}
				break;
			}
			case 5:
			{
				if ((memory.flagRegister == 4)||(memory.flagRegister == 1))
				{
					memory.insCounter += (steps - 1); // -1 is needed because on end of loop insCounter is getting one bigger
				}
				break;
			}
			case 6:
			{
				if ((memory.flagRegister == 2) || (memory.flagRegister == 1))
				{
					memory.insCounter += (steps - 1); // -1 is needed because on end of loop insCounter is getting one bigger
				}
				break;
			}
			default:
			{
				cout << "B³¹d wykonywania programu w lini " << memory.insCounter <<"Nieobslugiwany warunek skoku";
				return 1;
			}
			}
			break;
		}
		case REA:
		{
			memory.reg[instructions[memory.insCounter].r1] = memory.constMem[instructions[memory.insCounter].r2];
			break;
		}
		case KIN:
		{
			int temp;
			cout << "podaj liczbe ";
			cin >> temp;
			while (cin.fail()) //When isn't number go in to loop, exit on only digits input
			{
				cout << "Podano znaki nie bedace liczbami. Sproboj ponownie" << endl;
				
				cin.clear(); //It's needed to cleanup cin stream before continue otherwise its starts to infinite loop
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cin >> temp; //try next input
			}
			memory.reg[instructions[memory.insCounter].r1] = temp;
			break;
		}
		case SOU:
		{
			cout << endl << memory.reg[instructions[memory.insCounter].r1];
			break;
		}
		case END:
		{
			cout << "Program siê spierdoli³ nigdy nie powinien tu wejœæ";
			break;
		}
		default:
		{
			cout << "Uzyto nieobslugiwanej instrukcji";
			return 1;//Goes out from loop
		}

		}
		memory.insCounter++; //Next line
		if (debug) printMemDebug(memory);
	} 
	cout << "Koniec programu" << endl;
	PAUSE;
	return 0;//Everything goes right :)

}

void clrMem(Memory &memory)
{
	memory.flagRegister = 0;
	memory.insCounter = 0;
	memory.jumpCounter = 0;
	memory.constCounter = 0;
	for (int i = 0; i < 64; i++) memory.reg[i] = 0; //Set all registers to 0
	vector<int>().swap(memory.constMem); //This construction deletes and frees all memory taken by vector, swap vector with empty vector;
	vector<int>().swap(memory.jumpMem);
}

void setFlags(Memory &memory, uint16_t value)
{
	if (value == 0)
	{
		memory.flagRegister = 1; //Z
	}
	else
	{
		if (value > 0)
		{
			memory.flagRegister = 2; //D
		}
		else
		{
			memory.flagRegister = 4; //U
		}
	}
}

void printMemDebug(Memory &memory)
{
	cout << endl;
	cout << "Linia: " << memory.insCounter << endl;
	cout << "Flaga: ";
	switch (memory.flagRegister)
	{

	case 1:
	{
		cout << "Z" << endl;
		break;
	}
	case 2:
	{
		cout << "D" << endl;
		break;
	}
	case 4:
	{
		cout << "U" << endl;
		break;
	}

	}
	cout << "Uzywane rejestry: ";
	for (int i = 0; i < 64; i++)
	{
		if (memory.reg[i] != 0)
		{
			cout << "R" << i << " = " << memory.reg[i];
			if (i % 5 == 0) cout << endl;
		}
	}
	cout << endl;
	PAUSE;
}

string translateInstruction(uint16_t instruction)
{
	switch (instruction)
	{
	case ADD: return "ADD";
	case SUB: return "SUB";
	case MUL: return "MUL";
	case DIV: return "DIV";
	case COM: return "COM";
	case CPY: return "CPY";
	case JMP: return "JMP";
	case REA: return "REA";
	case KIN: return "KIN";
	case SOU: return "SOU";
	case END: return "END";
	default: return "ERR";
	}
}

void printFileEditor(vector <Instruction> &instruction, Memory memory)
{
	cout << setw(3) << "Nr." << "|" << setw(4) << "Inst" << "|"<<setw(5)<<"r1"<<"|"<<setw(5)<<"r2"<<"|"<<endl;
	for (unsigned int i = 0; i < instruction.size(); i++) 
	{
		cout << setw(3) << i << "|";
		cout << setw(4) << translateInstruction(instruction[i].Op) << "|";
		cout << setw(5) << instruction[i].r1 << "|";
		if (!(instruction[i].Op==JMP || instruction[i].Op==REA))//Delete values added by this program
		{
			cout << setw(5) << instruction[i].r2 << "|" << endl;
		}
		else
		{
			cout << setw(5) << "0" << "|" << endl;
		}
	}
}

int countSpaces(string command)
{
	int counter = 0;
	for (unsigned int i = 0; i < command.size(); i++)
	{
		if (command[i] == ' ')
		{
			counter++;
		}
	}
	return counter;
}

uint16_t translateStringToInstruction(string instruction)
{
	if (instruction == "ADD") return ADD;
	else if (instruction == "SUB")return SUB;
	else if (instruction == "MUL")return MUL;
	else if (instruction == "DIV") return DIV;
	else if (instruction == "COM") return COM;
	else if (instruction == "CPY")return CPY;
	else if (instruction == "JMP")return JMP;
	else if (instruction == "REA")return REA;
	else if (instruction == "KIN")return KIN;
	else if (instruction == "SOU")return SOU;
	else if (instruction == "END")return END;
	else return 11;
}

void displayHelp(void)
{
	CLS;
	cout << endl;
	cout << "Skladnia polecen edytora: " << endl;
	cout << "1. Wywolanie pomocy: 'help', 'h', '?'" << endl;
	cout << "2. Dodanie nowej lini na koncu pliku: 'Instrukcja R1 R2'" << endl;
	cout << "3. Dodanie nowej lini wewnatrz pliku: 'Nr lini przed ktora wstawic instrukcja R1 R2'" << endl;
	cout << "4. Usuniecie lini: 'del nr lini'" << endl;
	cout << "5. Zapis i wyjscie: 'save'" << endl;
	cout << "6. Wyjscie: 'exit'" << endl;
	PAUSE;
}

int interpretCommand(string command, vector<Instruction> &instructions, Memory memory)
{
	if (command=="h"||command=="?"||command=="help")
	{
		displayHelp();
		return 1;
	}
	

	//Also add save
	if (command == "save") return 3;
	//And exit without save
	if (command == "exit") return 2;
	//////////////////////////////////////////////////////////////////////////
	// Command format:
	// Instruction r1 r2 if it will be a new line
	// line number instruction r1 r2 insert before line number
	// Other goes to error
	// TODO DELETE LINE NUMBER X
	//////////////////////////////////////////////////////////////////////////
	int spaces = countSpaces(command);
	
	if (spaces < 2)//delete
	{
		string comm, line;
		int field = 0;

		for (unsigned int i = 0; i < command.size(); i++)
		{
			switch (field)
			{
			case 0:
			{
				if (command[i] == ' ')
				{
					field++;
					continue;
				}
				comm += command[i];
				break;
			}
			case 1:
			{
				line += command[i];
				break;
			}
			}
		}
		if (comm != "del") return 0;
		if (atoi(line.c_str()) < 0 || atoi(line.c_str()) > instructions.size()-1) return 0;//ERR
		instructions.erase(instructions.begin()+atoi(line.c_str()));
		return 1;

	}
	if (spaces>3)
	{
		
		return 0;//Err
	}
	//String
	Instruction temp;
	string line, instruction, r1, r2;
	if (spaces == 2)//new line
	{
		int field = 0;

		for (int i=0;i<command.size();i++)
		{
			switch (field)
			{
			case 0:
			{
				if (command[i]==' ')
				{
					field++;
					continue;
				}
				instruction += command[i];
				break;
			}
			case 1:
			{
				if (command[i] == ' ')
				{
					field++;
					continue;
				}
				r1 += command[i];
				break;
			}
			case 2:
			{
				
				r2 += command[i];
				break;
			}
			
			}
		}
		
		temp.r1 = atoi(r1.c_str());
		temp.r2 = atoi(r2.c_str());
		temp.Op = translateStringToInstruction(instruction);
		if (temp.Op > 10) return 0;//error
		instructions.push_back(temp);
		return 1;//OK
	}
	else//with line number
	{
		int field = 0;

		for (unsigned int i = 0; i < command.size(); i++)
		{
			switch (field)
			{
			case 0:
			{
				if (command[i] == ' ')
				{
					field++;
					continue;
				}
				line += command[i];
				break;
			}
			case 1:
			{
				if (command[i] == ' ')
				{
					field++;
					continue;
				}
				instruction += command[i];
				break;
			}
			case 2:
			{
				if (command[i] == ' ')
				{
					field++;
					continue;
				}
				r1 += command[i];
				break;
			}
			case 3:
			{

				r2 += command[i];
				break;
			}

			}
		}
		temp.r1 = atoi(r1.c_str());
		temp.r2 = atoi(r2.c_str());
		temp.Op = translateStringToInstruction(instruction);
		if (temp.Op > 10) return 0;//error
		//Adding inside
		if (atoi(line.c_str()) < 0) return 0;
		if (atoi(line.c_str()) > instructions.size()) return 0;

		instructions.insert(instructions.begin() + atoi(line.c_str()), temp);//This insert line in "middle" of vector
		
		return 1;//ok
		


	}
}

void saveVectorToFile(string fileName, vector <Instruction> instructions, Memory memory)
{
	ofstream output;
	if (!openFileOut(fileName, output)) //Open file and check error. If error go to hell
	{
		cout << "Nie udalo sie otworzyc pliku do zapisu\n";
		PAUSE;
	}
	else
	{
		
		for (int i = 0; i < instructions.size(); i++)
		{
			uint16_t temp;
			temp = instructions[i].Op;
			temp += (instructions[i].r1 << 4);
			temp += (instructions[i].r1 << 10);
			output.write((char*)& temp, sizeof temp);
			if (instructions[i].Op == JMP || instructions[i].Op == REA)
			{

			}


		}
		output.close();
		/*uint16_t temp;
	file.read((char *)& temp, sizeof temp);
	return temp;*/
	/*Instruction temp;
temp.Op = line & 0b1111;
temp.r1 = (line & 0b1111110000) >> 4;
temp.r2 = (line & 0b1111110000000000) >> 10;
return temp;*/
	}
}




