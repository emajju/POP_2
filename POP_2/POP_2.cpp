//POP 2017-01-23 projekt 2 Mironkiewicz Jan EIT 3 165233 Microsoft Visual Studio Community 2015r
// POP_2.cpp
// 
//  
//	Simple VM
// 
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <limits>
#include <string>
#include <algorithm>
#include <iomanip>
#include <time.h>

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

//#define PERFORMANCE //uncomment for check speed, run file which have no end ex. jmp 0 -1 and end 0 0

#define PAUSE system("pause")
#define CLS system("cls")

struct Instruction //This struct help us when for example we need to convert line to instruction
{
	uint16_t r2;
	uint16_t r1;
	uint16_t Op;
};

struct Memory //Whole mem in one set
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

bool VM(Memory & memory, bool debug);

bool DBG(Memory & memory);

bool EDIT(Memory & memory);

bool NEW(Memory & memory);

bool examples(Memory & memory);

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
int interpretCommand(string command, vector<Instruction>& instructions, Memory &memory);
void saveVectorToFile(string fileName, vector<Instruction> instructions, Memory memory);
bool endExist(vector<Instruction> instructions);
void react(Instruction &instruction, Memory &memory);
void react(Instruction & instruction, Memory & memory, int value);
void reactInsert(Instruction & instruction, Memory & memory, vector<Instruction> &instructions, int line);
void reactInsert(Instruction & instruction, Memory & memory, int value, vector<Instruction> &instructions, int line);
void deleteMemory(Memory & memory, vector<Instruction>& instructions, int lineOfInstr);
bool tryInput(vector<Instruction> instructions);
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
			VM(memory, false);
			break; 
		}

		case 2: //Editor, 2 conditions new file or edit existing file 
		{
			CLS;
			int menu2;
			cout << "1. Edycja" << endl;
			cout << "2. Utworz nowy plik" << endl;
			cout << "0. Wroc do poprzedniego menu" << endl;
			cin >> menu2;
			switch (menu2)
			{
			case 1:
			{
				EDIT(memory);
				break;
			}
			case 2:
			{
				NEW(memory);
				break;
			}
			case 0:
				break;
			}
			break;
		}

		case 3://Debugger, work step by step
		{
			DBG(memory);
			break;
		}
		case 4:
		{
			examples(memory);
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

bool VM(Memory &memory, bool debug)
{
	clrMem(memory); //Clear on init

					//Get file path
	cout << "Przeciagnij plik .bin na okienko programu, nastepnie nacisnij enter\n";
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
		return false;
	}
	fileToVec(instructions, input_file, memory);
	if (instructions.size() == 0) return false;
	if (!tryInput(instructions))
	{
		cout << "Plik ktory probojesz zawiera bledy(brak zakonczenia lub odwoluje sie do rejestrow spoza dopuszczalnego zakresu)" << endl;
		cout << "Nie ma mozliwosci wykonania takiego pliku" << endl;
		PAUSE;
		return false;
	}
	//Make program
	compile(memory, instructions, debug);

	//End
	clrMem(memory);
	CLS;
	return false;
}
bool DBG(Memory &memory)
{
	cout << "Tryb pracy krokowej\n";
	VM(memory, true);
	return false;
}
bool fileWorks(vector <Instruction> &editedFile, Memory &memory, string path)
{
	string command;
	int status = 0;
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');//One error less
	while (1)
	{
		
		CLS;
		printFileEditor(editedFile, memory);
		cout << "Podaj komende: ";
		cin.clear();
		cin.sync();
		getline(cin, command);
		//cin >> command;
		status = interpretCommand(command, editedFile, memory);
		if (status == 2) return false;
		if (status == 3)
		{
			//save
			if (!endExist(editedFile))
			{
				cout << "Plik ktory probojesz zapisac nie posiada instrukcji zakonczenia" << endl;
				cout << "Dodaj instrukcje aby zapisac plik" << endl;
				PAUSE;
				continue;
			}
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
	return false;
}
bool EDIT(Memory &memory)
{
	//Get file path
	cout << "Przeciagnij plik .bin na okienko programu, nastepnie nacisnij enter\n";
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
		return false;
	}
	clrMem(memory);
	fileToVec(editedFile, input_file, memory);
	input_file.close();
	fileWorks(editedFile, memory, path);
	return false;
}
bool NEW(Memory &memory)
{
	cout << "Plik zostanie utworzony w lokalizacji programu z rozszerzeniem .bin\n";
	cout << "Podaj nazwe pliku do utworzenia\n";
	string name;
	cin >> name;
	vector <Instruction> createdFile;
	fileWorks(createdFile, memory, name+".bin");
	return false;
}
bool examples(Memory &memory)
{
	
	int menu = 1;
	while (1)
	{
		CLS;
		cout << "W tej czesci programu mozemy wybrac jeden z przygotowanych przykladow. " << endl;
		cout << "A nastepnie zapisac go pod podana przez nas nazwa" << endl << endl;
		cout << "1. Potegowanie" << endl;
		cout << "2. Silnia" << endl;
		cout << "3. Delta z rownania kwadratowego" << endl;
		cout << "0. Powrot" << endl;
		cin >> menu;
		if (cin.fail())
		{
			cout << "podales zly znak sproboj ponownie" << endl;
			PAUSE;
			cin.clear(); //It's needed to cleanup cin stream before continue otherwise its starts to infinite loop
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			continue;
		}

		switch (menu)
		{
		case 1:
		{
			///
			vector<Instruction> temp;
			interpretCommand("KIN 0 0", temp, memory);
			interpretCommand("KIN 1 0", temp, memory);
			interpretCommand("REA 2 1", temp, memory);
			interpretCommand("CPY 3 0", temp, memory);
			interpretCommand("SUB 1 2", temp, memory);
			interpretCommand("MUL 0 3", temp, memory);
			interpretCommand("SUB 1 2", temp, memory);
			interpretCommand("COM 1 63", temp, memory);
			interpretCommand("JMP 2 -3", temp, memory);
			interpretCommand("SOU 0 0", temp, memory);
			interpretCommand("END 0 0", temp, memory);
			cout << "Plik zostanie utworzony w lokalizacji programu z rozszerzeniem .bin\n";
			cout << "Podaj nazwe pliku do utworzenia\n";
			string name;
			cin >> name;
			saveVectorToFile(name+".bin", temp, memory);
			break;
		}
		case 2:
		{
			vector<Instruction> temp;
			interpretCommand("KIN 0 0", temp, memory);
			interpretCommand("REA 1 1", temp, memory);
			interpretCommand("CPY 2 1", temp, memory);
			interpretCommand("MUL 2 0", temp, memory);
			interpretCommand("SUB 0 1", temp, memory);
			interpretCommand("JMP 2 -2", temp, memory);
			interpretCommand("SOU 2 0", temp, memory);
			interpretCommand("END 0 0", temp, memory);
			cout << "Plik zostanie utworzony w lokalizacji programu z rozszerzeniem .bin\n";
			cout << "Podaj nazwe pliku do utworzenia\n";
			string name;
			cin >> name;
			saveVectorToFile(name + ".bin", temp, memory);
			break;
		}
		case 3:
		{
			vector<Instruction> temp;
			interpretCommand("KIN 0 0", temp, memory);
			interpretCommand("KIN 1 0", temp, memory);
			interpretCommand("KIN 2 0", temp, memory);
			interpretCommand("MUL 1 1", temp, memory);
			interpretCommand("REA 3 4", temp, memory);
			interpretCommand("MUL 3 0", temp, memory);
			interpretCommand("MUL 3 2", temp, memory);
			interpretCommand("SUB 1 3", temp, memory);
			interpretCommand("SOU 1 0", temp, memory);
			interpretCommand("END 0 0", temp, memory);
			cout << "Plik zostanie utworzony w lokalizacji programu z rozszerzeniem .bin\n";
			cout << "Podaj nazwe pliku do utworzenia\n";
			string name;
			cin >> name;
			saveVectorToFile(name + ".bin", temp, memory);
			break;
		}
		default:
			return true;
		}
	}
	return true;
}

void mainMenuDisplay(void)
{
	CLS;
	cout << "Program maszyny wirtualnej, wykonuje pliki, edytuje.\nEdytor zawiera pomoc n.t. dostepnych instrukcji" << endl;
	cout << "Autor Jan Mironkiewicz Nr. indeksu 165233" << endl;
	cout << "Wybierz odpowiednia opcje wpisujac jej numer oraz wcisnij enter"<<endl;
	cout << "1. Maszyna wirtualna" << endl;
	cout << "2. Edytor kodu" << endl;
	cout << "3. Praca krokowa" << endl;
	cout << "4. Przykladowe programy" << endl;
	cout << "0. Wyjscie z programu" << endl;
	cout << "Podaj numer opcji: ";

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

int compile(Memory &memory, vector <Instruction> &instructions, bool debug)
{
	CLS;//Clean screen for better effect
	int summaryCounter = 0;
	//clock_t end1 = 0;
	bool helper = false;//Helps with error handling
	
#ifdef PERFORMANCE
	clock_t beg = clock();
#endif
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
				PAUSE;
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
				cout << "Blad operacja skoku probowala odwolac sie do lini o numerze mniejszym od zera\n";
				PAUSE;
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
				cout << "Blad wykonywania programu w lini " << memory.insCounter <<"Nieobslugiwany warunek skoku";
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
			cout << endl << "podaj liczbe ";
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
			cout << endl << memory.reg[instructions[memory.insCounter].r1] << endl;
			break;
		}
		case END:
		{
			cout << "Ta czesc programu nigdy nie powinna sie wykonac";
			PAUSE;
			break;
		}
		default:
		{
			cout << "Uzyto nieobslugiwanej instrukcji";
			PAUSE;
			return 1;//Goes out from loop
		}

		}
		memory.insCounter++; //Next line
		if (debug) printMemDebug(memory);

		summaryCounter++;
		if (summaryCounter>1000)
		{
			
			cout << "Program wykonal sie juz ponad 1000 instrukcji, czy jest to zamierzony efekt?(t/n)" << endl;
#ifdef PERFORMANCE
			
			clock_t end = clock();
			static double tmp_sec = 0;
			double elapsed_secs = (double(end - beg)/ CLOCKS_PER_SEC);
			
			cout << "i zajelo to " << elapsed_secs - tmp_sec  <<"sec"<< "dla " <<endl;
			tmp_sec = elapsed_secs - tmp_sec;
			
#endif 

			
			string ans;
			cin >> ans;
			if (ans == "t")
			{
				summaryCounter = 0;
				
				continue;
			}
			else if (ans == "n") break;
			else {
				while (1)
				{
					cout << "podano zla odpowiedz, odpowiedz t (tak) lub n (nie)" << endl;
					cin >> ans;
					if (ans == "t")
					{
						summaryCounter = 0;
					
						break;
					}
					else if (ans == "n")
					{
						memory.insCounter = -1;//This makes error on first line check
						helper = true;
						break;
					}
				}
			}
		}
		if ((memory.insCounter > instructions.size()) || (memory.insCounter < 0))
		{
			if (!helper)
			{
				cout << "Blad programu, proba odwolania do lini ktora nie istnieje" << endl;
				PAUSE;
			}
			return 1;
		}
	} 
	cout << "Koniec programu" << endl;
	PAUSE;
	return 0;
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
		{//rewrite this code, in r2 display value 
			if (instruction[i].Op == JMP)
			{
				cout << setw(5) << memory.jumpMem[instruction[i].r2] << "|" << endl;
			}
			else
			{
				cout << setw(5) << memory.constMem[instruction[i].r2] << "|" << endl;
			}
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
	cout << endl << "Lista instrukcji: " << endl;
	cout << "ADD R1 R2 - R1:= R1 + R2" << endl;
	cout << "SUB R1 R2 - R1:= R1 - R2" << endl;
	cout << "MUL R1 R2 - R1:= R1 * R2" << endl;
	cout << "DIV R1 R2 - R1:= R1 / R2, R2:= R1 % R2" << endl;
	cout << "COM R1 R2 - Porownaj R1 R2, wynik w rej flag" << endl;
	cout << "CPY R1 R2 - R1:= R2" << endl;
	cout << "JMP R1 R2 - Skok opis dalej" << endl;
	cout << "REA R1 R2 - Wczytaj stala, opis dalej" << endl;
	cout << "KIN R1 R2 - Wczytaj do R1 z klawiatury za R2 nalezy podac 0" << endl;
	cout << "END R1 R2 - Koniec programu W R1 i R2 Nalezy podac 0" << endl;
	cout << endl << "Instrukcja REA: " << endl;
	cout << "W R1 podac nalezy do ktorego rejestru zapiusujemy stala" << endl;
	cout << "W R2 wpisujemy wartosc stalej" << endl;
	cout << endl << "Instrukcja JMP: " << endl;
	cout << "W R1 nalezy podac informacje kiedy ma dojsc do skoku" << endl;
	cout << "0 - zawsze, 1 - gdy flaga Z jest ustawiona, 2 - gdy flaga Z nie jest ustawiona" << endl;
	cout << "3 - Gdy ustawiona jest flaga D, 4 - gdy ustawiona jest flaga U" << endl;
	cout << "5 - Gdy ustawiona jest flaga D lub Z, 6 - gdy ustawiona jest flaga U lub Z" << endl;
	cout << "W R2 podajemy ilosc lini do przeskoczenia"<<endl;
	PAUSE;
}

