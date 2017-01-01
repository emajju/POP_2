// POP_2.cpp
//  [12/31/2016 Janek]
//
//	Simple VM
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h" //Header files

using namespace std;// No more std:: witch cout etc.
//Declaration
void mainMenuDisplay(void);

int main()
{
	int menu = 0;
	while (1) //Infinite loop for program
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

			//Get file path

			//Read input file


			//Make program

			//End or next file

			break;
		}

		case 2: //Editor
		{
			break;
		}

		case 3://Debugger
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