bool checkIsDigit(string input)
{
	for (unsigned int i = 0; i < input.size(); i++)
	{
		if ((input[i] - '0') > 9)return false;//If any chars in string after 'conversion' is bigger than 9 means it isn't digit
	}
	return true;
}

int interpretCommand(string command, vector<Instruction> &instructions, Memory &memory)
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
	// 
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
		if (!checkIsDigit(line)) return 0;
		int intLine;
		if (line.empty())intLine = instructions.size() - 1;//if there is no line number were going to delete last
		else intLine = atoi(line.c_str());//but when there is line number convert it from string to int

		if ((intLine < 0) || (intLine > instructions.size() - 1))return 0;//This protect from trying to del something what don't exists

		if (instructions[intLine].Op == REA || instructions[intLine].Op == JMP) deleteMemory(memory, instructions, intLine);

		instructions.erase(instructions.begin()+intLine);//Do this delete
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
		if (!(checkIsDigit(r1) && checkIsDigit(r2))) return 0;//If r1 or r2 isn't digits go to error
		
		temp.r1 = atoi(r1.c_str());
		int r2I = atoi(r2.c_str());

		temp.Op = translateStringToInstruction(instruction);
		if (temp.Op > 10) return 0;//error, wrong instruction

		if (r2I < 0)
		{
			react(temp, memory, r2I);
		}
		else //non minus
		{
			temp.r2 = r2I;
			react(temp, memory);
		}
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

		if (!(checkIsDigit(r1) && checkIsDigit(r2) && checkIsDigit(line))) return 0;//If r1 or r2 or line isn't digits go to error
		temp.r1 = atoi(r1.c_str());
		int r2I = atoi(r2.c_str());

		temp.Op = translateStringToInstruction(instruction);
		if (temp.Op > 10) return 0;//error, wrong instruction
		//Adding inside
		if (atoi(line.c_str()) < 0) return 0;
		if (atoi(line.c_str()) > instructions.size()) return 0;

		if (r2I < 0)
		{
			reactInsert(temp, memory, r2I, instructions, atoi(line.c_str()));
		}
		else //non minus
		{
			temp.r2 = r2I;
			reactInsert(temp, memory, instructions, atoi(line.c_str()));
		}
		//////////////////////////////////////////////////////////////////////////		
		//?
		//////////////////////////////////////////////////////////////////////////
		instructions.insert(instructions.begin() + atoi(line.c_str()), temp);//This insert line in "middle" of vector
		
		return 1;//ok
		


	}
}

void saveIntToFile(ofstream &output, int data)
{
	output.write((char*)& data, sizeof data);
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
		
		for (unsigned int i = 0; i < instructions.size(); i++)
		{
			uint16_t temp = 0;
			temp = instructions[i].Op;
			temp += (instructions[i].r1 << 4);
			
			if (!(instructions[i].Op == JMP || instructions[i].Op == REA))temp += (instructions[i].r2 << 10);//R2 need's to be saved only when isn't JMP or REA
			output.write((char*)& temp, sizeof temp);//Save binary
			if (instructions[i].Op == JMP || instructions[i].Op == REA)//Save value
			{
				switch (instructions[i].Op)
				{

				case JMP:
				{
					saveIntToFile(output,memory.jumpMem[instructions[i].r2]);
					break;
				}
				case REA:
				{
					saveIntToFile(output, memory.constMem[instructions[i].r2]);
					break;
				}

				}
			
			}
		}
		output.close(); //After use needed to close file;
		
	}
}

bool endExist(vector <Instruction> instructions)
{
	for (unsigned int i = 0; i< instructions.size(); i++)
	{
		if (instructions[i].Op == END) return true;
	}
	return false;
}

void react(Instruction &instruction, Memory &memory)
{
	if (instruction.Op==REA)
	{
		memory.constMem.push_back(instruction.r2);
		instruction.r2 = memory.constCounter;
		memory.constCounter++;
	}
	else if (instruction.Op==JMP)
	{
		memory.jumpMem.push_back(instruction.r2);
		instruction.r2 = memory.jumpCounter;
		memory.jumpCounter++;
	}
}

void react(Instruction &instruction, Memory &memory, int value)//OVL
{
	if (instruction.Op == REA)
	{
		memory.constMem.push_back(value);
		instruction.r2 = memory.constCounter;
		memory.constCounter++;
	}
	else if (instruction.Op == JMP)
	{
		memory.jumpMem.push_back(value);
		instruction.r2 = memory.jumpCounter;
		memory.jumpCounter++;
	}
}

void reactInsert(Instruction &instruction, Memory &memory, vector <Instruction> &instructions, int line)
{
	int index;
	bool once = false;
	//Scan for get index in rea or jmp vector
	

	if (instruction.Op == REA)
	{
		for (int i = line; i < instructions.size(); i++)//Starting from line look for first address of memo type
		{
			if (instructions[i].Op == REA)
			{
				if (!once)
				{
					index = instructions[i].r2;
					once = true;
				}
				instructions[i].r2++;
			}
		}
		memory.constMem.insert(memory.constMem.begin() + index, instruction.r2);
		//memory.constMem.push_back(instruction.r2);
		instruction.r2 = index;
		memory.constCounter++;
	}
	else if (instruction.Op == JMP)
	{
		for (int i = line; i < instructions.size(); i++)//Starting from line look for first address of memo type
		{
			if (instructions[i].Op == JMP)
			{
				if (!once)
				{
					index = instructions[i].r2;
					once = true;
				}
				instructions[i].r2++;
			}
		}
		memory.jumpMem.insert(memory.jumpMem.begin() + index, instruction.r2);
		//memory.jumpMem.push_back(instruction.r2);
		instruction.r2 = index;
		memory.jumpCounter++;
	}
}

void reactInsert(Instruction &instruction, Memory &memory, int value, vector <Instruction> &instructions, int line)//OVL
{
	int index;
	bool once = false;
	if (instruction.Op == REA)
	{
		for (int i = line; i < instructions.size(); i++)//Starting from line look for first address of memo type
		{
			if (instructions[i].Op == REA)
			{
				if (!once)
				{
					index = instructions[i].r2;
					once = true;
				}
				instructions[i].r2++;
			}
		}
		memory.constMem.insert(memory.constMem.begin() + index, value);
		//memory.constMem.push_back(instruction.r2);
		instruction.r2 = index;
		memory.constCounter++;
	}
	else if (instruction.Op == JMP)
	{
		for (int i = line; i < instructions.size(); i++)//Starting from line look for first address of memo type
		{
			if (instructions[i].Op == JMP)
			{
				if (!once)
				{
					index = instructions[i].r2;
					once = true;
				}
				instructions[i].r2++;
			}
		}
		memory.jumpMem.insert(memory.jumpMem.begin() + index, value);
		//memory.jumpMem.push_back(instruction.r2);
		instruction.r2 = index;
		memory.jumpCounter++;
	}
}

void deleteMemory(Memory &memory, vector <Instruction> &instructions, int lineOfInstr)//Not sure if works
{
	Instruction temp;
	temp = instructions[lineOfInstr];

	if (temp.Op == REA)
	{
		memory.constMem.erase(memory.constMem.begin() + temp.r2); //R2 storing number of index in vector
		if (!(temp.r2==memory.constCounter-1))
		{
			//rewrite others but only in upper lines
			for (int i = lineOfInstr; i<instructions.size();i++)
			{
				if (instructions[i].Op == REA)
				{
					instructions[i].r2--; //All of them needs to be decreased by one
				}
			}

		}
		
		memory.constCounter--;//One less const in memory
	}
	else if (temp.Op == JMP) //checked again, if error this part won't make more errors
	{
		memory.jumpMem.erase(memory.jumpMem.begin() + temp.r2); //R2 storing number of index in vector
		if (!(temp.r2 == memory.jumpCounter-1))
			//rewrite others but only in upper lines
			for (int i = lineOfInstr; i < instructions.size(); i++)
			{
				if (instructions[i].Op == JMP)
				{
					instructions[i].r2--; //All of them needs to be decreased by one
				}
			}

	

	memory.jumpCounter--;//One less const in memory
	}

}

bool tryInput(vector <Instruction> instructions)
{
	if (!endExist(instructions)) return false; //There is no end
	for (int i =0; i<instructions.size(); i++)
	{
		if (!(instructions[i].Op == JMP || instructions[i].Op == REA))//When its not jump or read try values
		{
			if (!(instructions[i].r1 < 64 && instructions[i].r2 < 64)) return false;//Program uses register bigger than permissible
		}
	}

	return true;

}

//////////////////////////////////////////////////////////////////////////